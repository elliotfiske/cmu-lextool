
#include "lm_trie_find.h"
#include "lm_trie_bits.h"

//uint8 lm_trie_find_ptr(
//    size_t (*calc_pivot)(uint64 off, uint64 range, uint64 width),
//    uint64 *before_it, uint64 before_v,
//    uint64 *after_it, uint64 after_v,
//    uint64 key, const uint64 **out)
//{
//    while (after_it - before_it > 1) {
//        uint64 *pivot = before_it + (1 + calc_pivot(key - before_v, after_v - before_v, after_it - before_it - 1));
//        uint64 mid = *pivot;
//        if (mid < key) {
//            before_it = pivot;
//            before_v = mid;
//        } else if (mid > key) {
//            after_it = pivot;
//            after_v = mid;
//        } else {
//            *out = pivot;
//            return TRUE;
//        }
//    }
//    return FALSE;
//}

uint8 lm_trie_find(
    size_t (*calc_pivot)(uint64 off, uint64 range, uint64 width),
    void *base, uint8 total_bits, uint8 key_bits, uint64 key_mask,
    uint64 before_it, uint64 before_v,
    uint64 after_it, uint64 after_v,
    uint64 key, uint64 *out)
{
    while (after_it - before_it > 1) {
        uint64 pivot = before_it + (1 + calc_pivot(key - before_v, after_v - before_v, after_it - before_it - 1));
        //access by pivot
        uint64 mid = read_int57(base, pivot * (uint64)total_bits, key_bits, key_mask);
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