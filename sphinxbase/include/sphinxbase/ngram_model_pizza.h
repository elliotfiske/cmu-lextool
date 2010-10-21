#ifndef __NGRAM_MODEL_PIZZA_H__
#define __NGRAM_MODEL_PIZZA_H__

#include <sphinxbase/ngram_model.h>
#include <sphinxbase/cmd_ln.h>
#include <sphinxbase/logmath.h>

/**
 * Declaration of the ngram_model_pizza_t class;
 */
typedef struct ngram_model_pizza_s ngram_model_pizza_t;

/**
 * Create and initialize a pizza language model.
 */
ngram_model_t *ngram_model_pizza_init(cmd_ln_t *config,
				      logmath_t *lmath);

#endif /* __NGRAM_MODEL_PIZZA_H__ */
