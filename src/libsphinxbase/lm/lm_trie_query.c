
#include "lm_trie.h"
#include "lm_trie_find.h"
#include "lm_trie_quant.h"

static unigram_t* unigram_find(unigram_t *u, word_idx word, node_range_t *next)
{
    unigram_t *ptr = &u[word];
    next->begin = ptr->next;
    next->end = (ptr+1)->next;
    return ptr;
}

static bit_adress_t middle_find(middle_t *middle, word_idx word, node_range_t *range)
{
    uint64 at_pointer;
    uint64  bit_offset;
    bit_adress_t adress;

    //finding BitPacked with uniform find
    if (!lm_trie_find(&calc_pivot32, (void *)middle->base.base, middle->base.total_bits, middle->base.word_bits, middle->base.word_mask, range->begin - 1, (uint64)0, range->end, middle->base.max_vocab, word, &at_pointer)) {
        adress.base = NULL;
        adress.offset = 0;
        return adress;
    }
    at_pointer *= middle->base.total_bits;
    at_pointer += middle->base.word_bits;
    //bhiksha read next
    bit_offset = at_pointer + middle->quant_bits;
    range->begin = read_int57(middle->base.base, bit_offset, middle->next_mask.bits, middle->next_mask.mask);
    range->end = read_int57(middle->base.base, bit_offset + middle->base.total_bits, middle->next_mask.bits, middle->next_mask.mask);
    adress.base = middle->base.base;
    adress.offset = at_pointer;
    return adress;
}

static bit_adress_t longest_find(longest_t *longest, word_idx word, node_range_t *range)
{
    uint64 at_pointer;
    bit_adress_t adress;

    //finding BitPacked with uniform find
    if (!lm_trie_find(&calc_pivot32, (void *)longest->base.base, longest->base.total_bits, longest->base.word_bits, longest->base.word_mask, range->begin - 1, (uint64)0, range->end, longest->base.max_vocab, word, &at_pointer)) {
        adress.base = NULL;
        adress.offset = 0;
        return adress;
    }
    at_pointer = at_pointer * longest->base.total_bits + longest->base.word_bits;
    adress.base = longest->base.base;
    adress.offset = at_pointer;
    return adress;
}

static void resume_score(lm_trie_t *trie, int32* hist_iter, int max_order, int32* hist_end, node_range_t *node, float* prob, int32 *n_used)
{
    bit_adress_t adress;
    int order_minus_2 = 0;
    uint8 independent_left = (node->begin == node->end);

    for (;;order_minus_2++, hist_iter++) {
        if (hist_iter == hist_end) return;
        if (independent_left) return;
        if (order_minus_2 == max_order - 2) break;

        adress = middle_find(&trie->middle_begin[order_minus_2], *hist_iter, node);
        independent_left = (adress.base == NULL) || (node->begin == node->end);

        //didn't find entry
        if (adress.base == NULL) return;
        *prob =  lm_trie_quant_mpread(trie->quant, adress, order_minus_2);
        *n_used = order_minus_2 + 2;
    }

    adress = longest_find(trie->longest, *hist_iter, node);
    if (adress.base != NULL) {
        *prob = lm_trie_quant_lpread(trie->quant, adress);
        *n_used = max_order;
    }
}

static float score_except_backoff(lm_trie_t *trie, int32 wid, int32 *hist, int max_order, int32 n_hist, int32 *n_used)
{
    float prob;
    node_range_t node;
    *n_used = 1;
    prob = unigram_find(trie->unigrams, wid, &node)->prob;
    if (n_hist == 0) {
        return prob;
    }
    //find ngrams of higher order if any
    resume_score(trie, hist, max_order, hist + n_hist, &node, &prob, n_used);
    return prob;
}

static uint8 fast_make_node(lm_trie_t *trie, int32 *begin, int32 *end, node_range_t *node) {
    int32 *it;
    assert(begin != end);
    unigram_find(trie->unigrams, *begin, node);
    if (node->begin == node->end) {
        return FALSE;
    }

    for (it = begin + 1; it < end; ++it) {
        bit_adress_t adress = middle_find(&trie->middle_begin[it - begin - 1], *it, node);
        if (node->begin == node->end || adress.base == NULL) {
            return FALSE;
        }
    }
    return TRUE;
}

static float score_backoff(lm_trie_t *trie, int32 start, int32 *hist, int32 n_hist)
{
    float backoff = 0.0f;
    int order_minus_2;
    int32 *hist_iter;
    node_range_t node;
    if (start <= 1) {
        backoff += unigram_find(trie->unigrams, hist[0], &node)->bo;
        start = 2;
    } else if (!fast_make_node(trie, hist, hist + start - 1, &node)) {
        return backoff;
    }
    order_minus_2 = start - 2;
    for (hist_iter = hist + start - 1; hist_iter < hist + n_hist; hist_iter++, order_minus_2++) {
        bit_adress_t adress = middle_find(&trie->middle_begin[order_minus_2], *hist_iter, &node);
        if (adress.base == NULL) break;
        backoff += lm_trie_quant_mboread(trie->quant, adress, order_minus_2);
    }
    return backoff;
}

static float lm_trie_nobo_score(lm_trie_t *trie, int32 wid, int32 *hist, int max_order, int32 n_hist, int32 *n_used)
{
    float prob = score_except_backoff(trie, wid, hist, max_order, n_hist, n_used);
    if (n_hist < *n_used) return prob;
    return prob + score_backoff(trie, *n_used, hist, n_hist);
}

static float lm_trie_hist_score(lm_trie_t *trie, int32 wid, int32 *hist, int32 n_hist, int32 *n_used)
{
    float prob;
    int i, j;
    node_range_t node;
    bit_adress_t adress;

    *n_used = 1;
    prob = unigram_find(trie->unigrams, wid, &node)->prob;
    for (i = 0; i < n_hist - 1; i++) {
        adress = middle_find(&trie->middle_begin[i], hist[i], &node);
        if (adress.base == NULL) {
            for (j = i; j < n_hist; j++) {
                prob += trie->backoff[j];
            }
            return prob;
        } else {
            (*n_used)++;
            prob = lm_trie_quant_mpread(trie->quant, adress, i);
        }
    }
    adress = longest_find(trie->longest, hist[n_hist - 1], &node);
    if (adress.base == NULL) {
        return prob + trie->backoff[n_hist - 1];
    } else {
        (*n_used)++;
        return lm_trie_quant_lpread(trie->quant, adress);
    }
}

static uint8 history_matches(int32* hist, int32* prev_hist, int32 n_hist)
{
    int i;
    for (i = 0; i < n_hist; i++) {
        if (hist[i] != prev_hist[i]) {
            return FALSE;
        }
    }
    return TRUE;
}

static void update_backoff(lm_trie_t *trie, int32 *hist, int32 n_hist)
{
    int i;
    node_range_t node;
    bit_adress_t adress;

    memset(trie->backoff, 0, sizeof(trie->backoff));
    trie->backoff[0] = unigram_find(trie->unigrams, hist[0], &node)->bo;
    for (i = 1; i < n_hist; i++) {
        adress = middle_find(&trie->middle_begin[i - 1], hist[i], &node);
        if (adress.base == NULL) {
            break;
        }
        trie->backoff[i] = lm_trie_quant_mboread(trie->quant, adress, i - 1);
    }
    memcpy(trie->prev_hist, hist, n_hist * sizeof(*hist));
}

static float lm_trie_score(lm_trie_t *trie, int order, int32 wid, int32 *hist, int32 n_hist, int32 *n_used)
{
    if (n_hist < order - 1) {
        return lm_trie_nobo_score(trie, wid, hist, order, n_hist, n_used);
    } else {
        assert(n_hist == order - 1);
        if (!history_matches(hist, (int32 *)trie->prev_hist, n_hist)) {
            update_backoff(trie, hist, n_hist);
        }
        return lm_trie_hist_score(trie, wid, hist, n_hist, n_used);
    }
}