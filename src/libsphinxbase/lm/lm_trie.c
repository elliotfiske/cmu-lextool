
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

static uint32 base_size(uint32 entries, uint32 max_vocab, uint8 remaining_bits)
{
    uint8 total_bits = required_bits(max_vocab) + remaining_bits;
    // Extra entry for next pointer at the end.  
    // +7 then / 8 to round up bits and convert to bytes
    // +sizeof(uint64) so that ReadInt57 etc don't go segfault.  
    // Note that this waste is O(order), not O(number of ngrams).
    return ((1 + entries) * total_bits + 7) / 8 + sizeof(uint64);
}

uint32 middle_size(uint8 quant_bits, uint32 entries, uint32 max_vocab, uint32 max_ptr)
{
    return base_size(entries, max_vocab, quant_bits + required_bits(max_ptr));
}

uint32 longest_size(uint8 quant_bits, uint32 entries, uint32 max_vocab)
{
    return base_size(entries, max_vocab, quant_bits);
}

static void base_init(base_t *base, void *base_mem, uint32 max_vocab, uint8 remaining_bits)
{
    bit_packing_sanity();
    base->word_bits = required_bits(max_vocab);
    base->word_mask = (1U << base->word_bits) - 1U;
    if (base->word_bits > 25)
        E_ERROR("Sorry, word indices more than %d are not implemented.  Edit util/bit_packing.hh and fix the bit packing functions\n", (1U << 25));
    base->total_bits = base->word_bits + remaining_bits;

    base->base = (uint8 *)base_mem;
    base->insert_index = 0;
    base->max_vocab = max_vocab;
}

void middle_init(middle_t *middle, void *base_mem, uint8 quant_bits, uint32 entries, uint32 max_vocab, uint32 max_next, void *next_source)
{
    middle->quant_bits = quant_bits;
    bit_mask_from_max(&middle->next_mask, max_next);
    middle->next_source = next_source;
    if (entries + 1 >= (1U << 25) || (max_next >= (1U << 25)))
        E_ERROR("Sorry, this does not support more than %d n-grams of a particular order.  Edit util/bit_packing.hh and fix the bit packing functions\n", (1U << 25));
    base_init(&middle->base, base_mem, max_vocab, quant_bits + middle->next_mask.bits);
}

void longest_init(longest_t *longest, void *base_mem, uint8 quant_bits, uint32 max_vocab)
{
    base_init(&longest->base, base_mem, max_vocab, quant_bits);
}

static bit_adress_t middle_insert(middle_t *middle, word_idx word, int order, int max_order)
{
    uint32 at_pointer;
    uint32 next;
    bit_adress_t adress;
    assert(word <= middle->base.word_mask);
    at_pointer = middle->base.insert_index * middle->base.total_bits;
    write_int25(middle->base.base, at_pointer, middle->base.word_bits, word);
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
    write_int25(middle->base.base, at_pointer, middle->next_mask.bits, next);
    middle->base.insert_index++;
    return adress;
}

static bit_adress_t longest_insert(longest_t *longest, word_idx index)
{
    uint32 at_pointer;
    bit_adress_t adress;
    assert(index <= longest->base.word_mask);
    at_pointer = longest->base.insert_index * longest->base.total_bits;
    write_int25(longest->base.base, at_pointer, longest->base.word_bits, index);
    at_pointer += longest->base.word_bits;
    longest->base.insert_index++;
    adress.offset = at_pointer;
    adress.base = longest->base.base;
    return adress;
}

static void middle_finish_loading(middle_t *middle, uint32 next_end)
{
    uint32 last_next_write = (middle->base.insert_index + 1) * middle->base.total_bits - middle->next_mask.bits;
    write_int25(middle->base.base, last_next_write, middle->next_mask.bits, next_end);
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

static uint32 unigram_next(lm_trie_t *trie, int order)
{
    return order == 2 ? trie->longest->base.insert_index : trie->middle_begin->base.insert_index;
}

static void recursive_insert(lm_trie_t *trie, lm_ngram_t **raw_ngrams, uint32 *counts, int order)
{
    word_idx unigram_idx = 0;
    word_idx *words;
    float *probs;
    const word_idx unigram_count = (word_idx)counts[0];
    priority_queue_t *grams = priority_queue_create(order, &gram_compare);
    gram_t *gram;
    uint32 *raw_ngrams_ptr;
    int i;

    words = (word_idx *)ckd_calloc(order, sizeof(*words)); //for blanks catching
    probs = (float *)ckd_calloc(order - 1, sizeof(*probs));    //for blanks prob generating
    gram = (gram_t *)ckd_calloc(1, sizeof(*gram));
    gram->order = 1;
    gram->instance.words = &unigram_idx;
    priority_queue_add(grams, gram);
    raw_ngrams_ptr = (uint32 *)ckd_calloc(order - 1, sizeof(*raw_ngrams_ptr));
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
            words[0] = unigram_idx;
            probs[0] = trie->unigrams[unigram_idx].prob;
            if (++unigram_idx == unigram_count + 1) {
                ckd_free(top);
                break;
            }
            priority_queue_add(grams, top);
        } else {
            for (i = 0; i < top->order - 1; i++) {
                if (words[i] != top->instance.words[i]) {
                    //need to insert dummy suffixes to make ngram of higher order reachable
                    int j;
                    assert(i > 0); //unigrams are not pruned without removing ngrams that contains them
                    for (j = i; j < top->order - 1; j++) {
                        middle_t *middle = &trie->middle_begin[j - 1];
                        bit_adress_t adress = middle_insert(middle, top->instance.words[j], j + 1, order);
                        //calculate prob for blank
                        float calc_prob = probs[j - 1] + trie->unigrams[top->instance.words[j]].bo;
                        probs[j] = calc_prob;
                        lm_trie_quant_mwrite(trie->quant, adress, j - 1, calc_prob, 0.0f);
                    }
                }
            }
            memcpy(words, top->instance.words, top->order * sizeof(*words));
            if (top->order == order) {
                float *weights = top->instance.weights;
                bit_adress_t adress = longest_insert(trie->longest, top->instance.words[top->order - 1]);
                lm_trie_quant_lwrite(trie->quant, adress, weights[0]);
            } else {
                float *weights = top->instance.weights;
                middle_t *middle = &trie->middle_begin[top->order - 2];
                bit_adress_t adress = middle_insert(middle, top->instance.words[top->order - 1], top->order, order);
                //write prob and backoff
                probs[top->order - 1] = weights[0];
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
    ckd_free(words);
    ckd_free(probs);
}

static lm_trie_t* lm_trie_init(uint32 unigram_count)
{
    lm_trie_t* trie;

    trie = (lm_trie_t *)ckd_calloc(1, sizeof(*trie));
    memset(trie->prev_hist, -1, sizeof(trie->prev_hist)); //prepare request history
    memset(trie->backoff, 0, sizeof(trie->backoff));
    trie->unigrams = (unigram_t *)ckd_calloc((unigram_count + 1), sizeof(*trie->unigrams));
    trie->ngram_mem = NULL;
    return trie;
}

lm_trie_t* lm_trie_create(uint32 unigram_count, lm_trie_quant_type_t quant_type, int order)
{
    lm_trie_t* trie = lm_trie_init(unigram_count);
    trie->quant = (order > 1) ? lm_trie_quant_create(quant_type, order) : 0;
    return trie;
}

lm_trie_t* lm_trie_read_bin(uint32 *counts, int order, FILE *fp)
{
    lm_trie_t* trie = lm_trie_init(counts[0]);
    trie->quant = (order > 1) ? lm_trie_quant_read_bin(fp, order) : NULL;
    fread(trie->unigrams, sizeof(*trie->unigrams), (counts[0] + 1), fp);
    if (order > 1) {
        lm_trie_alloc_ngram(trie, counts, order);
        fread(trie->ngram_mem, 1, trie->ngram_mem_size, fp);
    }
    return trie;
}

void lm_trie_write_bin(lm_trie_t *trie, uint32 unigram_count, FILE *fp)
{

    if (trie->quant)
        lm_trie_quant_write_bin(trie->quant, fp);
    //uint64 to size_t convertion
    fwrite(trie->unigrams, sizeof(*trie->unigrams), (unigram_count + 1), fp);
    if (trie->ngram_mem)
        fwrite(trie->ngram_mem, 1, trie->ngram_mem_size, fp);
}

void lm_trie_free(lm_trie_t *trie)
{
    if (trie->ngram_mem) {
        ckd_free(trie->ngram_mem);
        ckd_free(trie->middle_begin);
        ckd_free(trie->longest);
    }
    if (trie->quant)
        lm_trie_quant_free(trie->quant);
    ckd_free(trie->unigrams);
    ckd_free(trie);
}

void lm_trie_fix_counts(lm_ngram_t **raw_ngrams, uint32 *counts, uint32 *fixed_counts, int order)
{
    priority_queue_t *grams = priority_queue_create(order - 1, &gram_compare);
    uint32 raw_ngram_ptrs[MAX_NGRAM_ORDER - 1];
    word_idx words[MAX_NGRAM_ORDER];
    int i;

    memset(words, -1, sizeof(words)); //since we have unsigned word idx that will give us unreachable MAX_WORD_IDX
    memcpy(fixed_counts, counts, order * sizeof(*fixed_counts));
    for (i = 2; i <= order; ++i) {
        gram_t *tmp_gram = (gram_t *)ckd_calloc(1, sizeof(*tmp_gram));
        tmp_gram->order = i;
        raw_ngram_ptrs[i-2] = 0;
        tmp_gram->instance = raw_ngrams[i - 2][raw_ngram_ptrs[i-2]];
        priority_queue_add(grams, tmp_gram);
    }

    for (;;) {
        uint8 to_increment = TRUE;
        gram_t *top;
        if (priority_queue_size(grams) == 0) {
            break;
        }
        top = (gram_t *)priority_queue_poll(grams);
        if (top->order == 2) {
            memcpy(words, top->instance.words, 2 * sizeof(*words));
        } else {
            for (i = 0; i < top->order - 1; i++) {
                if (words[i] != top->instance.words[i]) {
                    int num;
                    num = (i == 0) ? 1 : i;
                    memcpy(words, top->instance.words, (num + 1) * sizeof(*words));
                    fixed_counts[num]++;
                    to_increment = FALSE;
                    break;
                }
            }
            words[top->order - 1] = top->instance.words[top->order - 1];
        }
        if (to_increment) {
            raw_ngram_ptrs[top->order - 2]++;
        }
        if (raw_ngram_ptrs[top->order - 2] < counts[top->order - 1]) {
            top->instance = raw_ngrams[top->order-2][raw_ngram_ptrs[top->order - 2]];
            priority_queue_add(grams, top);
        } else {
            ckd_free(top);
        }
    }

    assert(priority_queue_size(grams) == 0);
    priority_queue_free(grams, NULL);
}

void lm_trie_alloc_ngram(lm_trie_t *trie, uint32 *counts, int order)
{
    int i;
    uint8 *mem_ptr;
    uint8 **middle_starts;

    trie->ngram_mem_size = 0;
    for (i = 1; i < order - 1; i++) {
        trie->ngram_mem_size += middle_size(lm_trie_quant_msize(trie->quant), counts[i], counts[0], counts[i+1]);
    }
    trie->ngram_mem_size += longest_size(lm_trie_quant_lsize(trie->quant), counts[order - 1], counts[0]);
    trie->ngram_mem = (uint8 *)ckd_calloc(trie->ngram_mem_size, sizeof(*trie->ngram_mem));
    mem_ptr = trie->ngram_mem;
    trie->middle_begin = (middle_t *)ckd_calloc(order - 2, sizeof(*trie->middle_begin));
    trie->middle_end = trie->middle_begin + (order - 2);
    middle_starts = (uint8 **)ckd_calloc(order - 2, sizeof(*middle_starts));
    for (i = 2; i < order; i++) {
        middle_starts[i-2] = mem_ptr;
        mem_ptr += middle_size(lm_trie_quant_msize(trie->quant), counts[i-1], counts[0], counts[i]);
    }
    trie->longest = (longest_t *)ckd_calloc(1, sizeof(*trie->longest));
    // Crazy backwards thing so we initialize using pointers to ones that have already been initialized
    for (i = order - 1; i >= 2; --i) {
        middle_t *middle_ptr = &trie->middle_begin[i - 2];
        middle_init(middle_ptr, middle_starts[i-2], lm_trie_quant_msize(trie->quant), counts[i-1], counts[0], counts[i], 
            (i == order - 1) ? (void *)trie->longest : (void *)&trie->middle_begin[i-1]);
    }
    ckd_free(middle_starts);
    longest_init(trie->longest, mem_ptr, lm_trie_quant_lsize(trie->quant), counts[0]);
}

void lm_trie_build(lm_trie_t *trie, lm_ngram_t **raw_ngrams, uint32 *counts, int order)
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
    recursive_insert(trie, raw_ngrams, counts, order);
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