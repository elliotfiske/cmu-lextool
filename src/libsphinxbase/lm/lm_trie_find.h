#ifndef __LM_TRIE_FIND_H__
#define __LM_TRIE_FIND_H__

#include <stdio.h>
#include <sphinxbase/prim_type.h>

//__inline static size_t calc_pivot64(uint64 off, uint64 range, uint64 width)
//{
//    size_t ret = (size_t)((float)(off) / (float)(range) * (float)(width));
//    // Cap for floating point rounding
//    return (ret < width) ? ret : (size_t)width - 1;
//}

__inline static size_t calc_pivot32(uint64 off, uint64 range, uint64 width)
{
    return (size_t)((off * width) / (range + 1));
}

//uint8 lm_trie_find_ptr(
//    size_t (*calc_pivot)(uint64 off, uint64 range, uint64 width),
//    uint64 *before_it, uint64 before_v,
//    uint64 *after_it, uint64 after_v,
//    uint64 key, const uint64 **out);

uint8 lm_trie_find(
    size_t (*calc_pivot)(uint64 off, uint64 range, uint64 width),
    void *base, uint8 total_bits, uint8 key_bits, uint64 key_mask,
    uint64 before_it, uint64 before_v,
    uint64 after_it, uint64 after_v,
    uint64 key, uint64 *out);

#endif /* __LM_TRIE_FIND_H__ */