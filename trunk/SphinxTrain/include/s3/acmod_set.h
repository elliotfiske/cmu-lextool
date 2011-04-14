/* ====================================================================
 * Copyright (c) 1995-2000 Carnegie Mellon University.  All rights 
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
/**********************************************************************
 *
 * File: acmod_set.h
 * 
 * Description: 
 *	This header defines the interface to the acmod_set (acoustic
 *	model set) module.  See acmod_set.c for the detailed
 *	descriptions of each function below.
 *
 * Author: 
 * 	Eric H. Thayer (eht@cs.cmu.edu)
 *********************************************************************/

#ifndef ACMOD_SET_H
#define ACMOD_SET_H


#ifdef __cplusplus
extern "C" {
#endif
#if 0
}
#endif

#include <s3/acmod_set_ds.h>	/* defines the data structures used by this
				 * module */
#include <sphinxbase/prim_type.h>

acmod_set_t *
acmod_set_new(void);

int32
acmod_set_set_n_ci_hint(acmod_set_t *acmod_set,
			uint32 n_ci_hint);

int32
acmod_set_set_n_tri_hint(acmod_set_t *acmod_set,
			 uint32 n_tri_hint);

acmod_id_t
acmod_set_add_ci(acmod_set_t *acmod_set,
		 const char *name,
		 const char **attrib);

acmod_id_t
acmod_set_add_tri(acmod_set_t *acmod_set,
		  acmod_id_t base,
		  acmod_id_t left_context,
		  acmod_id_t right_context,
		  word_posn_t posn,
		  const char **attrib);

acmod_id_t
acmod_set_name2id(acmod_set_t *acmod_set,
		  const char *name);

const char *
acmod_set_id2name(acmod_set_t *acmod_set,
		  acmod_id_t id);

const char *
acmod_set_id2fullname(acmod_set_t *acmod_set,
		      acmod_id_t id);

const char *
acmod_set_id2s2name(acmod_set_t *acmod_set,
		    acmod_id_t id);

acmod_id_t
acmod_set_enum_init(acmod_set_t *acmod_set,
		    acmod_id_t base);

acmod_id_t
acmod_set_enum(void);

acmod_id_t
acmod_set_tri2id(acmod_set_t *acmod_set,
		 acmod_id_t base,
		 acmod_id_t left_context,
		 acmod_id_t right_context,
		 word_posn_t posn);

int32
acmod_set_id2tri(acmod_set_t *acmod_set,
		 acmod_id_t *base,
		 acmod_id_t *left_context,
		 acmod_id_t *right_context,
		 word_posn_t *posn,
		 acmod_id_t id);

const char *
acmod_set_s2_id2name(acmod_set_t *acmod_set,
		     acmod_id_t id);

uint32
acmod_set_n_acmod(acmod_set_t *acmod_set);

uint32
acmod_set_n_base_acmod(acmod_set_t *acmod_set);

uint32
acmod_set_n_multi(acmod_set_t *acmod_set);

uint32
acmod_set_n_ci(acmod_set_t *acmod_set);

acmod_id_t
acmod_set_base_phone(acmod_set_t *acmod_set,
		     acmod_id_t id);

uint32
acmod_set_has_attrib(acmod_set_t *acmod_set,
		     acmod_id_t id,
		     const char *attrib);

const char **
acmod_set_attrib(acmod_set_t *acmod_set,
		 acmod_id_t id);

int
acmod_set_s2_parse_triphone(acmod_set_t *acmod_set,
			    acmod_id_t *base,
			    acmod_id_t *left,
			    acmod_id_t *right,
			    word_posn_t *posn,
			    char *str);
#ifdef __cplusplus
}
#endif

#endif /* ACMOD_SET_H */ 
