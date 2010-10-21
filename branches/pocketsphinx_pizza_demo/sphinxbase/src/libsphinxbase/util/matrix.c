/* -*- c-basic-offset: 4 -*- */
/* ====================================================================
 * Copyright (c) 1997-2006 Carnegie Mellon University.  All rights 
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
#include <string.h>
#include <stdlib.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "sphinxbase/clapack_lite.h"
#include "sphinxbase/matrix.h"
#include "sphinxbase/err.h"
#include "sphinxbase/ckd_alloc.h"

#ifndef WITH_LAPACK
float64
determinant(float32 **a, int32 n)
{
    E_FATAL("No LAPACK library available, cannot compute determinant (FIXME)\n");
    return 0.0;
}
int32
invert(float32 **ainv, float32 **a, int32 n)
{
    E_FATAL("No LAPACK library available, cannot compute matrix inverse (FIXME)\n");
    return 0;
}
int32
solve(float32 **a, float32 *b, float32 *out_x, int32   n)
{
    E_FATAL("No LAPACK library available, cannot solve linear equations (FIXME)\n");
    return 0;
}

void
matrixmultiply(float32 ** c, float32 ** a, float32 ** b, int32 n)
{
    int32 i, j, k;

    memset(c[0], 0, n*n*sizeof(float32));
    for (i = 0; i < n; ++i) {
	for (j = 0; j < n; ++j) {
	    for (k = 0; k < n; ++k) {
		c[i][k] += a[i][j] * b[j][k];
	    }
	}
    }
}
#else /* WITH_LAPACK */
/* Find determinant through LU decomposition. */
float64
determinant(float32 ** a, int32 n)
{
    float32 **tmp_a;
    float64 det;
    char uplo;
    int32 info, i;

    /* a is assumed to be symmetric, so we don't need to switch the
     * ordering of the data.  But we do need to copy it since it is
     * overwritten by LAPACK. */
    tmp_a = (float32 **)ckd_calloc_2d(n, n, sizeof(float32));
    memcpy(tmp_a[0], a[0], n*n*sizeof(float32));

    uplo = 'L';
    spotrf_(&uplo, &n, tmp_a[0], &n, &info);
    det = tmp_a[0][0];
    /* det = prod(diag(l))^2 */
    for (i = 1; i < n; ++i)
	det *= tmp_a[i][i];
    ckd_free_2d((void **)tmp_a);
    if (info > 0)
	return -1.0; /* Generic "not positive-definite" answer */
    else
	return det * det;
}

int32
solve(float32 **a, /*Input : an n*n matrix A */
      float32 *b,  /*Input : a n dimesion vector b */
      float32 *out_x,  /*Output : a n dimesion vector x */
      int32   n)
{
    char uplo;
    float32 **tmp_a;
    int32 info, nrhs;

    /* a is assumed to be symmetric, so we don't need to switch the
     * ordering of the data.  But we do need to copy it since it is
     * overwritten by LAPACK. */
    tmp_a = (float32 **)ckd_calloc_2d(n, n, sizeof(float32));
    memcpy(tmp_a[0], a[0], n*n*sizeof(float32));
    memcpy(out_x, b, n*sizeof(float32));
    uplo = 'L';
    nrhs = 1;
    sposv_(&uplo, &n, &nrhs, tmp_a[0], &n, out_x, &n, &info);
    ckd_free_2d((void **)tmp_a);

    if (info != 0)
	return -1;
    else
	return info;
}

/* Find inverse by solving AX=I. */
int32
invert(float32 ** ainv, float32 ** a, int32 n)
{
    char uplo;
    float32 **tmp_a;
    int32 info, nrhs, i;

    /* Construct an identity matrix. */
    memset(ainv[0], 0, sizeof(float32) * n * n);
    for (i = 0; i < n; i++)
        ainv[i][i] = 1.0;
    /* a is assumed to be symmetric, so we don't need to switch the
     * ordering of the data.  But we do need to copy it since it is
     * overwritten by LAPACK. */
    tmp_a = (float32 **)ckd_calloc_2d(n, n, sizeof(float32));
    memcpy(tmp_a[0], a[0], n*n*sizeof(float32));
    uplo = 'L';
    nrhs = n;
    sposv_(&uplo, &n, &nrhs, tmp_a[0], &n, ainv[0], &n, &info);
    ckd_free_2d((void **)tmp_a);

    if (info != 0)
	return -1;
    else
	return info;
}

void
matrixmultiply(float32 ** c, float32 ** a, float32 ** b, int32 n)
{
    char side, uplo;
    float32 alpha;

    side = 'L';
    uplo = 'L';
    alpha = 1.0;
    ssymm_(&side, &uplo, &n, &n, &alpha, a[0], &n, b[0], &n, &alpha, c[0], &n);
}

#endif /* WITH_LAPACK */

void
outerproduct(float32 ** a, float32 * x, float32 * y, int32 len)
{
    int32 i, j;

    for (i = 0; i < len; ++i) {
        a[i][i] = x[i] * y[i];
        for (j = i + 1; j < len; ++j) {
            a[i][j] = x[i] * y[j];
            a[j][i] = x[j] * y[i];
        }
    }
}

void
scalarmultiply(float32 ** a, float32 x, int32 n)
{
    int32 i, j;

    for (i = 0; i < n; ++i) {
	a[i][i] *= x;
        for (j = i+1; j < n; ++j) {
            a[i][j] *= x;
            a[j][i] *= x;
	}
    }
}

void
matrixadd(float32 ** a, float32 ** b, int32 n)
{
    int32 i, j;

    for (i = 0; i < n; ++i)
        for (j = 0; j < n; ++j)
            a[i][j] += b[i][j];
}


/*
 * Log record.  Maintained by RCS.
 *
 * $Log$
 * Revision 1.4  2004/07/21  18:05:40  egouvea
 * Changed the license terms to make it the same as sphinx2 and sphinx3.
 * 
 * Revision 1.3  2001/04/05 20:02:30  awb
 * *** empty log message ***
 *
 * Revision 1.2  2000/09/29 22:35:13  awb
 * *** empty log message ***
 *
 * Revision 1.1  2000/09/24 21:38:31  awb
 * *** empty log message ***
 *
 * Revision 1.1  97/07/16  11:36:22  eht
 * Initial revision
 * 
 *
 */
