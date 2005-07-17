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
 * ascr.c -- Acoustic (senone) scores
 * 
 * **********************************************
 * CMU ARPA Speech Project
 *
 * Copyright (c) 1999 Carnegie Mellon University.
 * ALL RIGHTS RESERVED.
 * **********************************************
 * 
 * HISTORY
 * $Log$
 * Revision 1.4.4.1  2005/07/17  05:44:30  arthchan2003
 * Added dag_write_header so that DAG header writer could be shared between 3.x and 3.0. However, because the backtrack pointer structure is different in 3.x and 3.0. The DAG writer still can't be shared yet.
 * 
 * Revision 1.4  2005/06/21 22:32:25  arthchan2003
 * Log. Significant expansion of ascr_t to be the container of all types
 * of acoustic scores.  Implementations of init, report, free functions
 * are now provided. ascr_shift_one_cache_frame is used to provide a
 * wrapper for shifting one frame of cache frame in phoneme lookahead.
 * It is expected to be used later more if more complicated scheme of
 * lookahead is used.
 *
 * Revision 1.6  2005/04/21 23:50:26  archan
 * Some more refactoring on the how reporting of structures inside kbcore_t is done, it is now 50% nice. Also added class-based LM test case into test-decode.sh.in.  At this moment, everything in search mode 5 is already done.  It is time to test the idea whether the search can really be used.
 *
 * Revision 1.5  2005/04/20 03:31:57  archan
 * Part of refactoring: add ascr_shift_one_cache_frame which could shift one cache frame.
 *
 * Revision 1.4  2005/03/30 01:22:46  archan
 * Fixed mistakes in last updates. Add
 *
 * 
 * 09-Feb-2000	M K Ravishankar (rkm@cs.cmu.edu) at Carnegie Mellon University
 * 		Started.
 */


#include "ascr.h"


ascr_t *ascr_init (int32 n_sen, int32 n_comsen, int32 n_sseq, int32 n_comsseq, int32 pl_win, int32 n_cisen)
{
    ascr_t *ascr;
    
    ascr = (ascr_t *) ckd_calloc (1, sizeof(ascr_t));
    ascr->sen = (int32 *) ckd_calloc (n_sen + n_comsen, sizeof(int32));
    ascr->comsen = ascr->sen + n_sen;


    /* MEMORY ALLOCATION : Active senones */
    ascr->sen_active = (int32 *) ckd_calloc (n_sen, sizeof(int32));
    ascr->rec_sen_active = (int32 *) ckd_calloc (n_sen, sizeof(int32));
    ascr->ssid_active = (int32 *) ckd_calloc (n_sseq, sizeof(int32));
    ascr->comssid_active = (int32 *) ckd_calloc (n_comsseq, sizeof(int32));

    /* MEMORY ALLOCATION : CI senones */
    ascr->cache_ci_senscr=(int32**)ckd_calloc_2d(pl_win,n_cisen,sizeof(int32));
    ascr->cache_best_list=(int32*)ckd_calloc(pl_win,sizeof(int32));

    ascr->n_sen=n_sen;
    ascr->n_comsen=n_comsen;
    ascr->n_sseq=n_sseq;
    ascr->n_comsseq=n_comsseq;
    ascr->pl_win=pl_win;
    ascr->n_cisen=n_cisen;
    return ascr;
}

void ascr_report(ascr_t *a)
{
  E_INFO_NOFN("Initialization of ascr_t, report:\n");
  E_INFO_NOFN("No. of CI senone =%d \n",a->n_cisen);
  E_INFO_NOFN("No. of senone = %d\n",a->n_sen);
  E_INFO_NOFN("No. of composite senone = %d\n",a->n_comsen);
  E_INFO_NOFN("No. of senone sequence = %d\n",a->n_sseq);
  E_INFO_NOFN("No. of composite senone sequence=%d \n",a->n_comsseq);
  E_INFO_NOFN("Parameters used in phoneme lookahead:\n");
  E_INFO_NOFN("Phoneme lookahead window = %d\n",a->pl_win);
  E_INFO_NOFN("\n");
}

/* Shift one frame. */
/* get the CI sen scores for the t+pl_win'th frame (a slice) */

void ascr_shift_one_cache_frame(ascr_t *a, int32 win_efv)
{
  int32 i,j;
  for(i=0;i<win_efv-1;i++){
    a->cache_best_list[i]=a->cache_best_list[i+1];
    for(j=0;j<a->n_cisen;j++){
      a->cache_ci_senscr[i][j]=a->cache_ci_senscr[i+1][j];
    }
  }
}

void ascr_free(ascr_t *a)
{
  if(a){
    if (a->sen_active)
      ckd_free ((void *)a->sen_active);
    if (a->ssid_active) 
      ckd_free ((void *)a->ssid_active);
    if (a->comssid_active)
      ckd_free ((void *)a->comssid_active);
    if (a->cache_ci_senscr) 
      ckd_free_2d ((void **)a->cache_ci_senscr);
    if(a->cache_best_list) 
      ckd_free((void*) a->cache_best_list);

    ckd_free(a);
  }

}
