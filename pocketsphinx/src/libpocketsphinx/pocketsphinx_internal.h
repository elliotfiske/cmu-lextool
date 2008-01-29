/* -*- c-basic-offset: 4; indent-tabs-mode: nil -*- */
/* ====================================================================
 * Copyright (c) 2008 Carnegie Mellon University.  All rights
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
 * @file pocketsphinx_internal.h Internal implementation of
 * PocketSphinx decoder.
 */

#ifndef __POCKETSPHINX_INTERNAL_H__
#define __POCKETSPHINX_INTERNAL_H__

/* SphinxBase headers. */
#include <cmd_ln.h>
#include <logmath.h>
#include <fe.h>
#include <feat.h>

/* Local headers. */
#include "acmod.h"
#include "dict.h"
#include "ngram_search.h"
#include "fsg_search.h"

/**
 * Utterance processing state.
 */
typedef enum {
    UTTSTATE_UNDEF = -1,  /**< Undefined state */
    UTTSTATE_IDLE = 0,    /**< Idle, can change models, etc. */
    UTTSTATE_BEGUN = 1,   /**< Begun, can only do recognition. */
    UTTSTATE_ENDED = 2,   /**< Ended, a result is now available. */
    UTTSTATE_STOPPED = 3  /**< Stopped, can be resumed. */
} uttstate_t;

/**
 * Decoder object.
 */
struct pocketsphinx_s {
    /* Model parameters and such. */
    cmd_ln_t *config;  /**< Configuration. */
    logmath_t *lmath;  /**< Log math computation. */
    fe_t *fe;          /**< Acoustic feature computation. */
    feat_t *fcb;       /**< Dynamic feature computation. */
    acmod_t *acmod;    /**< Acoustic model. */
    dict_t *dict;      /**< Pronunciation dictionary. */

    /* Search modules. */
    ngram_search_t *ngs; /**< N-Gram search module. */
    fsg_search_t *fsgs;  /**< Finite-State search module. */

    /* Utterance-processing related stuff. */
    uttstate_t uttstate;/**< Current state of utterance processing. */
    int32 uttno;        /**< Utterance counter. */
    char *uttid;        /**< Utterance ID for current utterance. */
    char *uttid_prefix; /**< Prefix for automatic utterance IDs. */
    char *mfclogdir;    /**< Log directory for MFCC files. */
    char *rawlogdir;    /**< Log directory for raw audio files. */
    char *hypstr;       /**< Hypothesis string for current utt. */
    search_hyp_t *hyp;  /**< Hypothesis segmentation for current utt. */
    FILE *matchfp;      /**< File for writing recognition results. */
    FILE *matchsegfp;   /**< File for writing segmentation results. */
    FILE *rawfp;        /**< File for writing raw audio data. */
    FILE *mfcfp;        /**< File for writing acoustic feature data. */

    mfcc_t **mfc_buf;   /**< Temporary buffer of acoustic features. */
    mfcc_t ***feat_buf; /**< Temporary buffer of dynamic features. */
    
}
typedef struct pocketsphinx_s pocketsphinx_t;

/**
 * Initialize the decoder from a configuration object.
 */
pocketsphinx_t *pocketsphinx_init(cmd_ln_t *config);

/**
 * Finalize the decoder.
 */
void pocketsphinx_free(pocketsphinx_t *pocketsphinx);

/**
 * Run a control file in batch mode.
 */
int pocketsphinx_run_ctl_file(pocketsphinx_t *ps,
                              char const *ctlfile);

/**
 * Start utterance processing.
 */
int pocketsphinx_start_utt(pocketsphinx_t *ps);

/**
 * Decode raw audio data.
 *
 * @param no_search If non-zero, perform feature extraction but don't
 * do any recognition yet.  This may be necessary if your processor
 * has trouble doing recognition in real-time.
 * @param full_utt If non-zero, this block of data is a full utterance
 * worth of data.  This may allow the recognizer to produce more
 * accurate results.
 */
int pocketsphinx_process_raw(pocketsphinx_t *ps,
                             int16 const *data,
                             int32 n_samples,
                             int do_search,
                             int full_utt);

/**
 * Decode acoustic feature data.
 *
 * @param no_search If non-zero, perform feature extraction but don't
 * do any recognition yet.  This may be necessary if your processor
 * has trouble doing recognition in real-time.
 * @param full_utt If non-zero, this block of data is a full utterance
 * worth of data.  This may allow the recognizer to produce more
 * accurate results.
 */
int pocketsphinx_process_cep(pocketsphinx_t *ps,
                             mfcc_t const **data,
                             int32 n_frames,
                             int no_search,
                             int full_utt);

/**
 * End utterance processing.
 */
int pocketsphinx_end_utt(pocketsphinx_t *ps);
                             

#endif /* __POCKETSPHINX_INTERNAL_H__ */
