/*
 * live_norm.c - feature normalization for live system
 * 
 * HISTORY
 * 
 * 20-Aug-98	M K Ravishankar (rkm@cs.cmu.edu) at Carnegie Mellon University
 * 		Added c0 to cepmean_set() and cepmean_get().  Changed initialization
 * 		to a more reasonable value for c0.
 * 
 * 11-Apr-98	M K Ravishankar (rkm@cs.cmu.edu) at Carnegie Mellon University
 * 		Extended CMN to c0.
 * 
 * 01-May-96	M K Ravishankar (rkm@cs.cmu.edu) at Carnegie Mellon University
 * 		Added functions cepmean_set and cepmean_get.
 */

/*
 |------------------------------------------------------------*
 | $Header$
 |------------------------------------------------------------*
 | Description
 |
 |   mean_norm_init(int32 vlen)
 |     Initialize subsystem to normalize feature vectors of
 |     length VLEN.
 |
 |   mean_norm_acc_sub(float *vec)
 |     Add coefficents to running total and subtract the current
 |     estimated mean from VEC.  Increments the number of input
 |     frames.
 |
 |   mean_norm_update()
 |     Computes a new mean from the accumulated coefficient totals and
 |     the number of input frames seen since the last update.
 |     The new mean is averaged with the prior value for the mean to
 |     obtain the estimated mean for the next utterance.
 */

#include <stdio.h>
#include <stdlib.h>
#include "err.h"


#include "s2types.h"

static int   veclen;		/* the feature vector length */
static float *cur_mean = NULL;	/* the mean subtracted from input frames */
static float *sum = NULL;	/* the sum over input frames */
static int   nframe;		/* the total number of input frames */

#define CMN_WIN_HWM	800	/* #frames after which window shifted */
#define CMN_WIN		500


int32 cepmean_set (float *vec)
{
    int32 i;
    
    for (i = 0; i < veclen; i++) {
	cur_mean[i] = vec[i];
	sum[i] = vec[i] * CMN_WIN;
    }
    nframe = CMN_WIN;
    
    return 0;
}

int32 cepmean_get (float *vec)
{
    int32 i;
    
    for (i = 0; i < veclen; i++)
	vec[i] = cur_mean[i];
    return 0;
}

void mean_norm_init(int32 vlen)
{
    veclen   = vlen;
    cur_mean = (float *) calloc(veclen, sizeof(float));
    cur_mean[0] = 8.0;
    sum      = (float *) calloc(veclen, sizeof(float));
    nframe   = 0;
    E_INFO("mean[0]= %.2f, mean[1..%d]= 0.0\n", cur_mean[0], veclen-1);
}

static void
mean_norm_shiftwin ( void )
{
    int32 i;
    double sf;

    sf = (1.0/nframe);
    for (i = 0; i < veclen; i++)
	cur_mean[i] = sum[i] * sf;

    /* Make the accumulation decay exponentially */
    if (nframe >= CMN_WIN_HWM) {
	sf = CMN_WIN * sf;
	for (i = 0; i < veclen; i++)
	    sum[i] *= sf;
	nframe = CMN_WIN;
    }
}

void mean_norm_acc_sub(float *vec)
{
    int32 i;

    for (i = 0; i < veclen; i++) {
	sum[i] += vec[i];
	vec[i] -= cur_mean[i];
    }

    ++nframe;

    if (nframe > CMN_WIN_HWM)
	mean_norm_shiftwin();
}

void mean_norm_update(void)
{
    int32 i;
    double sf;
    
    if (nframe <= 0)
	return;
    
    E_INFO("mean_norm_update: from < ");
    for (i = 0; i < veclen; i++)
	E_INFO("%5.2f ", cur_mean[i]);
    E_INFO(">\n");

    sf = (1.0/nframe);
    for (i = 0; i < veclen; i++)
	cur_mean[i] = sum[i] * sf;

    /* Make the accumulation decay exponentially */
    if (nframe > CMN_WIN_HWM) {
	sf = CMN_WIN * sf;
	for (i = 0; i < veclen; i++)
	    sum[i] *= sf;
	nframe = CMN_WIN;
    }

    E_INFO("mean_norm_update: to   < ");
    for (i = 0; i < veclen; i++)
	E_INFO("%5.2f ", cur_mean[i]);
    E_INFO(">\n");
}
