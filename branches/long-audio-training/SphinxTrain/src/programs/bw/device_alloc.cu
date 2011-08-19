
#include "device_alloc.h"
#include <sphinxbase/ckd_alloc.h>
#include <cutil.h>

void gauden_dev_free(gauden_dev_t *g) {

    cudaFree((void *)g->d_veclen);
    cudaFree((void *)g->d_norm);
    
    cudaFree((void *)g->d_cb);
    cudaFree((void *)g->d_l_cb);
    cudaFree((void *)g->d_mixw);
    
    cudaFree((void *)g->d_mean_idx);
    cudaFree((void *)g->d_mean_buf);

    cudaFree((void *)g->d_var_idx);
    cudaFree((void *)g->d_var_buf);

    ckd_free((void *)g);
}

gauden_dev_t *gauden_dev_copy(model_inventory_t *inv, state_t *state_seq, uint32 n_state) {

    gauden_dev_t *g;
    uint32 *buf;
    uint32 s;
    
    g = (gauden_dev_t *)ckd_calloc(1, sizeof(gauden_dev_t));
    
    g->n_feat = inv->gauden->n_feat;
    g->n_mgau = inv->gauden->n_mgau;
    g->n_density = inv->gauden->n_density;
    g->n_top = inv->gauden->n_top;
    g->n_cb_inverse = inv->n_cb_inverse;
    g->n_state = n_state;
    
    E_INFO("MICHAL: %u %u %u %u %u %u\n", g->n_feat, g->n_mgau, g->n_density, g->n_top, g->n_cb_inverse, g->n_state);
    
    cudaMalloc(&g->d_veclen, g->n_feat * sizeof(uint32));
    cudaMalloc(&g->d_norm, g->n_mgau * g->n_feat * g->n_density * sizeof(float32));
    
    cudaMalloc(&g->d_cb, g->n_state * sizeof(uint32));
    cudaMalloc(&g->d_l_cb, g->n_state * sizeof(uint32));
    cudaMalloc(&g->d_mixw, g->n_state * sizeof(uint32));
    
    g->d_mean_buflen = inv->gauden->mean[0][0][g->n_mgau * g->n_feat * g->n_density - 1] - inv->gauden->mean[0][0][0] + inv->gauden->veclen[g->n_feat - 1];
    cudaMalloc(&g->d_mean_idx, g->n_mgau * g->n_feat * g->n_density * sizeof(float *));
    cudaMalloc(&g->d_mean_buf, g->d_mean_buflen * sizeof(float));

    g->d_var_buflen = inv->gauden->var[0][0][g->n_mgau * g->n_feat * g->n_density - 1] - inv->gauden->var[0][0][0] + inv->gauden->veclen[g->n_feat - 1];
    cudaMalloc(&g->d_var_idx, g->n_mgau * g->n_feat * g->n_density * sizeof(float *));
    cudaMalloc(&g->d_var_buf, g->d_var_buflen * sizeof(float));
    
    /* veclen, norm, den, den_idx */
    cudaMemcpy(g->d_veclen, inv->gauden->veclen, g->n_feat * sizeof(uint32), cudaMemcpyHostToDevice);
    cudaMemcpy(g->d_norm, inv->gauden->norm[0][0], g->n_mgau * g->n_feat * g->n_density * sizeof(float32), cudaMemcpyHostToDevice);
    
    /* state_seq -> d_cb, d_l_cb, d_mixw */
    buf = (uint32 *)ckd_calloc(g->n_state, sizeof(uint32));
    for (s = 0; s < g->n_state; s++) buf[s] = state_seq[s].cb;
    cudaMemcpy(g->d_cb, buf, g->n_state * sizeof(uint32), cudaMemcpyHostToDevice);
    for (s = 0; s < g->n_state; s++) buf[s] = state_seq[s].l_cb;
    cudaMemcpy(g->d_l_cb, buf, g->n_state * sizeof(uint32), cudaMemcpyHostToDevice);
    for (s = 0; s < g->n_state; s++) buf[s] = state_seq[s].mixw;
    cudaMemcpy(g->d_mixw, buf, g->n_state * sizeof(uint32), cudaMemcpyHostToDevice);
    ckd_free((void *)buf);
    
    /* mean, var, feature */
    cudaMemcpy(g->d_mean_idx, inv->gauden->mean[0][0], g->n_mgau * g->n_feat * g->n_density * sizeof(float *), cudaMemcpyHostToDevice);
    cudaMemcpy(g->d_mean_buf, inv->gauden->mean[0][0][0], g->d_mean_buflen * sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(g->d_var_idx, inv->gauden->var[0][0], g->n_mgau * g->n_feat * g->n_density * sizeof(float *), cudaMemcpyHostToDevice);
    cudaMemcpy(g->d_var_buf, inv->gauden->var[0][0][0], g->d_var_buflen * sizeof(float), cudaMemcpyHostToDevice);

    return g;
}


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

