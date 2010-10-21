#include <pocketsphinx.h>
#include <stdio.h>
#include <string.h>

#include <sphinxbase/ngram_model_pizza.h>

#include "test_macros.h"

int
main(int argc, char *argv[])
{
	char const *hyp;
	char const *uttid;
	ps_decoder_t *ps;
	cmd_ln_t *config;
	ngram_model_t *lmset, *lm;
	FILE *rawfh;
	int32 score;

	/* Initialize the decoder. */
	TEST_ASSERT(config =
		    cmd_ln_init(NULL, ps_args(), TRUE,
				"-hmm", MODELDIR "/hmm/en_US/hub4wsj_sc_8k",
				/* FIXME: Currently we need to specify
				 * a language model at initialization
				 * even if we don't end up using it */
				"-lm", MODELDIR "/lm/en_US/wsj0vp.5000.DMP",
				"-dict", DATADIR "/pizza.dic",
				"-input_endian", "little",
				"-samprate", "16000", NULL));
	TEST_ASSERT(ps = ps_init(config));

	/* Create the pizza language model. */
	lm = ngram_model_pizza_init(config, ps_get_logmath(ps));
	lmset = ps_get_lmset(ps);
	/* Add it to the decoder and select it. */
	ngram_model_set_add(lmset, lm, "pizza", 1.0, TRUE);
	ngram_model_set_select(lmset, "pizza");
	ps_update_lmset(ps, lmset);

	/* Decode some speech. */
	if ((rawfh = fopen(DATADIR "/goforward.raw", "rb")) == NULL) {
		perror("Could not open goforward.raw");
		return 1;
	}
	TEST_ASSERT(ps_decode_raw(ps, rawfh, "goforward", -1));
	hyp = ps_get_hyp(ps, &score, &uttid);
	printf("%s: %s (%d)\n", uttid, hyp, score);
	fclose(rawfh);
	ps_free(ps);

	return 0;
}

