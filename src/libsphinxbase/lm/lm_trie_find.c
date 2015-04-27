
#include "lm_trie_find.h"
#include "lm_trie_bits.h"

unigram_t* unigram_find(unigram_t *u, word_idx word, node_range_t *next)
{
    unigram_t *ptr = &u[word];
    next->begin = ptr->next;
    next->end = (ptr+1)->next;
    return ptr;
}

static size_t calc_pivot(uint32 off, uint32 range, uint32 width)
{
    return (size_t)((off * width) / (range + 1));
}

uint8 lm_trie_find(
    void *base, uint8 total_bits, uint8 key_bits, uint32 key_mask,
    uint32 before_it, uint32 before_v,
    uint32 after_it, uint32 after_v,
    uint32 key, uint32 *out)
{
    while (after_it - before_it > 1) {
        uint32 pivot = before_it + (1 + calc_pivot(key - before_v, after_v - before_v, after_it - before_it - 1));
        //access by pivot
        uint32 mid = read_int25(base, pivot * (uint32)total_bits, key_bits, key_mask);
        if (mid < key) {
            before_it = pivot;
            before_v = mid;
        } else if (mid > key) {
            after_it = pivot;
            after_v = mid;
        } else {
            *out = pivot;
            return TRUE;
        }
    }
    return FALSE;
}