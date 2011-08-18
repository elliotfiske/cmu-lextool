
#include "device_alloc.h"
#include <cutil.h>


__global__ void device_init_3d_kernel(char *mem, char ***ref1, char **ref2, size_t elemsize, size_t d1, size_t d2, size_t d3);
__global__ void device_init_4d_kernel(char *mem, char ****ref1, char ***ref2, char **ref3, size_t elemsize, size_t d1, size_t d2, size_t d3, size_t d4);


void *
device_alloc_3d(size_t d1, size_t d2, size_t d3, size_t elemsize)
{
    dim3 bdim(16, 1, 1);
    dim3 gdim(ceil(d1 / (float)bdim.x), 1, 1);

    char ***ref1, **ref2, *mem;

    cudaMalloc(&mem, d1 * d2 * d3 * elemsize);
    cudaMalloc(&ref2, d1 * d2 * sizeof(void *));
    cudaMalloc(&ref1, d1 * sizeof(void **));

    device_init_3d_kernel<<<gdim, bdim>>>(mem, ref1, ref2, elemsize, d1, d2, d3);

    return ref1;
}


void *
device_alloc_4d(size_t d1,
		  size_t d2,
		  size_t d3,
		  size_t d4,
		  size_t elemsize)
{
    dim3 bdim(16, 1, 1);
    dim3 gdim(ceil(d1 / (float)bdim.x), 1, 1);

    char *mem;
    char ****ref1;
    char ***ref2;
    char **ref3;

    cudaMalloc(&mem, d1 * d2 * d3 * d4 * elemsize);
    cudaMalloc(&ref3, d1 * d2 * d3 * sizeof(void *));
    cudaMalloc(&ref2, d1 * d2 * sizeof(void **));
    cudaMalloc(&ref1, d1 * sizeof(void ***));
    
    device_init_4d_kernel<<<gdim, bdim>>>(mem, ref1, ref2, ref3, elemsize, d1, d2, d3, d4);
    
    return ref1;
}


void
device_free_3d(void *inptr) {
    void ***ptr = (void ***)inptr;
    if (ptr == NULL)
        return;
    cudaFree(ptr[0][0]);
    cudaFree(ptr[0]);
    cudaFree(ptr);
}


void
device_free_4d(void *inptr) {
    void ****ptr = (void ****)inptr;
    if (ptr == NULL)
        return;
    cudaFree(ptr[0][0][0]);
    cudaFree(ptr[0][0]);
    cudaFree(ptr[0]);
    cudaFree(ptr);
}


/* kernels */

__global__ void device_init_3d_kernel(char *mem, char ***ref1, char **ref2, size_t elemsize, size_t d1, size_t d2, size_t d3) {
    int i, j;
    
    i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i >= d1) return;
    
    ref1[i] = ref2 + i * d2;
    
    __syncthreads();
    
    for (j = 0; j < d2; j++) {
        ref1[i][j] = mem + (i * d2 + j) * d3 * elemsize;
    }
}


__global__ void device_init_4d_kernel(char *mem, char ****ref1, char ***ref2, char **ref3, size_t elemsize, size_t d1, size_t d2, size_t d3, size_t d4) {
    int i, j, k;
    
    i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i >= d1) return;
    
    ref1[i] = ref2 + i * d2;
    
    __syncthreads();
    
    for (j = 0; j < d2; j++) {
        ref1[i][j] = ref3 + (i * d2 + j) * d3;
    }
    
    __syncthreads();
    
    for (j = 0; j < d2; j++) {
        for (k = 0; k < d3; k++) {
            ref1[i][j][k] = mem + ((i * d2 + j) * d3 + k) * d4 * elemsize;
        }
    }
}

