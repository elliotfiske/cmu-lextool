/* -*- c-basic-offset: 4; indent-tabs-mode: nil -*- */

#include <sphinxbase/ckd_alloc.h>

#include "ngram_model_pizza.h"
#include "ngram_model_internal.h"

/**
 * Implementation of the ngram_model_pizza_t class.
 */
struct ngram_model_pizza_s {
    /**
     * Base language model structure, must come first.
     */
    ngram_model_t base;

    /* And, whatever else you want... */
    int *bogus;
};

/**
 * Forward declare the function table.
 */
static ngram_funcs_t ngram_model_pizza_funcs;

ngram_model_t *
ngram_model_pizza_init(cmd_ln_t *config,
		       logmath_t *lmath)
{
	ngram_model_pizza_t *model;
	ngram_model_t *base;
	int i;

	/* Allocate the pizza model object. */
	model = ckd_calloc(1, sizeof(*model));
	base = &model->base;
	/* Initialize the base class part of it. */
	ngram_model_init(base,
			 &ngram_model_pizza_funcs,
			 lmath,
			 1,  /* N-gram order (use 1 if unknown/unimportant) */
			 8); /* Number of word strings to allocate */

        model->bogus = ckd_calloc(42, sizeof(*model->bogus));

	/* Fill in the word strings. */
	base->word_str[0] = "pizza";
	base->word_str[1] = "spinach";
	base->word_str[2] = "olives";
	base->word_str[3] = "pepperoni";
	base->word_str[4] = "extra_cheese";
	base->word_str[5] = "sausage";
	base->word_str[6] = "<s>";
	base->word_str[7] = "</s>";
	/* Add them to the word ID mapping. */
	for (i = 0; i < 8; ++i)
		(void)hash_table_enter_int32(base->wid, base->word_str[i], i);

	/* Return the base class pointer. */
	return base;
}

static int
ngram_model_pizza_apply_weights(ngram_model_t *base, float32 lw,
				float32 wip, float32 uw)
{
    /* Here we would pre-apply the language model weight, word
     * insertion penalty, and unigram weight to the language model
     * scores, if we wanted to.  We don't so we'll just
     * store them. */
    base->lw = lw;
    base->log_wip = logmath_log(base->lmath, wip);
    base->log_uw = logmath_log(base->lmath, uw);

    return 0;
}

static int32
ngram_model_pizza_raw_score(ngram_model_t *base, int32 wid,
			    int32 *history, int32 n_hist,
			    int32 *n_used)
{
    /* Just use a uniform language model.  Note how the language
     * weight and word insertion penalty are not applied here. */
    if (n_used) *n_used = 0;
    return logmath_log(base->lmath, 1.0 / 7);
}

static int32
ngram_model_pizza_score(ngram_model_t *base, int32 wid,
			int32 *history, int32 n_hist,
			int32 *n_used)
{
    ngram_model_pizza_t *model = (ngram_model_pizza_t *)base;
    /* Note how the language weight and word insertion penalty are
     * applied here. */
    return ngram_model_pizza_raw_score(base, wid, history, n_hist, n_used)
        * base->lw + base->log_wip;
}

static void
ngram_model_pizza_free(ngram_model_t *base)
{
    ngram_model_pizza_t *model = (ngram_model_pizza_t *)base;
    ckd_free(model->bogus);
}

static ngram_funcs_t ngram_model_pizza_funcs = {
    ngram_model_pizza_free,          /* free */
    ngram_model_pizza_apply_weights, /* apply_weights */
    ngram_model_pizza_score,         /* score */
    ngram_model_pizza_raw_score      /* raw_score */
};

