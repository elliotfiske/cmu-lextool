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
/*********************************************************************
 *
 * File: s3gau_io.h
 * 
 * Description: 
 * 
 * Author: 
 * 
 *********************************************************************/

#ifndef S3GAU_IO_H
#define S3GAU_IO_H
#ifdef __cplusplus
extern "C" {
#endif
#if 0
}
#endif

#include <s3/vector.h>

#define GAU_FILE_VERSION	"1.0"
#define GAUCNT_FILE_VERSION	"1.0"
#define GAUDNOM_FILE_VERSION	"1.0"

int
s3gau_read(const char *fn,
	   vector_t ****out,
	   uint32 *out_n_mgau,
	   uint32 *out_n_feat,
	   uint32 *out_n_density,
	   uint32 **out_veclen);

int
s3gau_read_full(const char *fn,
		vector_t *****out,
		uint32 *out_n_mgau,
		uint32 *out_n_feat,
		uint32 *out_n_density,
		uint32 **out_veclen);

int
s3gau_read_maybe_full(const char *fn,
		      vector_t *****out,
		      uint32 *out_n_mgau,
		      uint32 *out_n_feat,
		      uint32 *out_n_density,
		      uint32 **out_veclen,
		      uint32 expect_full);

int
s3gau_write(const char *fn,
	    const vector_t ***out,
	    uint32 n_mgau,
	    uint32 n_feat,
	    uint32 n_density,
	    const uint32 *veclen);

int
s3gau_write_full(const char *fn,
		 const vector_t ****out,
		 uint32 n_mgau,
		 uint32 n_feat,
		 uint32 n_density,
		 const uint32 *veclen);

int
s3gaucnt_read(const char *fn,
	      vector_t ****out_wt_mean,
	      vector_t ****out_wt_var,
	      int32 *out_pass2var,
	      float32 ****out_dnom,
	      uint32 *out_n_mgau,
	      uint32 *out_n_feat,
	      uint32 *out_n_density,
	      const uint32 **out_veclen);

int
s3gaucnt_write(const char *fn,
	       vector_t ***wt_mean,
	       vector_t ***wt_var,
	       int32 pass2var,
	       float32 ***dnom,
	       uint32 n_cb,
	       uint32 n_feat,
	       uint32 n_density,
	       const uint32 *veclen);

int
s3gaudnom_read(const char *fn,
	       float32 ****out_dnom,
	       uint32 *out_n_cb,
	       uint32 *out_n_feat,
	       uint32 *out_n_density);

int
s3gaucnt_read_full(const char *fn,
		   vector_t ****out_wt_mean,
		   vector_t *****out_wt_var,
		   int32 *out_pass2var,
		   float32 ****out_dnom,
		   uint32 *out_n_cb,
		   uint32 *out_n_feat,
		   uint32 *out_n_density,
		   const uint32 **out_veclen);

int
s3gaucnt_write_full(const char *fn,
		    vector_t ***wt_mean,
		    vector_t ****wt_var,
		    int32 pass2var,
		    float32 ***dnom,
		    uint32 n_cb,
		    uint32 n_feat,
		    uint32 n_density,
		    const uint32 *veclen);

int
s3gaudnom_write(const char *fn,
		float32 ***dnom,
		uint32 n_cb,
		uint32 n_feat,
		uint32 n_density);

#ifdef __cplusplus
}
#endif
#endif /* S3GAU_IO_H */ 


/*
 * Log record.  Maintained by RCS.
 *
 * $Log$
 * Revision 1.6  2004/07/22  00:16:13  egouvea
 * Roll back because of mistake in commit
 * 
 * Revision 1.4  2004/07/21 17:46:10  egouvea
 * Changed the license terms to make it the same as sphinx2 and sphinx3.
 *
 * Revision 1.3  2001/04/05 20:02:30  awb
 * *** empty log message ***
 *
 * Revision 1.2  2000/09/29 22:35:12  awb
 * *** empty log message ***
 *
 * Revision 1.1  2000/09/24 21:38:30  awb
 * *** empty log message ***
 *
 * Revision 1.1  97/07/16  11:39:10  eht
 * Initial revision
 * 
 *
 */
