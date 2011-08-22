
/*
 * CUDA device related structures and methods for Baum-Welch algorithm.
 */
#ifndef DEVICE_ALLOC_H
#define DEVICE_ALLOC_H

#include <s3/vector.h>
#include <s3/model_inventory.h>
#include <s3/state.h>


/* measure time spent in gauden precomputation */
/*#define STOPWATCH*/

/* gauden precomputation on device */
#define GAUDEN_DEV

/* gauden precomputation on host */
/*#define GAUDEN_HOST*/

/* output precomputed densities */
/*#define DENSITIES_DEBUG*/


#ifdef __cplusplus
/* for NVCC compiles code as C++ and mangles the names */
extern "C" {
#endif

typedef struct gauden_dev_s {

    uint32 n_feat;
    uint32 n_mgau;
    uint32 n_density;
    uint32 n_top;
    uint32 n_cb_inverse;
    uint32 n_active_state;

    uint32 *d_veclen;
    uint32 maxveclen;
    float32 *d_norm;
    
    float **d_mean_idx;
    float *d_mean_buf;
    uint32 d_mean_buflen;
    
    float **d_var_idx;
    float *d_var_buf;
    uint32 d_var_buflen;
    
    float **d_feature_idx;
    float *d_feature_buf;
    uint32 d_feature_buflen;
    uint32 d_feature_n_obs;
    
    float64 *d_den;
    uint32 *d_den_idx;

    uint32 *d_cb;
    uint32 *d_l_cb;
    uint32 *d_active_states;
    
} gauden_dev_t;


#ifdef STOPWATCH
void startTimer(struct timeval *timer);
uint32 stopTimer(struct timeval *timer);
#endif


/* Device multidimensional matrix allocation and dealocation. */
void
gauden_dev_free(gauden_dev_t *g);

gauden_dev_t *
gauden_dev_copy(uint32 block_size, vector_t **feature, uint32 n_obs, model_inventory_t *inv, state_t *state_seq, uint32 n_state);


void *
device_alloc_3d(size_t d1, size_t d2, size_t d3, size_t elemsize);

void *
device_alloc_4d(size_t d1, size_t d2, size_t d3, size_t d4, size_t elemsize);

void
device_free_3d(void *inptr);

void
device_free_4d(void *inptr);


#ifdef __cplusplus
}
#endif

#endif /* DEVICE_ALLOC_H */

