/* ====================================================================
 * Copyright (c) 1994-2000 Carnegie Mellon University.  All rights 
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
 * File: accum.h
 * 
 * Description: 
 * 
 * Author: 
 * 
 *********************************************************************/

#ifndef ACCUM_H
#define ACCUM_H

#include <s3/vector.h>
#include <s3/prim_type.h>

int
accum_state_mean(vector_t ***mean_acc,
		 float32  ***dnom,
		 vector_t **f,
		 uint32 *del_b,
		 uint32 *del_e,
		 uint32 n_del,
		 uint32 n_feat,
		 const uint32 *veclen,
		 uint32 *sseq,
		 uint32 *ci_sseq,
		 uint32 n_frame);

int
accum_state_var(vector_t ***var_acc,
		vector_t ***mean,
		float32  ***dnom,
		vector_t **f,
		uint32 *del_b,
		uint32 *del_e,
		uint32 n_del,
		uint32 n_feat,
		const uint32 *veclen,
		uint32 *sseq,
		uint32 *ci_sseq,
		uint32 n_frame);

#endif /* ACCUM_H */ 


/*
 * Log record.  Maintained by RCS.
 *
 * $Log$
 * Revision 1.2  2000/09/29  22:35:14  awb
 * *** empty log message ***
 * 
 * Revision 1.1  2000/09/24 21:38:31  awb
 * *** empty log message ***
 *
 * Revision 1.4  97/07/16  11:38:16  eht
 * *** empty log message ***
 * 
 * Revision 1.3  1996/08/06  14:13:23  eht
 * added "const" attribute to veclen arguments
 *
 * Revision 1.2  1996/02/02  17:35:39  eht
 * formatting change
 *
 * Revision 1.1  1996/01/30  17:11:47  eht
 * Initial revision
 *
 *
 */
