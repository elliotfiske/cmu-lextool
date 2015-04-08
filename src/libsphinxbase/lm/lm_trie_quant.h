#ifndef __LM_TRIE_QUANT_H__
#define __LM_TRIE_QUANT_H__

#include "lm_trie_bits.h"
#include "lm_ngrams_raw.h"

typedef struct lm_trie_quant_s lm_trie_quant_t;

typedef enum lm_trie_quant_type_e {
    NO_QUANT,
    QUANT_16
} lm_trie_quant_type_t;

/**
 * Memory required to store quantizing info
 */
uint64 lm_trie_quant_size(lm_trie_quant_type_t quant_type, int order);

/**
 * Memory required for storing weights of middle-order ngrams.
 * Both backoff and probability should be stored
 */
uint8 lm_trie_quant_msize(lm_trie_quant_type_t quant_type);

/**
 * Memory required for storing weights of largest-order ngrams.
 * Only probability should be stored
 */
uint8 lm_trie_quant_lsize(lm_trie_quant_type_t quant_type);

/**
 * Create qunatizing in provided memory
 */
lm_trie_quant_t* lm_trie_quant_create(lm_trie_quant_type_t quant_type, uint8 *mem, int order);

/**
 * Checks whether quantizing should be trained
 */
uint8 lm_trie_quant_to_train(lm_trie_quant_t *quant);

/**
 * Gets quantizing type. Casts enum to int and returns it
 */
int lm_trie_quant_type(lm_trie_quant_t *quant);

/**
 * Trains prob and backoff quantizer for specified ngram order on provided raw ngram list
 */
void lm_trie_quant_train(lm_trie_quant_t *quant, int order, uint64 counts, lm_ngram_t* raw_ngrams);

/**
 * Trains only prob quantizer for specified ngram order on provided raw ngram list
 */
void lm_trie_quant_train_prob(lm_trie_quant_t *quant, int order, uint64 counts, lm_ngram_t* raw_ngrams);

/**
 * Writes specified weight for middle-order ngram. Quantize it if needed
 */
void lm_trie_quant_mwrite(lm_trie_quant_t *quant, bit_adress_t adress, int order_minus_2, float prob, float backoff);

/**
 * Writes specified weight for largest-order ngram. Quantize it if needed
 */
void lm_trie_quant_lwrite(lm_trie_quant_t *quant, bit_adress_t adress, float prob);

/**
 * Reads and decodes if needed backoff for middle-order ngram
 */
float lm_trie_quant_mboread(lm_trie_quant_t *quant, bit_adress_t adress, int order_minus_2);

/**
 * Reads and decodes if needed prob for middle-order ngram
 */
float lm_trie_quant_mpread(lm_trie_quant_t *quant, bit_adress_t adress, int order_minus_2);

/**
 * Reads and decodes if needed prob for largest-order ngram
 */
float lm_trie_quant_lpread(lm_trie_quant_t *quant, bit_adress_t adress);

#endif /* __LM_TRIE_QUANT_H__ */