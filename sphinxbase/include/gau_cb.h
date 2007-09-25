/* -*- c-basic-offset: 4; indent-tabs-mode: nil -*- */
/* ====================================================================
 * Copyright (c) 2007 Carnegie Mellon University.  All rights
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
/**
 * \file gau_cb.h
 * \author David Huggins-Daines <dhuggins@cs.cmu.edu>
 *
 * Gaussian distribution parameters (common functions)
 */

#ifndef __GAU_CB_H__
#define __GAU_CB_H__

#include <sphinx_config.h>
#include <cmd_ln.h>
#include <fixpoint.h>

/**
 * Abstract type representing a set (codebook) of Gaussians.
 */
typedef struct gau_cb_s gau_cb_t;

#ifdef FIXED_POINT
/** Gaussian mean storage type. */
typedef fixed32 mean_t;
/** Gaussian precision storage type. */
typedef int32 var_t;
#else
typedef float32 mean_t;
typedef float32 var_t;
#endif

/**
 * Read a codebook of Gaussians from mean and variance files.
 */
gau_cb_t *gau_cb_read(cmd_ln_t *config,    /**< Configuration parameters */
                      const char *meanfn,  /**< Filename for means */
                      const char *varfn,   /**< Filename for variances */
                      const char *normfn   /**< (optional) Filename for normalization constants  */
    );

/**
 * Retrieve the dimensionality of a codebook.
 */
void gau_cb_get_dims(gau_cb_t *cb, int *out_n_gau, int *out_n_feat,
		     const int **out_veclen);

/**
 * Precompute normalizing constants and inverse variances, if required.
 */
void gau_cb_precomp(gau_cb_t *cb);

/**
 * Retrieve the mean vectors from the codebook.
 */
mean_t ***gau_cb_get_means(gau_cb_t *cb);

/**
 * Retrieve the scaled inverse variance vectors from the codebook.
 */
var_t ***gau_cb_get_invvars(gau_cb_t *cb);

/**
 * Retrieve the normalization constants from the codebook.
 */
int32 **gau_cb_get_norms(gau_cb_t *cb);

/**
 * Release memory and/or file descriptors associated with Gaussian codebook
 */
void gau_cb_free(gau_cb_t *cb);

#endif /* __GAU_CB_H__ */
