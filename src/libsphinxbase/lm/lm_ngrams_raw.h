#ifndef __LM_NGRAMS_RAW_H__
#define __LM_NGRAMS_RAW_H__

#include <sphinxbase/hash_table.h>
#include <sphinxbase/logmath.h>
#include <sphinxbase/prim_type.h>
#include <sphinxbase/pio.h>
#include <sphinxbase/err.h>

#include "lm_trie_misc.h"

typedef struct lm_ngram_s {
    word_idx *words; /* array of word indexes, length corresponds to ngram order */
    float *weights;  /* prob and backoff or just prob for longest order */
}lm_ngram_t;

/**
 * Raw ngrams comparator. Usage:
 * > ngram_comparator(NULL, &order); - to set order of ngrams
 * > qsort(ngrams, count, sizeof(lm_ngram_t), &ngram_comparator); - to sort ngrams in increasing order
 */
int ngram_comparator(const void *first_void, const void *second_void);

lm_ngram_t** lm_ngrams_raw_read(lineiter_t **li, hash_table_t *wid, logmath_t *lmath, uint64 *counts, int order);

void lm_ngrams_raw_fix_counts(lm_ngram_t **raw_ngrams, uint64 *counts, uint64 *fixed_counts, int order);

void lm_ngrams_raw_free(lm_ngram_t **raw_ngrams, uint64 *counts, int order);

#endif /* __LM_NGRAMS_RAW_H__ */