
#include <string.h>
#include <stdio.h>

#include <sphinxbase/priority_queue.h>
#include <sphinxbase/prim_type.h>
#include <sphinxbase/ckd_alloc.h>
#include <sphinxbase/err.h>

#include "lm_trie.h"
#include "lm_trie_misc.h"
#include "lm_trie_quant.h"
#include "lm_trie_find.h"

typedef struct gram_s {
    lm_ngram_t instance;
    int order;
}gram_t;

static uint64 base_size(uint64 entries, uint64 max_vocab, uint8 remaining_bits)
{
    uint8 total_bits = required_bits(max_vocab) + remaining_bits;
    // Extra entry for next pointer at the end.  
    // +7 then / 8 to round up bits and convert to bytes
    // +sizeof(uint64_t) so that ReadInt57 etc don't go segfault.  
    // Note that this waste is O(order), not O(number of ngrams).
    return ((1 + entries) * total_bits + 7) / 8 + sizeof(uint64);
}

uint64 middle_size(uint8 quant_bits, uint64 entries, uint64 max_vocab, uint64 max_ptr)
{
    return base_size(entries, max_vocab, quant_bits + required_bits(max_ptr));
}

uint64 longest_size(uint8 quant_bits, uint64 entries, uint64 max_vocab)
{
    return base_size(entries, max_vocab, quant_bits);
}

static void base_init(base_t *base, void *base_mem, uint64 max_vocab, uint8 remaining_bits)
{
    bit_packing_sanity();
    base->word_bits = required_bits(max_vocab);
    base->word_mask = (1ULL << base->word_bits) - 1ULL;
    if (base->word_bits > 57)
        E_ERROR("Sorry, word indices more than %llu are not implemented.  Edit util/bit_packing.hh and fix the bit packing functions\n", (1ULL << 57));
    base->total_bits = base->word_bits + remaining_bits;

    base->base = (uint8 *)base_mem;
    base->insert_index = 0;
    base->max_vocab = max_vocab;
}

void middle_init(middle_t *middle, void *base_mem, uint8 quant_bits, uint64 entries, uint64 max_vocab, uint64 max_next, void *next_source)
{
    middle->quant_bits = quant_bits;
    bit_mask_from_max(&middle->next_mask, max_next);
    middle->next_source = next_source;
    if (entries + 1 >= (1ULL << 57) || (max_next >= (1ULL << 57)))
        E_ERROR("Sorry, this does not support more than %llu n-grams of a particular order.  Edit util/bit_packing.hh and fix the bit packing functions\n", (1ULL << 57));
    base_init(&middle->base, base_mem, max_vocab, quant_bits + middle->next_mask.bits);
}

void longest_init(longest_t *longest, void *base_mem, uint8 quant_bits, uint64 max_vocab)
{
    base_init(&longest->base, base_mem, max_vocab, quant_bits);
}

static bit_adress_t middle_insert(middle_t *middle, word_idx word, int order, int max_order)
{
    uint64 at_pointer;
    uint64 next;
    bit_adress_t adress;
    assert(word <= middle->base.word_mask);
    at_pointer = middle->base.insert_index * middle->base.total_bits;
    write_int57(middle->base.base, at_pointer, middle->base.word_bits, word);
    at_pointer += middle->base.word_bits;
    adress.base = middle->base.base;
    adress.offset = at_pointer;
    at_pointer += middle->quant_bits;
    if (order == max_order - 1) {
        next = ((longest_t *)middle->next_source)->base.insert_index;
    } else {
        next = ((middle_t *)middle->next_source)->base.insert_index;
    }
    //bhiksha write next
    write_int57(middle->base.base, at_pointer, middle->next_mask.bits, next);
    middle->base.insert_index++;
    return adress;
}

static bit_adress_t longest_insert(longest_t *longest, word_idx index)
{
    uint64 at_pointer;
    bit_adress_t adress;
    assert(index <= longest->base.word_mask);
    at_pointer = longest->base.insert_index * longest->base.total_bits;
    write_int57(longest->base.base, at_pointer, longest->base.word_bits, index);
    at_pointer += longest->base.word_bits;
    longest->base.insert_index++;
    adress.offset = at_pointer;
    adress.base = longest->base.base;
    return adress;
}

static void middle_finish_loading(middle_t *middle, uint64 next_end)
{
    uint64 last_next_write = (middle->base.insert_index + 1) * middle->base.total_bits - middle->next_mask.bits;
    write_int57(middle->base.base, last_next_write, middle->next_mask.bits, next_end);
}

static size_t unigram_size(uint64 count)
{
    //+1 for the final next. 
    return (size_t)(count + 1) * sizeof(unigram_t);
}

static uint64 lm_trie_size(lm_trie_quant_type_t quant_type, uint64 *counts, int order)
{
    int i;
    uint64 res = lm_trie_quant_size(quant_type, order) + unigram_size(counts[0]);
    for (i = 1; i < order - 1; i++)
        res += middle_size(lm_trie_quant_msize(quant_type), counts[i], counts[0], counts[i+1]);
    return res + longest_size(lm_trie_quant_lsize(quant_type), counts[order - 1], counts[0]);
}

int gram_compare(void *a_raw, void *b_raw)
{
    gram_t *a = (gram_t *)a_raw;
    gram_t *b = (gram_t *)b_raw;
    int a_w_ptr = 0;
    int b_w_ptr = 0;
    while (a_w_ptr < a->order && b_w_ptr < b->order) {
        if (a->instance.words[a_w_ptr] == b->instance.words[b_w_ptr]) {
            a_w_ptr++;
            b_w_ptr++;
            continue;
        }
        if (a->instance.words[a_w_ptr] < b->instance.words[b_w_ptr])
            return 1;
        else
            return -1;
    }
    return b->order - a->order;
}

static uint64 unigram_next(lm_trie_t *trie, int order)
{
    return order == 2 ? trie->longest->base.insert_index : trie->middle_begin->base.insert_index;
}

static void recursice_insert(lm_trie_t *trie, lm_ngram_t **raw_ngrams, uint64 *counts, int order)
{
    word_idx unigram_idx = 0;
    const word_idx unigram_count = (word_idx)counts[0];
    priority_queue_t *grams = priority_queue_create(order, &gram_compare);
    gram_t *gram;
    uint64 *raw_ngrams_ptr;
    int i;
    
    gram = (gram_t *)ckd_calloc(1, sizeof(*gram));
    gram->order = 1;
    gram->instance.words = &unigram_idx;
    priority_queue_add(grams, gram);
    raw_ngrams_ptr = (uint64 *)ckd_calloc(order - 1, sizeof(*raw_ngrams_ptr));
    for (i = 2; i <= order; ++i) {
        gram_t *tmp_gram = (gram_t *)ckd_calloc(1, sizeof(*tmp_gram));
        tmp_gram->order = i;
        raw_ngrams_ptr[i-2] = 0;
        tmp_gram->instance = raw_ngrams[i - 2][raw_ngrams_ptr[i-2]];
        priority_queue_add(grams, tmp_gram);
    }

    for (;;) {
        gram_t *top = (gram_t *)priority_queue_poll(grams);
        if (top->order == 1) {
            trie->unigrams[unigram_idx].next = unigram_next(trie, order);
            if (++unigram_idx == unigram_count + 1) {
                ckd_free(top);
                break;
            }
            priority_queue_add(grams, top);
        } else {
            if (top->order == order) {
                float *weights = top->instance.weights;
                bit_adress_t adress = longest_insert(trie->longest, top->instance.words[top->order - 1]);
                lm_trie_quant_lwrite(trie->quant, adress, weights[0]);
            } else {
                float *weights = top->instance.weights;
                middle_t *middle = &trie->middle_begin[top->order - 2];
                bit_adress_t adress = middle_insert(middle, top->instance.words[top->order - 1], top->order, order);
                //write prob and backoff
                lm_trie_quant_mwrite(trie->quant, adress, top->order - 2, weights[0], weights[1]);
            }
            raw_ngrams_ptr[top->order - 2]++;
            if (raw_ngrams_ptr[top->order - 2] < counts[top->order - 1]) {
                top->instance = raw_ngrams[top->order-2][raw_ngrams_ptr[top->order - 2]];
                priority_queue_add(grams, top);
            } else {
                ckd_free(top);
            }
        }
    }
    assert(priority_queue_size(grams) == 0);
    priority_queue_free(grams, NULL);
    ckd_free(raw_ngrams_ptr);
}

static void lm_trie_map_mem(lm_trie_t *trie, lm_trie_quant_type_t quant_type, uint64 *counts, int order)
{
    uint8 *mem_ptr;
    uint8 **middle_starts;
    int i;

    mem_ptr = trie->mem;
    trie->quant = lm_trie_quant_create(quant_type, mem_ptr, order);
    mem_ptr += lm_trie_quant_size(quant_type, order);
    trie->unigrams = (unigram_t *)mem_ptr;
    mem_ptr += unigram_size(counts[0]);
    trie->middle_begin = (middle_t *)ckd_calloc(order - 2, sizeof(*trie->middle_begin));
    trie->middle_end = trie->middle_begin + (order - 2);
    middle_starts = (uint8 **)ckd_calloc(order - 2, sizeof(*middle_starts));
    for (i = 2; i < order; i++) {
        middle_starts[i-2] = mem_ptr;
        mem_ptr += middle_size(lm_trie_quant_msize(quant_type), counts[i-1], counts[0], counts[i]);
    }
    trie->longest = (longest_t *)ckd_calloc(1, sizeof(*trie->longest));
    // Crazy backwards thing so we initialize using pointers to ones that have already been initialized
    for (i = order - 1; i >= 2; --i) {
        middle_t *middle_ptr = &trie->middle_begin[i - 2];
        middle_init(middle_ptr, middle_starts[i-2], lm_trie_quant_msize(quant_type), counts[i-1], counts[0], counts[i], 
            (i == order - 1) ? (void *)trie->longest : (void *)&trie->middle_begin[i-1]);
    }
    longest_init(trie->longest, mem_ptr, lm_trie_quant_lsize(quant_type), counts[0]);
}

lm_trie_t* lm_trie_create(lm_trie_quant_type_t quant_type, uint64 *counts, int order)
{
    lm_trie_t* trie;

    trie = (lm_trie_t *)ckd_calloc(1, sizeof(*trie));
    trie->mem_size = lm_trie_size(quant_type, counts, order);
    //TODO uint64 to size_t cast
    trie->mem = (uint8 *)ckd_calloc((size_t)trie->mem_size, sizeof(*trie->mem));
    lm_trie_map_mem(trie, quant_type, counts, order);
    return trie;
}

void lm_trie_build(lm_trie_t *trie, lm_ngram_t **raw_ngrams, uint64 *counts, int order)
{
    int i;
    if (lm_trie_quant_to_train(trie->quant)) {
        E_INFO("Training quantizer\n");
        for (i = 2; i < order; i++) {
            lm_trie_quant_train(trie->quant, i, counts[i - 1], raw_ngrams[i-2]); 
        }
        lm_trie_quant_train_prob(trie->quant, order, counts[order - 1], raw_ngrams[order - 2]);
    }
    E_INFO("Building LM trie\n");
    //TODO casting uint64 to word_idx
    recursice_insert(trie, raw_ngrams, counts, order);
    /* Set ending offsets so the last entry will be sized properly */
    // Last entry for unigrams was already set.  
    if (trie->middle_begin != trie->middle_end) {
        middle_t *middle_ptr;
        for (middle_ptr = trie->middle_begin; middle_ptr != trie->middle_end - 1; ++middle_ptr) {
            middle_t *next_middle_ptr = middle_ptr + 1;
            middle_finish_loading(middle_ptr, next_middle_ptr->base.insert_index);
        }
        middle_ptr = trie->middle_end - 1;
        middle_finish_loading(middle_ptr, trie->longest->base.insert_index);
    }
}

//void lm_trie_write_bin(lm_trie_t *trie, FILE *fb)
//{
//    //uint64 to size_t convertion
//    fwrite(trie->mem, 1, (size_t)trie->mem_size, fb);
//}
//
//tsearch_t* tsearch_read_bin(FILE *fb, uint64* counts, int order, int quant_type_int)
//{
//    quant_type_t quant_type = (quant_type_t)quant_type_int;
//    tsearch_t *search = (tsearch_t *)ckd_calloc(1, sizeof(*search));
//    search->mem_size = tsearch_size(quant_type, counts, order);
//    //TODO cast from uint64 to size_t
//    search->mem = (uint8 *)ckd_calloc((size_t)search->mem_size, sizeof(*search->mem));
//    fread(search->mem, 1, (size_t)search->mem_size, fb);
//    tsearch_init(search, quant_type, counts, order);
//    return search;
//}
//
//static void copy_remaining_history(const word_idx *from, state_t *out_state)
//{
//    const word_idx *in;
//    word_idx *out = &out_state->words[1];
//    const word_idx *in_end = &from[out_state->length - 1];
//    for (in = from; in < in_end; in++, out++) *out = *in;
//}
//
//static void resume_score(tsearch_t *search, const word_idx *hist_iter, const word_idx *const context_rend, int order_minus_2, int max_order, node_range_t *node, float *backoff_out, int *next_use, score_return_t *ret)
//{
//    bit_adress_t adress;
//    for (; ; ++order_minus_2, ++hist_iter, ++backoff_out) {
//        if (hist_iter == context_rend) return;
//        if (ret->independent_left) return;
//        if (order_minus_2 == max_order - 2) break;
//
//        adress = middle_find(&search->middle_begin[order_minus_2], *hist_iter, node, &ret->extend_left);
//        ret->independent_left = (adress.base == NULL) || (node->begin == node->end);
//
//        //didn't find entry
//        if (adress.base == NULL) return;
//        *backoff_out = quant_read_middle_backoff(search->quant, adress, order_minus_2);
//        ret->prob = quant_read_middle_prob(search->quant, adress, order_minus_2);
//        ret->rest = ret->prob; //TODO why do we need ret->rest?
//        ret->ngram_length = order_minus_2 + 2;
//        if (has_extension(*backoff_out)) {
//            *next_use = ret->ngram_length;
//        }
//    }
//    ret->independent_left = TRUE;
//    adress = longest_find(search->longest, *hist_iter, node);
//    if (adress.base != NULL) {
//        ret->prob = quant_read_longest_prob(search->quant, adress);
//        ret->rest = ret->prob;
//        ret->ngram_length = max_order;
//    }
//}
//
//static prob_bo_t* lookup_unigram(unigram_t *unigram, word_idx word, node_range_t* next, uint8 *independent_left, uint64 *extend_left)
//{
//    prob_bo_t *weights = unigram_find(unigram, word, next);
//    *extend_left = (uint64)(word);
//    *independent_left = (next->begin == next->end);
//    return weights;
//}
//
//score_return_t score_except_backoff(tsearch_t *search,
//                                    int max_order,
//                                    const word_idx *const context_rbegin, 
//                                    const word_idx *const context_rend,
//                                    const word_idx new_word,
//                                    state_t *out_state)
//{
//    score_return_t ret;
//    node_range_t node;
//    prob_bo_t *weights = lookup_unigram(search->unigrams, new_word, &node, &ret.independent_left, &ret.extend_left);
//    // ret.ngram_length contains the last known non-blank ngram length.
//    ret.ngram_length = 1;
//    out_state->backoff[0] = weights->backoff;
//    ret.prob = weights->prob;
//    ret.rest = weights->prob;
//
//    // This is the length of the context that should be used for continuation to the right.
//    out_state->length = has_extension(out_state->backoff[0]) ? 1 : 0;
//    // We'll write the word anyway since it will probably be used and does no harm being there.  
//    out_state->words[0] = new_word;
//    if (context_rbegin == context_rend) return ret;
//    resume_score(search, context_rbegin, context_rend, 0, max_order, &node, &out_state->backoff[1], &out_state->length, &ret);
//    copy_remaining_history(context_rbegin, out_state);
//    return ret;
//}