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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "sphinxbase/ckd_alloc.h"
#include "sphinxbase/err.h"

#include "fe_prespch_buf.h"

struct prespch_buf_s {
	/* saved mfcc frames */
    mfcc_t** buffer;
	/* write pointer */
	int16 write_ptr;
	/* read pointer */
	int16 read_ptr;
	/* frames amount */
	int16 num_frames;
	/* filters amount */
	int16 num_cepstra;
};

prespch_buf_t *
fe_init_prespch(int num_frames, int num_cepstra)
{
	prespch_buf_t *prespch_buf;

    prespch_buf = (prespch_buf_t *) ckd_calloc(1, sizeof(prespch_buf_t));
	
	prespch_buf->num_cepstra = num_cepstra;
	prespch_buf->num_frames = num_frames;
	prespch_buf->write_ptr = 0;
	prespch_buf->read_ptr = 0;
    
	prespch_buf->buffer = (mfcc_t **)
		ckd_calloc_2d(num_frames, num_cepstra, sizeof(**prespch_buf->buffer));

	return prespch_buf;
}

int 
fe_prespch_read(prespch_buf_t* prespch_buf, mfcc_t * fea)
{
	if (prespch_buf->read_ptr >= prespch_buf->num_frames)
		return 0; //nothing to read
	if (prespch_buf->read_ptr >= prespch_buf->write_ptr)
		return 0; //nothing to read
	memcpy(fea, prespch_buf->buffer[prespch_buf->read_ptr], sizeof(mfcc_t)*prespch_buf->num_cepstra);
	prespch_buf->read_ptr++;
	return 1;
}

void 
fe_prespch_write(prespch_buf_t* prespch_buf, mfcc_t * fea)
{
	assert(prespch_buf->write_ptr < prespch_buf->num_frames);
	memcpy(prespch_buf->buffer[prespch_buf->write_ptr], fea, sizeof(mfcc_t)*prespch_buf->num_cepstra);
	prespch_buf->write_ptr++;
}

void 
fe_prespch_reset(prespch_buf_t* prespch_buf)
{
	prespch_buf->read_ptr = 0;
	prespch_buf->write_ptr = 0;
}

void 
fe_free_prespch(prespch_buf_t* prespch_buf)
{
	if (prespch_buf->buffer)
        ckd_free_2d((void **)prespch_buf->buffer);
	ckd_free(prespch_buf);
}