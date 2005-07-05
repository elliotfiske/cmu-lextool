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
 * cmn.h -- Various forms of cepstral mean normalization
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
 * Revision 1.11.4.1  2005/07/05  06:25:08  arthchan2003
 * Fixed dox-doc.
 * 
 * Revision 1.11  2005/06/21 19:28:00  arthchan2003
 * 1, Fixed doxygen documentation. 2, Added $ keyword.
 *
 * Revision 1.4  2005/06/13 04:02:56  archan
 * Fixed most doxygen-style documentation under libs3decoder.
 *
 * Revision 1.3  2005/03/30 01:22:46  archan
 * Fixed mistakes in last updates. Add
 *
 * 
 * 20.Apr.2001  RAH (rhoughton@mediasite.com, ricky.houghton@cs.cmu.edu)
 *              Added cmn_free() and moved *mean and *var out global space and named them cmn_mean and cmn_var
 * 
 * 28-Apr-1999	M K Ravishankar (rkm@cs.cmu.edu) at Carnegie Mellon University
 * 		Copied from previous version.
 */


#ifndef _S3_CMN_H_
#define _S3_CMN_H_

#include <s3types.h>

#ifdef __cplusplus
extern "C" {
#endif

  /** \file cmn.h
 * \brief Apply Cepstral Mean Normalization (CMN) to the set of input mfc frames.
 *
 * By subtractingthe mean of the input from each frame.  C0 is also included in this process.
 * This function operates on an entire utterance at a time.  Hence, the entire utterance
 * must be available beforehand (batchmode).
 */

  /** \struct cmn_t
   *  \brief wrapper of operation of the cepstral mean normalization. 
   */

typedef struct {
  /*These two are used in cmn*/
  float32 *cmn_mean;  /**< Temporary variables: stored the cmn mean */
  float32 *cmn_var;    /**< Temporary variables: stored the cmn variance */
  /*These three are used in cmn_prior*/
  float32 *cur_mean;   /**< Temporary variable: current means */
  float32 *sum;        /**< The sume of the cmn frames */
  int32 nframe; /**< Number of frames*/
}cmn_t;

cmn_t* cmn_init();

  /**
     CMN for the whole sentence
   */
void cmn (float32 **mfc,	/**< In/Out: mfc[f] = mfc vector in frame f */
	  int32 varnorm,	/**< In: if not FALSE, variance normalize the input vectors
				   to have unit variance (along each dimension independently);
				   Irrelevant if no cmn is performed */
	  int32 n_frame,	/**< In: #frames of mfc vectors */
	  int32 veclen,         /**< In: mfc vector length */
	  cmn_t *cmn	        /**< In/Out: cmn normalization, which contains the cmn_mean and cmn_var) */
	  );

#define CMN_WIN_HWM     800     /* #frames after which window shifted */
#define CMN_WIN         500

#if 0
void cmn_prior(float32 **incep,  /* In/Out: mfc[f] = mfc vector in frame f*/
	      int32 varnorm,    /* This flag should always be 0 for live */
	      int32 nfr,        /* Number of incoming frames */
              int32 ceplen,     /* Length of the cepstral vector */
	      int32 endutt,
	       cmn_t *cmn);    /* Flag indicating end of utterance */
#endif

/* RAH, free previously allocated memory */
void cmn_free ();

#ifdef __cplusplus
}
#endif

#endif
