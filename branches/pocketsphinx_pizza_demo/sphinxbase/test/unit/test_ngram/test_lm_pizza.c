#include <sphinxbase/ngram_model_pizza.h>
#include <sphinxbase/logmath.h>
#include <sphinxbase/ckd_alloc.h>

#include "test_macros.h"

int
main(int argc, char *argv[])
{
	logmath_t *lmath;
	ngram_model_t *model;
	int32 score;

	/* Initialize a log-arithmetic object. */
	lmath = logmath_init(1.0001, 0, 0);
	/* Create the pizza language model. */
	model = ngram_model_pizza_init(NULL, lmath);
	ngram_model_apply_weights(model, 42, 1.0, 1.0);
	/* Score a word with history using it. */
	score = ngram_score(model, "spinach", "olives",
			    "pepperoni", "extra_cheese", NULL);
	printf("score = %d = %g\n", score, logmath_exp(lmath, score));
	TEST_ASSERT(score == -19460 * 42);
	score = ngram_prob(model, "spinach", "olives",
			   "pepperoni", "extra_cheese", NULL);
	printf("score = %d = %g\n", score, logmath_exp(lmath, score));
	TEST_ASSERT(score == -19460);
	/* Free it. */
	ngram_model_free(model);
	/* Free the log-arithmetic object. */
	logmath_free(lmath);

	return 0;
}
