#ifndef __NGRAM_MODEL_TRIE_H__
#define __NGRAM_MODEL_TRIE_H__


#include <sphinxbase/prim_type.h>
#include <sphinxbase/logmath.h>

typedef struct ngram_model_trie_s ngram_model_trie_t;

void trie_write_bin(ngram_model_trie_t *model, const char *path);

ngram_model_trie_t* trie_read_bin(const char *path);

float trie_score(ngram_model_trie_t *model, const char*const *words, int32 n);

#endif /* __NGRAM_MODEL_TRIE_H__ */