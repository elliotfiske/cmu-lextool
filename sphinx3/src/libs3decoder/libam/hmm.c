/* -*- c-basic-offset: 4; indent-tabs-mode: nil -*- */
/* ====================================================================
 * Copyright (c) 1999-2004 Carnegie Mellon University.  All rights
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
/*
 * hmm.c -- HMM Viterbi search.
 *
 * **********************************************
 * CMU ARPA Speech Project
 *
 * Copyright (c) 1997 Carnegie Mellon University.
 * ALL RIGHTS RESERVED.
 * **********************************************
 *
 * HISTORY
 * $Log$
 * Revision 1.6  2006/02/22  16:46:38  arthchan2003
 * Merged from SPHINX3_5_2_RCI_IRII_BRANCH: 1, Added function hmm_vit_eval, a wrapper of computing the hmm level scores. 2, Fixed issues in , 3, Fixed issues of dox-doc
 * 
 * Revision 1.5.4.1  2005/09/25 18:53:36  arthchan2003
 * Added hmm_vit_eval, in lextree.c, hmm_dump and hmm_vit_eval is now separated.
 *
 * Revision 1.5  2005/06/21 18:34:41  arthchan2003
 * Log. 1, Fixed doxygen documentation for all functions. 2, Add $Log$
 * Revision 1.6  2006/02/22  16:46:38  arthchan2003
 * Merged from SPHINX3_5_2_RCI_IRII_BRANCH: 1, Added function hmm_vit_eval, a wrapper of computing the hmm level scores. 2, Fixed issues in , 3, Fixed issues of dox-doc
 * 
 *
 * Revision 1.3  2005/03/30 01:22:46  archan
 * Fixed mistakes in last updates. Add
 *
 * 
 * 30-Dec-2000  Rita Singh (rsingh@cs.cmu.edu) at Carnegie Mellon University
 *		Modified hmm_vit_eval_3st() to include explicit checks for
 *		tr[0][2] and tr[1][3]. Included compiler directive activated
 *		checks for int32 underflow
 *
 * 29-Feb-2000	M K Ravishankar (rkm@cs.cmu.edu) at Carnegie Mellon University
 * 		Modified hmm_t.state to be a run-time array instead of a compile-time
 * 		one.  Modified compile-time 3 and 5-state versions of hmm_vit_eval
 * 		into hmm_vit_eval_3st and hmm_vit_eval_5st, to allow run-time selection.
 * 		Removed hmm_init().
 * 
 * 11-Dec-1999	M K Ravishankar (rkm@cs.cmu.edu) at Carnegie Mellon University.
 * 		Bugfix in computing HMM exit state score.
 * 
 * 08-Dec-1999	M K Ravishankar (rkm@cs.cmu.edu) at Carnegie Mellon University.
 * 		Added HMM_SKIPARCS compile-time option and hmm_init().
 * 
 * 20-Sep-1999	M K Ravishankar (rkm@cs.cmu.edu) at Carnegie Mellon University.
 * 		Bugfix in hmm_eval: If state1->state2 transition took place,
 * 		state1 history didn't get propagated to state2.
 * 		Also, included tp[][] in HMM evaluation.
 * 
 * 10-May-1999	M K Ravishankar (rkm@cs.cmu.edu) at Carnegie Mellon University.
 * 		Started, based on an earlier version.
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "hmm.h"
#include "ckd_alloc.h"
#include "s3types.h"

#define HMM_BLOCK_SIZE 1000
#define WORST_SCORE S3_LOGPROB_ZERO

hmm_context_t *
hmm_context_init(int32 n_emit_state, int32 mpx,
		 int32 ***tp,
		 int32 *senscore,
		 s3senid_t **sseq)
{
    hmm_context_t *ctx;

    assert(n_emit_state > 0);
    assert(tp != NULL);
    /* Multiplex HMMs all have their own senone sequences */
    assert(mpx || (sseq != NULL));

    ctx = ckd_calloc(1, sizeof(*ctx));
    ctx->n_emit_state = n_emit_state;
    ctx->mpx = mpx;
    ctx->tp = (const int32 ***)tp;
    ctx->senscore = senscore;
    ctx->sseq = (const s3senid_t **)sseq;
    ctx->st_sen_scr = ckd_calloc(n_emit_state, sizeof(*ctx->st_sen_scr));

    return ctx;
}

void
hmm_context_free(hmm_context_t *ctx)
{
    ckd_free(ctx);
}

void
hmm_init(hmm_context_t *ctx, hmm_t *hmm, int32 ssid, s3tmatid_t tmatid)
{
    hmm->ctx = ctx;
    hmm->state = ckd_calloc(hmm_n_emit_state(hmm), sizeof(hmm_state_t));
    if (ctx->mpx) {
        hmm->s.mpx_ssid = ckd_calloc(hmm_n_emit_state(hmm), sizeof(*hmm->s.mpx_ssid));
        memset(hmm->s.mpx_ssid, -1, sizeof(*hmm->s.mpx_ssid) * hmm_n_emit_state(hmm));
        hmm->s.mpx_ssid[0] = ssid;
        hmm->t.mpx_tmatid = ckd_calloc(hmm_n_emit_state(hmm), sizeof(*hmm->t.mpx_tmatid));
        memset(hmm->t.mpx_tmatid, -1, sizeof(*hmm->t.mpx_tmatid) * hmm_n_emit_state(hmm));
        hmm->t.mpx_tmatid[0] = tmatid;
    }
    else {
        hmm->s.ssid = ssid;
        hmm->t.tmatid = tmatid;
    }
    hmm_clear(hmm);
}

void
hmm_deinit(hmm_t *hmm)
{
    ckd_free(hmm->state);
    if (hmm->ctx->mpx) {
        ckd_free(hmm->s.mpx_ssid);
        ckd_free(hmm->t.mpx_tmatid);
    }
}

void
hmm_dump(hmm_t * hmm,
         FILE * fp)
{
    int32 i;

    if (hmm->ctx->mpx) {
        fprintf(fp, "MPX    %11s    ", "");
        for (i = 0; i < hmm_n_emit_state(hmm); i++)
            fprintf(fp, " %11d", hmm_senid(hmm, i));
        fprintf(fp, "\n");
    }
    else {
        fprintf(fp, "SSID   %11d    ", hmm_ssid(hmm, 0));
        for (i = 0; i < hmm_n_emit_state(hmm); i++)
            fprintf(fp, " %11d", hmm_senid(hmm, i));
        fprintf(fp, "\n");
    }

    if (hmm->ctx->senscore) {
        fprintf(fp, "SENSCR %-11s    ", "");
        for (i = 0; i < hmm_n_emit_state(hmm); i++)
            fprintf(fp, " %11d", hmm_senscr(hmm, i));
        fprintf(fp, "\n");
    }

    fprintf(fp, "SCORES %11d    ", hmm_in_score(hmm));
    for (i = 0; i < hmm_n_emit_state(hmm); i++)
        fprintf(fp, " %11d", hmm_score(hmm, i));
    fprintf(fp, " %11d", hmm_out_score(hmm));
    fprintf(fp, "\n");

    fprintf(fp, "HISTID %11d    ", hmm_in_history(hmm));
    for (i = 0; i < hmm_n_emit_state(hmm); i++)
        fprintf(fp, " %11d", hmm_history(hmm, i));
    fprintf(fp, " %11d", hmm_out_history(hmm));
    fprintf(fp, "\n");

    fprintf(fp, "TMATID %11s    ", "");
    for (i = 0; i < hmm_n_emit_state(hmm); i++)
        fprintf(fp, " %11d", hmm_tmatid(hmm, i));
    fprintf(fp, "\n");

    if (hmm_in_score(hmm) > 0)
        fprintf(fp,
                "ALERT!! The input score %d is large than 0. Probably wrap around.\n",
                hmm_in_score(hmm));
    if (hmm_out_score(hmm) > 0)
        fprintf(fp,
                "ALERT!! The output score %d is large than 0. Probably wrap around\n.",
                hmm_out_score(hmm));

    fflush(fp);
}


void
hmm_clear_scores(hmm_t * h)
{
    int32 i;

    for (i = 0; i < hmm_n_emit_state(h); i++)
        hmm_score(h, i) = WORST_SCORE;
    hmm_in_score(h) = WORST_SCORE;
    hmm_out_score(h) = WORST_SCORE;

    h->bestscore = WORST_SCORE;
}

void
hmm_clear(hmm_t * h)
{
    int32 i;

    for (i = 0; i < hmm_n_emit_state(h); i++) {
        hmm_score(h, i) = WORST_SCORE;
        hmm_history(h, i) = -1;
    }
    hmm_in_score(h) = WORST_SCORE;
    hmm_in_history(h) = -1;
    hmm_out_score(h) = WORST_SCORE;
    hmm_out_history(h) = -1;

    h->bestscore = WORST_SCORE;
    h->frame = -1;
}

void
hmm_enter(hmm_t *h, int32 score, int32 histid, int32 frame)
{
    hmm_in_score(h) = score;
    hmm_in_history(h) = histid;
    hmm_frame(h) = frame;
}

void
hmm_normalize(hmm_t *h, int32 bestscr)
{
    int32 i;

    for (i = 0; i < hmm_n_emit_state(h); i++) {
        if (hmm_score(h, i) > WORST_SCORE)
            hmm_score(h, i) -= bestscr;
    }
    if (hmm_in_score(h) > WORST_SCORE)
        hmm_in_score(h) -= bestscr;
    if (hmm_out_score(h) > WORST_SCORE)
        hmm_out_score(h) -= bestscr;
}

#define hmm_tprob_5st(i, j) (tp[(i)*6+(j)])
#define nonmpx_senscr(i) (senscore[sseq[i]])

static int32
hmm_vit_eval_5st_lr(hmm_t * hmm)
{
    const int32 *senscore = hmm->ctx->senscore;
    const int32 *tp = hmm->ctx->tp[hmm->t.tmatid][0];
    const s3senid_t *sseq = hmm->ctx->sseq[hmm_ssid(hmm, 0)];
    int32 s5, s4, s3, s2, s1, s0, t2, t1, t0, bestScore;

    /* Calculate "from" scores.  Note that we emit before transition
     * here, which is equivalent to but not the same as the usual
     * Viterbi algorithm. */
    s0 = hmm_score(hmm, 0) + nonmpx_senscr(0);
    s1 = hmm_score(hmm, 1) + nonmpx_senscr(1);
    s2 = hmm_score(hmm, 2) + nonmpx_senscr(2);
    s3 = hmm_score(hmm, 3) + nonmpx_senscr(3);
    s4 = hmm_score(hmm, 4) + nonmpx_senscr(4);

    /* Transition from non-emitting initial state */
    t0 = hmm_in_score(hmm) + nonmpx_senscr(0);
    if (t0 > s0) {
        s0 = t0;
        hmm_history(hmm, 0) = hmm_in_history(hmm);
    }
    hmm_in_score(hmm) = WORST_SCORE; /* consumed */

    /* It was the best of scores, it was the worst of scores. */
    bestScore = WORST_SCORE;

    /* Transitions into non-emitting state 5 */
    if (s3 > WORST_SCORE) {
        t1 = s4 + hmm_tprob_5st(4, 5);
        t2 = s3 + hmm_tprob_5st(3, 5);
        if (t1 > t2) {
            s5 = t1;
            hmm_out_history(hmm)  = hmm_history(hmm, 4);
        } else {
            s5 = t2;
            hmm_out_history(hmm)  = hmm_history(hmm, 3);
        }
        hmm_out_score(hmm) = s5;
        bestScore = s5;
    }

    /* All transitions into state 4 */
    if (s2 > WORST_SCORE) {
        t0 = s4 + hmm_tprob_5st(4, 4);
        t1 = s3 + hmm_tprob_5st(3, 4);
        t2 = s2 + hmm_tprob_5st(2, 4);
        if (t0 > t1) {
            if (t2 > t0) {
                s4 = t2;
                hmm_history(hmm, 4)  = hmm_history(hmm, 2);
            } else
                s4 = t0;
        } else {
            if (t2 > t1) {
                s4 = t2;
                hmm_history(hmm, 4)  = hmm_history(hmm, 2);
            } else {
                s4 = t1;
                hmm_history(hmm, 4)  = hmm_history(hmm, 3);
            }
        }
        if (s4 > bestScore) bestScore = s4;
        hmm_score(hmm, 4) = s4;
    }

    /* All transitions into state 3 */
    if (s1 > WORST_SCORE) {
        t0 = s3 + hmm_tprob_5st(3, 3);
        t1 = s2 + hmm_tprob_5st(2, 3);
        t2 = s1 + hmm_tprob_5st(1, 3);
        if (t0 > t1) {
            if (t2 > t0) {
                s3 = t2;
                hmm_history(hmm, 3)  = hmm_history(hmm, 1);
            } else
                s3 = t0;
        } else {
            if (t2 > t1) {
                s3 = t2;
                hmm_history(hmm, 3)  = hmm_history(hmm, 1);
            } else {
                s3 = t1;
                hmm_history(hmm, 3)  = hmm_history(hmm, 2);
            }
        }
        if (s3 > bestScore) bestScore = s3;
        hmm_score(hmm, 3) = s3;
    }

    /* All transitions into state 2 */
    if (s0 > WORST_SCORE) {
        t0 = s2 + hmm_tprob_5st(2, 2);
        t1 = s1 + hmm_tprob_5st(1, 2);
        t2 = s0 + hmm_tprob_5st(0, 2);
        if (t0 > t1) {
            if (t2 > t0) {
                s2 = t2;
                hmm_history(hmm, 2)  = hmm_history(hmm, 0);
            } else
                s2 = t0;
        } else {
            if (t2 > t1) {
                s2 = t2;
                hmm_history(hmm, 2)  = hmm_history(hmm, 0);
            } else {
                s2 = t1;
                hmm_history(hmm, 2)  = hmm_history(hmm, 1);
            }
        }
        if (s2 > bestScore) bestScore = s2;
        hmm_score(hmm, 2) = s2;
    }


    /* All transitions into state 1 */
    if (s0 > WORST_SCORE) {
        t0 = s1 + hmm_tprob_5st(1, 1);
        t1 = s0 + hmm_tprob_5st(0, 1);
        if (t0 > t1) {
            s1 = t0;
        } else {
            s1 = t1;
            hmm_history(hmm, 1)  = hmm_history(hmm, 0);
        }
        if (s1 > bestScore) bestScore = s1;
        hmm_score(hmm, 1) = s1;
    }

    /* All transitions into state 0 */
    if (s0 > WORST_SCORE) {
        s0 = s0 + hmm_tprob_5st(0, 0);
        if (s0 > bestScore) bestScore = s0;
        hmm_score(hmm, 0) = s0;
    }

    hmm_bestscore(hmm) = bestScore;
    return bestScore;
}

#define mpx_senid(st) sseq[ssid[st]][st]
#define mpx_senscr(st) senscore[mpx_senid(st)]
#define mpx_tprob(i,j) tp[tmatid[i]][i][j]

static int32
hmm_vit_eval_5st_lr_mpx(hmm_t * hmm)
{
    const int32 ***tp = hmm->ctx->tp;
    const int32 *senscore = hmm->ctx->senscore;
    const s3senid_t **sseq = hmm->ctx->sseq;
    int32 *ssid = hmm->s.mpx_ssid;
    s3tmatid_t *tmatid = hmm->t.mpx_tmatid;
    int32 bestScore;
    int32 s5, s4, s3, s2, s1, s0, t2, t1, t0;

    /* Don't propagate WORST_SCORE */
    if (ssid[4] == -1)
        s4 = t1 = WORST_SCORE;
    else {
        s4 = hmm_score(hmm, 4) + mpx_senscr(4);
        t1 = s4 + mpx_tprob(4, 5);
    }
    if (ssid[3] == -1)
        s3 = t2 = WORST_SCORE;
    else {
        s3 = hmm_score(hmm, 3) + mpx_senscr(3);
        t2 = s3 + mpx_tprob(3, 5);
    }
    if (t1 > t2) {
        s5 = t1;
        hmm_out_history(hmm) = hmm_history(hmm, 4);
    }
    else {
        s5 = t2;
        hmm_out_history(hmm) = hmm_history(hmm, 3);
    }
    hmm_out_score(hmm) = s5;
    bestScore = s5;

    /* Don't propagate WORST_SCORE */
    if (ssid[2] == -1)
        s2 = t2 = WORST_SCORE;
    else {
        s2 = hmm_score(hmm, 2) + mpx_senscr(2);
        t2 = s2 + mpx_tprob(2, 4);
    }

    t0 = t1 = WORST_SCORE;
    if (s4 != WORST_SCORE)
        t0 = s4 + mpx_tprob(4, 4);
    if (s3 != WORST_SCORE)
        t1 = s3 + mpx_tprob(3, 4);
    if (t0 > t1) {
        if (t2 > t0) {
            s4 = t2;
            hmm_history(hmm, 4) = hmm_history(hmm, 2);
            ssid[4] = ssid[2];
            tmatid[4] = tmatid[2];
        }
        else
            s4 = t0;
    }
    else {
        if (t2 > t1) {
            s4 = t2;
            hmm_history(hmm, 4) = hmm_history(hmm, 2);
            ssid[4] = ssid[2];
            tmatid[4] = tmatid[2];
        }
        else {
            s4 = t1;
            hmm_history(hmm, 4) = hmm_history(hmm, 3);
            ssid[4] = ssid[3];
            tmatid[4] = tmatid[3];
        }
    }
    if (s4 > bestScore)
        bestScore = s4;
    hmm_score(hmm, 4) = s4;

    /* Don't propagate WORST_SCORE */
    if (ssid[1] == -1)
        s1 = t2 = WORST_SCORE;
    else {
        s1 = hmm_score(hmm, 1) + mpx_senscr(1);
        t2 = s1 + mpx_tprob(1, 3);
    }
    t0 = t1 = WORST_SCORE;
    if (s3 != WORST_SCORE)
        t0 = s3 + mpx_tprob(3, 3);
    if (s2 != WORST_SCORE)
        t1 = s2 + mpx_tprob(2, 3);
    if (t0 > t1) {
        if (t2 > t0) {
            s3 = t2;
            hmm_history(hmm, 3) = hmm_history(hmm, 1);
            ssid[3] = ssid[1];
            tmatid[3] = tmatid[1];
        }
        else
            s3 = t0;
    }
    else {
        if (t2 > t1) {
            s3 = t2;
            hmm_history(hmm, 3) = hmm_history(hmm, 1);
            ssid[3] = ssid[1];
            tmatid[3] = tmatid[1];
        }
        else {
            s3 = t1;
            hmm_history(hmm, 3) = hmm_history(hmm, 2);
            ssid[3] = ssid[2];
            tmatid[3] = tmatid[2];
        }
    }
    if (s3 > bestScore)
        bestScore = s3;
    hmm_score(hmm, 3) = s3;

    /* Handle transition from non-emitting initial state */
    s0 = hmm_score(hmm, 0) + mpx_senscr(0);
    if (hmm_in_score(hmm) + mpx_senscr(0) > s0) {
        s0 = hmm_in_score(hmm) + mpx_senscr(0);
        hmm_history(hmm, 0) = hmm_in_history(hmm);
    }
    hmm_in_score(hmm) = WORST_SCORE; /* consumed */

    /* Don't propagate WORST_SCORE */
    t0 = t1 = WORST_SCORE;
    if (s2 != WORST_SCORE)
        t0 = s2 + mpx_tprob(2, 2);
    if (s1 != WORST_SCORE)
        t1 = s1 + mpx_tprob(1, 2);
    t2 = s0 + mpx_tprob(0, 2);
    if (t0 > t1) {
        if (t2 > t0) {
            s2 = t2;
            hmm_history(hmm, 2) = hmm_history(hmm, 0);
            ssid[2] = ssid[0];
            tmatid[2] = tmatid[0];
        }
        else
            s2 = t0;
    }
    else {
        if (t2 > t1) {
            s2 = t2;
            hmm_history(hmm, 2) = hmm_history(hmm, 0);
            ssid[2] = ssid[0];
            tmatid[2] = tmatid[0];
        }
        else {
            s2 = t1;
            hmm_history(hmm, 2) = hmm_history(hmm, 1);
            ssid[2] = ssid[1];
            tmatid[2] = tmatid[1];
        }
    }
    if (s2 > bestScore)
        bestScore = s2;
    hmm_score(hmm, 2) = s2;

    /* Don't propagate WORST_SCORE */
    t0 = WORST_SCORE;
    if (s1 != WORST_SCORE)
        t0 = s1 + mpx_tprob(1, 1);
    t1 = s0 + mpx_tprob(0, 1);
    if (t0 > t1) {
        s1 = t0;
    }
    else {
        s1 = t1;
        hmm_history(hmm, 1) = hmm_history(hmm, 0);
        ssid[1] = ssid[0];
        tmatid[1] = tmatid[0];
    }
    if (s1 > bestScore)
        bestScore = s1;
    hmm_score(hmm, 1) = s1;

    s0 = s0 + mpx_tprob(0, 0);
    if (s0 > bestScore)
        bestScore = s0;
    hmm_score(hmm, 0) = s0;

    hmm_bestscore(hmm) = bestScore;
    return bestScore;
}

#define hmm_tprob_3st(i, j) (tp[(i)*4+(j)])

static int32
hmm_vit_eval_3st_lr(hmm_t * hmm)
{
    const int32 *senscore = hmm->ctx->senscore;
    const int32 *tp = hmm->ctx->tp[hmm->t.tmatid][0];
    const s3senid_t *sseq = hmm->ctx->sseq[hmm_ssid(hmm, 0)];
    int32 s3, s2, s1, s0, t2, t1, t0, bestScore;

    s2 = hmm_score(hmm, 2) + nonmpx_senscr(2);
    s1 = hmm_score(hmm, 1) + nonmpx_senscr(1);
    s0 = hmm_score(hmm, 0) + nonmpx_senscr(0);

    /* Transition from non-emitting initial state */
    if (hmm_in_score(hmm) + nonmpx_senscr(0) > s0) {
        s0 = hmm_in_score(hmm) + nonmpx_senscr(0);
        hmm_history(hmm, 0) = hmm_in_history(hmm);
    }
    hmm_in_score(hmm) = WORST_SCORE; /* consumed */

    /* It was the best of scores, it was the worst of scores. */
    bestScore = WORST_SCORE;

    /* Transitions into non-emitting state 3 */
    if (s1 > WORST_SCORE) {
        t1 = s2 + hmm_tprob_3st(2, 3);
        t2 = s1 + hmm_tprob_3st(1, 3);
        if (t1 > t2) {
            s3 = t1;
            hmm_out_history(hmm)  = hmm_history(hmm, 2);
        } else {
            s3 = t2;
            hmm_out_history(hmm)  = hmm_history(hmm, 1);
        }
        hmm_out_score(hmm) = s3;
        bestScore = s3;
    }

    /* All transitions into state 2 */
    if (s0 > WORST_SCORE) {
        t0 = s2 + hmm_tprob_3st(2, 2);
        t1 = s1 + hmm_tprob_3st(1, 2);
        t2 = s0 + hmm_tprob_3st(0, 2);
        if (t0 > t1) {
            if (t2 > t0) {
                s2 = t2;
                hmm_history(hmm, 2)  = hmm_history(hmm, 0);
            } else
                s2 = t0;
        } else {
            if (t2 > t1) {
                s2 = t2;
                hmm_history(hmm, 2)  = hmm_history(hmm, 0);
            } else {
                s2 = t1;
                hmm_history(hmm, 2)  = hmm_history(hmm, 1);
            }
        }
        if (s2 > bestScore) bestScore = s2;
        hmm_score(hmm, 2) = s2;
    }

    /* All transitions into state 1 */
    if (s0 > WORST_SCORE) {
        t0 = s1 + hmm_tprob_3st(1, 1);
        t1 = s0 + hmm_tprob_3st(0, 1);
        if (t0 > t1) {
            s1 = t0;
        } else {
            s1 = t1;
            hmm_history(hmm, 1)  = hmm_history(hmm, 0);
        }
        if (s1 > bestScore) bestScore = s1;
        hmm_score(hmm, 1) = s1;
    }

    /* All transitions into state 0 */
    if (s0 > WORST_SCORE) {
        s0 = s0 + hmm_tprob_3st(0, 0);
        if (s0 > bestScore) bestScore = s0;
        hmm_score(hmm, 0) = s0;
    }

    hmm_bestscore(hmm) = bestScore;
    return bestScore;
}

static int32
hmm_vit_eval_3st_lr_mpx(hmm_t * hmm)
{
    const int32 ***tp = hmm->ctx->tp;
    const int32 *senscore = hmm->ctx->senscore;
    const s3senid_t **sseq = hmm->ctx->sseq;
    int32 *ssid = hmm->s.mpx_ssid;
    s3tmatid_t *tmatid = hmm->t.mpx_tmatid;
    int32 bestScore;
    int32 s3, s2, s1, s0, t2, t1, t0;

    /* Don't propagate WORST_SCORE */
    if (ssid[2] == -1)
        s2 = t1 = WORST_SCORE;
    else {
        s2 = hmm_score(hmm, 2) + mpx_senscr(2);
        t1 = s2 + mpx_tprob(2, 3);
    }
    if (ssid[1] == -1)
        s1 = t2 = WORST_SCORE;
    else {
        s1 = hmm_score(hmm, 1) + mpx_senscr(1);
        t2 = s1 + mpx_tprob(1, 3);
    }
    if (t1 > t2) {
        s3 = t1;
        hmm_out_history(hmm) = hmm_history(hmm, 1);
    }
    else {
        s3 = t2;
        hmm_out_history(hmm) = hmm_history(hmm, 2);
    }
    hmm_out_score(hmm) = s3;
    bestScore = s3;

    /* Handle transition from non-emitting initial state */
    s0 = hmm_score(hmm, 0) + mpx_senscr(0);
    if (hmm_in_score(hmm) + mpx_senscr(0) > s0) {
        s0 = hmm_in_score(hmm) + mpx_senscr(0);
        hmm_history(hmm, 0) = hmm_in_history(hmm);
    }
    hmm_in_score(hmm) = WORST_SCORE; /* consumed */

    /* Don't propagate WORST_SCORE */
    t0 = t1 = WORST_SCORE;
    if (s2 != WORST_SCORE)
        t0 = s2 + mpx_tprob(2, 2);
    if (s1 != WORST_SCORE)
        t1 = s1 + mpx_tprob(1, 2);
    t2 = s0 + mpx_tprob(0, 2);
    if (t0 > t1) {
        if (t2 > t0) {
            s2 = t2;
            hmm_history(hmm, 2) = hmm_history(hmm, 0);
            ssid[2] = ssid[0];
            tmatid[2] = tmatid[0];
        }
        else
            s2 = t0;
    }
    else {
        if (t2 > t1) {
            s2 = t2;
            hmm_history(hmm, 2) = hmm_history(hmm, 0);
            ssid[2] = ssid[0];
            tmatid[2] = tmatid[0];
        }
        else {
            s2 = t1;
            hmm_history(hmm, 2) = hmm_history(hmm, 1);
            ssid[2] = ssid[1];
            tmatid[2] = tmatid[1];
        }
    }
    if (s2 > bestScore)
        bestScore = s2;
    hmm_score(hmm, 2) = s2;

    /* Don't propagate WORST_SCORE */
    t0 = WORST_SCORE;
    if (s1 != WORST_SCORE)
        t0 = s1 + mpx_tprob(1, 1);
    t1 = s0 + mpx_tprob(0, 1);
    if (t0 > t1) {
        s1 = t0;
    }
    else {
        s1 = t1;
        hmm_history(hmm, 1) = hmm_history(hmm, 0);
        ssid[1] = ssid[0];
        tmatid[1] = tmatid[0];
    }
    if (s1 > bestScore)
        bestScore = s1;
    hmm_score(hmm, 1) = s1;

    s0 = s0 + mpx_tprob(0, 0);
    if (s0 > bestScore)
        bestScore = s0;
    hmm_score(hmm, 0) = s0;

    hmm_bestscore(hmm) = bestScore;
    return bestScore;
}

static int32
hmm_vit_eval_anytopo(hmm_t * h)
{
    hmm_context_t *ctx = h->ctx;
    int32 to, from, bestfrom;
    int32 newscr, scr, bestscr;
    int final_state;

    /* Compute previous state-score + observation output prob for each emitting state */
    for (from = 0; from < hmm_n_emit_state(h); ++from) {
        if ((ctx->st_sen_scr[from] =
             hmm_score(h, from) + hmm_senscr(h, from)) < WORST_SCORE)
            ctx->st_sen_scr[from] = WORST_SCORE;
    }
    /* Handle transitions from initial non-emitting state */
    if (hmm_in_score(h) + hmm_senscr(h, 0) > ctx->st_sen_scr[0]) {
        ctx->st_sen_scr[0] = hmm_in_score(h) + hmm_senscr(h, 0);
        hmm_history(h, 0) = hmm_in_history(h);
    }
    hmm_in_score(h) = WORST_SCORE; /* consumed */

    /* FIXME/TODO: Use the BLAS for all this. */
    /* Evaluate final-state first, which does not have a self-transition */
    final_state = hmm_n_emit_state(h);
    to = final_state;
    scr = WORST_SCORE;
    bestfrom = -1;
    for (from = to - 1; from >= 0; --from) {
        if ((hmm_tprob(h, from, to) > WORST_SCORE) &&
            ((newscr = ctx->st_sen_scr[from]
              + hmm_tprob(h, from, to)) > scr)) {
            scr = newscr;
            bestfrom = from;
        }
    }
    hmm_out_score(h) = scr;
    if (bestfrom >= 0)
        hmm_out_history(h) = hmm_history(h, bestfrom);
    bestscr = scr;

    /* Evaluate all other states, which might have self-transitions */
    for (to = final_state - 1; to >= 0; --to) {
        /* Score from self-transition, if any */
        scr =
            (hmm_tprob(h, to, to) > WORST_SCORE)
            ? ctx->st_sen_scr[to] + hmm_tprob(h, to, to)
            : WORST_SCORE;

        /* Scores from transitions from other states */
        bestfrom = -1;
        for (from = to - 1; from >= 0; --from) {
            if ((hmm_tprob(h, from, to) > WORST_SCORE) &&
                ((newscr = ctx->st_sen_scr[from]
                  + hmm_tprob(h, from, to)) > scr)) {
                scr = newscr;
                bestfrom = from;
            }
        }

        /* Update new result for state to */
        hmm_score(h, to) = scr;
        if (bestfrom >= 0) {
            hmm_history(h, to) = hmm_history(h, bestfrom);
            if (ctx->mpx) {
                h->s.mpx_ssid[to] = h->s.mpx_ssid[bestfrom];
                h->t.mpx_tmatid[to] = h->t.mpx_tmatid[bestfrom];
            }
        }

        if (bestscr < scr)
            bestscr = scr;
    }

    h->bestscore = bestscr;
    return bestscr;
}

int32
hmm_vit_eval(hmm_t * hmm)
{
    if (hmm->ctx->mpx) {
        if (hmm_n_emit_state(hmm) == 5)
            return hmm_vit_eval_5st_lr_mpx(hmm);
        else if (hmm_n_emit_state(hmm) == 3)
            return hmm_vit_eval_3st_lr_mpx(hmm);
        else
            return hmm_vit_eval_anytopo(hmm);
    }
    else {
        if (hmm_n_emit_state(hmm) == 5)
            return hmm_vit_eval_5st_lr(hmm);
        else if (hmm_n_emit_state(hmm) == 3)
            return hmm_vit_eval_3st_lr(hmm);
        else
            return hmm_vit_eval_anytopo(hmm);
    }
}

int32
hmm_dump_vit_eval(hmm_t * hmm, FILE * fp)
{
    int32 bs = 0;

    if (fp) {
        fprintf(fp, "BEFORE:\n");
        hmm_dump(hmm, fp);
    }
    bs = hmm_vit_eval(hmm);
    if (fp) {
        fprintf(fp, "AFTER:\n");
        hmm_dump(hmm, fp);
    }

    return bs;
}
