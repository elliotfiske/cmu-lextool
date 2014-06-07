/* ====================================================================
 * Copyright (c) 2014 Carnegie Mellon University.  All rights
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

/*
* allphone_search.c -- Search for phonetic decoding.
*/

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <sphinxbase/err.h>
#include <sphinxbase/ckd_alloc.h>
#include <sphinxbase/strfuncs.h>
#include <sphinxbase/pio.h>
#include <sphinxbase/cmd_ln.h>

#include "pocketsphinx_internal.h"
#include "allphone_search.h"

static ps_lattice_t *
allphone_search_lattice(ps_search_t * search)
{
    //cap
    return NULL;
}

static int
allphone_search_prob(ps_search_t * search)
{
    return 0;
}

static void
allphone_clear_phseg(allphone_search_t * allphs);

static phseg_t *
allphone_backtrace(allphone_search_t * allphs, int32 f);

static void
allphone_search_seg_free(ps_seg_t *seg)
{
    return;
}

static ps_seg_t *
allphone_search_seg_next(ps_seg_t *seg)
{
    phseg_t *phseg = (phseg_t*)seg;
    ps_seg_t *nextseg = (ps_seg_t*)phseg->next;
    
    if (nextseg == NULL)
	return NULL;
    
    nextseg->vt = seg->vt;
    nextseg->search = seg->search;
    nextseg->sf = phseg->sf;
    nextseg->ef = phseg->ef;
    nextseg->word = bin_mdef_ciphone_str(ps_search_acmod(seg->search)->mdef, phseg->ci);

    return nextseg;
}

static ps_segfuncs_t fsg_segfuncs = {
    /* seg_next */ allphone_search_seg_next,
    /* seg_free */ allphone_search_seg_free
};


static ps_seg_t *
allphone_search_seg_iter(ps_search_t * search, int32 * out_score)
{
    allphone_search_t *allphs = (allphone_search_t *)search;
    phseg_t *phseg;
    allphone_clear_phseg(allphs);
    phseg = allphs->phseg = allphone_backtrace(allphs, allphs->frame - 1);

    if (phseg == NULL)
	return NULL;
    
    phseg->base.vt = &fsg_segfuncs;
    phseg->base.search = search;
    
    return (ps_seg_t *)allphs->phseg;
}

static ps_searchfuncs_t allphone_funcs = {
    /* name: */ "allphone",
    /* start: */ allphone_search_start,
    /* step: */ allphone_search_step,
    /* finish: */ allphone_search_finish,
    /* reinit: */ allphone_search_reinit,
    /* free: */ allphone_search_free,
    /* lattice: */ allphone_search_lattice,
    /* hyp: */ allphone_search_hyp,
    /* prob: */ allphone_search_prob,
    /* seg_iter: */ allphone_search_seg_iter,
};

/**
 * Find PHMM node with same senone sequence and tmat id as the given triphone.
 * Return ptr to PHMM node if found, NULL otherwise.
 */
static phmm_t *
phmm_lookup(allphone_search_t * allphs, s3pid_t pid)
{
    phmm_t *p;
    bin_mdef_t *mdef;
    phmm_t **ci_phmm;

    mdef = ((ps_search_t *) allphs)->acmod->mdef;
    ci_phmm = allphs->ci_phmm;

    for (p = ci_phmm[bin_mdef_pid2ci(mdef, pid)]; p; p = p->next) {
        if (mdef_pid2tmatid(mdef, p->pid) == mdef_pid2tmatid(mdef, pid))
            if (mdef_pid2ssid(mdef, p->pid) == mdef_pid2ssid(mdef, pid))
                return p;
    }

    //not found
    return NULL;
}

static int32
phmm_link(allphone_search_t * allphs)
{
    s3cipid_t ci, rc;
    phmm_t *p, *p2;
    int32 *rclist;
    int32 i, n_link;
    plink_t *l;
    bin_mdef_t *mdef;
    phmm_t **ci_phmm;

    mdef = ((ps_search_t *) allphs)->acmod->mdef;
    ci_phmm = allphs->ci_phmm;

    rclist = (int32 *) ckd_calloc(mdef->n_ciphone + 1, sizeof(int32));

    /* Create successor links between PHMM nodes */
    n_link = 0;
    for (ci = 0; ci < mdef->n_ciphone; ci++) {
        for (p = ci_phmm[ci]; p; p = p->next) {
            /* Build rclist for p */
            i = 0;
            for (rc = 0; rc < mdef->n_ciphone; rc++) {
                if (bitvec_is_set(p->rc, rc))
                    rclist[i++] = rc;
            }
            rclist[i] = BAD_S3CIPID;

            /* For each rc in rclist, transition to PHMMs for rc if left context = ci */
            for (i = 0; IS_S3CIPID(rclist[i]); i++) {
                for (p2 = ci_phmm[rclist[i]]; p2; p2 = p2->next) {
                    if (bitvec_is_set(p2->lc, ci)) {
                        /* transition from p to p2 */
                        l = (plink_t *) ckd_calloc(1, sizeof(*l));
                        l->phmm = p2;
                        l->next = p->succlist;
                        p->succlist = l;

                        n_link++;
                    }
                }
            }
        }
    }

    ckd_free(rclist);

    return n_link;
}

/**
 * Build net from phone HMMs
 */
static int
phmm_build(allphone_search_t * allphs)
{
    phmm_t *p, **pid2phmm;
    bin_mdef_t *mdef;
    int32 lrc_size;
    uint32 *lc, *rc;
    s3pid_t pid;
    s3cipid_t ci;
    s3cipid_t *filler;
    int n_phmm, n_link;
    int i;

    E_INFO("Building PHMM net\n");
    mdef = ((ps_search_t *) allphs)->acmod->mdef;
    allphs->ci_phmm =
        (phmm_t **) ckd_calloc(bin_mdef_n_ciphone(mdef), sizeof(phmm_t *));
    pid2phmm =
        (phmm_t **) ckd_calloc(bin_mdef_n_phone(mdef), sizeof(phmm_t *));

    /* For each unique ciphone/triphone entry in mdef, create a PHMM node */
    n_phmm = 0;
    for (pid = 0; pid < bin_mdef_n_phone(mdef); pid++) {
        if ((p = phmm_lookup(allphs, pid)) == NULL) {
            //not found, should be created
            p = (phmm_t *) ckd_calloc(1, sizeof(*p));
            hmm_init(allphs->hmmctx, &(p->hmm), FALSE,
                     mdef_pid2ssid(mdef, pid), mdef->phone[pid].tmat);
            p->pid = pid;
            p->ci = bin_mdef_pid2ci(mdef, pid);
            p->succlist = NULL;
            p->next = allphs->ci_phmm[p->ci];
            allphs->ci_phmm[p->ci] = p;
            n_phmm++;
        }
        pid2phmm[pid] = p;
    }

    /* Fill out bitvecs of each PHMM node, alloc continuous memory chunk for context bitvectors */
    lrc_size = bitvec_size(bin_mdef_n_ciphone(mdef));
    lc = ckd_calloc(n_phmm * 2 * lrc_size, sizeof(bitvec_t));
    rc = lc + (n_phmm * lrc_size);
    for (ci = 0; ci < mdef->n_ciphone; ci++) {
        for (p = allphs->ci_phmm[ci]; p; p = p->next) {
            p->lc = lc;
            lc += lrc_size;
            p->rc = rc;
            rc += lrc_size;
        }
    }

    /* Fill out lc and rc bitmaps (remember to map all fillers to each other!!) */
    filler =
        (s3cipid_t *) ckd_calloc(bin_mdef_n_ciphone(mdef) + 1,
                                 sizeof(s3cipid_t));

    /* Connect fillers */
    i = 0;
    for (ci = 0; ci < bin_mdef_n_ciphone(mdef); ci++) {
        p = pid2phmm[ci];
        bitvec_set_all(p->lc, bin_mdef_n_ciphone(mdef));
        bitvec_set_all(p->rc, bin_mdef_n_ciphone(mdef));
        if (mdef->phone[ci].info.ci.filler) {
            filler[i++] = ci;
        }
    }
    filler[i] = BAD_S3CIPID;


    /* Loop over cdphones */
    for (pid = bin_mdef_n_ciphone(mdef); pid < bin_mdef_n_phone(mdef);
         pid++) {
        p = pid2phmm[pid];

        if (mdef->phone[mdef->phone[pid].info.cd.ctx[1]].info.ci.filler) {
            for (i = 0; IS_S3CIPID(filler[i]); i++)
                bitvec_set(p->lc, filler[i]);
        }
        else
            bitvec_set(p->lc, mdef->phone[pid].info.cd.ctx[1]);

        if (mdef->phone[mdef->phone[pid].info.cd.ctx[2]].info.ci.filler) {
            for (i = 0; IS_S3CIPID(filler[i]); i++)
                bitvec_set(p->rc, filler[i]);
        }
        else
            bitvec_set(p->rc, mdef->phone[pid].info.cd.ctx[2]);
    }
    ckd_free(pid2phmm);
    ckd_free(filler);

    /* Create links between PHMM nodes */
    n_link = phmm_link(allphs);

    E_INFO("%d nodes, %d links\n", n_phmm, n_link);
    return 0;
}

static void
phmm_free(allphone_search_t * allphs)
{
    s3cipid_t ci;
    bin_mdef_t *mdef;

    if (!allphs->ci_phmm)
        //nothing to free
        return;
    ckd_free(allphs->ci_phmm[0]->lc);
    mdef = ((ps_search_t *) allphs)->acmod->mdef;
    for (ci = 0; ci < mdef_n_ciphone(mdef); ++ci) {
        phmm_t *p, *next;

        for (p = allphs->ci_phmm[ci]; p; p = next) {
            plink_t *l, *lnext;

            next = p->next;
            for (l = p->succlist; l; l = lnext) {
                lnext = l->next;
                ckd_free(l);
            }
            hmm_deinit(&(p->hmm));
            ckd_free(p);
        }
    }
    ckd_free(allphs->ci_phmm);
}

/** Evaluate active PHMMs */
static int32
phmm_eval_all(allphone_search_t * allphs, const int16 * senscr)
{
    s3cipid_t ci;
    phmm_t *p;
    int32 best;
    bin_mdef_t *mdef;
    phmm_t **ci_phmm;

    mdef = ((ps_search_t *) allphs)->acmod->mdef;
    ci_phmm = allphs->ci_phmm;

    best = S3_LOGPROB_ZERO;

    hmm_context_set_senscore(allphs->hmmctx, senscr);
    for (ci = 0; ci < mdef->n_ciphone; ci++) {
        for (p = ci_phmm[(unsigned) ci]; p; p = p->next) {
            if (hmm_frame(&(p->hmm)) == allphs->frame) {
                int32 score;
                allphs->n_hmm_eval++;
                score = hmm_vit_eval((hmm_t *) p);
                if (score > best)
                    best = score;
            }
        }
    }

    return best;
}

static void
phmm_exit(allphone_search_t * allphs, int32 best)
{
    s3cipid_t ci;
    phmm_t *p;
    int32 th, nf;
    history_t *h;
    blkarray_list_t *frm_hist;
    bin_mdef_t *mdef;
    int32 curfrm;
    phmm_t **ci_phmm;
    int32 *ci2lmwid;

    th = best + allphs->pbeam;

    frm_hist = allphs->frm_hist;
    mdef = ps_search_acmod(allphs)->mdef;
    curfrm = allphs->frame;
    ci_phmm = allphs->ci_phmm;
    ci2lmwid = allphs->ci2lmwid;

    nf = curfrm + 1;

    for (ci = 0; ci < mdef->n_ciphone; ci++) {
        for (p = ci_phmm[(unsigned) ci]; p; p = p->next) {
            if (hmm_frame(&(p->hmm)) == curfrm) {

                if (hmm_bestscore(&(p->hmm)) >= th) {

                    h = (history_t *) ckd_calloc(1, sizeof(*h));
                    h->ef = curfrm;
                    h->phmm = p;
                    h->hist = hmm_out_history(&(p->hmm));
                    h->score = hmm_out_score(&(p->hmm));

                    if (!allphs->lmset) {
                        h->tscore = allphs->inspen;
                    }
                    else {
                        if (h->hist > 0) {
                            int32 n_used;
                            history_t *pred =
                                blkarray_list_get(frm_hist, h->hist);

                            if (pred->hist > 0) {
                                history_t *pred_pred =
                                    blkarray_list_get(frm_hist,
                                                      h->hist);
                                h->tscore =
                                    ngram_tg_score(allphs->lmset,
                                                   ci2lmwid
                                                   [pred_pred->phmm->ci],
                                                   ci2lmwid[pred->phmm->
                                                            ci],
                                                   ci2lmwid[p->ci],
                                                   &n_used) >>
                                    SENSCR_SHIFT;
                            }
                            else {
                                h->tscore =
                                    ngram_bg_score(allphs->lmset,
                                                   ci2lmwid
                                                   [pred->phmm->ci],
                                                   ci2lmwid[p->ci],
                                                   &n_used) >>
                                    SENSCR_SHIFT;
                            }
                        }
                        else {
                            /*
                             * This is the beginning SIL and in srch_allphone_begin()
                             * it's inscore is set to 0.
                             */
                            h->tscore = 0;
                        }
                    }

                    blkarray_list_append(frm_hist, h);

                    /* Mark PHMM active in next frame */
                    hmm_frame(&(p->hmm)) = nf;
                }
                else {
                    /* Reset state scores */
                    hmm_clear(&(p->hmm));
                }
            }
        }
    }
}

static void
phmm_trans(allphone_search_t * allphs, int32 best)
{
    history_t *h;
    phmm_t *from, *to;
    plink_t *l;
    int32 newscore, nf, curfrm;
    int32 *ci2lmwid;
    int32 hist_idx;

    curfrm = allphs->frame;
    nf = curfrm + 1;
    ci2lmwid = allphs->ci2lmwid;

    /* Transition from exited nodes to initial states of HMMs */
    for (hist_idx = allphs->hist_start;
         hist_idx < blkarray_list_n_valid(allphs->frm_hist); hist_idx++) {
        h = blkarray_list_get(allphs->frm_hist, hist_idx);
        from = h->phmm;
        for (l = from->succlist; l; l = l->next) {
            int32 tscore;
            to = l->phmm;

            /* No LM, just use uniform (insertion penalty). */
            if (!allphs->lmset)
                tscore = allphs->inspen;
            /* If they are not in the LM, kill this
             * transition. */
            else if (ci2lmwid[to->ci] == NGRAM_INVALID_WID)
                continue;
            else {
                int32 n_used;
                if (h->hist > 0) {
                    history_t *pred =
                        blkarray_list_get(allphs->frm_hist, h->hist);
                    tscore =
                        ngram_tg_score(allphs->lmset,
                                       ci2lmwid[pred->phmm->ci],
                                       ci2lmwid[from->ci],
                                       ci2lmwid[to->ci],
                                       &n_used) >> SENSCR_SHIFT;
                }
                else {
                    tscore = ngram_bg_score(allphs->lmset,
                                            ci2lmwid[from->ci],
                                            ci2lmwid[to->ci],
                                            &n_used) >> SENSCR_SHIFT;
                }
            }

            newscore = h->score + tscore;
            if ((newscore > best + allphs->beam)
                && (newscore > hmm_in_score(&(to->hmm)))) {
                hmm_enter(&(to->hmm), newscore, hist_idx, nf);
            }
        }
    }
}

ps_search_t *
allphone_search_init(ngram_model_t * lm,
                     cmd_ln_t * config,
                     acmod_t * acmod, dict_t * dict, dict2pid_t * d2p)
{
    int i;
    bin_mdef_t *mdef;
    allphone_search_t *allphs;
    static char *lmname = "default";

    allphs = (allphone_search_t *) ckd_calloc(1, sizeof(*allphs));
    ps_search_init(ps_search_base(allphs), &allphone_funcs, config, acmod,
                   dict, d2p);
    mdef = acmod->mdef;
    allphs->hmmctx = hmm_context_init(bin_mdef_n_emit_state(mdef),
                                      acmod->tmat->tp, NULL, mdef->sseq);
    if (allphs->hmmctx == NULL) {
        ps_search_free(ps_search_base(allphs));
        return NULL;
    }
    phmm_build(allphs);
    if (lm) {
        //language model is defined
        allphs->lmset =
            ngram_model_set_init(config, &lm, &lmname, NULL, 1);
        if (!allphs->lmset) {
            E_ERROR
                ("Failed to initialize ngram model set for phoneme decoding");
            allphone_search_free((ps_search_t *) allphs);
            return NULL;
        }
        allphs->ci2lmwid =
            (int32 *) ckd_calloc(mdef->n_ciphone,
                                 sizeof(*allphs->ci2lmwid));
        for (i = 0; i < mdef->n_ciphone; i++) {
            allphs->ci2lmwid[i] =
                ngram_wid(allphs->lmset,
                          (char *) bin_mdef_ciphone_str(mdef, i));
            /* Map filler phones to silence if not found */
            if (allphs->ci2lmwid[i] == NGRAM_INVALID_WID
                && bin_mdef_ciphone_str(mdef, i))
                allphs->ci2lmwid[i] =
                    ngram_wid(allphs->lmset,
                              (char *) bin_mdef_ciphone_str(mdef,
                                                            mdef_silphone
                                                            (mdef)));
        }
    }
    else {
        E_WARN
            ("-lm argument missing; doing unconstrained phone-loop decoding\n");
        allphs->inspen =
            (int32) (logmath_log
                     (acmod->lmath, cmd_ln_float32_r(config, "-pip"))
                     * allphs->lw) >> SENSCR_SHIFT;
    }

    allphs->frame = -1;
    allphs->phseg = NULL;
    /* Get search pruning parameters */
    allphs->beam_factor = 1.0f;
    allphs->beam = allphs->beam_orig
        =
        (int32) logmath_log(acmod->lmath,
                            cmd_ln_float64_r(config, "-beam"))
        >> SENSCR_SHIFT;
    allphs->pbeam = allphs->pbeam_orig
        =
        (int32) logmath_log(acmod->lmath,
                            cmd_ln_float64_r(config, "-pbeam"))
        >> SENSCR_SHIFT;

    /* LM related weights/penalties */
    allphs->lw = cmd_ln_float32_r(config, "-lw");

    allphs->frm_hist = blkarray_list_init();

    /* Acoustic score scale for posterior probabilities. */
    allphs->ascale = 1.0 / cmd_ln_float32_r(config, "-ascale");

    E_INFO("Allphone(beam: %d, pbeam: %d)\n",
           allphs->beam_orig, allphs->pbeam_orig);

    ptmr_init(&allphs->perf);

    return (ps_search_t *) allphs;
}

int
allphone_search_reinit(ps_search_t * search, dict_t * dict,
                       dict2pid_t * d2p)
{
    allphone_search_t *allphs = (allphone_search_t *) search;

    /* Free old dict2pid, dict */
    ps_search_base_reinit(search, dict, d2p);

    if (!allphs->lmset) {
        E_WARN
            ("-lm argument missing; doing unconstrained phone-loop decoding\n");
        allphs->inspen =
            (int32) (logmath_log
                     (search->acmod->lmath,
                      cmd_ln_float32_r(search->config,
                                       "-pip")) *
                     allphs->lw) >> SENSCR_SHIFT;
    }

    return 0;
}

void
allphone_search_free(ps_search_t * search)
{
    allphone_search_t *allphs = (allphone_search_t *) search;

    ps_search_deinit(search);
    hmm_context_free(allphs->hmmctx);
    phmm_free(allphs);
    if (allphs->lmset)
        ngram_model_free(allphs->lmset);
    if (allphs->ci2lmwid)
        ckd_free(allphs->ci2lmwid);

    blkarray_list_free(allphs->frm_hist);

    ckd_free(allphs);
}

int
allphone_search_start(ps_search_t * search)
{
    allphone_search_t *allphs;
    bin_mdef_t *mdef;
    s3cipid_t ci;
    phmm_t *p;

    allphs = (allphone_search_t *) search;
    mdef = search->acmod->mdef;

    /* Reset all HMMs. */
    for (ci = 0; ci < bin_mdef_n_ciphone(mdef); ci++) {
        for (p = allphs->ci_phmm[(unsigned) ci]; p; p = p->next) {
            hmm_clear(&(p->hmm));
        }
    }

    allphs->n_hmm_eval = 0;
    allphs->n_sen_eval = 0;

    /* Free history nodes, if any */
    allphs->hist_start = 0;
    blkarray_list_reset(allphs->frm_hist);

    /* Initialize start state of the SILENCE PHMM */
    allphs->frame = 0;
    ci = bin_mdef_silphone(mdef);
    if (NOT_S3CIPID(ci))
        E_FATAL("Cannot find CI-phone %s\n", S3_SILENCE_CIPHONE);
    for (p = allphs->ci_phmm[ci]; p && (p->pid != ci); p = p->next);
    if (!p)
        E_FATAL("Cannot find HMM for %s\n", S3_SILENCE_CIPHONE);
    hmm_enter(&(p->hmm), 0, 0, allphs->frame);

    ptmr_reset(&allphs->perf);
    ptmr_start(&allphs->perf);

    return 0;
}

static void
allphone_search_sen_active(allphone_search_t * allphs)
{
    acmod_t *acmod;
    bin_mdef_t *mdef;
    phmm_t *p;
    int32 ci;

    acmod = ps_search_acmod(allphs);
    mdef = acmod->mdef;

    acmod_clear_active(acmod);
    for (ci = 0; ci < bin_mdef_n_ciphone(mdef); ci++)
        for (p = allphs->ci_phmm[ci]; p; p = p->next)
            if (hmm_frame(&(p->hmm)) == allphs->frame)
                acmod_activate_hmm(acmod, &(p->hmm));
}

int
allphone_search_step(ps_search_t * search, int frame_idx)
{
    int32 bestscr;
    const int16 *senscr;
    allphone_search_t *allphs;
    acmod_t *acmod;

    allphs = (allphone_search_t *) search;
    acmod = search->acmod;

    allphs->hist_start = blkarray_list_n_valid(allphs->frm_hist);

    if (!acmod->compallsen)
        allphone_search_sen_active(allphs);
    senscr = acmod_score(search->acmod, &frame_idx);
    bestscr = phmm_eval_all(allphs, senscr);
    phmm_exit(allphs, bestscr);
    phmm_trans(allphs, bestscr);

    allphs->frame++;

    return 0;
}

static int32
ascore(allphone_search_t * allphs, history_t * h)
{
    int32 score = h->score;
    int32 sf = 0;

    if (h->hist >= 0) {
        history_t *pred = blkarray_list_get(allphs->frm_hist, h->hist);
        score -= pred->score;
        sf = pred->ef + 1;
    }

    return score - h->tscore;
}

static phseg_t *
allphone_backtrace(allphone_search_t * allphs, int32 f)
{
    int32 best, hist_idx, best_idx;
    int32 frm, last_frm;
    history_t *h;
    phseg_t *phseg, *s;

    /* Find the first history entry for the requested frame */
    hist_idx = blkarray_list_n_valid(allphs->frm_hist) - 1;
    while (hist_idx > 0) {
        h = blkarray_list_get(allphs->frm_hist, hist_idx);
        if (h->ef <= f) {
            frm = last_frm = h->ef;
            break;
        }
    }

    if (hist_idx < 0)
        return NULL;

    /* Find bestscore */
    best = (int32) 0x80000000;
    best_idx = -1;
    while (frm == last_frm && hist_idx > 0) {
        h = blkarray_list_get(allphs->frm_hist, hist_idx);
        frm = h->ef;
        if (h->score > best && frm == last_frm) {
            best = h->score;
            best_idx = hist_idx;
        }
        hist_idx--;
    }

    if (best_idx < 0)
        return NULL;

    /* Keep track of the end node for use in DAG generation. */
    phseg = NULL;

    /* Backtrace */
    while (best_idx > 0) {
        h = blkarray_list_get(allphs->frm_hist, best_idx);
        s = (phseg_t *) ckd_calloc(1, sizeof(phseg_t));
        s->ci = h->phmm->ci;
        s->sf =
            (h->hist >=
             0) ? ((history_t *) blkarray_list_get(allphs->frm_hist,
                                                   h->hist))->ef + 1 : 0;
        s->ef = h->ef;
        s->score = ascore(allphs, h);
        s->tscore = h->tscore;
        s->next = phseg;
        phseg = s;

        best_idx = h->hist;
    }

    E_INFO("%10d history nodes created\n",
           blkarray_list_n_valid(allphs->frm_hist));
    return phseg;
}

static void
allphone_clear_phseg(allphone_search_t * allphs)
{
    phseg_t *s, *nexts;
    for (s = allphs->phseg; s; s = nexts) {
        nexts = s->next;
        ckd_free((char *) s);
    }
    allphs->phseg = NULL;
}

int
allphone_search_finish(ps_search_t * search)
{
    allphone_search_t *allphs;
    int32 cf;

    allphs = (allphone_search_t *) search;

    /* Free old phseg, if any */
    allphone_clear_phseg(allphs);

    /* Now backtrace. */
    allphs->phseg = allphone_backtrace(allphs, allphs->frame - 1);

    /* Print out some statistics. */
    ptmr_stop(&allphs->perf);
    /* This is the number of frames processed. */
    cf = ps_search_acmod(allphs)->output_frame;
    if (cf > 0) {
        double n_speech = (double) (cf + 1)
            / cmd_ln_int32_r(ps_search_config(allphs), "-frate");
        E_INFO("allphone %.2f CPU %.3f xRT\n",
               allphs->perf.t_cpu, allphs->perf.t_cpu / n_speech);
        E_INFO("allphone %.2f wall %.3f xRT\n",
               allphs->perf.t_elapsed, allphs->perf.t_elapsed / n_speech);
    }


    return 0;
}

char const *
allphone_search_hyp(ps_search_t * search, int32 * out_score,
                    int32 * out_is_final)
{
    allphone_search_t *allphs;
    phseg_t *p;
    bin_mdef_t *mdef;
    int32 f;
    int len, hyp_idx, phone_idx;

    allphs = (allphone_search_t *) search;
    mdef = search->acmod->mdef;
    allphone_clear_phseg(allphs);

    for (f = allphs->frame - 1;
         (f >= 0) && (blkarray_list_get(allphs->frm_hist, f) == NULL);
         --f);

    allphs->phseg = allphone_backtrace(allphs, f);

    if (allphs->phseg == NULL) {
        return NULL;
    }

    /* Create hypothesis */
    if (search->hyp_str)
        ckd_free(search->hyp_str);
    len = 0;
    for (p = allphs->phseg; p; p = p->next)
        len += 10; // maximum length of one phone with spacebar
    search->hyp_str = (char *) ckd_calloc(len, sizeof(*search->hyp_str));
    hyp_idx = 0;
    for (p = allphs->phseg; p; p = p->next) {
        char *phone_str = (char *) bin_mdef_ciphone_str(mdef, p->ci);
        phone_idx = 0;
        while (phone_str[phone_idx] != '\0')
            search->hyp_str[hyp_idx++] = phone_str[phone_idx++];
        search->hyp_str[hyp_idx++] = ' ';
    }
    search->hyp_str[--hyp_idx] = '\0';
    E_INFO("Hyp: %s\n", search->hyp_str);
    return search->hyp_str;
}
