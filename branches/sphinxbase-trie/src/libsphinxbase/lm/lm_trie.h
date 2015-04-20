#ifndef __LM_TRIE_H__
#define __LM_TRIE_H__

#include <sphinxbase/pio.h>

#include "lm_trie_bits.h"
#include "lm_trie_quant.h"

typedef struct unigram_s {
    float prob;
    float bo;
    uint64 next;
}unigram_t;

typedef struct node_range_s {
    uint64 begin;
    uint64 end;
}node_range_t;

typedef struct base_s {
    uint8 word_bits;
    uint8 total_bits;
    uint64 word_mask;
    uint8 *base;
    uint64 insert_index;
    uint64 max_vocab;
}base_t;

typedef struct middle_s {
    base_t base;
    bit_mask_t next_mask; 
    uint8 quant_bits;
    void *next_source;
}middle_t;

typedef struct longest_s {
    base_t base;
    uint8 quant_bits;
}longest_t;

typedef struct lm_trie_s {
    uint8 *misc_mem;         /**< Contains quantatization data and unigrams */
    uint64 misc_mem_size;
    uint8 *ngram_mem;
    uint64 ngram_mem_size;
    unigram_t *unigrams;
    middle_t *middle_begin;
    middle_t *middle_end;
    longest_t *longest;
    lm_trie_quant_t *quant;

    float backoff[MAX_NGRAM_ORDER];
    word_idx prev_hist[MAX_NGRAM_ORDER - 1];
}lm_trie_t;

/**
 * Creates lm_trie structure. Fills it if binary file with correspondent data is provided
 */
lm_trie_t* lm_trie_create();

void lm_trie_alloc_misc(lm_trie_t *trie, uint64 unigram_count, lm_trie_quant_type_t quant_type, int order);

void lm_trie_alloc_ngram(lm_trie_t *trie, uint64 *counts, int order);

void lm_trie_build(lm_trie_t *trie, lm_ngram_t **raw_ngrams, uint64 *counts, int order);

void lm_trie_write_bin(lm_trie_t *trie, FILE *fp);

void lm_trie_read_bin(lm_trie_t *trie, FILE *fp);

void lm_trie_free(lm_trie_t *trie);

#endif /* __LM_TRIE_H__ */