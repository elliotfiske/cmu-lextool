/* -*- c-basic-offset: 4; indent-tabs-mode: nil -*- */
/* ====================================================================
 * Copyright (c) 2013 Carnegie Mellon University.  All rights
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
 * kws_search.c -- Search structures for key word spotting.
 * 
 * **********************************************
 * CMU ARPA Speech Project
 *
 * Copyright (c) 2013 Carnegie Mellon University.
 * ALL RIGHTS RESERVED.
 * **********************************************
 * 
 * HISTORY
 *
 * 13-Dec-2013
 * 		Started.
 */

/* System headers. */
#include <stdio.h>
#include <string.h>
#include <assert.h>

/* SphinxBase headers. */
#include <sphinxbase/err.h>
#include <sphinxbase/ckd_alloc.h>
#include <sphinxbase/strfuncs.h>
#include <sphinxbase/cmd_ln.h>

/* Local headers. */
#include "pocketsphinx_internal.h"
#include "kws_search.h"

/* Turn this on for detailed debugging dump */
#define __KWS_DBG__		0

/* Cap functions to meet ps_search api */
static ps_seg_t* kws_search_seg_iter(ps_search_t *search, int32 *out_score)
{
    *out_score = 0;
    return NULL;
}

static ps_lattice_t* kws_search_lattice(ps_search_t *search)
{
    return NULL;
}

static int kws_search_prob(ps_search_t *search)
{
    return 0;
}

static ps_searchfuncs_t kws_funcs = {
    /* name: */   "kws",
    /* start: */  kws_search_start,
    /* step: */   kws_search_step,
    /* finish: */ kws_search_finish,
    /* reinit: */ kws_search_reinit,
    /* free: */   kws_search_free,
    /* lattice: */  kws_search_lattice,
    /* hyp: */      kws_search_hyp,
    /* prob: */     kws_search_prob,
    /* seg_iter: */ kws_search_seg_iter,
};

/* Scans the dictionary and check if all words are present. */
static int
kws_search_check_dict(kws_search_t *kwss)
{
    dict_t *dict;
    char** wrdptr;
    int32 nwrds, wid;
    int i;

    dict = ps_search_dict(kwss);
    nwrds = str2words(kwss->key_phrase, NULL, 0);
    wrdptr = (char**)ckd_calloc(nwrds, sizeof(*wrdptr));
    str2words(kwss->key_phrase, wrdptr, nwrds);
    for (i = 0; i<nwrds; i++) {
        wid = dict_wordid(dict, wrdptr[i]);
        if (wid == BAD_S3WID) {
            E_ERROR("The word '%s' is missing in the dictionary\n", wrdptr[i]);
            ckd_free(wrdptr);
            return FALSE;
        }
    }
    ckd_free(wrdptr);
    return TRUE;
}

/* TODO activate senones for every node in search space */
static void
kws_search_sen_active(kws_search_t *kwss)
{
    int i;

    acmod_clear_active(ps_search_acmod(kwss));
    for (i=0; i<kwss->n_ci; i++) {
        hmm_t *hmm = &kwss->ci_loop_hmms[i];
        acmod_activate_hmm(ps_search_acmod(kwss), hmm);
    }

}

/* Inits hmms for phone and filler loops */
static int
kws_search_init_bckgrnd(kws_search_t *kwss)
{
    dict_t *dict;
    bin_mdef_t *mdef;
    int i;

    dict = ps_search_dict(kwss);
    mdef = ps_search_acmod(kwss)->mdef;
    
    /* init hmms for ci phone loop */
    kwss->n_ci = mdef->n_ciphone;
    kwss->ci_loop_hmms = (hmm_t*)ckd_calloc(kwss->n_ci, sizeof(hmm_t));
    for (i=0; i<mdef->n_ciphone; i++) {
        int ci, tmatid;
        ci = bin_mdef_ciphone_id(mdef, mdef->ciname[i]);
        tmatid = bin_mdef_pid2tmatid(mdef, ci);
        hmm_init(kwss->hmmctx, &kwss->ci_loop_hmms[i], FALSE, ci, tmatid);
    }
    return 0;
}

/* Perform phone loop transitions. 
   TODO insert phone loop probability here to move ROC */
static void
kws_search_bckgrnd_trans(kws_search_t *kwss)
{
    int i;
    for (i=0; i<kwss->n_ci; i++) {
        hmm_t *hmm = &kwss->ci_loop_hmms[i];
        hmm_enter(hmm, kwss->nodes->score, 0, kwss->frame+1);
    }
}

/*
 * Evaluate all the active HMMs.
 * (Executed once per frame.)
 */
static void
kws_search_hmm_eval(kws_search_t *kwss)
{
    int32 bestscore;
    int32 i, maxhmmpf;

    bestscore = WORST_SCORE;

    for (i=0; i<kwss->n_ci; i++) {
        int32 score;
        hmm_t *hmm = &kwss->ci_loop_hmms[i];

#if __KWS_DBG__
        E_INFO("hmm(%08x) active @frm %5d\n", (int32) hmm,
               kwss->frame);
        hmm_dump(hmm, stdout);
#endif
        score = hmm_vit_eval(hmm);
#if __KWS_DBG__
        E_INFO("hmm(%08x) after eval @frm %5d\n",
               (int32) hmm, kwss->frame);
        hmm_dump(hmm, stdout);
#endif

        if (score BETTER_THAN bestscore) {
            bestscore = score;
            kwss->nodes->hmm = hmm;
            kwss->nodes->score = score;
        }
    }
#if __KWS_DBG__
    E_INFO("[%5d] bestscr: %11d\n", kwss->frame, bestscore);
#endif

    /* Adjust beams if #active HMMs larger than absolute threshold */
    maxhmmpf = cmd_ln_int32_r(ps_search_config(kwss), "-maxhmmpf");
    if (maxhmmpf != -1 && i > maxhmmpf) {
        /*
         * Too many HMMs active; reduce the beam factor applied to the default
         * beams, but not if the factor is already at a floor (0.1).
         */
        if (kwss->beam_factor > 0.1) {        /* Hack!!  Hardwired constant 0.1 */
            kwss->beam_factor *= 0.9f;        /* Hack!!  Hardwired constant 0.9 */
            kwss->beam =
                (int32) (kwss->beam_orig * kwss->beam_factor);
            kwss->pbeam =
                (int32) (kwss->pbeam_orig * kwss->beam_factor);
            kwss->wbeam =
                (int32) (kwss->wbeam_orig * kwss->beam_factor);
        }
    }
    else {
        kwss->beam_factor = 1.0f;
        kwss->beam = kwss->beam_orig;
        kwss->pbeam = kwss->pbeam_orig;
        kwss->wbeam = kwss->wbeam_orig;
    }

    kwss->bestscore = bestscore;
}

ps_search_t *kws_search_init(const char* key_phrase,
                             cmd_ln_t *config,
                             acmod_t *acmod,
                             dict_t *dict,
                             dict2pid_t *d2p)
{
    kws_search_t *kwss = (kws_search_t*)ckd_calloc(1, sizeof(*kwss));
    ps_search_init(ps_search_base(kwss), &kws_funcs, config, acmod, dict, d2p);

    /* Initialize HMM context. */
    kwss->hmmctx = hmm_context_init(bin_mdef_n_emit_state(acmod->mdef),
                                    acmod->tmat->tp, NULL, acmod->mdef->sseq);
    if (kwss->hmmctx == NULL) {
        ps_search_free(ps_search_base(kwss));
        return NULL;
    }
    
    /* Get search pruning parameters */
    kwss->beam_factor = 1.0f;
    kwss->beam = kwss->beam_orig
        = (int32) logmath_log(acmod->lmath, cmd_ln_float64_r(config, "-beam"))
        >> SENSCR_SHIFT;
    kwss->pbeam = kwss->pbeam_orig
        = (int32) logmath_log(acmod->lmath, cmd_ln_float64_r(config, "-pbeam"))
        >> SENSCR_SHIFT;
    kwss->wbeam = kwss->wbeam_orig
        = (int32) logmath_log(acmod->lmath, cmd_ln_float64_r(config, "-wbeam"))
        >> SENSCR_SHIFT;

    /* LM related weights/penalties */
    kwss->lw = cmd_ln_float32_r(config, "-lw");
    kwss->pip = (int32) (logmath_log(acmod->lmath, cmd_ln_float32_r(config, "-pip"))
                           * kwss->lw)
        >> SENSCR_SHIFT;
    kwss->wip = (int32) (logmath_log(acmod->lmath, cmd_ln_float32_r(config, "-wip"))
                           * kwss->lw)
        >> SENSCR_SHIFT;

    /* Acoustic score scale for posterior probabilities. */
    kwss->ascale = 1.0f / cmd_ln_float32_r(config, "-ascale");

    E_INFO("KWS(beam: %d, pbeam: %d, wbeam: %d; wip: %d, pip: %d)\n",
           kwss->beam_orig, kwss->pbeam_orig, kwss->wbeam_orig,
           kwss->wip, kwss->pip);

    kwss->key_phrase = ckd_salloc(key_phrase);

    /* check if all words are in dictionary */
    if (!kws_search_check_dict(kwss)) {
        kws_search_free(ps_search_base(kwss));
        return NULL;
    }

    /* reinit for provided keyword */
    if (kws_search_reinit(ps_search_base(kwss),
                          ps_search_dict(kwss),
                          ps_search_dict2pid(kwss)) < 0) {
        ps_search_free(ps_search_base(kwss));
        return NULL;
    }
        
    /* init phone loop phones list */
    kws_search_init_bckgrnd(kwss);

    return ps_search_base(kwss);
}

void kws_search_free(ps_search_t *search)
{
    kws_search_t *kwss = (kws_search_t *)search;

    ps_search_deinit(search);
    hmm_context_free(kwss->hmmctx);

    /* free background hmms */
    ckd_free(kwss->ci_loop_hmms);
    ckd_free(kwss->nodes);
    ckd_free(kwss);
}

int kws_search_reinit(ps_search_t *search, dict_t *dict, dict2pid_t *d2p)
{
    kws_search_t *kwss = (kws_search_t *)search;

    /* Free old dict2pid, dict */
    ps_search_base_reinit(search, dict, d2p);

    kwss->nodes = (kws_node_t*)ckd_calloc(1, sizeof(kws_node_t));
    return 0;
}

int kws_search_start(ps_search_t *search)
{
    kws_search_t *kwss = (kws_search_t *)search;

    /* Reset dynamic adjustment factor for beams */
    kwss->beam_factor = 1.0f;
    kwss->beam = kwss->beam_orig;
    kwss->pbeam = kwss->pbeam_orig;
    kwss->wbeam = kwss->wbeam_orig;

    kwss->frame = -1;
    kwss->nodes->active = 1;
    kwss->nodes->score = 0;

    kws_search_bckgrnd_trans(kwss);
    kwss->frame++;
    kwss->bestscore = 0;
	return 0;
}

int kws_search_step(ps_search_t *search, int frame_idx)
{
    int16 const *senscr;
    kws_search_t *kwss = (kws_search_t *)search;
    acmod_t *acmod = search->acmod;

    if (!acmod->compallsen)
        kws_search_sen_active(kwss);

    senscr = acmod_score(acmod, &frame_idx);
    hmm_context_set_senscore(kwss->hmmctx, senscr);

    /* Evaluate all active pnodes (HMMs) */
    kws_search_hmm_eval(kwss);
    
    /* enter all active hmms for next evaluation */
    kws_search_bckgrnd_trans(kwss);

    ++kwss->frame;
    return 0;
}

int kws_search_finish(ps_search_t *search)
{
    return 0;
}

char const *kws_search_hyp(ps_search_t *search, int32 *out_score, int32 *out_is_final)
{
    return NULL;
}