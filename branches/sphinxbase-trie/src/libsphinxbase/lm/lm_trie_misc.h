#ifndef __LM_TRIE_CONST_H__
#define __LM_TRIE_CONST_H__

#define MAX_NGRAM_ORDER 5
#define NO_EXTENSIONS_BACKOFF (-0.0f)
#define K_SIGN_BIT (0x80000000)
#define FLOAT_NEG_INF (0x7f800000)

/**
 * TODO This one is used widely. Where I should locate it?
 */

typedef unsigned int word_idx;

// This compiles down nicely.  
__inline static uint8 has_extension(const float backoff) {
  typedef union { float f; uint32 i; } union_value;
  union_value compare, interpret;
  compare.f = NO_EXTENSIONS_BACKOFF;
  interpret.f = backoff;
  return compare.i != interpret.i;
}

#endif /* __LM_TRIE_CONST_H__ */