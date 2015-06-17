
#include <string.h>
#include <assert.h>

#include <sphinxbase/err.h>
#include <sphinxbase/pio.h>
#include <sphinxbase/strfuncs.h>
#include <sphinxbase/ckd_alloc.h>
#include <sphinxbase/priority_queue.h>

#include "ngrams_raw.h"

int ngram_comparator(const void *first_void, const void *second_void)
{
    static int order = -1;
    word_idx *first, *second, *end;

    if (first_void == NULL) {
        //technical usage, setuping order
        order = *(int *)second_void;
        return 0;
    }
    if (order < 2) {
        E_ERROR("Order for ngram comprator was not set\n");
        return 0;
    }
    first = ((ngram_raw_t *)first_void)->words;
    second = ((ngram_raw_t *)second_void)->words;
    end = first + order;
    for (; first != end; ++first, ++second) {
        if (*first < *second) return -1;
        if (*first > *second) return 1;
    }
    return 0;
}

int ngram_ord_comparator(void *a_raw, void *b_raw)
{
    ngram_raw_ord_t *a = (ngram_raw_ord_t *)a_raw;
    ngram_raw_ord_t *b = (ngram_raw_ord_t *)b_raw;
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

static void read_ngram_instance(lineiter_t **li, hash_table_t *wid, logmath_t *lmath, int order, int order_max, ngram_raw_t *raw_ngram)
{
    int n;
    int words_expected;
    int i;
    char *wptr[MAX_NGRAM_ORDER + 1];
    word_idx *word_out;

    *li = lineiter_next(*li);
    if (*li == NULL) {
        E_ERROR("Unexpected end of ARPA file. Failed to read %d-gram\n", order);
        return;
    }
    string_trim((*li)->buf, STRING_BOTH);
    words_expected = order == order_max ? order + 1 : order + 2;
    if ((n = str2words((*li)->buf, wptr, MAX_NGRAM_ORDER + 1)) < words_expected) {
        if ((*li)->buf[0] != '\0') {
            E_WARN("Format error; %d-gram ignored: %s\n", order, (*li)->buf);
        }
    } else {
        if (order == order_max) {
            raw_ngram->weights = (float *)ckd_calloc(1, sizeof(*raw_ngram->weights));
            raw_ngram->weights[0] = (float)atof_c(wptr[0]);
            if (raw_ngram->weights[0] > 0) {
                E_WARN("%d-gram [%s] has positive probability. Zeroize\n", order, wptr[1]);
                raw_ngram->weights[0] = 0.0f;
            }
            raw_ngram->weights[0] = logmath_log10_to_log_float(lmath, raw_ngram->weights[0]);
        } else {
            raw_ngram->weights = (float *)ckd_calloc(2, sizeof(*raw_ngram->weights));
            raw_ngram->weights[0] = (float)atof_c(wptr[0]);
            if (raw_ngram->weights[0] > 0) {
                E_WARN("%d-gram [%s] has positive probability. Zeroize\n", order, wptr[1]);
                raw_ngram->weights[0] = 0.0f;
            }
            raw_ngram->weights[0] = logmath_log10_to_log_float(lmath, raw_ngram->weights[0]);
            raw_ngram->weights[1] = (float)atof_c(wptr[order + 1]);
            raw_ngram->weights[1] = logmath_log10_to_log_float(lmath, raw_ngram->weights[1]);
            //TODO classify float with fpclassify and warn if bad value occurred
        }
        raw_ngram->words = (word_idx *)ckd_calloc(order, sizeof(*raw_ngram->words));
        for (word_out = raw_ngram->words + order - 1, i = 1; word_out >= raw_ngram->words; --word_out, i++) {
            hash_table_lookup_int32(wid, wptr[i], (int32 *)word_out);
        }
    }
}

static void ngrams_raw_read_order(ngram_raw_t **raw_ngrams, lineiter_t **li, hash_table_t *wid, logmath_t *lmath, uint32 count, int order, int order_max)
{
    char expected_header[20];
    uint32 i;

    sprintf(expected_header, "\\%d-grams:", order);
    while ((*li = lineiter_next(*li))) {
        string_trim((*li)->buf, STRING_BOTH);
        if (strcmp((*li)->buf, expected_header) == 0)
            break;
    }
    *raw_ngrams = (ngram_raw_t *)ckd_calloc(count, sizeof(ngram_raw_t));
    for (i = 0; i < count; i++) {
        read_ngram_instance(li, wid, lmath, order, order_max, &((*raw_ngrams)[i]));
    }

    //sort raw ngrams that was read
    ngram_comparator(NULL, &order); //setting up order in comparator
    qsort(*raw_ngrams, count, sizeof(ngram_raw_t), &ngram_comparator);
}

ngram_raw_t** ngrams_raw_read_arpa(lineiter_t **li, hash_table_t *wid, logmath_t *lmath, uint32 *counts, int order)
{
    ngram_raw_t **raw_ngrams;
    int order_it;

    raw_ngrams = (ngram_raw_t **)ckd_calloc(order - 1, sizeof(*raw_ngrams));
    for (order_it = 2; order_it <= order; order_it++) {
        ngrams_raw_read_order(&raw_ngrams[order_it - 2], li, wid, lmath, counts[order_it - 1], order_it, order);
    }
    //check for end-mark in arpa file
    *li = lineiter_next(*li);
    string_trim((*li)->buf, STRING_BOTH);
    //skip empty lines if any
    while (*li && strlen((*li)->buf) == 0) {
        *li = lineiter_next(*li);
        string_trim((*li)->buf, STRING_BOTH);
    }
    //check if we finished reading
    if (*li == NULL)
        E_ERROR("ARPA file ends without end-mark\n");
    //check if we found ARPA end-mark
    if (strcmp((*li)->buf, "\\end\\") != 0)
        E_ERROR("Finished reading ARPA file. Expecting end mark but found [%s]\n", (*li)->buf);

    return raw_ngrams;
}

void ngrams_raw_fix_counts(ngram_raw_t **raw_ngrams, uint32 *counts, uint32 *fixed_counts, int order)
{
    priority_queue_t *ngrams = priority_queue_create(order - 1, &ngram_ord_comparator);
    uint32 raw_ngram_ptrs[MAX_NGRAM_ORDER - 1];
    word_idx words[MAX_NGRAM_ORDER];
    int i;

    memset(words, -1, sizeof(words)); //since we have unsigned word idx that will give us unreachable MAX_WORD_IDX
    memcpy(fixed_counts, counts, order * sizeof(*fixed_counts));
    for (i = 2; i <= order; ++i) {
        ngram_raw_ord_t *tmp_ngram = (ngram_raw_ord_t *)ckd_calloc(1, sizeof(*tmp_ngram));
        tmp_ngram->order = i;
        raw_ngram_ptrs[i-2] = 0;
        tmp_ngram->instance = raw_ngrams[i - 2][raw_ngram_ptrs[i-2]];
        priority_queue_add(ngrams, tmp_ngram);
    }

    for (;;) {
        uint8 to_increment = TRUE;
        ngram_raw_ord_t *top;
        if (priority_queue_size(ngrams) == 0) {
            break;
        }
        top = (ngram_raw_ord_t *)priority_queue_poll(ngrams);
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
            priority_queue_add(ngrams, top);
        } else {
            ckd_free(top);
        }
    }

    assert(priority_queue_size(ngrams) == 0);
    priority_queue_free(ngrams, NULL);
}

void ngrams_raw_free(ngram_raw_t **raw_ngrams, uint32 *counts, int order)
{
    uint32 num;
    int order_it;

    for (order_it = 0; order_it < order - 1; order_it++) {
        for (num = 0; num < counts[order_it + 1]; num++) {
            ckd_free(raw_ngrams[order_it][num].weights);
            ckd_free(raw_ngrams[order_it][num].words);
        }
        ckd_free(raw_ngrams[order_it]);
    }
    ckd_free(raw_ngrams);
}