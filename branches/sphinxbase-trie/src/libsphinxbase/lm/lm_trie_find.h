#ifndef __LM_TRIE_FIND_H__
#define __LM_TRIE_FIND_H__

#include <stdio.h>
#include <sphinxbase/prim_type.h>

#include "lm_trie.h"

unigram_t* unigram_find(unigram_t *u, word_idx word, node_range_t *next);

uint8 lm_trie_find(
    void *base, uint8 total_bits, uint8 key_bits, uint64 key_mask,
    uint64 before_it, uint64 before_v,
    uint64 after_it, uint64 after_v,
    uint64 key, uint64 *out);

#endif /* __LM_TRIE_FIND_H__ */