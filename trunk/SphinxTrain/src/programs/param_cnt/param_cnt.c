/* ====================================================================
 * Copyright (c) 1996-2000 Carnegie Mellon University.  All rights 
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
 * 3. The names "Sphinx" and "Carnegie Mellon" must not be used to
 *    endorse or promote products derived from this software without
 *    prior written permission. To obtain permission, contact 
 *    sphinx@cs.cmu.edu.
 *
 * 4. Products derived from this software may not be called "Sphinx"
 *    nor may "Sphinx" appear in their names without prior written
 *    permission of Carnegie Mellon University. To obtain permission,
 *    contact sphinx@cs.cmu.edu.
 *
 * 5. Redistributions of any form whatsoever must retain the following
 *    acknowledgment:
 *    "This product includes software developed by Carnegie
 *    Mellon University (http://www.speech.cs.cmu.edu/)."
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
/*********************************************************************
 *
 * File: param_cnt.c
 * 
 * Description: 
 * 
 * Author: 
 * 
 *********************************************************************/

#ifndef PARAM_CNT_H
#define PARAM_CNT_H

#include "param_cnt.h"

#include "cnt_fn.h"
#include "ts_cnt.h"
#include "cb_cnt.h"
#include "phone_cnt.h"
#include "enum_corpus.h"

#include <s3/acmod_set.h>
#include <s3/ckd_alloc.h>

#include <string.h>

#include <stdio.h>

int
param_cnt(lexicon_t *lex,
	  model_def_t *mdef,
	  const char *param_type)
{
    acmod_set_t *acmod_set;
    cnt_fn_t cnt_fn=0;
    uint32 *cnt;
    uint32 n_cnt=0;
    uint32 i;

    acmod_set = mdef->acmod_set;

    if (strcmp(param_type, "state") == 0) {
	n_cnt = mdef->n_tied_state;
	cnt_fn = ts_cnt;
    }
    else if (strcmp(param_type, "cb") == 0) {
	n_cnt = mdef->n_cb;
	cnt_fn = cb_cnt;
    }
    else if (strcmp(param_type, "phone") == 0) {
	/* Total # of acoustic models */
	n_cnt = mdef->acmod_set->next_id;
	cnt_fn = phone_cnt;
    }
    cnt = ckd_calloc(n_cnt, sizeof(uint32));

    E_INFO("Scanning corpus\n");

    enum_corpus(lex, mdef, cnt, cnt_fn);

    if (strcmp(param_type, "phone") != 0) {
	for (i = 0; i < n_cnt; i++)
	    printf("%u %u\n", i, cnt[i]);
    }
    else {
	for (i = 0; i < n_cnt; i++)
	    printf("%s %u\n", acmod_set_id2fullname(acmod_set, (acmod_id_t)i), cnt[i]);
    }

    return 0;
}

#endif /* PARAM_CNT_H */ 


/*
 * Log record.  Maintained by RCS.
 *
 * $Log$
 * Revision 1.2  2000/09/29  22:35:14  awb
 * *** empty log message ***
 * 
 * Revision 1.1  2000/09/24 21:38:32  awb
 * *** empty log message ***
 *
 * Revision 1.1  97/07/16  11:36:22  eht
 * Initial revision
 * 
 *
 */
