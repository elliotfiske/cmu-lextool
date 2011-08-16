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
 *         Eric H. Thayer (eht@cs.cmu.edu)
 *********************************************************************/

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

#include "forward.h"

#define FORWARD_DEBUG 0
#define INACTIVE        0xffff

void
forward_init_arrays(
        float64 ***active_alpha,
        uint32 ***active_astate,
        uint32 **n_active_astate,
        uint32 ***bp,
        float64 **scale,
        float64 ***dscale,
        uint32 n_obs)
{
    *active_alpha = (float64 **)ckd_calloc(n_obs, sizeof(float64 *));
    *active_astate = (uint32 **)ckd_calloc(n_obs, sizeof(uint32 *));
    *n_active_astate = (uint32 *)ckd_calloc(n_obs, sizeof(uint32));
    *scale = (float64 *)ckd_calloc(n_obs, sizeof(float64));
    *dscale = (float64 **)ckd_calloc(n_obs, sizeof(float64 *));
    if (bp) {
        *bp = (uint32 **)ckd_calloc(n_obs, sizeof(uint32 *));
    }
    memset(*active_alpha, 0, n_obs * sizeof(float64 *));
    memset(*active_astate, 0, n_obs * sizeof(uint32 *));
    memset(*n_active_astate, 0, n_obs * sizeof(uint32));
    memset(*scale, 0, n_obs * sizeof(float64));
    memset(*dscale, 0, n_obs * sizeof(float64 *));
    if (bp) {
        memset(*bp, 0, n_obs * sizeof(uint32 *));
    }
}

void forward_free_arrays(
        float64 ***active_alpha,
        uint32 ***active_astate,
        uint32 **n_active_astate,
        uint32 ***bp,
        float64 **scale,
        float64 ***dscale)
{
    ckd_free(*active_alpha);
    ckd_free(*active_astate);
    ckd_free(*n_active_astate);
    ckd_free(*scale);
    ckd_free(*dscale);
    if (bp) {
        ckd_free(*bp);
    }

    *active_alpha = NULL;
    *active_astate = NULL;
    *n_active_astate = NULL;
    *scale = NULL;
    *dscale = NULL;
    if (bp) {
        *bp = NULL;
    }
}

void
forward_clear_arrays(
        float64 **active_alpha,
        uint32 **active_astate,
        uint32 **bp,
        float64 **dscale,
        uint32 n_obs)
{
    uint32 t;
    
    for (t = 0; t < n_obs; t++) {
        ckd_free(active_alpha[t]);
        ckd_free(active_astate[t]);
        ckd_free(dscale[t]);
        if (bp) {
            ckd_free(bp[t]);
        }
    }
}



/*********************************************************************
 *
 * Function: 
 *         forward
 *
 * Description: 
 *         This function computes the scaled forward variable, alpha.
 *
 *        In order to conserve memory for long utterances, only the
 *        active (i.e. non-zero) alpha values are stored.
 *
 * Function Inputs: 
 *         float64 **active_alpha -
 *                On the successful return of this function,
 *                this array contains the scaled alpha variable for
 *                active states for all input observations.  For any
 *                input observation at time t (t >= 0  && t < n_obs), the number
 *                of active states can be found by accessing
 *                n_active_astate[t].  So for some time t, the active
 *                scaled alpha values are active_alpha[t][i] where
 *                i >= 0 and i < n_active_astate[t].  For some valid t and
 *                i, the sentence HMM state id can be found by accessing
 *                active_astate[t][i].
 *
 *         uint32 **active_astate -
 *                On the successful return of this function,
 *                this array contains the mapping of active state indexes
 *                into sentence HMM indexes (i.e. indexes into the state_seq[]
 *                array).  The active states of the sentence HMM may be
 *                enumerated by accessing active_astate[t][i] over all
 *                t (t >= 0 && t < n_obs) and i
 *                (i >= 0 && i < n_active_astate[t]).
 *
 *         uint32 *n_active_astate -
 *                On the successful return of this function,
 *                this array contains the number of active states for all
 *                t (t >= 0 && t < n_obs).
 *
 *        float64 *scale -
 *                On the successful return of this function,
 *                 this variable contains the scale factor applied to the
 *                alpha variable for all t (t >= 0 && t < n_obs).
 *
 *        vector_t **feature -
 *                This variable contains the input observation vectors.
 *                The value feature[t][f] must be valid for
 *                t >= 0 && t < n_obs and f >= 0 && f < n_feat) where
 *                n_feat is the # of assumed statistically independent
 *                feature streams to be modelled.
 *
 *         uint32 **bp -
 *                On the successful return of this function,
 *                this array contains backtrace pointers for active states
 *              for all input observations except the first timepoint.
 *              As in active_alpha[], the sentence HMM state id can be
 *              found by accessing active_astate[t][i].  NOTE!
 *              This is a "raw" backpointer array and as such, it contains
 *              pointers to non-emitting states.  These pointers refer to
 *              the current frame rather than the previous one.  Thus,
 *              the state id of the backpointer is either
 *              active_astate[t-1][bp[t][i]] (for normal states) or
 *              active_astate[t][bp[t][i]] (for non-emitting states).
 *
 *        uint32 n_obs -
 *                This variable contains the number of input observation
 *                vectors seen given the model.
 *
 *        state_t *state_seq -
 *                This is a list of model state structures which define
 *                the sentence HMM for this observation sequence.
 *
 *        uint32 n_state -
 *                The total # of states in the sentence HMM for this
 *                utterance.
 *
 *        model_inventory_t *inv -
 *                This structure contains the inventory of initial acoustic
 *                model parameters.
 *
 *        float64 beam -
 *                A pruning beam to apply to the evaluation of the alpha
 *                variable.
 *
 *      s3phseg_t *phseg -
 *              An optional phone segmentation to use to constrain the
 *              forward lattice.
 *
 * Global Inputs: 
 *         None
 *
 * Return Values: 
 *
 *         S3_SUCCESS
 *                The alpha variable was completed successfully for this
 *                observation sequence and model.
 *
 *        S3_ERROR
 *                Some error was detected that prevented the computation of the
 *                variable.
 *
 * Global Outputs: 
 *         None
 *
 * Errors: 
 *         - Initial alpha value < epsilon
 *        - If semi-continuous models, unable to normalize input frame
 *        - Output liklihood underflow
 *        - Alpha variable < epsilon for all active states
 *
 *********************************************************************/


int32
forward(float64 **active_alpha,
        uint32 **active_astate,
        uint32 *n_active_astate,
        uint32 **bp,
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
    int32 retval = S3_SUCCESS;
        
    float64 **red_active_alpha;
    uint32 **red_active_astate;
    uint32 *red_n_active_astate;
    uint32 **red_bp = NULL;
    float64 *red_scale;
    float64 **red_dscale;
    
    uint32 block_size = 11;
    uint32 n_red = ceil(n_obs / (float64)block_size);
    int32 t;    /* should be signed! iterating backwards to 0 and checking by >= 0 */

    /*
     * Allocate space for the initial state in the alpha
     * and active state arrays
     * Allocate the bestscore array for embedded Viterbi
     */
    
    forward_init_arrays(&red_active_alpha, &red_active_astate, &red_n_active_astate, &red_bp, &red_scale, &red_dscale, n_red);
    
    retval = forward_reduced(
            red_active_alpha, red_active_astate, red_n_active_astate, red_bp, red_scale, red_dscale,
            feature, block_size, n_obs, state_seq, n_state, inv, beam, phseg, mmi_train);
    if (retval != S3_SUCCESS) {
        goto cleanup;
    }
    
    for (t = n_red - 1; t >= 0; t--) {
        retval = forward_recompute(
            active_alpha + (t * block_size), active_astate + (t * block_size), n_active_astate + (t * block_size),
            bp + (t * block_size), scale + (t * block_size), dscale + (t * block_size),
            red_active_alpha, red_active_astate, red_n_active_astate, red_bp, red_scale, red_dscale,
            feature, t, block_size, n_obs, state_seq, n_state, inv, beam, phseg, mmi_train);
            
        if (retval != S3_SUCCESS) {
            goto cleanup;
        }
        
    }
    
cleanup:
    forward_clear_arrays(red_active_alpha, red_active_astate, red_bp, red_dscale, n_red);
    forward_free_arrays(&red_active_alpha, &red_active_astate, &red_n_active_astate, &red_bp, &red_scale, &red_dscale);

    return retval;
}


int32
forward_recompute(float64 **loc_active_alpha,
        uint32 **loc_active_astate,
        uint32 *loc_n_active_astate,
        uint32 **loc_bp,
        float64 *loc_scale,
        float64 **loc_dscale,
        float64 **red_active_alpha,
        uint32 **red_active_astate,
        uint32 *red_n_active_astate,
        uint32 **red_bp,
        float64 *red_scale,
        float64 **red_dscale,
        vector_t **feature,
        uint32 block_idx,
        uint32 block_size,
        uint32 n_obs,
        state_t *state_seq,
        uint32 n_state,
        model_inventory_t *inv,
        float64 beam,
        s3phseg_t *phseg,
        uint32 mmi_train)
{
    int32 retval = S3_SUCCESS;

    uint32 block_obs = block_size;
    
    if (block_idx * block_size + block_obs > n_obs) {
        block_obs = n_obs - block_idx * block_size;
    }
    
    loc_n_active_astate[0] = red_n_active_astate[block_idx];
    
    loc_active_alpha[0] = (float64 *)ckd_calloc(loc_n_active_astate[0], sizeof(float64));
    memcpy(loc_active_alpha[0], red_active_alpha[block_idx], loc_n_active_astate[0] * sizeof(float64));
    
    loc_active_astate[0] = (uint32 *)ckd_calloc(loc_n_active_astate[0], sizeof(uint32));
    memcpy(loc_active_astate[0], red_active_astate[block_idx], loc_n_active_astate[0] * sizeof(uint32));
    
    loc_scale[0] = red_scale[block_idx];
    
    loc_dscale[0] = (float64 *)ckd_calloc(inv->gauden->n_feat, sizeof(float64));
    memcpy(loc_dscale[0], red_dscale[block_idx], inv->gauden->n_feat * sizeof(float64));
    
    if (loc_bp) {
        loc_bp[0] = (uint32 *)ckd_calloc(loc_n_active_astate[0], sizeof(uint32));
        memcpy(loc_bp[0], red_bp[block_idx], loc_n_active_astate[0] * sizeof(uint32));
    }
    
    retval = forward_local(
        loc_active_alpha, loc_active_astate, loc_n_active_astate, loc_bp, loc_scale, loc_dscale,
        feature + (block_idx * block_size), block_obs, state_seq, n_state, inv, beam, phseg, mmi_train, (block_idx * block_size));
    
    return retval;
}

int32
forward_reduced(float64 **active_alpha,
        uint32 **active_astate,
        uint32 *n_active_astate,
        uint32 **bp,
        float64 *scale,
        float64 **dscale,
        vector_t **feature,
        uint32 block_size,
        uint32 n_obs,
        state_t *state_seq,
        uint32 n_state,
        model_inventory_t *inv,
        float64 beam,
        s3phseg_t *phseg,
        uint32 mmi_train)
{
    int32 retval = S3_SUCCESS;
        
    float64 **loc_active_alpha;
    uint32 **loc_active_astate;
    uint32 *loc_n_active_astate;
    uint32 **loc_bp = NULL;
    float64 *loc_scale;
    float64 **loc_dscale;
    
    uint32 n_red = ceil(n_obs / (float64)block_size);
    int t;

    /*
     * Allocate space for the initial state in the alpha
     * and active state arrays
     * Allocate the bestscore array for embedded Viterbi
     */
    
    forward_init_arrays(&loc_active_alpha, &loc_active_astate, &loc_n_active_astate, &loc_bp, &loc_scale, &loc_dscale, block_size + 1);
    
    loc_active_alpha[0] = (float64 *)ckd_calloc(1, sizeof(float64));
    loc_active_astate[0] = (uint32 *)ckd_calloc(1, sizeof(uint32));
    loc_n_active_astate[0] = 1;

    loc_active_alpha[0][0] = 1.0;
    loc_active_astate[0][0] = 0;
    
    if (bp) {
        loc_bp[0] = (uint32 *)ckd_calloc(1, sizeof(uint32));
    }
    
    for (t = 0; t < n_red; t++) {
        uint32 block_obs = block_size + 1;
        
        if (t * block_size + block_obs > n_obs) {
            block_obs = n_obs - t * block_size;
        }

        retval = forward_local(
            loc_active_alpha, loc_active_astate, loc_n_active_astate, loc_bp, loc_scale, loc_dscale,
            feature + (t * block_size), block_obs, state_seq, n_state, inv, beam, phseg, mmi_train, (t * block_size));
        if (retval != S3_SUCCESS) {
            goto cleanup;
        }
        
        n_active_astate[t] = loc_n_active_astate[0];

        active_alpha[t] = (float64 *)ckd_calloc(n_active_astate[t], sizeof(float64));
        memcpy(active_alpha[t], loc_active_alpha[0], n_active_astate[t] * sizeof(float64));

        active_astate[t] = (uint32 *)ckd_calloc(n_active_astate[t], sizeof(uint32));
        memcpy(active_astate[t], loc_active_astate[0], n_active_astate[t] * sizeof(uint32));

        scale[t] = loc_scale[0];

        dscale[t] = (float64 *)ckd_calloc(inv->gauden->n_feat, sizeof(float64));
        memcpy(dscale[t], loc_dscale[0], inv->gauden->n_feat * sizeof(float64));
        if (bp) {
            bp[t] = (uint32 *)ckd_calloc(n_active_astate[t], sizeof(uint32));
            memcpy(bp[t], loc_bp[0], n_active_astate[t] * sizeof(uint32));
        }
        
        if (t < n_red - 1) {
            loc_n_active_astate[0] = loc_n_active_astate[block_size];
            
            loc_active_alpha[0] = (float64 *)ckd_realloc(loc_active_alpha[0], loc_n_active_astate[0] * sizeof(float64));
            memcpy(loc_active_alpha[0], loc_active_alpha[block_size], loc_n_active_astate[0] * sizeof(float64));
            
            loc_active_astate[0] = (uint32 *)ckd_realloc(loc_active_astate[0], loc_n_active_astate[0] * sizeof(uint32));
            memcpy(loc_active_astate[0], loc_active_astate[block_size], loc_n_active_astate[0] * sizeof(uint32));
            
            loc_scale[0] = loc_scale[block_size];
            
            memcpy(loc_dscale[0], loc_dscale[block_size], inv->gauden->n_feat * sizeof(float64));
            if (bp) {
                loc_bp[0] = (uint32 *)ckd_realloc(loc_bp[0], loc_n_active_astate[0] * sizeof(uint32));
                memcpy(loc_bp[0], loc_bp[block_size], loc_n_active_astate[0] * sizeof(uint32));
            }
        }
        forward_clear_arrays(loc_active_alpha + 1, loc_active_astate + 1, (bp ? (loc_bp + 1) : NULL), loc_dscale + 1, block_obs - 1);
    }
    forward_clear_arrays(loc_active_alpha, loc_active_astate, (bp ? loc_bp : NULL), loc_dscale, 1);
    
cleanup:
    forward_free_arrays(&loc_active_alpha, &loc_active_astate, &loc_n_active_astate, &loc_bp, &loc_scale, &loc_dscale);

    return retval;
}

/*#include <sys/time.h>

void startTimer(struct timeval *timer){
        gettimeofday(timer, NULL);
}

int stopTimer(struct timeval *timer){
        struct timeval tmp;
        gettimeofday(&tmp, NULL);
        tmp.tv_sec -= timer->tv_sec;
        tmp.tv_usec -= timer->tv_usec;
        if (tmp.tv_usec < 0){
                tmp.tv_usec+=1000000;
                tmp.tv_sec--;
        }
        return (int)(tmp.tv_usec + tmp.tv_sec*1000000);
}*/


int32
forward_local(float64 **active_alpha,
        uint32 **active_astate,
        uint32 *n_active_astate,
        uint32 **bp,
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
        uint32 t_offset)
{
    int32 retval = S3_SUCCESS;
    
    uint32 *next_active = (uint32 *)ckd_calloc(n_state, sizeof(uint32));
    uint32 *active_l_cb = (uint32 *)ckd_calloc(n_state, sizeof(uint32));
    uint16 *amap = (uint16 *)ckd_calloc(n_state, sizeof(uint16));
    uint32 *acbframe = (uint32 *)ckd_calloc(inv->n_cb_inverse, sizeof(uint32));

    float64 ****now_den = (float64 ****)ckd_calloc_4d(n_obs, inv->n_cb_inverse, gauden_n_feat(inv->gauden), gauden_n_top(inv->gauden),
                                         sizeof(float64));
    uint32 ****now_den_idx = (uint32 ****)ckd_calloc_4d(n_obs, inv->n_cb_inverse, gauden_n_feat(inv->gauden), gauden_n_top(inv->gauden),
                                            sizeof(uint32));

    float64 *best_pred = (float64 *)ckd_calloc(1, sizeof(float64));
    uint32 aalpha_alloc = n_active_astate[0];
    
    float64 outprob_0;
    uint32 t, i;

    if (t_offset == 0) {
        /* Compute the component Gaussians for state 0 mixture density */
        gauden_compute_log(now_den[0][state_seq[0].l_cb],
                       now_den_idx[0][state_seq[0].l_cb],
                       feature[0],
                       inv->gauden,
                       state_seq[0].cb, NULL);

        active_l_cb[0] = state_seq[0].l_cb;
        acbframe[state_seq[0].l_cb] = 0;
                       
        dscale[0] = gauden_scale_densities_fwd(now_den[0], now_den_idx[0],
                                           active_l_cb, 1, inv->gauden);

        /* Compute the mixture density value for state 0 time 0 */
        outprob_0 = gauden_mixture(now_den[0][state_seq[0].l_cb],
                                now_den_idx[0][state_seq[0].l_cb],
                                inv->mixw[state_seq[0].mixw],
                                inv->gauden);
        if (outprob_0 <= MIN_IEEE_NORM_POS_FLOAT32) {
            E_ERROR("Small output prob (== %.2e) seen at frame 0 state 0\n", outprob_0);
            retval = S3_ERROR;
            goto cleanup;
        }
        /* Compute scale for t == 0 */
        scale[0] = 1.0 / outprob_0;
    }
    
/*    struct timeval timer;
    startTimer(&timer);*/
    
    for (t = 1; t < n_obs; t++) {
        for (i = 0; i < n_state; i++) {
            if (state_seq[i].mixw != TYING_NON_EMITTING) {
                uint32 l_cb = state_seq[i].l_cb;

                gauden_compute_log(now_den[t][l_cb], now_den_idx[t][l_cb],
                   feature[t], inv->gauden, state_seq[i].cb, ((inv->n_cb_inverse == 1) ? now_den_idx[t-1][l_cb] : NULL));
                           /* Preinitializing topn only really makes a difference 
                              for semi-continuous (inv->n_cb_inverse == 1) models. */
            }
        }
    }
    
/*    int tm = stopTimer(&timer);
    E_INFO("MICHAL: n_obs=%u n_state=%u time=%u\n", n_obs, n_state, tm);*/

    /* Initialize the active state map such that all states are inactive */
    for (i = 0; i < n_state; i++) {
        amap[i] = INACTIVE;
    }
    
    if (bp) {
        best_pred = (float64 *)ckd_calloc(aalpha_alloc, sizeof(float64));
    }
    
    /* Compute scaled alpha over all remaining time in the utterance */
    for (t = 1; t < n_obs; t++) {
    
        uint32 n_next_active = 0;
        uint32 n_active_l_cb = 0;
        int can_prune_phseg = 0;
        float64 balpha = 0.0;
        uint32 i, j, s, u;

        /* assume next active state set about the same size as current;
           adjust to actual size as necessary later */
        active_alpha[t] = (float64 *)ckd_calloc(aalpha_alloc, sizeof(float64));
        if (bp) {
            bp[t] = (uint32 *)ckd_calloc(aalpha_alloc, sizeof(uint32));
            /* reallocate the best score array and zero it out */
            best_pred = (float64 *)ckd_realloc(best_pred, aalpha_alloc * sizeof(float64));
            memset(best_pred, 0, aalpha_alloc * sizeof(float64));
        }

        /* For all active states at the previous frame, activate their
           successors in this frame and compute codebooks. */
        /* (these are pre-computed so they can be scaled to avoid underflows) */
        for (s = 0; s < n_active_astate[t-1]; s++) {
            i = active_astate[t-1][s];
            /* get list of states adjacent to active state i */
            /* activate them all, computing their codebook densities if necessary */
            for (u = 0; u < state_seq[i].n_next; u++) {
            
                j = state_seq[i].next_state[u];

                if (state_seq[j].mixw != TYING_NON_EMITTING) {
                    if (amap[j] == INACTIVE) {
                        uint32 l_cb = state_seq[j].l_cb;
                        
                        if (acbframe[l_cb] != t) {
                            /* Component density values not yet computed */
                            active_l_cb[n_active_l_cb++] = l_cb;
                            acbframe[l_cb] = t;
                        }

                        /* Initialize the alpha variable to zero */
                        active_alpha[t][n_next_active] = 0;

                        /* Put next state j into the active list */
                        /* Map active state list index to sentence HMM index */
                        amap[j] = n_next_active;
                        next_active[n_next_active] = j;

                        ++n_next_active;

                        if (n_next_active == aalpha_alloc) {
                            /* Need to reallocate the active_alpha array */
                            aalpha_alloc *= 2;
                            active_alpha[t] = (float64 *)ckd_realloc(active_alpha[t],
                                                          sizeof(float64) * aalpha_alloc);
                            
                            if (bp) {
                                bp[t] = (uint32 *)ckd_realloc(bp[t],
                                                    sizeof(uint32) * aalpha_alloc);
                                best_pred = (float64 *)ckd_realloc(best_pred,
                                                                   sizeof(float64) * aalpha_alloc);
                                memset(bp[t] + aalpha_alloc / 2,
                                       0, sizeof(uint32) * (aalpha_alloc / 2));
                                memset(best_pred + aalpha_alloc / 2,
                                       0, sizeof(float64) * (aalpha_alloc / 2));
                            }
                        }
                    }
                }
            }
        }

        /* Cope w/ numerical issues by dividing densities by max density */
        dscale[t] = gauden_scale_densities_fwd(now_den[t], now_den_idx[t],
                                               active_l_cb, n_active_l_cb, inv->gauden);
        
        /* Now, for all active states in the previous frame, compute
           alpha for all successors in this frame. */
        for (s = 0; s < n_active_astate[t-1]; s++) {
            i = active_astate[t-1][s];

            /* For all emitting states j adjacent to i, update their
             * alpha values.  */
            for (u = 0; u < state_seq[i].n_next; u++) {
                j = state_seq[i].next_state[u];
                uint32 l_cb = state_seq[j].l_cb;

                if (state_seq[j].mixw != TYING_NON_EMITTING) {
                    /* Next state j is an emitting state */
                    float64 outprob_j = gauden_mixture(now_den[t][l_cb],
                                        now_den_idx[t][l_cb],
                                        inv->mixw[state_seq[j].mixw],
                                        inv->gauden);
                    /* update backpointers bp[t][j] */
                    float64 x = active_alpha[t-1][s] * state_seq[i].next_tprob[u];
                    if (bp) {
                        if (x > best_pred[amap[j]]) {
                            best_pred[amap[j]] = x;
                            bp[t][amap[j]] = s;
                        }
                    }
                    
                    /* update the unscaled alpha[t][j] */
                    active_alpha[t][amap[j]] += x * outprob_j;
                }
                else {
                    /* already done below in the prior time frame */
                }
            }
        }

        /* Now, for all active states in this frame, consume any
           following non-emitting states (multiplying in their
           transition probabilities)  */
        for (s = 0; s < n_next_active; s++) {
            i = next_active[s];

            for (u = 0; u < state_seq[i].n_next; u++) {
                j = state_seq[i].next_state[u];
                /* for any non-emitting ones */
                if (state_seq[j].mixw == TYING_NON_EMITTING) {
                    float64 x = active_alpha[t][s] * state_seq[i].next_tprob[u];

                    /* activate this state if necessary */
                    if (amap[j] == INACTIVE) {
                        active_alpha[t][n_next_active] = 0;
                        amap[j] = n_next_active;
                        next_active[n_next_active] = j;
                        ++n_next_active;

                        if (n_next_active == aalpha_alloc) {
                            aalpha_alloc *= 2;
                            active_alpha[t] = (float64 *)ckd_realloc(active_alpha[t],
                                                          sizeof(float64) * aalpha_alloc);
                            
                            if (bp) {
                                bp[t] = (uint32 *)ckd_realloc(bp[t],
                                                    sizeof(uint32) * aalpha_alloc);
                                best_pred = (float64 *)ckd_realloc(best_pred,
                                                                   sizeof(float64) * aalpha_alloc);
                                memset(bp[t] + aalpha_alloc / 2,
                                       0, sizeof(uint32) * (aalpha_alloc / 2));
                                memset(best_pred + aalpha_alloc / 2,
                                       0, sizeof(float64) * (aalpha_alloc / 2));
                            }
                        }
                        if (bp) {
                            /* Give its backpointer a default value */
                            bp[t][amap[j]] = s;
                            best_pred[amap[j]] = x;
                        }
                    }

                    /* update backpointers bp[t][j] */
                    if (bp && x > best_pred[amap[j]]) {
                        bp[t][amap[j]] = s;
                        best_pred[amap[j]] = x;
                    }
                    /* update its alpha value */
                    active_alpha[t][amap[j]] += x;
                }
            }
        }

        /* find best alpha value in current frame for pruning and scaling purposes */
        /* also take the argmax to find the best backtrace */
        for (s = 0; s < n_next_active; s++) {
            if (balpha < active_alpha[t][s]) {
                balpha = active_alpha[t][s];
            }
        }

        /* cope with some pathological case */
        if (balpha == 0.0 && n_next_active > 0) {
            E_ERROR("All %u active states,", n_next_active);
            for (s = 0; s < n_next_active; s++) {
                if (state_seq[next_active[s]].mixw != TYING_NON_EMITTING)
                    fprintf(stderr, " %u", state_seq[next_active[s]].mixw);
                else
                    fprintf(stderr, " N(%u,%u)",
                            state_seq[next_active[s]].tmat, state_seq[next_active[s]].m_state);

            }
            fprintf(stderr, ", zero at time %u\n", t);
            fflush(stderr);
            retval = S3_ERROR;
            goto cleanup;
        }

        /* and some related pathological cases */
        if (balpha < 1e-300) {
            E_ERROR("Best alpha < 1e-300\n");
            retval = S3_ERROR;
            goto cleanup;
        }
        if (n_next_active == 0) {
            E_ERROR("No active states at time %u\n", t);
            retval = S3_ERROR;
            goto cleanup;
        }

        /* compute the scale factor */
        scale[t] = 1.0 / balpha;
    
        /* Determine if phone segmentation-based pruning would leave
         * us with an empty active list (that would be bad!) */
        if (phseg) {
            /* Find active phone for this timepoint. */
            /* Move the pointer forward if necessary. */
            while ((t + t_offset) > phseg->ef)
                phseg = phseg->next;
                
            for (s = 0; s < n_next_active; ++s) 
                if (acmod_set_base_phone(inv->mdef->acmod_set, state_seq[next_active[s]].phn)
                    == acmod_set_base_phone(inv->mdef->acmod_set, phseg->phone))
                    break;
            can_prune_phseg = (s != n_next_active);
        }
        
        /* Prune active states for the next frame and rescale their alphas. */
        n_active_astate[t] = 0;
        active_astate[t] = (uint32 *)ckd_calloc(n_next_active, sizeof(uint32));
        for (s = 0; s < n_next_active; s++) {
            /* "Snap" the backpointers for non-emitting states, so
               that they don't point to bogus indices (we will use
               amap to recover them). */
            if (bp && state_seq[next_active[s]].mixw == TYING_NON_EMITTING) {
                bp[t][s] = next_active[bp[t][s]];
            }
            /* If we have a phone segmentation, use it instead of the beam. */
            if (phseg && can_prune_phseg) {
                if (acmod_set_base_phone(inv->mdef->acmod_set, state_seq[next_active[s]].phn)
                    == acmod_set_base_phone(inv->mdef->acmod_set, phseg->phone)) {
                    active_alpha[t][n_active_astate[t]] = active_alpha[t][s] * scale[t];
                    active_astate[t][n_active_astate[t]] = next_active[s];
                    if (bp)
                        bp[t][n_active_astate[t]] = bp[t][s];
                    amap[next_active[s]] = n_active_astate[t];
                    n_active_astate[t]++;
                }
                else {
                    amap[next_active[s]] = INACTIVE;
                }
            }
            else {
                /* compute the pruning threshold based on the beam */
                float64 pthresh = ((log10(balpha) + log10(beam) > -300) ? (balpha * beam) : 1e-300);
        
                if (active_alpha[t][s] > pthresh) {
                    active_alpha[t][n_active_astate[t]] = active_alpha[t][s] * scale[t];
                    active_astate[t][n_active_astate[t]] = next_active[s];
                    if (bp)
                        bp[t][n_active_astate[t]] = bp[t][s];
                    amap[next_active[s]] = n_active_astate[t];
                    n_active_astate[t]++;
                }
                else {
                    amap[next_active[s]] = INACTIVE;
                }
            }
        }
        
        /* Now recover the backpointers for non-emitting states. */
        if (bp) {
                for (s = 0; s < n_active_astate[t]; ++s) {
                    if (state_seq[active_astate[t][s]].mixw == TYING_NON_EMITTING) {
                        bp[t][s] = amap[bp[t][s]];
                    }
                }
        }
        
        /* And finally deactive all states. */
        for (s = 0; s < n_active_astate[t]; ++s) {
            amap[active_astate[t][s]] = INACTIVE;
        }
    }
    
cleanup:
    ckd_free(next_active);
    ckd_free(amap);
    ckd_free(active_l_cb);

    ckd_free(best_pred);

    ckd_free(acbframe);

    ckd_free_4d((void ****)now_den);
    ckd_free_4d((void ****)now_den_idx);
    
    return retval;
}

