/* -*- c-basic-offset: 4; indent-tabs-mode: nil -*- */
/* ====================================================================
 * Copyright (c) 1997-2000 Carnegie Mellon University.  All rights 
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
 * File: matrix.h
 * 
 * Description: Matrix and linear algebra functions
 * 
 * Author: 
 * 
 *********************************************************************/

#ifndef MATRIX_H
#define MATRIX_H

/** \file matrix.h
    \brief Matrix and linear algebra functions.
 */
#ifdef __cplusplus
extern "C" {
#endif
#if 0
/* Fool Emacs. */
}
#endif

#include "prim_type.h"

/**
 * Calculate the determinant of a matrix.
 * @arg a The input matrix, must be square.
 * @arg len The dimension of the input matrix.
 * @return The determinant of the input matrix.
 *
 * \note These can be vanishingly small hence the float64 return type.
 **/
float64 determinant(float32 **a, int32 len);

/**
 * Invert (if possible) a matrix.
 * @arg out_ainv The inverse of a will be stored here.
 * @arg a The input matrix, must be square.
 * @arg len The dimension of the input matrix.
 * @return 0 for success or -1 for a singular matrix.
 **/
int32 invert(float32 **out_ainv, float32 **a, int32 len);

/**
 * Solve (if possible) a system of linear equations AX=B for X.
 * @arg a The A matrix on the left-hand side of the equation.
 * @arg b The B vector on the right-hand side of the equation.
 * @arg out_x The X vector will be stored here.
 * @arg n The dimension of the A matrix (n by n) and the B and X vectors.
 * @return 0 for success or -1 for a singular matrix.
 **/
int32 solve(float32 **a, float32 *b,
            float32 *out_x, int32 n);

void outerproduct(float32 **out_a, float32 *x, float32 *y, int32 len);

/**
 * Decompose a matrix into eigenvectors and eigenvalues.
 * @arg a The matrix to decompose.
 * @arg out_ur The real part of the eigenvalues will be stored here.
 * @arg out_ur The imaginary part of the eigenvalues will be stored here.
 * @arg out_vr The real part of the eigenvectors will be stored here .as row vectors.
 * @arg out_vi The imaginary part of the eigenvalues will be stored here as row vectors.
 * @arg len The dimension of a (and the outputs).
 * @return 0 for success, -1 for an error, or >0 for failure to
 * converge (the return value is the number of valid eigenvectors).
 **/
int32 eigenvectors(float32 **a,
                   float32 *out_ur, float32 *out_ui,
                   float32 **out_vr, float32 **out_vi,
                   int32 len);

/**
 * Multiply C=AB.
 * @arg out_c The output matrix C.
 * @arg a The input matrix A.
 * @arg b The input matrix B.
 **/
void matrixmultiply(float32 **out_c, /* = */
                    float32 **a, /* * */ float32 **b,
                    int32 m, /**< #rows of a. */
                    int32 n, /**< #cols of b. */
                    int32 k  /**< #cols of a, #rows of b. */
    );

/**
 * Multiply a matrix by a constant in-place.
 * @arg inout_a The matrix to multiply.
 * @arg x The constant to multiply it by.
 * @arg m #rows of a.
 * @arg n #columns of a.
 **/
void scalarmultiply(float32 **inout_a, float32 x, int32 m, int32 n);

/**
 * Add A += B.
 * @arg inout_a The A matrix to add.
 * @arg b The B matrix to add to A.
 * @arg m #rows of a and b
 * @arg n #columns of a and b.
 **/
void matrixadd(float32 **inout_a, float32 **b, int32 m, int32 n);

#if 0
{ /* Fool indent. */
#endif
#ifdef __cplusplus
}
#endif

#endif /* MATRIX_H */ 


/*
 * Log record.  Maintained by RCS.
 *
 * $Log$
 * Revision 1.4  2004/07/21  17:46:09  egouvea
 * Changed the license terms to make it the same as sphinx2 and sphinx3.
 * 
 * Revision 1.3  2001/04/05 20:02:30  awb
 * *** empty log message ***
 *
 * Revision 1.2  2000/09/29 22:35:12  awb
 * *** empty log message ***
 *
 * Revision 1.1  2000/09/24 21:38:30  awb
 * *** empty log message ***
 *
 * Revision 1.1  97/07/16  11:39:10  eht
 * Initial revision
 * 
 *
 */
