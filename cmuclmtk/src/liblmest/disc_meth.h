/* ====================================================================
 * Copyright (c) 1999-2006 Carnegie Mellon University.  All rights
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
 * disc_meth.h - the wrapper of all discounting methods. 
 * 
 * **********************************************
 * CMU ARPA Speech Project
 *
 * Copyright (c) 2006 Carnegie Mellon University.
 * ALL RIGHTS RESERVED.
 * **********************************************
 * 
 * HISTORY
 * $Log: disc_meth.h,v $
 * Revision 1.3  2006/04/13 17:33:26  archan
 * 0, This particular change enable 32bit LM creation in ARPA format.  1, rationalized/messed up the data type, (Careful, with reading and writing for 8-byte data structure, they are not exactly working at this point.) 2, all switches in idngram2lm is changed to be implemented by the disc_meth object.
 *
 * Revision 1.2  2006/04/02 23:41:02  archan
 * Introduced 2 more methods in disc_meth object.
 *
 * Revision 1.1  2006/03/31 04:10:18  archan
 * Start to add a class for smoothing methods.  Only one method is implemented. In the Project L's case, we probably want to mix C-class and switch for a while.
 *
 *
 */

#include "ngram.h"
#include "disc_meth_good_turing.h"
#include "disc_meth_absolute.h"
#include "disc_meth_linear.h"
#include "disc_meth_witten_bell.h"
/**
   \type disc_meth_t
   An object for discounting method. 
 */
typedef struct {
  int32 type; /**< Type of discounting method */
  void (*verbose_method)(ng_t* ng, unsigned short verbosity);
  double (*dump_discounted_ngram_count)(ng_t* ng, /**< N-gram */
					int N,      /**< Which N in the N-gram */
					count_t ngcount, /**< The ngram count.  Used in every type of discountings */
					count_t margcount, 
					int* current_pos /**< Used in Witten-Bell */
					);
  void  (*allocate_freq_of_freq) (ng_t *ng);
  void  (*update_freq_of_freq)(ng_t *ng, int N, count_t countno);
  void  (*reduce_ug_freq_of_freq)(ng_t *ng,int i);
  void  (*compute_discount_aux)(ng_t *ng, int verbosity);
} disc_meth_t;

#define NG_DISC_METH(ng) ((disc_meth_t*) ((ng)->disc_meth))

disc_meth_t* disc_meth_init(int32 disc_meth);
