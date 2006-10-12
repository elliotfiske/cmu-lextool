/* -*- c-basic-offset: 4; indent-tabs-mode: nil -*- */
/* ====================================================================
 * Copyright (c) 1996-2004 Carnegie Mellon University.  All rights 
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

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "prim_type.h"
#include "fixpoint.h"
#include "fe.h"
#include "fe_internal.h"
#include "fe_warp.h"
#include "genrand.h"
#include "err.h"

int32
fe_build_melfilters(melfb_t * MEL_FB)
{
    int32 i, whichfilt, start_pt;
    float32 leftfr, centerfr, rightfr, fwidth, height, *filt_edge;
    float32 melmax, melmin, dmelbw, freq, dfreq, leftslope =
        0, rightslope = 0;

    /*estimate filter coefficients */
    MEL_FB->filter_coeffs =
        (mfcc_t **) fe_create_2d(MEL_FB->num_filters, MEL_FB->fft_size,
                                 sizeof(mfcc_t));
    MEL_FB->left_apex =
        (mfcc_t *) calloc(MEL_FB->num_filters, sizeof(mfcc_t));
    MEL_FB->width = (int32 *) calloc(MEL_FB->num_filters, sizeof(int32));

    if (MEL_FB->doublewide)
        filt_edge =
            (float32 *) calloc(MEL_FB->num_filters + 4, sizeof(float32));
    else
        filt_edge =
            (float32 *) calloc(MEL_FB->num_filters + 2, sizeof(float32));

    if (MEL_FB->filter_coeffs == NULL || MEL_FB->left_apex == NULL
        || MEL_FB->width == NULL || filt_edge == NULL) {
        E_WARN("memory alloc failed in fe_build_mel_filters()\n");
        return FE_MEM_ALLOC_ERROR;
    }

    dfreq = MEL_FB->sampling_rate / (float32) MEL_FB->fft_size;

    melmax = fe_mel(MEL_FB->upper_filt_freq);
    melmin = fe_mel(MEL_FB->lower_filt_freq);
    dmelbw = (melmax - melmin) / (MEL_FB->num_filters + 1);

    if (MEL_FB->doublewide) {
        melmin = melmin - dmelbw;
        melmax = melmax + dmelbw;
        if ((fe_melinv(melmin) < 0) ||
            (fe_melinv(melmax) > MEL_FB->sampling_rate / 2)) {
            E_WARN
                ("Out of Range: low  filter edge = %f (%f)\n",
                 fe_melinv(melmin), 0.0);
            E_WARN
                ("              high filter edge = %f (%f)\n",
                 fe_melinv(melmax), MEL_FB->sampling_rate / 2);
            return FE_INVALID_PARAM_ERROR;
        }
    }

    if (MEL_FB->doublewide) {
        for (i = 0; i <= MEL_FB->num_filters + 3; ++i) {
            filt_edge[i] = fe_melinv(i * dmelbw + melmin);
        }
    }
    else {
        for (i = 0; i <= MEL_FB->num_filters + 1; ++i) {
            filt_edge[i] = fe_melinv(i * dmelbw + melmin);
        }
    }

    for (whichfilt = 0; whichfilt < MEL_FB->num_filters; ++whichfilt) {
        /*line triangle edges up with nearest dft points... */
        if (MEL_FB->doublewide) {
            leftfr = (float32) ((int32)
                                ((filt_edge[whichfilt] /
                                  dfreq) + 0.5)) * dfreq;
            centerfr = (float32) ((int32)
                                  ((filt_edge[whichfilt + 2] /
                                    dfreq) + 0.5)) * dfreq;
            rightfr = (float32) ((int32)
                                 ((filt_edge[whichfilt + 4] /
                                   dfreq) + 0.5)) * dfreq;
        }
        else {
            leftfr = (float32) ((int32)
                                ((filt_edge[whichfilt] /
                                  dfreq) + 0.5)) * dfreq;
            centerfr = (float32) ((int32)
                                  ((filt_edge[whichfilt + 1] /
                                    dfreq) + 0.5)) * dfreq;
            rightfr = (float32) ((int32)
                                 ((filt_edge[whichfilt + 2] /
                                   dfreq) + 0.5)) * dfreq;
        }
#ifdef FIXED_POINT
        MEL_FB->left_apex[whichfilt] = (int32) (leftfr + 0.5);
#else                           /* FIXED_POINT */
        MEL_FB->left_apex[whichfilt] = leftfr;
#endif                          /* FIXED_POINT */
        fwidth = rightfr - leftfr;

        /* 2/fwidth for triangles of area 1 */
        height = 2 / (float32) fwidth;
        if (centerfr != leftfr) {
            leftslope = height / (centerfr - leftfr);
        }
        if (centerfr != rightfr) {
            rightslope = height / (centerfr - rightfr);
        }

        /* Round to the nearest integer instead of truncating and adding
           one, which breaks if the divide is already an integer */
        start_pt = (int32) (leftfr / dfreq + 0.5);
        freq = (float32) start_pt *dfreq;
        i = 0;

        while (freq < centerfr) {
#ifdef FIXED_POINT
            MEL_FB->filter_coeffs[whichfilt][i] =
                FE_LOG((freq - leftfr) * leftslope);
#else                           /* FIXED_POINT */
            MEL_FB->filter_coeffs[whichfilt][i] =
                FLOAT2MFCC((freq - leftfr) * leftslope);
#endif                          /* FIXED_POINT */
            freq += dfreq;
            i++;
        }
        /* If the two floats are equal, the leftslope computation above
           results in Inf, so we handle the case here. */
        if (freq == centerfr) {
#ifdef FIXED_POINT
            MEL_FB->filter_coeffs[whichfilt][i] = FE_LOG(height);
#else                           /* FIXED_POINT */
            MEL_FB->filter_coeffs[whichfilt][i] = FLOAT2MFCC(height);
#endif                          /* FIXED_POINT */
            freq += dfreq;
            i++;
        }
        while (freq < rightfr) {
#ifdef FIXED_POINT
            MEL_FB->filter_coeffs[whichfilt][i] =
                FE_LOG((freq - rightfr) * rightslope);
#else                           /* FIXED_POINT */
            MEL_FB->filter_coeffs[whichfilt][i] =
                FLOAT2MFCC((freq - rightfr) * rightslope);
#endif                          /* FIXED_POINT */
            freq += dfreq;
            i++;
        }
        MEL_FB->width[whichfilt] = i;
    }

    free(filt_edge);
    return (0);
}

int32
fe_compute_melcosine(melfb_t * MEL_FB)
{

    float64 freqstep;
    int32 i, j;

    if ((MEL_FB->mel_cosine =
         (mfcc_t **) fe_create_2d(MEL_FB->num_cepstra,
                                  MEL_FB->num_filters,
                                  sizeof(mfcc_t))) == NULL) {
        E_WARN("memory alloc failed in fe_compute_melcosine()\n");
        return FE_MEM_ALLOC_ERROR;
    }

    freqstep = M_PI / MEL_FB->num_filters;
    /* NOTE: The first row vector is actually unnecessary but we leave
     * it in to avoid confusion. */
    for (i = 0; i < MEL_FB->num_cepstra; i++) {
        for (j = 0; j < MEL_FB->num_filters; j++) {
            float64 cosine;

            cosine = cos(freqstep * i * (j + 0.5));
            MEL_FB->mel_cosine[i][j] = FLOAT2MFCC(cosine);
        }
    }

    /* Also precompute normalization constants for unitary DCT. */
    MEL_FB->sqrt_inv_n = FLOAT2MFCC(sqrt(1.0 / MEL_FB->num_filters));
    MEL_FB->sqrt_inv_2n = FLOAT2MFCC(sqrt(2.0 / MEL_FB->num_filters));

    return (0);
}


float32
fe_mel(float32 x)
{
    float32 warped = fe_warp_unwarped_to_warped(x);

    return (float32) (2595.0 * log10(1.0 + warped / 700.0));
}

float32
fe_melinv(float32 x)
{
    float32 warped = (float32) (700.0 * (pow(10.0, x / 2595.0) - 1.0));
    return fe_warp_warped_to_unwarped(warped);
}

/* adds 1/2-bit noise */
int32
fe_dither(int16 * buffer, int32 nsamps)
{
    int32 i;
    for (i = 0; i < nsamps; i++)
        buffer[i] += (short) ((!(s3_rand_int31() % 4)) ? 1 : 0);

    return 0;
}

void
fe_pre_emphasis(int16 const *in, frame_t * out, int32 len,
                float32 factor, int16 prior)
{
    int32 i;
#if defined(FIXED16)
    int32 fxd_factor = (int32) (factor * (float32) (1 << DEFAULT_RADIX));
    out[0] = in[0] - ((int32) (prior * fxd_factor) >> DEFAULT_RADIX);

    for (i = 1; i < len; ++i) {
        out[i] =
            in[i] - ((int32) (in[i - 1] * fxd_factor) >> DEFAULT_RADIX);
    }
#elif defined(FIXED_POINT)
    /* Use extra precision for alpha under the assumption it is between 0 and 1. */
    fixed32 fxd_factor = FLOAT2FIX_ANY(factor, 30);
    out[0] = (int32) (in[0] << DEFAULT_RADIX)
        - FIXMUL_ANY(prior, fxd_factor, 30-DEFAULT_RADIX);

    for (i = 1; i < len; ++i) {
        out[i] = ((int32)in[i] << DEFAULT_RADIX)
            - FIXMUL_ANY(in[i-1], fxd_factor, 30-DEFAULT_RADIX);
    }
#else
    out[0] = (frame_t) in[0] - factor * (frame_t) prior;
    for (i = 1; i < len; i++) {
        out[i] = (frame_t) in[i] - factor * (frame_t) in[i - 1];
    }
#endif
}

void
fe_short_to_frame(int16 const *in, frame_t * out, int32 len)
{
    int32 i;

#ifdef  FIXED_POINT
    for (i = 0; i < len; i++)
        out[i] = (int32) in[i] << DEFAULT_RADIX;
#else                           /* FIXED_POINT */
    for (i = 0; i < len; i++)
        out[i] = (frame_t) in[i];
#endif                          /* FIXED_POINT */
}

void
fe_create_hamming(window_t * in, int32 in_len)
{
    int i;

    if (in_len > 1)
        for (i = 0; i < in_len; i++) {
            float64 hamm;

            hamm  = (0.54 - 0.46 * cos(2 * M_PI * i /
                                       ((float64) in_len - 1.0)));

            /* Use extra precision for the window, after all, it is
             * always between 0 and 1! */
#ifdef FIXED_POINT
            in[i] = FLOAT2FIX_ANY(hamm, 30);
#else
            in[i] = hamm;
#endif
        }

    return;
}


void
fe_hamming_window(frame_t * in, window_t * window, int32 in_len)
{
    int i;

    if (in_len > 1)
        for (i = 0; i < in_len; i++) {
            /* Use extra precision for the window, after all, it is
             * always between 0 and 1! */
#ifdef FIXED_POINT
            in[i] = FIXMUL_ANY(in[i], window[i], 30);
#else
            in[i] = FLOAT2MFCC(MFCC2FLOAT(in[i]) * window[i]);
#endif
        }

    return;
}


int32
fe_frame_to_fea(fe_t * FE, frame_t * in, mfcc_t * fea)
{
    powspec_t *spec, *mfspec;

    if (FE->FB_TYPE == MEL_SCALE) {
        spec = (powspec_t *) calloc(FE->FFT_SIZE, sizeof(powspec_t));
        mfspec =
            (powspec_t *) calloc(FE->MEL_FB->num_filters,
                                 sizeof(powspec_t));

        if (spec == NULL || mfspec == NULL) {
            E_WARN("memory alloc failed in fe_frame_to_fea()\n");
            return FE_MEM_ALLOC_ERROR;
        }

        fe_spec_magnitude(in, FE->FRAME_SIZE, spec, FE->FFT_SIZE);
        fe_mel_spec(FE, spec, mfspec);
        fe_mel_cep(FE, mfspec, fea);

        free(spec);
        free(mfspec);
    }
    else {
        E_WARN("MEL SCALE IS CURRENTLY THE ONLY IMPLEMENTATION!\n");
        return FE_INVALID_PARAM_ERROR;
    }
    return 0;
}

void
fe_spec_magnitude(frame_t const *data, int32 data_len,
                  powspec_t * spec, int32 fftsize)
{
    int32 j, wrap;
    frame_t *fft;

    fft = calloc(fftsize, sizeof(frame_t));
    if (fft == NULL) {
        E_FATAL
            ("memory alloc failed in fe_spec_magnitude()\n...exiting\n");
    }
    wrap = (data_len < fftsize) ? data_len : fftsize;
    memcpy(fft, data, wrap * sizeof(frame_t));
    if (data_len > fftsize) {    /*aliasing */
        E_WARN
            ("Aliasing. Consider using fft size (%d) > buffer size (%d)\n",
             fftsize, data_len);
        for (wrap = 0, j = fftsize; j < data_len; wrap++, j++)
            fft[wrap] += data[j];
    }
    fe_fft_real(fft, fftsize);

    for (j = 0; j <= fftsize / 2; j++) {
#ifdef FIXED_POINT
        uint32 r = abs(fft[j]);
        uint32 i = abs(fft[fftsize - j]);
#ifdef FIXED16
        int32 rr = INTLOG(r) * 2;
        int32 ii = INTLOG(i) * 2;
#else
        int32 rr = FIXLOG(r) * 2;
        int32 ii = FIXLOG(i) * 2;
#endif

        spec[j] = FE_LOG_ADD(rr, ii);
#else                           /* !FIXED_POINT */
        spec[j] = fft[j] * fft[j] + fft[fftsize - j] * fft[fftsize - j];
#endif                          /* !FIXED_POINT */
    }

    free(fft);
    return;
}

void
fe_mel_spec(fe_t * FE, powspec_t const *spec, powspec_t * mfspec)
{
    int32 whichfilt, start, i;
#ifdef FIXED_POINT
    int32 dfreq = FE->SAMPLING_RATE / FE->FFT_SIZE;
#else                           /* FIXED_POINT */
    float32 dfreq = FE->SAMPLING_RATE / (float32) FE->FFT_SIZE;
#endif                          /* FIXED_POINT */

    for (whichfilt = 0; whichfilt < FE->MEL_FB->num_filters; whichfilt++) {
#ifdef FIXED_POINT
        /* There is an implicit floor here instead of rounding, hope
         * it doesn't break stuff... */
        start = FE->MEL_FB->left_apex[whichfilt] / dfreq;
#else                           /* FIXED_POINT */
        /* Round to the nearest integer instead of truncating and
           adding one, which breaks if the divide is already an
           integer */
        start = (int32) (FE->MEL_FB->left_apex[whichfilt] / dfreq + 0.5);
#endif                          /* FIXED_POINT */
#ifdef FIXED_POINT
        mfspec[whichfilt] = spec[start]
            + FE->MEL_FB->filter_coeffs[whichfilt][0];
        for (i = 1; i < FE->MEL_FB->width[whichfilt]; i++)
            mfspec[whichfilt] = FE_LOG_ADD(mfspec[whichfilt],
                                           spec[start + i] +
                                           FE->MEL_FB->
                                           filter_coeffs[whichfilt][i]);
#else                           /* !FIXED_POINT */
        mfspec[whichfilt] = 0;
        for (i = 0; i < FE->MEL_FB->width[whichfilt]; i++)
            mfspec[whichfilt] +=
                spec[start + i] * FE->MEL_FB->filter_coeffs[whichfilt][i];
#endif                          /* !FIXED_POINT */
    }
}

void
fe_mel_cep(fe_t * FE, powspec_t * mfspec, mfcc_t * mfcep)
{
    int32 i;

    for (i = 0; i < FE->MEL_FB->num_filters; ++i) {
#if defined(FIXED_POINT)
        /* It's already in log domain!  Don't check if it's
         * greater than zero... */
        mfspec[i] = LOG_TO_FIXLN(mfspec[i]);
#else                           /* !FIXED_POINT */
        if (mfspec[i] > 0)
            mfspec[i] = log(mfspec[i]);
        else                    /* This number should be smaller than anything
                                 * else, but not too small, so as to avoid
                                 * infinities in the inverse transform (this is
                                 * the frequency-domain equivalent of
                                 * dithering) */
            mfspec[i] = -10.0;
#endif                          /* !FIXED_POINT */
    }

    /* If we are doing LOG_SPEC, then do nothing. */
    if (FE->LOG_SPEC == RAW_LOG_SPEC) {
        for (i = 0; i < FE->FEATURE_DIMENSION; i++) {
            mfcep[i] = (mfcc_t) mfspec[i];
        }
    }
    /* For smoothed spectrum, do DCT-II followed by (its inverse) DCT-III */
    else if (FE->LOG_SPEC == SMOOTH_LOG_SPEC) {
        /* FIXME: This is probably broken for fixed-point. */
        fe_dct2(FE, mfspec, mfcep);
        fe_dct3(FE, mfcep, mfspec);
        for (i = 0; i < FE->FEATURE_DIMENSION; i++) {
            mfcep[i] = (mfcc_t) mfspec[i];
        }
    }
    else if (FE->transform == DCT_II)
        fe_dct2(FE, mfspec, mfcep);
    else
        fe_spec2cep(FE, mfspec, mfcep);
    return;
}

void
fe_spec2cep(fe_t * FE, const powspec_t * mflogspec, mfcc_t * mfcep)
{
    int32 i, j, beta;

    /* Compute C0 separately (its basis vector is 1) to avoid
     * costly multiplications. */
    mfcep[0] = mflogspec[0] / 2; /* beta = 0.5 */
    for (j = 1; j < FE->MEL_FB->num_filters; j++)
	mfcep[0] += mflogspec[j]; /* beta = 1.0 */
    mfcep[0] /= (frame_t) FE->MEL_FB->num_filters;

    for (i = 1; i < FE->NUM_CEPSTRA; ++i) {
        mfcep[i] = 0;
        for (j = 0; j < FE->MEL_FB->num_filters; j++) {
            if (j == 0)
                beta = 1;       /* 0.5 */
            else
                beta = 2;       /* 1.0 */
            mfcep[i] += MFCCMUL(mflogspec[j],
                                FE->MEL_FB->mel_cosine[i][j]) * beta;
        }
	/* Note that this actually normalizes by num_filters, like the
	 * original Sphinx front-end, due to the doubled 'beta' factor
	 * above.  */
        mfcep[i] /= (frame_t) FE->MEL_FB->num_filters * 2;
    }
}

void
fe_dct2(fe_t * FE, const powspec_t * mflogspec, mfcc_t * mfcep)
{
    int32 i, j;

    /* Compute C0 separately (its basis vector is 1) to avoid
     * costly multiplications. */
    mfcep[0] = mflogspec[0];
    for (j = 1; j < FE->MEL_FB->num_filters; j++)
	mfcep[0] += mflogspec[j];
    mfcep[0] = MFCCMUL(mfcep[0], FE->MEL_FB->sqrt_inv_n);

    for (i = 1; i < FE->NUM_CEPSTRA; ++i) {
        mfcep[i] = 0;
        for (j = 0; j < FE->MEL_FB->num_filters; j++) {
	    mfcep[i] += MFCCMUL(mflogspec[j],
				FE->MEL_FB->mel_cosine[i][j]);
        }
        mfcep[i] = MFCCMUL(mfcep[i], FE->MEL_FB->sqrt_inv_2n);
    }
}

void
fe_dct3(fe_t * FE, const mfcc_t * mfcep, powspec_t * mflogspec)
{
    int32 i, j;

    for (i = 0; i < FE->MEL_FB->num_filters; ++i) {
        mflogspec[i] = MFCCMUL(mfcep[0], SQRT_HALF);
        for (j = 1; j < FE->NUM_CEPSTRA; j++) {
            mflogspec[i] += MFCCMUL(mfcep[j],
                                    FE->MEL_FB->mel_cosine[j][i]);
        }
        mflogspec[i] = MFCCMUL(mflogspec[i], FE->MEL_FB->sqrt_inv_2n);
    }
}

int32
fe_fft(complex const *in, complex * out, int32 N, int32 invert)
{
    int32 s, k,                 /* as above                             */
     lgN;                       /* log2(N)                              */
    complex *f1, *f2,           /* pointers into from array             */
    *t1, *t2,                   /* pointers into to array               */
    *ww;                        /* pointer into w array                 */
    complex *from, *to,         /* as above                             */
     wwf2,                      /* temporary for ww*f2                  */
    *exch,                      /* temporary for exchanging from and to */
    *wEnd;                      /* to keep ww from going off end        */

    /* Cache the weight array and scratchpad for all FFTs of the same
     * order (we could actually do better than this, but we'd need a
     * different algorithm). */
    static complex *w;
    static complex *buffer;     /* from and to flipflop btw out and buffer */
    static int32 lastN;

    /* check N, compute lgN                                             */
    for (k = N, lgN = 0; k > 1; k /= 2, lgN++) {
        if (k % 2 != 0 || N < 0) {
            E_WARN("fft: N must be a power of 2 (is %d)\n", N);
            return (-1);
        }
    }

    /* check invert                                                     */
    if (!(invert == 1 || invert == -1)) {
        E_WARN("fft: invert must be either +1 or -1 (is %d)\n", invert);
        return (-1);
    }

    /* Initialize weights and scratchpad buffer.  This will cause a
     * slow startup and "leak" a small, constant amount of memory,
     * don't worry about it. */
    if (lastN != N) {
        if (buffer)
            free(buffer);
        if (w)
            free(w);
        buffer = (complex *) calloc(N, sizeof(complex));
        w = (complex *) calloc(N / 2, sizeof(complex));
        /* w = exp(-2*PI*i/N), w[k] = w^k                                       */
        for (k = 0; k < N / 2; k++) {
            float64 x = -2 * M_PI * invert * k / N;
            w[k].r = FLOAT2MFCC(cos(x));
            w[k].i = FLOAT2MFCC(sin(x));
        }
        lastN = N;
    }

    wEnd = &w[N / 2];

    /* Initialize scratchpad pointers. */
    if (lgN % 2 == 0) {
        from = out;
        to = buffer;
    }
    else {
        to = out;
        from = buffer;
    }
    memcpy(from, in, N * sizeof(*in));

    /* go for it!                                                               */
    for (k = N / 2; k > 0; k /= 2) {
        for (s = 0; s < k; s++) {
            /* initialize pointers                                              */
            f1 = &from[s];
            f2 = &from[s + k];
            t1 = &to[s];
            t2 = &to[s + N / 2];
            ww = &w[0];
            /* compute <s,k>                                                    */
            while (ww < wEnd) {
                /* wwf2 = ww * f2                                                       */
                wwf2.r = MFCCMUL(f2->r, ww->r) - MFCCMUL(f2->i, ww->i);
                wwf2.i = MFCCMUL(f2->r, ww->i) + MFCCMUL(f2->i, ww->r);
                /* t1 = f1 + wwf2                                                       */
                t1->r = f1->r + wwf2.r;
                t1->i = f1->i + wwf2.i;
                /* t2 = f1 - wwf2                                                       */
                t2->r = f1->r - wwf2.r;
                t2->i = f1->i - wwf2.i;
                /* increment                                                    */
                f1 += 2 * k;
                f2 += 2 * k;
                t1 += k;
                t2 += k;
                ww += k;
            }
        }
        exch = from;
        from = to;
        to = exch;
    }

    /* Normalize for inverse FFT (not used but hey...) */
    if (invert == -1) {
        for (s = 0; s < N; s++) {
            from[s].r = in[s].r / N;
            from[s].i = in[s].i / N;

        }
    }

    return (0);
}

/* Translated from the FORTRAN (obviously) from "Real-Valued Fast
 * Fourier Transform Algorithms" by Henrik V. Sorensen et al., IEEE
 * Transactions on Acoustics, Speech, and Signal Processing, vol. 35,
 * no.6.  Optimized to use a static array of sine/cosines.
 */
int32
fe_fft_real(frame_t * x, int n)
{
    int32 i, j, k, n1, n2, n4, i1, i2, i3, i4;
    frame_t t1, t2, xt, cc, ss;
    static frame_t *ccc = NULL, *sss = NULL;
    static int32 lastn = 0;
    int m;

    /* check fft size, compute fft order (log_2(n)) */
    for (k = n, m = 0; k > 1; k >>= 1, m++) {
        if (((k % 2) != 0) || (n <= 0)) {
            E_FATAL("fft: number of points must be a power of 2 (is %d)\n", n);
        }
    }
    if (ccc == NULL || n != lastn) {
        if (ccc != NULL) {
            free(ccc);
        }
        if (sss != NULL) {
            free(sss);
        }
        ccc = calloc(n / 4, sizeof(*ccc));
        sss = calloc(n / 4, sizeof(*sss));
        for (i = 0; i < n / 4; ++i) {
            float64 a;

            a = 2 * M_PI * i / n;

            /* Use extra precision for twiddle factors (after all they
             * are always between -1 and 1!) */
#if defined(FIXED16)
            ccc[i] = cos(a) * 32768;
            sss[i] = sin(a) * 32768;
#elif defined(FIXED_POINT)
            ccc[i] = FLOAT2FIX_ANY(cos(a), 30);
            sss[i] = FLOAT2FIX_ANY(sin(a), 30);
#else /* Not fixed-point */
            ccc[i] = cos(a);
            sss[i] = sin(a);
#endif
        }
        lastn = n;
    }

    j = 0;
    n1 = n - 1;
    for (i = 0; i < n1; ++i) {
        if (i < j) {
            xt = x[j];
            x[j] = x[i];
            x[i] = xt;
        }
        k = n / 2;
        while (k <= j) {
            j -= k;
            k /= 2;
        }
        j += k;
    }
    for (i = 0; i < n; i += 2) {
        xt = x[i];
        x[i] = xt + x[i + 1];
        x[i + 1] = xt - x[i + 1];
    }
    n2 = 0;
    for (k = 1; k < m; ++k) {
        n4 = n2;
        n2 = n4 + 1;
        n1 = n2 + 1;
        for (i = 0; i < n; i += (1 << n1)) {
            xt = x[i];
            x[i] = xt + x[i + (1 << n2)];
            x[i + (1 << n2)] = xt - x[i + (1 << n2)];
            x[i + (1 << n4) + (1 << n2)] = -x[i + (1 << n4) + (1 << n2)];
            for (j = 1; j < (1 << n4); ++j) {
                i1 = i + j;
                i2 = i - j + (1 << n2);
                i3 = i + j + (1 << n2);
                i4 = i - j + (1 << n1);

                /* a = 2*M_PI * j / n1; */
                /* cc = cos(a); ss = sin(a); */
                cc = ccc[j << (m - n1)];
                ss = sss[j << (m - n1)];
#if defined(FIXED16)
                t1 = (((int32) x[i3] * cc) >> 15)
                    + (((int32) x[i4] * ss) >> 15);
                t2 = ((int32) x[i3] * ss >> 15)
                    - (((int32) x[i4] * cc) >> 15);
#elif defined(FIXED_POINT)
                t1 = FIXMUL_ANY(x[i3], cc, 30)
                    + FIXMUL_ANY(x[i4], ss, 30);
                t2 = FIXMUL_ANY(x[i3], ss, 30)
                    - FIXMUL_ANY(x[i4], cc, 30);
#else
                t1 = x[i3] * cc + x[i4] * ss;
                t2 = x[i3] * ss - x[i4] * cc;
#endif
                x[i4] = x[i2] - t2;
                x[i3] = -x[i2] - t2;
                x[i2] = x[i1] - t1;
                x[i1] = x[i1] + t1;
            }
        }
    }
    return 0;
}


void *
fe_create_2d(int32 d1, int32 d2, int32 elem_size)
{
    char *store;
    void **out;
    int32 i, j;
    store = calloc(d1 * d2, elem_size);

    if (store == NULL) {
        E_WARN("fe_create_2d failed\n");
        return (NULL);
    }

    out = calloc(d1, sizeof(void *));

    if (out == NULL) {
        E_WARN("fe_create_2d failed\n");
        free(store);
        return (NULL);
    }

    for (i = 0, j = 0; i < d1; i++, j += d2) {
        out[i] = store + (j * elem_size);
    }

    return out;
}

void
fe_free_2d(void *arr)
{
    if (arr != NULL) {
        /* FIXME: memory leak */
        free(((void **) arr)[0]);
        free(arr);
    }

}

void
fe_parse_general_params(param_t const *P, fe_t * FE)
{

    if (P->SAMPLING_RATE != 0)
        FE->SAMPLING_RATE = P->SAMPLING_RATE;
    else
        FE->SAMPLING_RATE = DEFAULT_SAMPLING_RATE;

    if (P->FRAME_RATE != 0)
        FE->FRAME_RATE = P->FRAME_RATE;
    else
        FE->FRAME_RATE = DEFAULT_FRAME_RATE;

    if (P->WINDOW_LENGTH != 0)
        FE->WINDOW_LENGTH = P->WINDOW_LENGTH;
    else
        FE->WINDOW_LENGTH = (float32) DEFAULT_WINDOW_LENGTH;

    if (P->FB_TYPE != 0)
        FE->FB_TYPE = P->FB_TYPE;
    else
        FE->FB_TYPE = DEFAULT_FB_TYPE;

    FE->dither = P->dither;
    FE->seed = P->seed;
    FE->swap = P->swap;

    if (P->PRE_EMPHASIS_ALPHA != 0)
        FE->PRE_EMPHASIS_ALPHA = P->PRE_EMPHASIS_ALPHA;
    else
        FE->PRE_EMPHASIS_ALPHA = (float32) DEFAULT_PRE_EMPHASIS_ALPHA;

    if (P->NUM_CEPSTRA != 0)
        FE->NUM_CEPSTRA = P->NUM_CEPSTRA;
    else
        FE->NUM_CEPSTRA = DEFAULT_NUM_CEPSTRA;

    if (P->FFT_SIZE != 0)
        FE->FFT_SIZE = P->FFT_SIZE;
    else
        FE->FFT_SIZE = DEFAULT_FFT_SIZE;

    FE->LOG_SPEC = P->logspec;
    FE->transform = P->transform;
    if (!FE->LOG_SPEC)
        FE->FEATURE_DIMENSION = FE->NUM_CEPSTRA;
    else {
        if (P->NUM_FILTERS != 0)
            FE->FEATURE_DIMENSION = P->NUM_FILTERS;
        else {
            if (FE->SAMPLING_RATE == BB_SAMPLING_RATE)
                FE->FEATURE_DIMENSION = DEFAULT_BB_NUM_FILTERS;
            else if (FE->SAMPLING_RATE == NB_SAMPLING_RATE)
                FE->FEATURE_DIMENSION = DEFAULT_NB_NUM_FILTERS;
            else {
                E_WARN("Please define the number of MEL filters needed\n");
                exit(1);
            }
        }
    }
}

void
fe_parse_melfb_params(param_t const *P, melfb_t * MEL)
{
    if (P->SAMPLING_RATE != 0)
        MEL->sampling_rate = P->SAMPLING_RATE;
    else
        MEL->sampling_rate = DEFAULT_SAMPLING_RATE;

    if (P->FFT_SIZE != 0)
        MEL->fft_size = P->FFT_SIZE;
    else {
        if (MEL->sampling_rate == BB_SAMPLING_RATE)
            MEL->fft_size = DEFAULT_BB_FFT_SIZE;
        if (MEL->sampling_rate == NB_SAMPLING_RATE)
            MEL->fft_size = DEFAULT_NB_FFT_SIZE;
        else
            MEL->fft_size = DEFAULT_FFT_SIZE;
    }

    if (P->NUM_CEPSTRA != 0)
        MEL->num_cepstra = P->NUM_CEPSTRA;
    else
        MEL->num_cepstra = DEFAULT_NUM_CEPSTRA;

    if (P->NUM_FILTERS != 0)
        MEL->num_filters = P->NUM_FILTERS;
    else {
        if (MEL->sampling_rate == BB_SAMPLING_RATE)
            MEL->num_filters = DEFAULT_BB_NUM_FILTERS;
        else if (MEL->sampling_rate == NB_SAMPLING_RATE)
            MEL->num_filters = DEFAULT_NB_NUM_FILTERS;
        else {
            E_WARN("Please define the number of MEL filters needed\n");
            E_FATAL("Modify include/fe.h and fe_sigproc.c\n");
        }
    }

    if (P->UPPER_FILT_FREQ != 0)
        MEL->upper_filt_freq = P->UPPER_FILT_FREQ;
    else {
        if (MEL->sampling_rate == BB_SAMPLING_RATE)
            MEL->upper_filt_freq = (float32) DEFAULT_BB_UPPER_FILT_FREQ;
        else if (MEL->sampling_rate == NB_SAMPLING_RATE)
            MEL->upper_filt_freq = (float32) DEFAULT_NB_UPPER_FILT_FREQ;
        else {
            E_WARN("Please define the upper filt frequency needed\n");
            E_FATAL("Modify include/fe.h and fe_sigproc.c\n");
        }
    }

    if (P->LOWER_FILT_FREQ != 0)
        MEL->lower_filt_freq = P->LOWER_FILT_FREQ;
    else {
        if (MEL->sampling_rate == BB_SAMPLING_RATE)
            MEL->lower_filt_freq = (float32) DEFAULT_BB_LOWER_FILT_FREQ;
        else if (MEL->sampling_rate == NB_SAMPLING_RATE)
            MEL->lower_filt_freq = (float32) DEFAULT_NB_LOWER_FILT_FREQ;
        else {
            E_WARN("Please define the lower filt frequency needed\n");
            E_FATAL("Modify include/fe.h and fe_sigproc.c\n");
        }
    }

    if (P->doublebw)
        MEL->doublewide = 1;
    else
        MEL->doublewide = 0;

    if (P->warp_type == NULL) {
        MEL->warp_type = DEFAULT_WARP_TYPE;
    }
    else {
        MEL->warp_type = P->warp_type;
    }
    MEL->warp_params = P->warp_params;

    if (fe_warp_set(MEL->warp_type) != FE_SUCCESS) {
        E_FATAL("Failed to initialize the warping function.\n");
    }
    fe_warp_set_parameters(MEL->warp_params, MEL->sampling_rate);
}

void
fe_print_current(fe_t const *FE)
{
    E_INFO("Current FE Parameters:\n");
    E_INFO("\tSampling Rate:             %f\n", FE->SAMPLING_RATE);
    E_INFO("\tFrame Size:                %d\n", FE->FRAME_SIZE);
    E_INFO("\tFrame Shift:               %d\n", FE->FRAME_SHIFT);
    E_INFO("\tFFT Size:                  %d\n", FE->FFT_SIZE);
    E_INFO("\tLower Frequency:           %g\n",
           FE->MEL_FB->lower_filt_freq);
    E_INFO("\tUpper Frequency:           %g\n",
           FE->MEL_FB->upper_filt_freq);
    E_INFO("\tNumber of filters:         %d\n", FE->MEL_FB->num_filters);
    E_INFO("\tNumber of Overflow Samps:  %d\n", FE->NUM_OVERFLOW_SAMPS);
    E_INFO("\tStart Utt Status:          %d\n", FE->START_FLAG);
    if (FE->dither) {
        E_INFO("Will add dither to audio\n");
        E_INFO("Dither seeded with %d\n", FE->seed);
    }
    else {
        E_INFO("Will not add dither to audio\n");
    }
    if (FE->MEL_FB->doublewide) {
        E_INFO("Will use double bandwidth in mel filter\n");
    }
    else {
        E_INFO("Will not use double bandwidth in mel filter\n");
    }
}
