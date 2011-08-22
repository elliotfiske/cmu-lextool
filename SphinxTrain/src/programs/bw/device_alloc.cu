
#include "device_alloc.h"
#include <sphinxbase/ckd_alloc.h>
#include <cutil.h>


/* performance measurement */
#ifdef STOPWATCH

#include <sys/time.h>

void startTimer(struct timeval *timer){
        gettimeofday(timer, NULL);
}

uint32 stopTimer(struct timeval *timer){
        struct timeval tmp;
        gettimeofday(&tmp, NULL);
        tmp.tv_sec -= timer->tv_sec;
        tmp.tv_usec -= timer->tv_usec;
        if (tmp.tv_usec < 0){
                tmp.tv_usec+=1000000;
                tmp.tv_sec--;
        }
        return (uint32)(tmp.tv_usec + tmp.tv_sec*1000000);
}

#endif


void gauden_dev_free(gauden_dev_t *g) {
#ifdef GAUDEN_DEV
    if (g) {
        cudaFree((void *)g->d_veclen);
        cudaFree((void *)g->d_norm);
        
        cudaFree((void *)g->d_cb);
        cudaFree((void *)g->d_l_cb);
        cudaFree((void *)g->d_active_states);
        
        cudaFree((void *)g->d_mean_idx);
        cudaFree((void *)g->d_mean_buf);

        cudaFree((void *)g->d_var_idx);
        cudaFree((void *)g->d_var_buf);
        
        cudaFree((void *)g->d_feature_idx);
        cudaFree((void *)g->d_feature_buf);
        
        cudaFree((void *)g->d_den);
        cudaFree((void *)g->d_den_idx);

        ckd_free((void *)g);
    }
#endif
}


gauden_dev_t *gauden_dev_copy(uint32 block_size, vector_t **feature, uint32 n_obs, model_inventory_t *inv, state_t *state_seq, uint32 n_state) {

    gauden_dev_t *g;
    uint32 *buf;
    uint32 s;
    
#ifndef GAUDEN_DEV
    return NULL;
#endif
    
    g = (gauden_dev_t *)ckd_calloc(1, sizeof(gauden_dev_t));
    
    g->n_feat = inv->gauden->n_feat;
    g->n_mgau = inv->gauden->n_mgau;
    g->n_density = inv->gauden->n_density;
    g->n_top = inv->gauden->n_top;
    g->n_cb_inverse = inv->n_cb_inverse;
    g->n_active_state = 0;  /* computed later */
    g->maxveclen = 0;
    int i, j, k;
    for (i = 0; i < g->n_feat; i++) {
        if (inv->gauden->veclen[i] > g->maxveclen) {
            g->maxveclen = inv->gauden->veclen[i];
        }
    }
    
    cudaMalloc(&g->d_veclen, g->n_feat * sizeof(uint32));
    cudaMalloc(&g->d_norm, g->n_mgau * g->n_feat * g->n_density * sizeof(float32));
    
    cudaMalloc(&g->d_cb, n_state * sizeof(uint32));
    cudaMalloc(&g->d_l_cb, n_state * sizeof(uint32));
    cudaMalloc(&g->d_active_states, n_state * sizeof(uint32));
    
    g->d_mean_buflen = inv->gauden->mean[0][0][g->n_mgau * g->n_feat * g->n_density - 1] - inv->gauden->mean[0][0][0] + inv->gauden->veclen[g->n_feat - 1];
    cudaMalloc(&g->d_mean_idx, g->n_mgau * g->n_feat * g->n_density * sizeof(float *));
    cudaMalloc(&g->d_mean_buf, g->d_mean_buflen * sizeof(float));

    g->d_var_buflen = inv->gauden->var[0][0][g->n_mgau * g->n_feat * g->n_density - 1] - inv->gauden->var[0][0][0] + inv->gauden->veclen[g->n_feat - 1];
    cudaMalloc(&g->d_var_idx, g->n_mgau * g->n_feat * g->n_density * sizeof(float *));
    cudaMalloc(&g->d_var_buf, g->d_var_buflen * sizeof(float));

    CUDA_SAFE_CALL(cudaMalloc(&g->d_den, (block_size + 1) * g->n_cb_inverse * g->n_feat * g->n_top * sizeof(float64)));
    CUDA_SAFE_CALL(cudaMalloc(&g->d_den_idx, (block_size + 1) * g->n_cb_inverse * g->n_feat * g->n_top * sizeof(uint32)));

    g->d_feature_buflen = feature[0][n_obs * g->n_feat - 1] - feature[0][0] + inv->gauden->veclen[g->n_feat - 1];
    CUDA_SAFE_CALL(cudaMalloc(&g->d_feature_idx, n_obs * g->n_feat * sizeof(float *)));
    CUDA_SAFE_CALL(cudaMalloc(&g->d_feature_buf, g->maxveclen * g->n_feat * n_obs * sizeof(float)));

    vector_t **feature_tr = (vector_t **)ckd_calloc_3d(g->maxveclen, g->n_feat, n_obs, sizeof(float));
    /* matrix transposition for coalescence */
    for (i = 0; i < n_obs; i++) {
        for (j = 0; j < g->n_feat; j++) {
            for (k = 0; k < inv->gauden->veclen[j]; k++) {
                feature_tr[k][j][i] = feature[i][j][k];
            }
        }
    }
    g->d_feature_n_obs = n_obs;
    CUDA_SAFE_CALL(cudaMemcpy(g->d_feature_buf, feature_tr[0][0], g->maxveclen * g->n_feat * n_obs * sizeof(float), cudaMemcpyHostToDevice));
    ckd_free_3d((void ***)feature_tr);
    
    /* veclen, norm, den, den_idx */
    cudaMemcpy(g->d_veclen, inv->gauden->veclen, g->n_feat * sizeof(uint32), cudaMemcpyHostToDevice);

    cudaMemcpy(g->d_norm, inv->gauden->norm[0][0], g->n_mgau * g->n_feat * g->n_density * sizeof(float32), cudaMemcpyHostToDevice);
    
    /* state_seq -> d_cb, d_l_cb, d_mixw */
    buf = (uint32 *)ckd_calloc(n_state, sizeof(uint32));
    for (s = 0; s < n_state; s++) buf[s] = state_seq[s].cb;
    cudaMemcpy(g->d_cb, buf, n_state * sizeof(uint32), cudaMemcpyHostToDevice);
    for (s = 0; s < n_state; s++) buf[s] = state_seq[s].l_cb;
    cudaMemcpy(g->d_l_cb, buf, n_state * sizeof(uint32), cudaMemcpyHostToDevice);
    for (s = 0; s < n_state; s++) {
        if (state_seq[s].mixw != TYING_NON_EMITTING) {
            buf[g->n_active_state++] = s;
        }
    }
    cudaMemcpy(g->d_active_states, buf, g->n_active_state * sizeof(uint32), cudaMemcpyHostToDevice);
    ckd_free((void *)buf);
    
    CUDA_SAFE_CALL(cudaMemcpy(g->d_mean_idx, inv->gauden->mean[0][0], g->n_mgau * g->n_feat * g->n_density * sizeof(float *), cudaMemcpyHostToDevice));
    CUDA_SAFE_CALL(cudaMemcpy(g->d_mean_buf, inv->gauden->mean[0][0][0], g->d_mean_buflen * sizeof(float), cudaMemcpyHostToDevice));

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


/*
 * Parallel gaussian densities precomputation.
 */
__global__ void
gauden_precompute_kernel_log_full_den(
        float64 *den,
        uint32 *den_idx,
        
        float **feature_idx,
        float *feature_buf,
        uint32 feature_n_obs,

        uint32 *veclen,
        float32 *norm,
        float **mean_idx,
        float *mean_buf,
        float **var_idx,
        float *var_buf,
        
        uint32 *d_cb,
        uint32 *d_l_cb,
        uint32 *d_active_states,
        
        uint32 n_feat,
        uint32 n_mgau,
        uint32 n_density,
        uint32 n_top,
        uint32 n_cb_inverse,
        uint32 n_active_state,
        uint32 n_obs,
        uint32 t_offset,
        uint32 maxveclen) {
    
    uint32 t = blockIdx.x * blockDim.x + threadIdx.x;
    uint32 s = blockIdx.y * blockDim.y + threadIdx.y;
    uint32 x;
    
    if (t == 0) return;
    if (t >= n_obs) return;
    if (s >= n_active_state) return;

    
    s = d_active_states[s];
    uint32 mgau = d_cb[s];
    uint32 l_cb = d_l_cb[s];
    
    float *feature_base_idx = feature_idx[0];
    float *mean_base_idx = mean_idx[0];
    float *var_base_idx = var_idx[0];
    
    __syncthreads();

    uint32 j;

    for (j = 0; j < n_feat; j++) {
        uint32 i;
        uint32 veclen_j = veclen[j];
        
        for (i = 0; i < n_density; i++) {
            float64 d = 0.0, diff;
            uint32 l;
            uint32 cur_mean_var = (mgau * n_feat + j) * n_density + i;

            for (l = 0; l < veclen_j; l++) {
                diff = feature_buf[((l * n_feat) + j) * feature_n_obs + (t + t_offset)]
                    - mean_buf[(mean_idx[cur_mean_var] - mean_base_idx) + l];

                d += var_buf[(var_idx[cur_mean_var] - var_base_idx) + l]
                    * diff * diff;
            }
            
            uint32 cur_den = ((t * n_cb_inverse + l_cb) * n_feat + j) * n_top + i;
            den[cur_den] =
                norm[(mgau * n_feat + j) * n_density + i] - d;
            
            den_idx[cur_den] = i;
        }
    }
}

