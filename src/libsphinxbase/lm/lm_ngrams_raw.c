
#include <string.h>

#include <sphinxbase/err.h>
#include <sphinxbase/pio.h>
#include <sphinxbase/strfuncs.h>
#include <sphinxbase/ckd_alloc.h>

#include "lm_ngrams_raw.h"

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
    first = ((lm_ngram_t *)first_void)->words;
    second = ((lm_ngram_t *)second_void)->words;
    end = first + order;
    for (; first != end; ++first, ++second) {
        if (*first < *second) return -1;
        if (*first > *second) return 1;
    }
    return 0;
}

static void read_ngram_instance(lineiter_t **li, hash_table_t *wid, logmath_t *lmath, int order, int order_max, lm_ngram_t *raw_ngram)
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

static void lm_ngrams_raw_read_order(lm_ngram_t **raw_ngrams, lineiter_t **li, hash_table_t *wid, logmath_t *lmath, uint64 count, int order, int order_max)
{
    char expected_header[20];
    uint64 i;

    sprintf(expected_header, "\\%d-grams:", order);
    while ((*li = lineiter_next(*li))) {
        string_trim((*li)->buf, STRING_BOTH);
        if (strcmp((*li)->buf, expected_header) == 0)
            break;
    }
    *raw_ngrams = (lm_ngram_t *)ckd_calloc((size_t)count, sizeof(lm_ngram_t));
    for (i = 0; i < count; i++) {
        read_ngram_instance(li, wid, lmath, order, order_max, &((*raw_ngrams)[i]));
    }

    //sort raw ngrams that was read
    ngram_comparator(NULL, &order); //setting up order in comparator
    qsort(*raw_ngrams, (size_t)count, sizeof(lm_ngram_t), &ngram_comparator);
}

lm_ngram_t** lm_ngrams_raw_read(lineiter_t **li, hash_table_t *wid, logmath_t *lmath, uint32 *counts, int order)
{
    lm_ngram_t **raw_ngrams;
    int order_it;

    raw_ngrams = (lm_ngram_t **)ckd_calloc(order - 1, sizeof(*raw_ngrams));
    for (order_it = 2; order_it <= order; order_it++) {
        lm_ngrams_raw_read_order(&raw_ngrams[order_it - 2], li, wid, lmath, counts[order_it - 1], order_it, order);
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

void lm_ngrams_raw_free(lm_ngram_t **raw_ngrams, uint32 *counts, int order)
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