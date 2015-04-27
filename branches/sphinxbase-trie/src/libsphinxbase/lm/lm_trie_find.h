#ifndef __LM_TRIE_FIND_H__
#define __LM_TRIE_FIND_H__

#include <stdio.h>
#include <sphinxbase/prim_type.h>

#include "lm_trie.h"

unigram_t* unigram_find(unigram_t *u, word_idx word, node_range_t *next);

uint8 lm_trie_find(
    void *base, uint8 total_bits, uint8 key_bits, uint32 key_mask,
    uint32 before_it, uint32 before_v,
    uint32 after_it, uint32 after_v,
    uint32 key, uint32 *out);

#endif /* __LM_TRIE_FIND_H__ */