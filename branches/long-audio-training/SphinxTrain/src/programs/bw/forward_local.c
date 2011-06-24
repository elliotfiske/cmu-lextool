/* -*- c-basic-offset: 4 -*- */
/* ====================================================================
 * Copyright (c) 1995-2000 Carnegie Mellon University.  All rights 
 * reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer. 
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * This work was supported in part by funding from the Defense Advanced 
 * Research Projects Agency and the National Science Foundation of the 
 * United States of America, and the CMU Sphinx Speech Consortium.
 *
 * THIS SOFTWARE IS PROVIDED BY CARNEGIE MELLON UNIVERSITY ``AS IS'' AND 
 * ANY EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY
 * NOR ITS EMPLOYEES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * ====================================================================
 *
 */
/*********************************************************************
 *
 * File: forward.c
 * 
 * Description: 
 * 
 * Author: 
 * 	Eric H. Thayer (eht@cs.cmu.edu)
 *********************************************************************/

#define ACHK 10

#include <s3/model_inventory.h>
#include <s3/s3phseg_io.h>
#include <s3/vector.h>
#include <sphinxbase/ckd_alloc.h>
#include <s3/gauden.h>
#include <s3/state.h>
#include <s3/s3.h>

#include <s3/profile.h>

#include <assert.h>
#include <math.h>
#include <string.h>

#define FORWARD_DEBUG 0
#define INACTIVE	0xffff

typedef struct forward_data {
    uint32 l_cb;
    uint32 *active;
    uint32 *active_l_cb;
    uint32 n_active;
    uint32 n_active_l_cb;
    uint32 n_sum_active;
    uint32 *next_active;
    uint32 n_next_active;
    uint32 aalpha_alloc;
    uint16 *amap;
    uint32 *next;
    float32 *tprob;
    float64 prior_alpha;
    float32 ***mixw;
    gauden_t *g;
    acmod_set_t *as;
    float64 x;
    float64 pthresh;
    float64 balpha;
    float64 ***now_den;
    uint32 ***now_den_idx;
    uint32 retval;
    uint32 n_l_cb;
    int32 *acbframe; /* Frame in which a codebook was last active */
    timing_t *gau_timer;
    float64 *outprob;
    /* Can we prune this frame using phseg? */
    int can_prune_phseg;
    float64 *best_pred;
    
    float64 *active_alpha_t;
    float64 *active_alpha_p;
    
    uint32 **bp;
    uint32 *bp_t;
    
    float64 *dscale_t;
    vector_t *feature_t;
    float64 scale_t;
    
    uint32 *active_astate_t;
    uint32 n_active_astate_t;
    
    uint32 t;
    
} forward_data;

int32
forward_local(float64 **active_alpha,
        float64 **reduced_alpha,
	uint32 **active_astate,
	uint32 *n_active_astate,
	uint32 **bp,
	uint32 **reduced_bp,
	float64 *scale,
	float64 **dscale,
	vector_t **feature,
	uint32 n_obs,
	state_t *state_seq,
	uint32 n_state,
	model_inventory_t *inv,
	float64 beam,
	s3phseg_t *phseg,
	uint32 mmi_train,
	forward_data *args);
int32
forward_step(
	uint32 n_obs,
	state_t *state_seq,
	uint32 n_state,
	model_inventory_t *inv,
	float64 beam,
	s3phseg_t *phseg,
	uint32 mmi_train,
	forward_data *args);

/*********************************************************************
 *
 * Function: 
 * 	forward
 *
 * Description: 
 * 	This function computes the scaled forward variable, alpha.
 *
 *	In order to conserve memory for long utterances, only the
 *	active (i.e. non-zero) alpha values are stored.
 *
 * Function Inputs: 
 * 	float64 **active_alpha -
 *		On the successful return of this function,
 *		this array contains the scaled alpha variable for
 *		active states for all input observations.  For any
 *		input observation at time t (t >= 0  && t < n_obs), the number
 *		of active states can be found by accessing
 *		n_args->active_astate_t.  So for some time t, the active
 *		scaled alpha values are active_alpha[t][i] where
 *		i >= 0 and i < n_args->active_astate_t.  For some valid t and
 *		i, the sentence HMM state id can be found by accessing
 *		args->active_astate_t[i].
 *
 * 	uint32 **active_astate -
 *		On the successful return of this function,
 *		this array contains the mapping of active state indexes
 *		into sentence HMM indexes (i.e. indexes into the state_seq[]
 *		array).  The active states of the sentence HMM may be
 *		enumerated by accessing args->active_astate_t[i] over all
 *		t (t >= 0 && t < n_obs) and i
 *		(i >= 0 && i < n_args->active_astate_t).
 *
 * 	uint32 *n_active_astate -
 *		On the successful return of this function,
 *		this array contains the number of active states for all
 *		t (t >= 0 && t < n_obs).
 *
 *	float64 *scale -
 *		On the successful return of this function,
 * 		this variable contains the scale factor applied to the
 *		alpha variable for all t (t >= 0 && t < n_obs).
 *
 *	vector_t **feature -
 *		This variable contains the input observation vectors.
 *		The value feature[t][f] must be valid for
 *		t >= 0 && t < n_obs and f >= 0 && f < n_feat) where
 *		n_feat is the # of assumed statistically independent
 *		feature streams to be modelled.
 *
 * 	uint32 **bp -
 *		On the successful return of this function,
 *		this array contains backtrace pointers for active states
 *              for all input observations except the first timepoint.
 *              As in active_alpha[], the sentence HMM state id can be
 *              found by accessing args->active_astate_t[i].  NOTE!
 *              This is a "raw" backpointer array and as such, it contains
 *              pointers to non-emitting states.  These pointers refer to
 *              the current frame rather than the previous one.  Thus,
 *              the state id of the backpointer is either
 *              active_astate[t-1][args->bp_t[i]] (for normal states) or
 *              args->active_astate_t[args->bp_t[i]] (for non-emitting states).
 *
 *	uint32 n_obs -
 *		This variable contains the number of input observation
 *		vectors seen given the model.
 *
 *	state_t *state_seq -
 *		This is a list of model state structures which define
 *		the sentence HMM for this observation sequence.
 *
 *	uint32 n_state -
 *		The total # of states in the sentence HMM for this
 *		utterance.
 *
 *	model_inventory_t *inv -
 *		This structure contains the inventory of initial acoustic
 *		model parameters.
 *
 *	float64 beam -
 *		A pruning beam to apply to the evaluation of the alpha
 *		variable.
 *
 *      s3phseg_t *phseg -
 *              An optional phone segmentation to use to constrain the
 *              forward lattice.
 *
 * Global Inputs: 
 * 	None
 *
 * Return Values: 
 *
 * 	S3_SUCCESS
 *		The alpha variable was completed successfully for this
 *		observation sequence and model.
 *
 *	S3_ERROR
 *		Some error was detected that prevented the computation of the
 *		variable.
 *
 * Global Outputs: 
 * 	None
 *
 * Errors: 
 * 	- Initial alpha value < epsilon
 *	- If semi-continuous models, unable to normalize input frame
 *	- Output liklihood underflow
 *	- Alpha variable < epsilon for all active states
 *
 *********************************************************************/

int32
forward(float64 **active_alpha,
        float64 **reduced_alpha,
	uint32 **active_astate,
	uint32 *n_active_astate,
	uint32 **bp,
	uint32 **reduced_bp,
	float64 *scale,
	float64 **dscale,
	vector_t **feature,
	uint32 n_obs,
	state_t *state_seq,
	uint32 n_state,
	model_inventory_t *inv,
	float64 beam,
	s3phseg_t *phseg,
	uint32 mmi_train)
{
    uint32 i;
    uint32 sqrt_pos;
    forward_data args;
    
    /* Get the CPU timer associated with mixture Gaussian evaluation */
    args.gau_timer = timing_get("gau");
    
    /* # of distinct codebooks referenced by this utterance */
    args.n_l_cb = inv->n_cb_inverse;

    /* active codebook frame index */
    args.acbframe = ckd_calloc(args.n_l_cb, sizeof(*args.acbframe));

    args.g = inv->gauden;
    args.as = inv->mdef->acmod_set;
    /* density values and indices (for top-N eval) for some time t */
    args.now_den = (float64 ***)ckd_calloc_3d(args.n_l_cb, gauden_n_feat(args.g), gauden_n_top(args.g),
					 sizeof(float64));
    args.now_den_idx = (uint32 ***)ckd_calloc_3d(args.n_l_cb, gauden_n_feat(args.g), gauden_n_top(args.g),
					    sizeof(uint32));
    /* Mixing weight array */
    args.mixw = inv->mixw;

    /* Scratch area for output probabilities at some time t */
    args.outprob = (float64 *)ckd_calloc(n_state, sizeof(float64));

    /* Active (local) codebooks for some time t */
    args.active_l_cb = ckd_calloc(n_state, sizeof(uint32));

    /* Mapping from sentence HMM state index to active state list index
    * for currently active time. */
    args.amap = ckd_calloc(n_state, sizeof(uint16));
    
    /* set up the active and next_active lists and associated counts */
    /* Active state lists for time t and t+1 */
    args.active = ckd_calloc(n_state, sizeof(uint32));
    args.next_active = ckd_calloc(n_state, sizeof(uint32));
    args.n_active = 0;
    args.n_active_l_cb = 0;
    args.n_sum_active = 0;
    args.n_next_active = 0;

    /* Initialize the active state map such that all states are inactive */
    for (i = 0; i < n_state; i++)
    	args.amap[i] = INACTIVE;

    /*
     * The following section computes the output liklihood of
     * the initial state for t == 0 and puts the initial state
     * in the active state list.
     */

    /* compute alpha for the initial state at t == 0 */
    if (args.gau_timer)
	    timing_start(args.gau_timer);

    /* Compute the component Gaussians for state 0 mixture density */
    gauden_compute_log(args.now_den[state_seq[0].l_cb],
		       args.now_den_idx[state_seq[0].l_cb],
		       feature[0],
		       args.g,
		       state_seq[0].cb, NULL);

    args.active_l_cb[0] = state_seq[0].l_cb;

    dscale[0] = gauden_scale_densities_fwd(args.now_den, args.now_den_idx,
					   args.active_l_cb, 1, args.g);

    /* Compute the mixture density value for state 0 time 0 */
    args.outprob[0] = gauden_mixture(args.now_den[state_seq[0].l_cb],
				args.now_den_idx[state_seq[0].l_cb],
				args.mixw[state_seq[0].mixw],
				args.g);
    if (args.gau_timer)
    	timing_stop(args.gau_timer);
    if (args.outprob[0] <= MIN_IEEE_NORM_POS_FLOAT32) {
	    E_ERROR("Small output prob (== %.2e) seen at frame 0 state 0\n", args.outprob[0]);

	    args.retval = S3_ERROR;
	
	    goto cleanup;
    }

    /*
     * Allocate space for the initial state in the alpha
     * and active state arrays
     */
    sqrt_pos = 0;
    
    active_alpha[0] = ckd_calloc(1, sizeof(float64));
    reduced_alpha[0] = ckd_calloc(1, sizeof(float64));
    active_astate[0] = ckd_calloc(1, sizeof(uint32));
    if (bp)
	bp[0] = ckd_calloc(1, sizeof(uint32)); /* Unused, actually */
    args.aalpha_alloc = 1;

    /*
     * Allocate the bestscore array for embedded Viterbi
     */
    if (bp)
    	args.best_pred = ckd_calloc(1, sizeof(float64));

    /* Compute scale for t == 0 */
    scale[0] = 1.0 / args.outprob[0];

    /* set the scaled alpha variable for the initial state */
    reduced_alpha[0][0] = 1.0;
    active_alpha[0][0] = 1.0;
    /* put the initial state in the active state array for t == 0 */
    active_astate[0][0] = 0;
    /* Only one initial state (for now) */
    n_active_astate[0] = 1;

    /* insert the initial state in the active list */
    args.active[args.n_active] = 0;
    args.n_active++;
    
    args.aalpha_alloc = args.n_active;
    
/*    for (;;) {*/
        forward_local(active_alpha, reduced_alpha, active_astate, n_active_astate, bp, reduced_bp, scale, dscale, feature, n_obs, state_seq, n_state,
            inv, beam, phseg, mmi_train, &args);
/*    }*/

    if (!mmi_train)
	printf(" %u ", args.n_sum_active / n_obs);

cleanup:
    ckd_free(args.active);
    ckd_free(args.next_active);
    ckd_free(args.amap);

    ckd_free(args.active_l_cb);
    ckd_free(args.acbframe);

    ckd_free(args.outprob);
    ckd_free(args.best_pred);

    ckd_free_3d((void ***)args.now_den);
    ckd_free_3d((void ***)args.now_den_idx);

    return args.retval;
}



int32
forward_local(float64 **active_alpha,
        float64 **reduced_alpha,
	uint32 **active_astate,
	uint32 *n_active_astate,
	uint32 **bp,
	uint32 **reduced_bp,
	float64 *scale,
	float64 **dscale,
	vector_t **feature,
	uint32 n_obs,
	state_t *state_seq,
	uint32 n_state,
	model_inventory_t *inv,
	float64 beam,
	s3phseg_t *phseg,
	uint32 mmi_train,
	forward_data *args)
{
    /* Compute scaled alpha over the remaining time in the utterance segment. */
    for (args->t = 1; args->t < n_obs; args->t++) {
        /* Find active phone for this timepoint. */
        if (phseg) {
            /* Move the pointer forward if necessary. */
            if (args->t > phseg->ef)
	        phseg = phseg->next;
        }

        args->active_alpha_t = active_alpha[args->t];
        args->active_alpha_p = active_alpha[args->t-1];
        args->bp = bp;
        args->bp_t = bp[args->t];
        args->dscale_t = dscale[args->t];
        args->feature_t = feature[args->t];
        args->scale_t = scale[args->t];
        args->active_astate_t = active_astate[args->t];
        args->n_active_astate_t = n_active_astate[args->t];
        
        forward_step(n_obs, state_seq, n_state, inv, beam, phseg, mmi_train, &args);
    }
}



int32
forward_step(
	uint32 n_obs,
	state_t *state_seq,
	uint32 n_state,
	model_inventory_t *inv,
	float64 beam,
	s3phseg_t *phseg,
	uint32 mmi_train,
	forward_data *args)
{
    uint32 i, j, s, u;
    
    args->n_active_l_cb = 0;

    /* assume next active state set about the same size as current;
       adjust to actual size as necessary later */
    args->active_alpha_t = (float64 *)ckd_calloc(args->aalpha_alloc, sizeof(float64));
        
    if (args->bp) {
        args->bp_t = (uint32 *)ckd_calloc(args->aalpha_alloc, sizeof(uint32));
        /* reallocate the best score array and zero it out */
        if (args->n_active > args->aalpha_alloc)
	    args->best_pred = (float64 *)ckd_realloc(args->best_pred, args->aalpha_alloc * sizeof(float64));
        memset(args->best_pred, 0, args->aalpha_alloc * sizeof(float64));
    }

    /* For all active states at the previous frame, activate their
       successors in this frame and compute codebooks. */
    /* (these are pre-computed so they can be scaled to avoid underflows) */
    for (s = 0; s < args->n_active; s++) {
        i = args->active[s];
#if FORWARD_DEBUG
        E_INFO("At time %d, In Gaussian computation, active state %d\n",t, i);
#endif
        /* get list of states adjacent to active state i */
        args->next = state_seq[i].next_state;	

        /* activate them all, computing their codebook densities if necessary */
        for (u = 0; u < state_seq[i].n_next; u++) {
	        j = args->next[u];
#if FORWARD_DEBUG
	        E_INFO("In Gaussian computation, active state %d, next state %d\n", i,j);
#endif
	        if (state_seq[j].mixw != TYING_NON_EMITTING) {
	            if (args->amap[j] == INACTIVE) {
		        args->l_cb = state_seq[j].l_cb;
		
		        if (args->acbframe[args->l_cb] != args->t) {
		            /* Component density values not yet computed */
		            if (args->gau_timer)
			            timing_start(args->gau_timer);
		            gauden_compute_log(args->now_den[args->l_cb],
				               args->now_den_idx[args->l_cb],
				               args->feature_t,
				               args->g,
				               state_seq[j].cb,
				               /* Preinitializing topn
					          only really makes a
					          difference for
					          semi-continuous
					          (n_l_cb == 1)
					          models. */
				               args->n_l_cb == 1
				               ? args->now_den_idx[args->l_cb] : NULL);

		            args->active_l_cb[args->n_active_l_cb++] = args->l_cb;
		            args->acbframe[args->l_cb] = args->t;

		            if (args->gau_timer)
			            timing_stop(args->gau_timer);
		        }

		        /* Put next state j into the active list */
		        args->amap[j] = args->n_next_active;

		        /* Initialize the alpha variable to zero */
		        args->active_alpha_t[args->n_next_active] = 0;

		        /* Map active state list index to sentence HMM index */
		        args->next_active[args->n_next_active] = j;

		        ++args->n_next_active;

		        if (args->n_next_active == args->aalpha_alloc) {
		            args->aalpha_alloc *= 2;
		            
		            args->active_alpha_t = ckd_realloc(args->active_alpha_t, sizeof(float64) * args->aalpha_alloc);
                                
		            if (args->bp) {
		                    args->bp_t = ckd_realloc(args->bp_t, sizeof(uint32) * args->aalpha_alloc);
			            args->best_pred = (float64 *)ckd_realloc(args->best_pred,
							               sizeof(float64) * args->aalpha_alloc);
			            memset(args->bp_t + args->aalpha_alloc / 2,
			                    0, sizeof(uint32) * (args->aalpha_alloc /2));
			            memset(args->best_pred + args->aalpha_alloc / 2,
			                   0, sizeof(float64) * (args->aalpha_alloc / 2));
		                }
        		    }
	            }
	        }
        }
    }

    /* Cope w/ numerical issues by dividing densities by max density */
    args->dscale_t = gauden_scale_densities_fwd(args->now_den, args->now_den_idx,
				           args->active_l_cb, args->n_active_l_cb, args->g);

    /* Now, for all active states in the previous frame, compute
       alpha for all successors in this frame. */
    for (s = 0; s < args->n_active; s++) {
        i = args->active[s];
        
#if FORWARD_DEBUG
        E_INFO("At time %d, In real state alpha update, active state %d\n",t, i);
#endif
        /* get list of states adjacent to active state i */
        args->next = state_seq[i].next_state;	
        /* get the associated transition probs */
        args->tprob = state_seq[i].next_tprob;

        /* the scaled alpha value for i at t-1 */
        args->prior_alpha = args->active_alpha_p[s];

        /* For all emitting states j adjacent to i, update their
         * alpha values.  */
        for (u = 0; u < state_seq[i].n_next; u++) {
	        j = args->next[u];
#if FORWARD_DEBUG
	        E_INFO("In real state update, active state %d, next state %d\n", i,j);
#endif
	        args->l_cb = state_seq[j].l_cb;

	        if (state_seq[j].mixw != TYING_NON_EMITTING) {
	            /* Next state j is an emitting state */
	            args->outprob[j] = gauden_mixture(args->now_den[args->l_cb],
					        args->now_den_idx[args->l_cb],
					        args->mixw[state_seq[j].mixw],
					        args->g);


	            /* update backpointers args->bp_t[j] */
	            args->x = args->prior_alpha * args->tprob[u];
	            if (args->bp) {
		            if (args->x > args->best_pred[args->amap[j]]) {
#if FORWARD_DEBUG
		                E_INFO("In real state update, backpointer %d => %d updated from %e to (%e * %e = %e)\n",
			               i, j, best_pred[amap[j]], prior_alpha, tprob[u], x);
#endif
		                args->best_pred[args->amap[j]] = args->x;
		                args->bp_t[args->amap[j]] = s;
		            }
	            }
	            
	            /* update the unscaled alpha[t][j] */
	            args->active_alpha_t[args->amap[j]] += args->x * args->outprob[j];
	        }
	        else {
	            /* already done below in the prior time frame */
	        }
        }
    }

#if FORWARD_DEBUG
    if (args->bp) {
        for (s = 0; s < args->n_next_active; ++s) {
	    j = args->next_active[s];
	    E_INFO("After real state update, best path to %d(%d) = %d(%d)\n",
	           j, args->amap[j], args->active[args->bp_t[s]], args->args->bp_t[s]);
        }
    }
#endif
    /* Now, for all active states in this frame, consume any
       following non-emitting states (multiplying in their
       transition probabilities)  */
    for (s = 0; s < args->n_next_active; s++) {
        i = args->next_active[s];

        /* find the successor states */
        args->next = state_seq[i].next_state;
        args->tprob = state_seq[i].next_tprob;

        for (u = 0; u < state_seq[i].n_next; u++) {
	        j = args->next[u];
	        /* for any non-emitting ones */
	        if (state_seq[j].mixw == TYING_NON_EMITTING) {
#if FORWARD_DEBUG
	            E_INFO("In non-emitting state update, active state %d, next state %d\n",i,j);
#endif
	            args->x = args->active_alpha_t[s] * args->tprob[u];

#if FORWARD_DEBUG
	            E_INFO("In non-emitting state update, active_alpha[t][s]: %f,tprob[u]:  %f\n",args->active_alpha_t[s],tprob[u]);
#endif
	            /* activate this state if necessary */
	            if (args->amap[j] == INACTIVE) {
		            args->amap[j] = args->n_next_active;
		            args->active_alpha_t[args->n_next_active] = 0;
		            args->next_active[args->n_next_active] = j;
		            ++args->n_next_active;

		            if (args->n_next_active == args->aalpha_alloc) {
		                args->aalpha_alloc = args->aalpha_alloc * 2;
		                
		                args->active_alpha_t = ckd_realloc(args->active_alpha_t, sizeof(float64) * args->aalpha_alloc);
                                    
		                if (args->bp) {
		                    args->bp_t = ckd_realloc(args->bp_t, sizeof(uint32) * args->aalpha_alloc);
			                args->best_pred = (float64 *)ckd_realloc(args->best_pred,
							                   sizeof(float64) * args->aalpha_alloc);
			                memset(args->bp_t + args->aalpha_alloc / 2,
			                        0, sizeof(uint32) * (args->aalpha_alloc /2));
			                memset(args->best_pred + args->aalpha_alloc / 2,
			                       0, sizeof(float64) * (args->aalpha_alloc / 2));
		                }
		            }
		            if (args->bp) {
		                /* Give its backpointer a default value */
		                args->bp_t[args->amap[j]] = s;
		                args->bp_t[args->amap[j]] = s;
		                args->best_pred[args->amap[j]] = args->x;
		            }
	            }

	            /* update backpointers args->bp_t[j] */
	            if (args->bp && args->x > args->best_pred[args->amap[j]]) {
		        args->bp_t[args->amap[j]] = s;
		        args->bp_t[args->amap[j]] = s;
		        args->best_pred[args->amap[j]] = args->x;
	            }
	            /* update its alpha value */
	            args->active_alpha_t[args->amap[j]] += args->x;
	        }
        }
    }

#if FORWARD_DEBUG
    for (s = 0; s < args->n_next_active; ++s) {
        j = args->next_active[s];
        if (args->bp && state_seq[j].mixw == TYING_NON_EMITTING) {
	    E_INFO("After non-emitting state update, best path to %d(%d) = %d(%d)\n",
	           j, args->amap[j], args->next_active[args->bp_t[s]], args->bp_t[s]);
	    /* Assumptions about topology that might not be valid
	     * but are useful for debugging. */
	    assert(args->next_active[args->bp_t[s]] <= j);
	    assert(j - args->next_active[args->bp_t[s]] <= 2);
        }
    }
#endif
    /* find best alpha value in current frame for pruning and scaling purposes */
    args->balpha = 0;
    /* also take the argmax to find the best backtrace */
    for (s = 0; s < args->n_next_active; s++) {
        if (args->balpha < args->active_alpha_t[s]) {
    	    args->balpha = args->active_alpha_t[s];
        }
    }

    /* cope with some pathological case */
    if (args->balpha == 0.0 && args->n_next_active > 0) {
        E_ERROR("All %u active states,", args->n_next_active);
        for (s = 0; s < args->n_next_active; s++) {
	    if (state_seq[args->next_active[s]].mixw != TYING_NON_EMITTING)
	        fprintf(stderr, " %u", state_seq[args->next_active[s]].mixw);
	    else
	        fprintf(stderr, " N(%u,%u)",
		        state_seq[args->next_active[s]].tmat, state_seq[args->next_active[s]].m_state);

        }
        fprintf(stderr, ", zero at time %u\n", args->t);
        fflush(stderr);
        
        return S3_ERROR;
    }

    /* and some related pathological cases */
    if (args->balpha < 1e-300) {
        E_ERROR("Best alpha < 1e-300\n");

        return S3_ERROR;
    }
    if (args->n_next_active == 0) {
        E_ERROR("No active states at time %u\n", args->t);
        return S3_ERROR;
    }

    /* compute the scale factor */
    args->scale_t = 1.0 / args->balpha;
    /* compute the pruning threshold based on the beam */
    if (log10(args->balpha) + log10(beam) > -300) {
        args->pthresh = args->balpha * beam;
    }
    else {
        /* avoiding underflow... */
        args->pthresh = 1e-300;
    }
    /* DEBUG XXXXX */
    /* pthresh = 0.0; */
    /* END DEBUG */

    /* Determine if phone segmentation-based pruning would leave
     * us with an empty active list (that would be bad!) */
    args->can_prune_phseg = 0;
    if (phseg) {
        for (s = 0; s < args->n_next_active; ++s) 
	    if (acmod_set_base_phone(args->as, state_seq[args->next_active[s]].phn)
	        == acmod_set_base_phone(args->as, phseg->phone))
	        break;
        args->can_prune_phseg = !(s == args->n_next_active);
#if FORWARD_DEBUG
        if (!can_prune_phseg) {
	    E_INFO("Will not apply phone-based pruning at timepoint %d "
	           "(%d != %d) (%s != %s)\n", t,
	           state_seq[args->next_active[s]].phn,
	           phseg->phone,
	           acmod_set_id2name(inv->mdef->acmod_set, state_seq[args->next_active[s]].phn),
	           acmod_set_id2name(inv->mdef->acmod_set, phseg->phone)
	           );
        }
#endif
    }
    /* Prune active states for the next frame and rescale their alphas. */
    args->active_astate_t = ckd_calloc(args->n_next_active, sizeof(uint32));
    for (s = 0, args->n_active = 0; s < args->n_next_active; s++) {
        /* "Snap" the backpointers for non-emitting states, so
           that they don't point to bogus indices (we will use
           amap to recover them). */
        if (args->bp && state_seq[args->next_active[s]].mixw == TYING_NON_EMITTING) {
#if FORWARD_DEBUG
	    E_INFO("Snapping backpointer for %d, %d => %d\n",
	           args->next_active[s], args->bp_t[s], args->next_active[args->bp_t[s]]);
#endif
	    args->bp_t[s] = args->next_active[args->bp_t[s]];
	    args->bp_t[s] = args->next_active[args->bp_t[s]];
        }
        /* If we have a phone segmentation, use it instead of the beam. */
        if (phseg && args->can_prune_phseg) {
	        if (acmod_set_base_phone(args->as, state_seq[args->next_active[s]].phn)
	            == acmod_set_base_phone(args->as, phseg->phone)) {
	            args->active_alpha_t[args->n_active] = args->active_alpha_t[s] * args->scale_t;
	            args->active[args->n_active] = args->active_astate_t[args->n_active] = args->next_active[s];
	            if (args->bp) {
		        args->bp_t[args->n_active] = args->bp_t[s];
		        args->bp_t[args->n_active] = args->bp_t[s];
	            }
	            args->amap[args->next_active[s]] = args->n_active;
	            args->n_active++;
	        }
	        else {
	            args->amap[args->next_active[s]] = INACTIVE;
	        }
        }
        else {
	        if (args->active_alpha_t[s] > args->pthresh) {
	            args->active_alpha_t[args->n_active] = args->active_alpha_t[s] * args->scale_t;
	            args->active[args->n_active] = args->active_astate_t[args->n_active] = args->next_active[s];
	            if (args->bp) {
		        args->bp_t[args->n_active] = args->bp_t[s];
		        args->bp_t[args->n_active] = args->bp_t[s];
                    }
	            args->amap[args->next_active[s]] = args->n_active;
	            args->n_active++;
	        }
	        else {
	            args->amap[args->next_active[s]] = INACTIVE;
	        }
        }
    }
    /* Now recover the backpointers for non-emitting states. */
    for (s = 0; s < args->n_active; ++s) {
        if (args->bp && state_seq[args->active[s]].mixw == TYING_NON_EMITTING) {
#if FORWARD_DEBUG
	    E_INFO("Snapping backpointer for %d, %d => %d(%d)\n",
	           args->active[s], args->bp_t[s], args->amap[args->bp_t[s]], args->active[amap[args->bp_t[s]]]);
#endif
	    args->bp_t[s] = args->amap[args->bp_t[s]];
	    args->bp_t[s] = args->amap[args->bp_t[s]];
        }
    }
    /* And finally deactive all states. */
    for (s = 0; s < args->n_active; ++s) {
        args->amap[args->active[s]] = INACTIVE;
    }
    args->n_active_astate_t = args->n_active;
    args->n_next_active = 0;

    args->n_sum_active += args->n_active;
}


/*
 * Log record.  Maintained by RCS.
 *
 * $Log$
 * Revision 1.6  2006/03/27  04:08:57  dhdfu
 * Optionally use a set of phoneme segmentations to constrain Baum-Welch
 * training.
 * 
 * Revision 1.5  2004/07/21 18:30:33  egouvea
 * Changed the license terms to make it the same as sphinx2 and sphinx3.
 *
 * Revision 1.4  2004/06/17 19:17:14  arthchan2003
 * Code Update for silence deletion and standardize the name for command -line arguments
 *
 * Revision 1.3  2001/04/05 20:02:31  awb
 * *** empty log message ***
 *
 * Revision 1.2  2000/09/29 22:35:13  awb
 * *** empty log message ***
 *
 * Revision 1.1  2000/09/24 21:38:31  awb
 * *** empty log message ***
 *
 * Revision 1.13  97/07/16  11:36:22  eht
 * *** empty log message ***
 * 
 * Revision 1.12  1996/07/29  16:15:18  eht
 * - gauden_compute call arguments made (hopefully) more rational
 * - change alpha and scale rep to float64 from float32
 * - smaller den and den_idx structures
 *
 * Revision 1.11  1996/03/26  13:51:08  eht
 * - Deal w/ beam float32 -> float64 bug
 * - Deal w/ case when # densities referenced in an utterance is much fewer than the
 *   total # of densities to train.
 * - Deal w/ adflag[] not set bug
 *
 * Revision 1.10  1996/03/04  15:58:56  eht
 * added more cpu time counters
 *
 * Revision 1.9  1996/02/02  17:39:53  eht
 * - Uncomment pruning.
 * - Allow a list of active Gaussians to be evaluated rather than all.
 *
 * Revision 1.8  1995/12/14  19:49:26  eht
 * - Add code to only normalize gaussian densities when 1 tied mixture is used.  O/W don't do.
 * - Add code to warn about zero output probability for emitting states.
 *
 * Revision 1.7  1995/10/12  18:30:22  eht
 * Made state.h a "local" header file
 *
 * Revision 1.6  1995/10/10  12:43:50  eht
 * Changed to use <sphinxbase/prim_type.h> = ckd_calloc(n_state, sizeof(uint32));
 *
 * Revision 1.5  1995/10/09  14:55:33  eht
 * Change interface to new ckd_alloc routines
 *
 * Revision 1.4  1995/09/14  14:19:36  eht
 * reformatted slightly
 *
 * Revision 1.3  1995/08/09  20:17:41  eht
 * hard code the beam for now
 *
 * Revision 1.2  1995/07/07  11:57:54  eht
 * Changed formatting and content of verbose output and
 * make scale sum of alphas for a given time rather than
 * max alpha to make SPHINX-II comparison easier.
 *
 * Revision 1.1  1995/06/02  20:41:22  eht
 * Initial revision
 *
 *
 */
