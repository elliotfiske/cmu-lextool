#include <ngram_model.h>
#include <logmath.h>
#include <strfuncs.h>

#include "test_macros.h"

#include <stdio.h>
#include <string.h>
#include <math.h>

int
main(int argc, char *argv[])
{
	logmath_t *lmath;
	ngram_model_t *model;
	int32 n_used;

	/* Initialize a logmath object to pass to ngram_read */
	lmath = logmath_init(1.0001, 0, 0);
	/* Read a language model */
	model = ngram_model_read(NULL, LMDIR "/100.arpa.gz", NGRAM_ARPA, lmath);
	TEST_ASSERT(model);
	TEST_EQUAL(ngram_wid(model, "<UNK>"), 0);
	TEST_EQUAL(strcmp(ngram_word(model, 0), "<UNK>"), 0);
	TEST_EQUAL(ngram_wid(model, "absolute"), 13);
	TEST_EQUAL(strcmp(ngram_word(model, 13), "absolute"), 0);
	/* Test unigrams. */
	TEST_EQUAL(ngram_score(model, "<UNK>", NULL), -75346);
	TEST_EQUAL(ngram_bg_score(model, ngram_wid(model, "<UNK>"),
				  NGRAM_INVALID_WID, &n_used), -75346);
	TEST_EQUAL(n_used, 1);
	TEST_EQUAL(ngram_score(model, "sphinxtrain", NULL), -64208);
	TEST_EQUAL(ngram_bg_score(model, ngram_wid(model, "sphinxtrain"),
				  NGRAM_INVALID_WID, &n_used), -64208);
	TEST_EQUAL(n_used, 1);
	/* Test bigrams. */
	TEST_EQUAL(ngram_score(model, "huggins", "david", NULL), -831);
	/* Test trigrams. */
	TEST_EQUAL_LOG(ngram_score(model, "daines", "huggins", "david", NULL), -9450);

	ngram_model_free(model);

	/* Read a language model */
	model = ngram_model_read(NULL, LMDIR "/100.arpa.DMP", NGRAM_DMP, lmath);
	TEST_ASSERT(model);
	TEST_EQUAL(ngram_wid(model, "<UNK>"), 0);
	TEST_EQUAL(strcmp(ngram_word(model, 0), "<UNK>"), 0);
	TEST_EQUAL(ngram_wid(model, "absolute"), 13);
	TEST_EQUAL(strcmp(ngram_word(model, 13), "absolute"), 0);
	/* Test unigrams. */
	TEST_EQUAL(ngram_score(model, "<UNK>", NULL), -75346);
	TEST_EQUAL(ngram_bg_score(model, ngram_wid(model, "<UNK>"),
				  NGRAM_INVALID_WID, &n_used), -75346);
	TEST_EQUAL(n_used, 1);
	TEST_EQUAL(ngram_score(model, "sphinxtrain", NULL), -64208);
	TEST_EQUAL(ngram_bg_score(model, ngram_wid(model, "sphinxtrain"),
				  NGRAM_INVALID_WID, &n_used), -64208);
	TEST_EQUAL(n_used, 1);
	/* Test bigrams. */
	TEST_EQUAL(ngram_score(model, "huggins", "david", NULL), -831);
	/* Test trigrams. */
	TEST_EQUAL(ngram_score(model, "daines", "huggins", "david", NULL), -9452);

	ngram_model_free(model);
	logmath_free(lmath);

	return 0;
}
