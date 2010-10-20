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

/* srch_fsg.h
 * HISTORY
 * 
 * $Log$
 * Revision 1.1.2.2  2005/06/28  07:01:21  arthchan2003
 * General fix of fsg routines to make a prototype of fsg_init and fsg_read. Not completed.  The number of empty functions in fsg_search is now decreased from 35 to 30.
 * 
 * Revision 1.1.2.1  2005/06/27 05:27:09  arthchan2003
 * Fixed added srch_fsg.[ch] and replace it by srch_fsm.c
 *
 */

#include <s3types.h>
#include <lm.h>
#include "kb.h"
#include "word_fsg.h"

int srch_FSG_init(kb_t *kb, /**< The KB */
		  void* srch_struct /**< The pointer to a search structure */
		  );

word_fsg_t* srch_FSG_read_fsgfile(void* srch_struct,const char* fsgname);

int srch_FSG_uninit(void* srch_struct);
int srch_FSG_begin(void* srch_struct);
int srch_FSG_end(void* srch_struct);
int srch_FSG_decode(void* srch_struct);

int srch_FSG_set_lm(void* srch_struct, const char* lmname);
int srch_FSG_add_lm(void* srch, lm_t *lm, const char *lmname);
int srch_FSG_delete_lm(void* srch, const char *lmname);

int srch_FSG_gmm_compute_lv2(void* srch_struct, float32 *feat, int32 time);
int srch_FSG_hmm_compute_lv1(void* srch_struct);
int srch_FSG_hmm_compute_lv2(void* srch_struct, int32 frmno);
int srch_FSG_eval_beams_lv1 (void* srch_struct);
int srch_FSG_eval_beams_lv2 (void* srch_struct);
int srch_FSG_propagate_graph_ph_lv1(void* srch_struct);
int srch_FSG_propagate_graph_wd_lv1(void* srch_struct);

int srch_FSG_propagate_graph_ph_lv2(void* srch_struct, int32 frmno);
int srch_FSG_propagate_graph_wd_lv2(void* srch_struct, int32 frmno);
int srch_FSG_rescoring(void *srch, int32 frmno);

int srch_FSG_compute_heuristic(void *srch, int32 win_efv);
int srch_FSG_frame_windup(void *srch_struct,int32 frmno);
int srch_FSG_shift_one_cache_frame(void *srch,int32 win_efv);
int srch_FSG_select_active_gmm(void *srch);