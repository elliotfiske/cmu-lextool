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
 * kb.h -- Knowledge bases, search parameters, and auxiliary structures for decoding
 * 
 * **********************************************
 * CMU ARPA Speech Project
 *
 * Copyright (c) 1999 Carnegie Mellon University.
 * ALL RIGHTS RESERVED.
 * **********************************************
 * 
 * HISTORY
 * 
 * 14-Jun-2004  Yitao Sun (yitao@cs.cmu.edu) at Carnegie Mellon University
 *              Modified struct kb_t to save the last hypothesis.
 *
 * 07-Jul-1999	M K Ravishankar (rkm@cs.cmu.edu) at Carnegie Mellon University
 * 		Added kb_t.ci_active.
 * 
 * 02-Jun-1999	M K Ravishankar (rkm@cs.cmu.edu) at Carnegie Mellon University
 * 		Started.
 */


#ifndef _S3_KB_H_
#define _S3_KB_H_

#include <libutil/libutil.h>
#include "kbcore.h"
#include "lextree.h"
#include "vithist.h"
#include "ascr.h"
#include "fast_algo_struct.h"
#include "mllr.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * There can be several unigram lextrees.  If we're at the end of frame f, we can only
 * transition into the roots of lextree[(f+1) % n_lextree]; same for fillertree[].  This
 * alleviates the problem of excessive Viterbi pruning in lextrees.
 */

/*
 * ARCHAN :20040229. Starting from s3.4, we accept users to specify mutliple LMs
 * using -lmctlfile. To avoid potential overhead caused by rebuilding the trees
 * at every sentence. We allocate all lexical trees in the intialization. If we
 * assume the number of words to be less than 64k and the number of LMs is smaller
 * than 100. This memory loss should be fine.  Later: We should allow users to specify
 * the memory address mode by their own discretion.
 */

typedef struct {
  kbcore_t *kbcore;		/* Core model structures */    

  /*Feature related variables*/
  float32 ***feat;		/* Feature frames */

  /*Search related variables, e.g lexical tree and Viterbi history*/
  int32 n_lextree;		/* See above comment about n_lextree */
  lextree_t **ugtree;
  lextree_t **fillertree;
  int32 n_lextrans;		/* #Transitions to lextree (root) made so far */
  lextree_t **ugtreeMulti;  /* This data structure allocate all trees for all LMs specified by the users */
  vithist_t *vithist;		/* Viterbi history, built during search */

  char *uttid;                  /* Utterance ID */
  int32 nfr;			/* #Frames in feat in current utterance */
  int32 tot_fr;                 /* The total number of frames that the
                                   recognizer has been
                                   recognized. Mainly for bookeeping.  */
    

  /* Thing that are used by multiple parts of the recognizer.  Pretty
     hard to wrap them into one data structure. Just leave it there
     for now. */

  int32 *ssid_active;		/* For determining the active senones in any frame */
  int32 *comssid_active;        /* Composite senone active */
  int32 *sen_active;            /* Structure that record whether the current state is active. */
  int32 *rec_sen_active;        /* Most recent senone active state */

  int32 **cache_ci_senscr;     /* Cache of ci senscr in the next pl_windows frames, include this frame.*/
  int32 *cache_best_list;        /* Cache of best the ci sensr the next pl_windows, include this frame*/

  int32 bestscore;	/* Best HMM state score in current frame */
  int32 bestwordscore;	/* Best wordexit HMM state score in current frame */
    
  ascr_t *ascr;		  /* Senone and composite senone scores for one frame */
  beam_t *beam;		  /* Structure that wraps up parameters related to beam pruning */
  histprune_t *histprune; /* Structure that wraps up parameters related to histogram pruning */
  fast_gmm_t *fastgmm;         /* Structure that wraps up fast GMM computation */

  int32 *hmm_hist;		/* Histogram: #frames in which a given no. of HMMs are active */
  int32 hmm_hist_bins;	/* #Bins in above histogram */
  int32 hmm_hist_binsize;	/* Binsize in above histogram (#HMMs/bin) */
    
  /* All structure that measure the time and stuffs we computed */
  ptmr_t tm_sen;
  ptmr_t tm_srch;
  ptmr_t tm_ovrhd;
  
  int32 utt_hmm_eval;
  int32 utt_sen_eval;
  int32 utt_gau_eval;
  
  float64 tot_sen_eval;	/* Senones evaluated over the entire session */
  float64 tot_gau_eval;	/* Gaussian densities evaluated over the entire session */
  float64 tot_hmm_eval;	/* HMMs evaluated over the entire session */
  float64 tot_wd_exit;	/* Words hypothesized over the entire session */
  
  FILE *matchfp;
  FILE *matchsegfp;



  /* Things I want to move to somewhere else. */
  int32 *phn_heur_list;          /* Cache of best the ci phoneme scores in the next pl_windows, include this frame*/
  int32 pl_win;            /* The window size of phoneme look-ahead */
  int32 pl_win_strt;      /* The start index of the window near the end of a block */
  int32 pl_win_efv;  /* Effective window size in livemode */
  int32 pl_beam;              /* Beam for phoneme look-ahead */

  /*variables for mllrmatrix */
  char* prevmllrfn;
  float32** regA;
  float32* regB;

} kb_t;


void kb_init (kb_t *kb);
void kb_lextree_active_swap (kb_t *kb);
void kb_free (kb_t *kb);	/* RAH 4.16.01 */
void kb_setlm(char* lmname,kb_t *kb);    /* ARCHAN 20040228 */
void kb_setmllr(char* mllrname,kb_t *kb);    /* ARCHAN 20040724 */

#ifdef __cplusplus
}
#endif

#endif
