
#include <string.h>

#include <sphinxbase/err.h>
#include <sphinxbase/pio.h>
#include <sphinxbase/strfuncs.h>
#include <sphinxbase/ckd_alloc.h>
#include <sphinxbase/qsort_ext.h>

#include "ngram_model_trie.h"
#include "ngram_model_internal.h"
#include "lm_trie.h"

static const char trie_hdr[] = "Trie Language Model";
static ngram_funcs_t ngram_model_trie_funcs;

/*
 * Read and return #unigrams, #bigrams, #trigrams as stated in input file.
 */
static int read_counts_arpa(lineiter_t **li, uint64* counts, int* order)
{
    int i;
    int32 ngram, prev_ngram;
    uint64 ngram_cnt;

    /* skip file until past the '\data\' marker */
    while (*li) {
        string_trim((*li)->buf, STRING_BOTH);
        if (strcmp((*li)->buf, "\\data\\") == 0)
            break;
        *li = lineiter_next(*li);
    }
    if (*li == NULL || strcmp((*li)->buf, "\\data\\") != 0) {
        E_INFO("No \\data\\ mark in LM file\n");
        return -1;
    }

    prev_ngram = 0;
    *order = 0;
    while ((*li = lineiter_next(*li))) {
        if (sscanf((*li)->buf, "ngram %d=%lld", &ngram, &ngram_cnt) != 2)
            break;
        if (ngram != prev_ngram + 1) {
            E_ERROR("Ngram counts in LM file is not in order. %d goes after %d\n", ngram, prev_ngram);
            return -1;
        }
        prev_ngram = ngram;
        counts[*order] = ngram_cnt;
        (*order)++;
    }
    if (*li == NULL) {
        E_ERROR("EOF while reading ngram counts\n");
        return -1;
    }

    /* Position iterator to the unigrams header '\1-grams:\' */
    while ((*li = lineiter_next(*li))) {
        string_trim((*li)->buf, STRING_BOTH);
        if (strcmp((*li)->buf, "\\1-grams:") == 0)
            break;
    }
    if (*li == NULL) {
        E_ERROR_SYSTEM("Failed to read \\1-grams: mark");
        return -1;
    }
    for (i = 0; i < *order; i++)
        if (counts[i] <= 0) {
            E_ERROR("Bad ngram count\n");
            return -1;
        }

    return 0;
}

int string_comparator(const void *a, const void *b) 
{ 
    const char **ia = (const char **)a;
    const char **ib = (const char **)b;
    return strcmp(*ia, *ib);
} 

static void read_1grams_arpa(lineiter_t **li, uint64 count, ngram_model_t *base, unigram_t *unigrams)
{
    uint64 i;
    int n;
    char *wptr[3];

    for (i = 0; i < count; i++) {
        *li = lineiter_next(*li);
        if (*li == NULL) {
            E_ERROR("Unexpected end of ARPA file. Failed to read %dth unigram\n", i+1);
            break;
        }
        string_trim((*li)->buf, STRING_BOTH);
        if ((n = str2words((*li)->buf, wptr, 3)) < 3) {
            if ((*li)->buf[0] != '\0')
                E_WARN("Format error; unigram ignored: %s\n", (*li)->buf);
            continue;
        } else {
            unigram_t *unigram = &unigrams[i];
            unigram->prob = (float)atof_c(wptr[0]);
            unigram->prob = logmath_log10_to_log_float(base->lmath, unigram->prob);
            if (unigram->prob > 0) {
                E_WARN("Unigram [%s] has positive probability. Zeroize\n", wptr[1]);
                unigram->prob = 0;
            }
            unigram->bo = (float)atof_c(wptr[2]);
            unigram->bo = logmath_log10_to_log_float(base->lmath, unigram->bo);
            //TODO classify float with fpclassify and warn if bad value occurred
            base->word_str[i] = ckd_salloc(wptr[1]);
        }
    }
    //finished loading unigrams, time to sort them and their names
    qsort2((void *)base->word_str, unigrams, (size_t)count, sizeof(char *), sizeof(unigram_t), &string_comparator);
    //fill hash-table that maps unigram names to their word ids
    for (i = 0; i < count; i++) {
        if ((hash_table_enter(base->wid, base->word_str[i], (void *)(long)i)) != (void *)(long)i) {
                E_WARN("Duplicate word in dictionary: %s\n", base->word_str[i]);
        }
    }
}

ngram_model_t* ngram_model_trie_read_arpa(cmd_ln_t *config,
                                          const char *path,
                                          logmath_t *lmath)
{
    FILE *fp;
    lineiter_t *li;
    ngram_model_trie_t *model;
    ngram_model_t *base;
    lm_ngram_t **raw_ngrams;
    int32 is_pipe;
    uint64 counts[MAX_NGRAM_ORDER];
    float32 wip; //TODO uw
    int order;
    int i;

    if ((fp = fopen_comp(path, "r", &is_pipe)) == NULL) {
        E_ERROR("File %s not found\n", path);
        return NULL;
    }

    model = (ngram_model_trie_t *)ckd_calloc(1, sizeof(*model));
    li = lineiter_start(fp);
    /* Read n-gram counts from file */
    if (read_counts_arpa(&li, counts, &order) == -1) {
        lineiter_free(li);
        fclose_comp(fp, is_pipe);
        return NULL;
    }
    //TODO kenlm checks for counts overflow on 32 bit machines here

    if (order < 2) {
        E_ERROR("This ngram implementation assumes at least a bigram model\n");
        lineiter_free(li);
        fclose_comp(fp, is_pipe);
        return NULL;
    }

    E_INFO("LM of order %d\n", order);
    for (i = 0; i < order; i++) {
        E_INFO("#%d-grams: %lld\n", i+1, counts[i]);
    }

    base = &model->base;
    ngram_model_init(base, &ngram_model_trie_funcs, lmath, order, (int32)counts[0]);
    for (i = 0; i < order; i++) {
        base->n_counts[i] = counts[i];
    }
    base->is_lm_trie = TRUE;
    base->writable = TRUE;

    wip = 1.0;
    model->lw = 1.0;
    if (cmd_ln_exists_r(config, "-wip"))
        wip = cmd_ln_float32_r(config, "-wip");
    if (cmd_ln_exists_r(config, "-lw"))
        model->lw = cmd_ln_float32_r(config, "-lw");
    model->log_wip = logmath_log(base->lmath, wip);

    model->trie = lm_trie_create(QUANT_16, counts, order, NULL);
    read_1grams_arpa(&li, counts[0], base, model->trie->unigrams);
    raw_ngrams = lm_ngrams_raw_read(&li, base->wid, base->lmath, counts, order);
    lm_trie_build(model->trie, raw_ngrams, counts, order);
    lm_ngrams_raw_free(raw_ngrams, counts, order);

    lineiter_free(li);
    fclose_comp(fp, is_pipe);

    return base;
}

static void read_word_str(ngram_model_t *base, FILE *fp)
{
    uint64 i, j, k;
    char *tmp_word_str;
    /* read ascii word strings */
    base->writable = TRUE;
    fread(&k, sizeof(k), 1, fp);
    //TODO size_t to uint64 cast
    tmp_word_str = (char *)ckd_calloc((size_t)k, 1);
    fread(tmp_word_str, 1, (size_t)k, fp);

    /* First make sure string just read contains n_counts[0] words (PARANOIA!!) */
    for (i = 0, j = 0; i < k; i++)
        if (tmp_word_str[i] == '\0')
            j++;
    if (j != base->n_counts[0]) {
        E_ERROR("Error reading word strings (%d doesn't match n_unigrams %d)\n", j, base->n_counts[0]);
    }

    /* Break up string just read into words */
    j = 0;
    for (i = 0; i < base->n_counts[0]; i++) {
        base->word_str[i] = ckd_salloc(tmp_word_str + j);
        if (hash_table_enter(base->wid, base->word_str[i],
                                 (void *)(long)i) != (void *)(long)i) {
            E_WARN("Duplicate word in dictionary: %s\n", base->word_str[i]);
        }
        j += strlen(base->word_str[i]) + 1;
    }
    free(tmp_word_str);
}

ngram_model_t* ngram_model_trie_read_bin(cmd_ln_t *config, 
                                          const char *path,
                                          logmath_t *lmath)
{
    int32 is_pipe;
    FILE *fp;
    size_t hdr_size;
    char *hdr;
    int cmp_res;
    uint8 i, order;
    int quant_type_int;
    lm_trie_quant_type_t quant_type;
    uint64 counts[MAX_NGRAM_ORDER];
    float32 wip;
    ngram_model_trie_t *model;
    ngram_model_t *base;

    if ((fp = fopen_comp(path, "rb", &is_pipe)) == NULL) {
        E_ERROR("File %s not found\n", path);
        return NULL;
    }
    hdr_size = strlen(trie_hdr);
    hdr = (char *)ckd_calloc(hdr_size + 1, sizeof(*hdr));
    fread(hdr, sizeof(*hdr), hdr_size, fp);
    cmp_res = strcmp(hdr, trie_hdr);
    ckd_free(hdr);
    if (cmp_res) {
        fclose_comp(fp, is_pipe);
        return NULL;
    }
    model = (ngram_model_trie_t *)ckd_calloc(1, sizeof(*model));
    base = &model->base;
    fread(&order, sizeof(order), 1, fp);
    for (i = 0; i < order; i++) {
        fread(&counts[i], sizeof(counts[i]), 1, fp);
    }
    ngram_model_init(base, &ngram_model_trie_funcs, lmath, order, (int32)counts[0]);
    for (i = 0; i < order; i++) {
        base->n_counts[i] = counts[i];
    }
    base->is_lm_trie = TRUE;

    wip = 1.0;
    model->lw = 1.0;
    if (cmd_ln_exists_r(config, "-wip"))
        wip = cmd_ln_float32_r(config, "-wip");
    if (cmd_ln_exists_r(config, "-lw"))
        model->lw = cmd_ln_float32_r(config, "-lw");
    model->log_wip = logmath_log(base->lmath, wip);

    fread(&quant_type_int, sizeof(quant_type_int), 1, fp);
    quant_type = (lm_trie_quant_type_t)quant_type_int;
    model->trie = lm_trie_create(quant_type, counts, order, fp);
    read_word_str(base, fp);
    fclose_comp(fp, is_pipe);

    return base;
}

static void write_word_str(FILE *fp, ngram_model_t *model)
{
    uint64 i, k;

    k = 0;
    for (i = 0; i < model->n_counts[0]; i++)
        k += strlen(model->word_str[i]) + 1;
    fwrite(&k, sizeof(k), 1, fp);
    for (i = 0; i < model->n_counts[0]; i++)
        fwrite(model->word_str[i], 1,
               strlen(model->word_str[i]) + 1, fp);
}

int ngram_model_trie_write_bin(ngram_model_t *base,
                               const char *path)
{
    int i;
    int32 is_pipe;
    ngram_model_trie_t *model = (ngram_model_trie_t *)base;
    FILE *fp = fopen_comp(path, "wb", &is_pipe);
    if (!fp) {
        E_ERROR("Unable to open %s to write binary trie LM\n", path);
        return -1;
    }

    fwrite(trie_hdr, sizeof(*trie_hdr), strlen(trie_hdr), fp);
    fwrite(&model->base.n, sizeof(model->base.n), 1, fp);
    for (i = 0; i < model->base.n; i++) {
        fwrite(&model->base.n_counts[i], sizeof(model->base.n_counts[i]), 1, fp);
    }
    lm_trie_write_bin(model->trie, fp);
    write_word_str(fp, base);
    fclose_comp(fp, is_pipe);
    return 0;
}

#include "lm_trie_query.c"

static void ngram_model_trie_free(ngram_model_t *base)
{
    ngram_model_trie_t *model = (ngram_model_trie_t *)base;
    lm_trie_free(model->trie);
}

static int trie_apply_weights(ngram_model_t *model, float32 lw, float32 wip, float32 uw)
{
    return 0;
}

static int32 apply_weights(ngram_model_trie_t *model, float score)
{
    return (int32)(score * model->lw + model->log_wip);
}

static int32 ngram_model_trie_score(ngram_model_t *base, int32 wid, int32 *hist, int32 n_hist, int32 *n_used)
{
    int32 i;
    float score;
    ngram_model_trie_t *model = (ngram_model_trie_t *)base;

    if (n_hist > model->base.n - 1)
        n_hist = model->base.n - 1;
    for (i = 0; i < n_hist; i++) {
        if (hist[i] < 0) {
            n_hist = i;
            break;
        }
    }

    score = lm_trie_score(model->trie, model->base.n, wid, hist, n_hist, n_used);
    return apply_weights(model, score);
}

static int32 lm_trie_raw_score(ngram_model_t *model, int32 wid, int32 *hist, int32 n_hist, int32 *n_used)
{
    return 0;
}

static int32 lm_trie_add_ug(ngram_model_t *model, int32 wid, int32 lweight)
{
    return 0;
}

static void lm_trie_flush(ngram_model_t *model)
{
}

static ngram_iter_t* lm_trie_iter(ngram_model_t *model, int32 wid, int32 *hist, int32 n_hist)
{
    return NULL;
}

static ngram_iter_t* lm_trie_mgrams(ngram_model_t *model, int32 m)
{
    return NULL;
}

static ngram_iter_t* lm_trie_successors(ngram_iter_t *itor)
{
    return NULL;
}

static const int32* lm_trie_iter_get(ngram_iter_t *iter, int32 *out_score, int32 *out_bowt)
{
    return NULL;
}

static ngram_iter_t* lm_trie_iter_next(ngram_iter_t *iter)
{
    return NULL;
}

static void lm_trie_iter_free(ngram_iter_t *iter)
{
}

static ngram_funcs_t ngram_model_trie_funcs = {
    ngram_model_trie_free,    /* free */
    trie_apply_weights,       /* apply_weights */
    ngram_model_trie_score,   /* score */
    lm_trie_raw_score,        /* raw_score */
    lm_trie_add_ug,           /* add_ug */
    lm_trie_flush,            /* flush */
    lm_trie_iter,             /* iter */
    lm_trie_mgrams,           /* mgrams */
    lm_trie_successors,       /* successors */
    lm_trie_iter_get,         /* iter_get */
    lm_trie_iter_next,        /* iter_next */
    lm_trie_iter_free         /* iter_free */
};