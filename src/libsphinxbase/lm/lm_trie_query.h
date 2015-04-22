#ifndef __LM_TRIE_QUERY_H__
#define __LM_TRIE_QUERY_H__

#include "lm_trie.h"

float lm_trie_score(lm_trie_t *trie, int order, int32 wid, int32 *hist, int32 n_hist, int32 *n_used);

#endif /* __LM_TRIE_QUERY_H__ */