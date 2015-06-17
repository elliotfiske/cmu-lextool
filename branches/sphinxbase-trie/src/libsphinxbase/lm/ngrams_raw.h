#ifndef __NGRAMS_RAW_H__
#define __NGRAMS_RAW_H__

#include <sphinxbase/hash_table.h>
#include <sphinxbase/logmath.h>
#include <sphinxbase/prim_type.h>
#include <sphinxbase/pio.h>
#include <sphinxbase/err.h>

#include "lm_trie_misc.h"

typedef struct ngram_raw_s {
    word_idx *words; /* array of word indexes, length corresponds to ngram order */
    float *weights;  /* prob and backoff or just prob for longest order */
}ngram_raw_t;

typedef struct ngram_raw_ord_s {
    ngram_raw_t instance;
    int order;
} ngram_raw_ord_t;

/**
 * Raw ngrams comparator. Usage:
 * > ngram_comparator(NULL, &order); - to set order of ngrams
 * > qsort(ngrams, count, sizeof(lm_ngram_t), &ngram_comparator); - to sort ngrams in increasing order
 */
int ngram_comparator(const void *first_void, const void *second_void);

/**
 * Raw ordered ngrams comparator
 */
int ngram_ord_comparator(void *a_raw, void *b_raw);

ngram_raw_t** ngrams_raw_read_arpa(lineiter_t **li, hash_table_t *wid, logmath_t *lmath, uint32 *counts, int order);

void ngrams_raw_fix_counts(ngram_raw_t **raw_ngrams, uint32 *counts, uint32 *fixed_counts, int order);

void ngrams_raw_free(ngram_raw_t **raw_ngrams, uint32 *counts, int order);

#endif /* __LM_NGRAMS_RAW_H__ */