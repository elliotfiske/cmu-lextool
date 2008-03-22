/* -*- c-basic-offset: 4; indent-tabs-mode: nil -*- */
/* ====================================================================
 * Copyright (c) 2008 Carnegie Mellon University.  All rights
 * reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer. 
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * This work was supported in part by funding from the Defense Advanced 
 * Research Projects Agency and the National Science Foundation of the 
 * United States of America, and the CMU Sphinx Speech Consortium.
 *
 * THIS SOFTWARE IS PROVIDED BY CARNEGIE MELLON UNIVERSITY ``AS IS'' AND 
 * ANY EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY
 * NOR ITS EMPLOYEES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * ====================================================================
 *
 */

/**
 * @file ngram_search.c N-Gram based multi-pass search ("FBS")
 */

/* System headers. */
#include <string.h>
#include <assert.h>

/* SphinxBase headers. */
#include <ckd_alloc.h>
#include <listelem_alloc.h>

/* Local headers. */
#include "ngram_search.h"
#include "ngram_search_fwdtree.h"
#include "ngram_search_fwdflat.h"
#include "ngram_search_dag.h"

static void
ngram_search_update_widmap(ngram_search_t *ngs)
{
    const char **words;
    int32 i, n_words;

    /* It's okay to include fillers since they won't be in the LM */
    n_words = ngs->dict->dict_entry_count;
    words = ckd_calloc(n_words, sizeof(*words));
    for (i = 0; i < n_words; ++i)
        words[i] = ngs->dict->dict_list[i]->word;
    ngram_model_set_map_words(ngs->lmset, words, n_words);
    ckd_free(words);
}

ngram_search_t *
ngram_search_init(cmd_ln_t *config,
		  acmod_t *acmod,
		  dict_t *dict)
{
	ngram_search_t *ngs;
        const char *path;

	ngs = ckd_calloc(1, sizeof(*ngs));
        ngs->config = config;
	ngs->acmod = acmod;
	ngs->dict = dict;
	ngs->hmmctx = hmm_context_init(bin_mdef_n_emit_state(acmod->mdef),
				       acmod->tmat->tp, NULL, acmod->mdef->sseq);
        ngs->chan_alloc = listelem_alloc_init(sizeof(chan_t));
        ngs->root_chan_alloc = listelem_alloc_init(sizeof(root_chan_t));
        ngs->latnode_alloc = listelem_alloc_init(sizeof(latnode_t));

        /* Calculate log beam widths. */
        ngs->beam = logmath_log(acmod->lmath, cmd_ln_float64_r(config, "-beam"));
        ngs->wbeam = logmath_log(acmod->lmath, cmd_ln_float64_r(config, "-wbeam"));
        ngs->pbeam = logmath_log(acmod->lmath, cmd_ln_float64_r(config, "-pbeam"));
        ngs->lpbeam = logmath_log(acmod->lmath, cmd_ln_float64_r(config, "-lpbeam"));
        ngs->lponlybeam = logmath_log(acmod->lmath, cmd_ln_float64_r(config, "-lponlybeam"));
        ngs->fwdflatbeam = logmath_log(acmod->lmath, cmd_ln_float64_r(config, "-fwdflatbeam"));
        ngs->fwdflatwbeam = logmath_log(acmod->lmath, cmd_ln_float64_r(config, "-fwdflatwbeam"));

        /* Absolute pruning parameters. */
        ngs->maxwpf = cmd_ln_int32_r(config, "-maxwpf");
        ngs->maxhmmpf = cmd_ln_int32_r(config, "-maxhmmpf");

        /* Various penalties which may or may not be useful. */
        ngs->wip = logmath_log(acmod->lmath, cmd_ln_float32_r(config, "-wip"));
        ngs->nwpen = logmath_log(acmod->lmath, cmd_ln_float32_r(config, "-nwpen"));
        ngs->pip = logmath_log(acmod->lmath, cmd_ln_float32_r(config, "-pip"));
        ngs->silpen = ngs->pip
            + logmath_log(acmod->lmath, cmd_ln_float32_r(config, "-silpen"));
        ngs->fillpen = ngs->pip
            + logmath_log(acmod->lmath, cmd_ln_float32_r(config, "-fillpen"));

        /* Language weight ratios for fwdflat and bestpath search. */
        ngs->fwdflat_fwdtree_lw_ratio =
            cmd_ln_float32_r(config, "-fwdflatlw")
            / cmd_ln_float32_r(config, "-lw");
        ngs->bestpath_fwdtree_lw_ratio =
            cmd_ln_float32_r(config, "-bestpathlw")
            / cmd_ln_float32_r(config, "-lw");

        /* This is useful for something. */
        ngs->start_wid = dict_to_id(ngs->dict, "<s>");
        ngs->finish_wid = dict_to_id(ngs->dict, "</s>");
        ngs->silence_wid = dict_to_id(ngs->dict, "<sil>");

        /* Allocate a billion different tables for stuff. */
        ngs->word_chan = ckd_calloc(dict->dict_entry_count,
                                    sizeof(*ngs->word_chan));
        ngs->word_lat_idx = ckd_calloc(dict->dict_entry_count,
                                       sizeof(*ngs->word_lat_idx));
        ngs->zeroPermTab = ckd_calloc(bin_mdef_n_ciphone(acmod->mdef),
                                      sizeof(*ngs->zeroPermTab));
        ngs->word_active = bitvec_alloc(dict->dict_entry_count);
        ngs->last_ltrans = ckd_calloc(ngs->dict->dict_entry_count,
                                      sizeof(*ngs->last_ltrans));

        /* FIXME: All these structures need to be made dynamic with
         * garbage collection. */
        ngs->bp_table_size = cmd_ln_int32_r(config, "-latsize");
        ngs->bp_table = ckd_calloc(ngs->bp_table_size,
                                   sizeof(*ngs->bp_table));
        /* FIXME: This thing is frickin' huge. */
        ngs->bscore_stack_size = ngs->bp_table_size * 20;
        ngs->bscore_stack = ckd_calloc(ngs->bscore_stack_size,
                                       sizeof(*ngs->bscore_stack));
        ngs->n_frame_alloc = 256;
        ngs->bp_table_idx = ckd_calloc(ngs->n_frame_alloc + 1,
                                       sizeof(*ngs->bp_table_idx));
        ++ngs->bp_table_idx; /* Make bptableidx[-1] valid */

        /* Allocate active word list array */
        ngs->active_word_list = ckd_calloc_2d(2, dict->dict_entry_count,
                                              sizeof(**ngs->active_word_list));

        /* Load language model(s) */
        if ((path = cmd_ln_str_r(config, "-lmctlfn"))) {
            ngs->lmset = ngram_model_set_read(config, path, acmod->lmath);
            if (ngs->lmset == NULL) {
                E_ERROR("Failed to read language model control file: %s\n",
                        path);
                goto error_out;
            }
            /* Set the default language model if needed. */
            if ((path = cmd_ln_str_r(config, "-lmname"))) {
                ngram_model_set_select(ngs->lmset, path);
            }
        }
        else if ((path = cmd_ln_str_r(config, "-lm"))) {
            static const char *name = "default";
            ngram_model_t *lm;

            lm = ngram_model_read(config, path, NGRAM_AUTO, acmod->lmath);
            if (lm == NULL) {
                E_ERROR("Failed to read language model file: %s\n", path);
                goto error_out;
            }
            ngs->lmset = ngram_model_set_init(config,
                                              &lm, (char **)&name,
                                              NULL, 1);
            if (ngs->lmset == NULL) {
                E_ERROR("Failed to initialize language model set\n");
                goto error_out;
            }
        }

        /* Create word mappings. */
        ngram_search_update_widmap(ngs);

        /* Initialize fwdtree, fwdflat, bestpath modules if necessary. */
        if (cmd_ln_boolean_r(config, "-fwdtree"))
            ngram_fwdtree_init(ngs);
        if (cmd_ln_boolean_r(config, "-fwdflat"))
            ngram_fwdflat_init(ngs);
	return ngs;

error_out:
        ngram_search_free(ngs);
        return NULL;
}

void
ngram_search_free(ngram_search_t *ngs)
{
    if (cmd_ln_boolean_r(ngs->config, "-fwdtree"))
        ngram_fwdtree_deinit(ngs);
    if (cmd_ln_boolean_r(ngs->config, "-fwdflat"))
        ngram_fwdflat_deinit(ngs);

    hmm_context_free(ngs->hmmctx);
    listelem_alloc_free(ngs->chan_alloc);
    listelem_alloc_free(ngs->root_chan_alloc);
    listelem_alloc_free(ngs->latnode_alloc);
    ngram_model_free(ngs->lmset);

    ckd_free(ngs->word_chan);
    ckd_free(ngs->word_lat_idx);
    ckd_free(ngs->zeroPermTab);
    bitvec_free(ngs->word_active);
    ckd_free(ngs->bp_table);
    ckd_free(ngs->bscore_stack);
    ckd_free(ngs->bp_table_idx - 1);
    ckd_free_2d(ngs->active_word_list);
    ckd_free(ngs->hyp_str);
    ckd_free(ngs->last_ltrans);
    ckd_free(ngs);
}

int
ngram_search_mark_bptable(ngram_search_t *ngs, int frame_idx)
{
    if (frame_idx >= ngs->n_frame_alloc) {
        ngs->n_frame_alloc *= 2;
        ngs->bp_table_idx = ckd_realloc(ngs->bp_table_idx - 1,
                                        (ngs->n_frame_alloc + 1)
                                        * sizeof(*ngs->bp_table_idx));
        if (ngs->frm_wordlist) {
            ngs->frm_wordlist = ckd_realloc(ngs->frm_wordlist,
                                            ngs->n_frame_alloc
                                            * sizeof(*ngs->frm_wordlist));
        }
        ++ngs->bp_table_idx; /* Make bptableidx[-1] valid */
    }
    ngs->bp_table_idx[frame_idx] = ngs->bpidx;
    return ngs->bpidx;
}

/**
 * Find trigram predecessors for a backpointer table entry.
 */
static void
cache_bptable_paths(ngram_search_t *ngs, int32 bp)
{
    int32 w, prev_bp;
    bptbl_t *bpe;

    bpe = &(ngs->bp_table[bp]);
    prev_bp = bp;
    w = bpe->wid;
    /* FIXME: This isn't the ideal way to tell if it's a filler. */
    while (w >= ngs->silence_wid) {
        prev_bp = ngs->bp_table[prev_bp].bp;
        w = ngs->bp_table[prev_bp].wid;
    }
    bpe->real_wid = ngs->dict->dict_list[w]->wid;

    prev_bp = ngs->bp_table[prev_bp].bp;
    bpe->prev_real_wid =
        (prev_bp != NO_BP) ? ngs->bp_table[prev_bp].real_wid : -1;
}

void
ngram_search_save_bp(ngram_search_t *ngs, int frame_idx,
                     int32 w, int32 score, int32 path, int32 rc)
{
    int32 _bp_;

    _bp_ = ngs->word_lat_idx[w];
    if (_bp_ != NO_BP) {
        if (ngs->bp_table[_bp_].score < score) {
            if (ngs->bp_table[_bp_].bp != path) {
                ngs->bp_table[_bp_].bp = path;
                cache_bptable_paths(ngs, _bp_);
            }
            ngs->bp_table[_bp_].score = score;
        }
        ngs->bscore_stack[ngs->bp_table[_bp_].s_idx + rc] = score;
    }
    else {
        int32 i, rcsize, *bss;
        dict_entry_t *de;
        bptbl_t *bpe;

        /* Expand the backpointer tables if necessary. */
        if (ngs->bpidx >= ngs->bp_table_size) {
            ngs->bp_table_size *= 2;
            ngs->bp_table = ckd_realloc(ngs->bp_table,
                                        ngs->bp_table_size
                                        * sizeof(*ngs->bp_table));
            E_INFO("Resized backpointer table to %d entries\n", ngs->bp_table_size);
        }
        if (ngs->bss_head >= ngs->bscore_stack_size
            - bin_mdef_n_ciphone(ngs->acmod->mdef)) {
            ngs->bscore_stack_size *= 2;
            ngs->bscore_stack = ckd_realloc(ngs->bscore_stack,
                                            ngs->bscore_stack_size
                                            * sizeof(*ngs->bscore_stack));
            E_INFO("Resized score stack to %d entries\n", ngs->bscore_stack_size);
        }

        de = ngs->dict->dict_list[w];
        ngs->word_lat_idx[w] = ngs->bpidx;
        bpe = &(ngs->bp_table[ngs->bpidx]);
        bpe->wid = w;
        bpe->frame = frame_idx;
        bpe->bp = path;
        bpe->score = score;
        bpe->s_idx = ngs->bss_head;
        bpe->valid = TRUE;

        if ((de->len != 1) && (de->mpx)) {
            bpe->r_diph = de->phone_ids[de->len - 1];
            rcsize = ngs->dict->rcFwdSizeTable[bpe->r_diph];
        }
        else {
            bpe->r_diph = -1;
            rcsize = 1;
        }
        for (i = rcsize, bss = ngs->bscore_stack + ngs->bss_head; i > 0; --i, bss++)
            *bss = WORST_SCORE;
        ngs->bscore_stack[ngs->bss_head + rc] = score;
        cache_bptable_paths(ngs, ngs->bpidx);

        ngs->bpidx++;
        ngs->bss_head += rcsize;
    }
}

int
ngram_search_find_exit(ngram_search_t *ngs, int frame_idx, int32 *out_best_score)
{
    /* End of backpointers for this frame. */
    int end_bpidx;
    int best_exit, bp;
    int32 best_score;

    if (frame_idx == -1)
        frame_idx = acmod_frame_idx(ngs->acmod);
    end_bpidx = ngs->bp_table_idx[frame_idx];

    /* FIXME: WORST_SCORE has to go away and be replaced with a log-zero number. */
    best_score = WORST_SCORE;
    best_exit = NO_BP;

    /* Scan back to find a frame with some backpointers in it. */
    while (frame_idx >= 0 && ngs->bp_table_idx[frame_idx] == end_bpidx)
        --frame_idx;
    if (frame_idx < 0) {
        E_ERROR("ngram_search_find_exit() called with empty backpointer table\n");
        return NO_BP;
    }

    /* Now find the best scoring entry. */
    for (bp = ngs->bp_table_idx[frame_idx]; bp < end_bpidx; ++bp) {
        if (ngs->bp_table[bp].score > best_score) {
            best_score = ngs->bp_table[bp].score;
            best_exit = bp;
        }
    }

    if (out_best_score) *out_best_score = best_score;
    return best_exit;
}

char const *
ngram_search_hyp(ngram_search_t *ngs, int bpidx)
{
    char *c;
    size_t len;
    int bp;

    if (bpidx == NO_BP)
        return NULL;

    bp = bpidx;
    len = 0;
    while (bp != NO_BP) {
        bptbl_t *be = &ngs->bp_table[bp];
        bp = be->bp;
        if (dict_is_filler_word(ngs->dict, be->wid))
            continue;
        len += strlen(ngs->dict->dict_list[be->wid]->word) + 1;
    }

    ckd_free(ngs->hyp_str);
    ngs->hyp_str = ckd_calloc(1, len);
    bp = bpidx;
    c = ngs->hyp_str + len - 1;
    while (bp != NO_BP) {
        bptbl_t *be = &ngs->bp_table[bp];
        size_t len;

        bp = be->bp;
        if (dict_is_filler_word(ngs->dict, be->wid))
            continue;

        len = strlen(ngs->dict->dict_list[be->wid]->word);
        c -= len;
        memcpy(c, ngs->dict->dict_list[be->wid]->word, len);
        if (c > ngs->hyp_str) {
            --c;
            *c = ' ';
        }
    }

    return ngs->hyp_str;
}

void
ngram_search_alloc_all_rc(ngram_search_t *ngs, int32 w)
{
    dict_entry_t *de;
    chan_t *hmm, *thmm;
    int32 *sseq_rc;             /* list of sseqid for all possible right context for w */
    int32 i;

    de = ngs->dict->dict_list[w];

    assert(de->mpx);

    sseq_rc = ngs->dict->rcFwdTable[de->phone_ids[de->len - 1]];

    hmm = ngs->word_chan[w];
    if ((hmm == NULL) || (hmm->hmm.s.ssid != *sseq_rc)) {
        hmm = listelem_malloc(ngs->chan_alloc);
        hmm->next = ngs->word_chan[w];
        ngs->word_chan[w] = hmm;

        hmm->info.rc_id = 0;
        hmm->ciphone = de->ci_phone_ids[de->len - 1];
        hmm_init(ngs->hmmctx, &hmm->hmm, FALSE, *sseq_rc, hmm->ciphone);
    }
    for (i = 1, sseq_rc++; *sseq_rc >= 0; sseq_rc++, i++) {
        if ((hmm->next == NULL) || (hmm->next->hmm.s.ssid != *sseq_rc)) {
            thmm = listelem_malloc(ngs->chan_alloc);
            thmm->next = hmm->next;
            hmm->next = thmm;
            hmm = thmm;

            hmm->info.rc_id = i;
            hmm->ciphone = de->ci_phone_ids[de->len - 1];
            hmm_init(ngs->hmmctx, &hmm->hmm, FALSE, *sseq_rc, hmm->ciphone);
        }
        else
            hmm = hmm->next;
    }
}

void
ngram_search_free_all_rc(ngram_search_t *ngs, int32 w)
{
    chan_t *hmm, *thmm;

    for (hmm = ngs->word_chan[w]; hmm; hmm = thmm) {
        thmm = hmm->next;
        hmm_deinit(&hmm->hmm);
        listelem_free(ngs->chan_alloc, hmm);
    }
    ngs->word_chan[w] = NULL;
}

/*
 * Compute acoustic and LM scores for each BPTable entry (segment).
 */
void
ngram_compute_seg_scores(ngram_search_t *ngs, float32 lwf)
{
    int32 bp, start_score;
    bptbl_t *bpe, *p_bpe;
    int32 *rcpermtab;
    dict_entry_t *de;

    for (bp = 0; bp < ngs->bpidx; bp++) {
        bpe = ngs->bp_table + bp;

        /* Start of utterance. */
        if (bpe->bp == NO_BP) {
            bpe->ascr = bpe->score;
            bpe->lscr = 0;
            continue;
        }

        /* Otherwise, calculate lscr and ascr. */
        de = ngs->dict->dict_list[bpe->wid];
        p_bpe = ngs->bp_table + bpe->bp;
        rcpermtab = (p_bpe->r_diph >= 0) ?
            ngs->dict->rcFwdPermTable[p_bpe->r_diph] : ngs->zeroPermTab;
        start_score =
            ngs->bscore_stack[p_bpe->s_idx + rcpermtab[de->ci_phone_ids[0]]];

        if (bpe->wid == ngs->silence_wid) {
            bpe->lscr = ngs->silpen;
        }
        else if (ISA_FILLER_WORD(ngs, bpe->wid)) {
            bpe->lscr = ngs->fillpen;
        }
        else {
            int32 n_used;
            bpe->lscr = ngram_tg_score(ngs->lmset, de->wid,
                                       p_bpe->real_wid,
                                       p_bpe->prev_real_wid, &n_used);
            bpe->lscr = bpe->lscr * lwf;
        }
        bpe->ascr = bpe->score - start_score - bpe->lscr;
    }
}
