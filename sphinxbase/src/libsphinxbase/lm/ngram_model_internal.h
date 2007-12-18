/* -*- c-basic-offset: 4; indent-tabs-mode: nil -*- */
/* ====================================================================
 * Copyright (c) 1999-2007 Carnegie Mellon University.  All rights
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
 * \file ngram_model_internal.h Internal structures for N-Gram models
 *
 * Author: David Huggins-Daines <dhuggins@cs.cmu.edu>
 */

#ifndef __NGRAM_MODEL_INTERNAL_H__
#define __NGRAM_MODEL_INTERNAL_H__

#include "ngram_model.h"
#include "hash_table.h"

/**
 * Common implementation of ngram_model_t.
 *
 * The details of bigram, trigram, and higher-order N-gram storage, if any, can
 * vary somewhat depending on the file format in use.
 */
struct ngram_model_s {
    int32 *n_counts;    /**< Counts for 1, 2, 3, ... grams */
    int32 n_1g_alloc;   /**< Number of allocated word strings (for new word addition) */
    int32 n_words;      /**< Number of actual word strings (NOT the same as the
                             number of unigrams, due to class words). */
    uint8 n;            /**< This is an n-gram model (1, 2, 3, ...). */
    uint8 n_classes;    /**< Number of classes (maximum 128) */
    uint8 writable;     /**< Are word strings writable? */
    uint8 flags;        /**< Any other flags we might care about
                             (FIXME: Merge this and writable) */
    logmath_t *lmath;   /**< Log-math object */
    float32 lw;         /**< Language model scaling factor */
    int32 log_wip;      /**< Log of word insertion penalty */
    int32 log_uw;       /**< Log of unigram weight */
    int32 log_uniform;  /**< Log of uniform (0-gram) probability */
    int32 log_uniform_weight; /**< Log of uniform weight (i.e. 1 - unigram weight) */
    char **word_str;    /**< Unigram names */
    hash_table_t *wid;  /**< Mapping of unigram names to word IDs. */
    struct ngram_class_s **classes; /**< Word class definitions. */
    struct ngram_funcs_s *funcs;   /**< Implementation-specific methods. */
};

/**
 * Implementation of ngram_class_t.
 */
struct ngram_class_s {
    int32 tag_wid;  /**< Base word ID for this class tag */
    int32 start_wid; /**< Starting base word ID for this class' words */
    int32 n_words;   /**< Number of base words for this class */
    int32 *prob1;    /**< Probability table for base words */
    /**
     * Custom hash table for additional words.
     */
    struct ngram_hash_s {
        int32 wid;    /**< Word ID of this bucket */
        int32 prob1;  /**< Probability for this word */
        int32 next;   /**< Index of next bucket (or -1 for no collision) */
    } *nword_hash;
    int32 n_hash;       /**< Number of buckets in nword_hash (power of 2) */
    int32 n_hash_inuse; /**< Number of words in nword_hash */
};

#define NGRAM_HASH_SIZE 128

#define NGRAM_BASEWID(wid) ((wid)&0xffffff)
#define NGRAM_CLASSID(wid) (((wid)>>24) & 0x7f)
#define NGRAM_CLASSWID(wid,classid) (((classid)<<24) | 0x80000000 | (wid))
#define NGRAM_IS_CLASSWID(wid) ((wid)&0x80000000)

#define UG_ALLOC_STEP 10

typedef struct ngram_funcs_s {
    /**
     * Implementation-specific function for freeing an ngram_model_t.
     */
    void (*free)(ngram_model_t *model);
    /**
     * Implementation-specific function for applying language model weights.
     */
    int (*apply_weights)(ngram_model_t *model,
                         float32 lw,
                         float32 wip,
                         float32 uw);
    /**
     * Implementation-specific function for querying language model score.
     */
    int32 (*score)(ngram_model_t *model,
                   int32 wid,
                   int32 *history,
                   int32 n_hist,
                   int32 *n_used);
    /**
     * Implementation-specific function for querying raw language
     * model probability.
     */
    int32 (*raw_score)(ngram_model_t *model,
                       int32 wid,
                       int32 *history,
                       int32 n_hist,
                       int32 *n_used);
    /**
     * Implementation-specific function for adding unigrams.
     *
     * This function updates the internal structures of a language
     * model to add the given unigram with the given weight (defined
     * as a log-factor applied to the uniform distribution).  This
     * includes reallocating or otherwise resizing the set of unigrams.
     *
     * @return The language model score (not raw log-probability) of
     * the new word, or NGRAM_SCORE_ERROR for failure.
     */
    int32 (*add_ug)(ngram_model_t *model,
                    int32 wid, int32 lweight);
} ngram_funcs_t;

/**
 * Initialize the base ngram_model_t structure.
 */
int32
ngram_model_init(ngram_model_t *model,
                 ngram_funcs_t *funcs,
                 logmath_t *lmath,
                 int32 n, int32 n_unigram);

/**
 * Read an N-Gram model from an ARPABO text file.
 */
ngram_model_t *ngram_model_arpa_read(cmd_ln_t *config,
				     const char *file_name,
				     logmath_t *lmath);
/**
 * Read an N-Gram model from a Sphinx .DMP binary file.
 */
ngram_model_t *ngram_model_dmp_read(cmd_ln_t *config,
				    const char *file_name,
				    logmath_t *lmath);
/**
 * Read an N-Gram model from a Sphinx .DMP32 binary file.
 */
ngram_model_t *ngram_model_dmp32_read(cmd_ln_t *config,
				     const char *file_name,
				     logmath_t *lmath);

/**
 * Write an N-Gram model to an ARPABO text file.
 */
int ngram_model_arpa_write(ngram_model_t *model,
			   const char *file_name);
/**
 * Write an N-Gram model to a Sphinx .DMP binary file.
 */
int ngram_model_dmp_write(ngram_model_t *model,
			  const char *file_name);
/**
 * Write an N-Gram model to a Sphinx .DMP32 binary file.
 */
int ngram_model_dmp32_write(ngram_model_t *model,
			    const char *file_name);
/**
 * Write an N-Gram model to an AT&T FSM format file.
 */
int ngram_model_fst_write(ngram_model_t *model,
			  const char *file_name);
/**
 * Write an N-Gram model to an HTK SLF word graph file.
 */
int ngram_model_htk_write(ngram_model_t *model,
			  const char *file_name);

/**
 * Allocate and initialize an N-Gram class.
 */
ngram_class_t *ngram_class_new(ngram_model_t *model, int32 tag_wid,
                               int32 start_wid, glist_t classwords);

/**
 * Deallocate an N-Gram class.
 */
void ngram_class_free(ngram_class_t *lmclass);

/**
 * Get the in-class log probability for a word in an N-Gram class.
 */
int32 ngram_class_prob(ngram_class_t *lmclass, int32 wid);

#endif /* __NGRAM_MODEL_INTERNAL_H__ */
