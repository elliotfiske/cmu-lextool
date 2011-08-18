
#ifndef DEVICE_ALLOC_H
#define DEVICE_ALLOC_H

#include <s3/vector.h>


typedef struct gauden_dev_s {
    uint32 n_feat;
    uint32 *veclen;
    uint32 max_veclen;

    uint32 n_mgau;
    uint32 n_density;
    uint32 n_top;

    float32 *norm;
    
    float *mean;
    uint64 *meanIdx;
    uint64 meanIdx0;
    
    float *var;
    uint64 *varIdx;
    uint64 varIdx0;
    
    float *fullvar;
    uint64 *fullvarIdx;
    uint64 fullvarIdx0;
    
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

