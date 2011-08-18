
#ifndef DEVICE_ALLOC_H
#define DEVICE_ALLOC_H

#include <s3/vector.h>


typedef struct gauden_dev_s {

    uint32 n_feat;
    uint32 n_mgau;
    uint32 n_density;
    uint32 n_top;
    uint32 n_cb_inverse;
    uint32 n_state;

    uint32 *d_veclen;
    float32 *d_norm;
    
    float *d_mean_idx;
    float *d_mean_buf;
    uint32 d_mean_buflen;
    
    float *d_var_idx;
    float *d_var_buf;
    uint32 d_var_buflen;
    
    uint32 *d_cb;
    uint32 *d_l_cb;
    uint32 *d_mixw;
    
} gauden_dev_t;


void *
device_alloc_3d(size_t d1, size_t d2, size_t d3, size_t elemsize);

void *
device_alloc_4d(size_t d1, size_t d2, size_t d3, size_t d4, size_t elemsize);

void
device_free_3d(void *inptr);

void
device_free_4d(void *inptr);


#endif /* DEVICE_ALLOC_H */

