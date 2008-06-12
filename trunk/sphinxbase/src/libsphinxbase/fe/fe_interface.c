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
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <assert.h>
#ifdef _WIN32_WCE
#include <windows.h>
#else
#include <time.h>
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "prim_type.h"
#include "byteorder.h"
#include "fixpoint.h"
#include "fe_internal.h"
#include "genrand.h"
#include "err.h"
#include "cmd_ln.h"
#include "ckd_alloc.h"
#include "fe_warp.h"

static const arg_t fe_args[] = {
    waveform_to_cepstral_command_line_macro()
};

int
fe_parse_general_params(cmd_ln_t *config, fe_t * fe)
{
    int j;

    fe->config = config;
    fe->sampling_rate = cmd_ln_float32_r(config, "-samprate");
    fe->frame_rate = cmd_ln_int32_r(config, "-frate");
    if (cmd_ln_boolean_r(config, "-dither")) {
        fe->dither = 1;
        fe->seed = cmd_ln_int32_r(config, "-seed");
    }
#ifdef WORDS_BIGENDIAN
    fe->swap = strcmp("big", cmd_ln_str_r(config, "-input_endian")) == 0 ? 0 : 1;
#else        
    fe->swap = strcmp("little", cmd_ln_str_r(config, "-input_endian")) == 0 ? 0 : 1;
#endif
    fe->window_length = cmd_ln_float32_r(config, "-wlen");
    fe->pre_emphasis_alpha = cmd_ln_float32_r(config, "-alpha");

    fe->num_cepstra = cmd_ln_int32_r(config, "-ncep");
    fe->fft_size = cmd_ln_int32_r(config, "-nfft");

    /* Check FFT size, compute FFT order (log_2(n)) */
    for (j = fe->fft_size, fe->fft_order = 0; j > 1; j >>= 1, fe->fft_order++) {
        if (((j % 2) != 0) || (fe->fft_size <= 0)) {
            E_ERROR("fft: number of points must be a power of 2 (is %d)\n",
                    fe->fft_size);
            return -1;
        }
    }
    /* Verify that FFT size is greater or equal to window length. */
    if (fe->fft_size < (int)(fe->window_length * fe->sampling_rate)) {
        E_ERROR("FFT: Number of points must be greater or equal to frame size (%d samples)\n",
                (int)(fe->window_length * fe->sampling_rate));
        return -1;
    }

    fe->remove_dc = cmd_ln_boolean_r(config, "-remove_dc");

    if (0 == strcmp(cmd_ln_str_r(config, "-transform"), "dct"))
        fe->transform = DCT_II;
    else if (0 == strcmp(cmd_ln_str_r(config, "-transform"), "legacy"))
        fe->transform = LEGACY_DCT;
    else if (0 == strcmp(cmd_ln_str_r(config, "-transform"), "htk"))
        fe->transform = DCT_HTK;
    else {
        E_ERROR("Invalid transform type (values are 'dct', 'legacy', 'htk')\n");
        return -1;
    }

    if (cmd_ln_boolean_r(config, "-logspec"))
        fe->log_spec = RAW_LOG_SPEC;
    if (cmd_ln_boolean_r(config, "-smoothspec"))
        fe->log_spec = SMOOTH_LOG_SPEC;

    return 0;
}

static int
fe_parse_melfb_params(cmd_ln_t *config, fe_t *fe, melfb_t * mel)
{
    mel->sampling_rate = fe->sampling_rate;
    mel->fft_size = fe->fft_size;
    mel->num_cepstra = fe->num_cepstra;
    mel->num_filters = cmd_ln_int32_r(config, "-nfilt");

    if (fe->log_spec)
        fe->feature_dimension = mel->num_filters;
    else
        fe->feature_dimension = fe->num_cepstra;

    mel->upper_filt_freq = cmd_ln_float32_r(config, "-upperf");
    mel->lower_filt_freq = cmd_ln_float32_r(config, "-lowerf");

    mel->doublewide = cmd_ln_boolean_r(config, "-doublebw");

    mel->warp_type = cmd_ln_str_r(config, "-warp_type");
    mel->warp_params = cmd_ln_str_r(config, "-warp_params");
    mel->lifter_val = cmd_ln_int32_r(config, "-lifter");

    mel->unit_area = cmd_ln_boolean_r(config, "-unit_area");
    mel->round_filters = cmd_ln_boolean_r(config, "-round_filters");

    if (fe_warp_set(mel, mel->warp_type) != FE_SUCCESS) {
        E_ERROR("Failed to initialize the warping function.\n");
        return -1;
    }
    fe_warp_set_parameters(mel, mel->warp_params, mel->sampling_rate);
    return 0;
}

void
fe_print_current(fe_t const *fe)
{
    E_INFO("Current FE Parameters:\n");
    E_INFO("\tSampling Rate:             %f\n", fe->sampling_rate);
    E_INFO("\tFrame Size:                %d\n", fe->frame_size);
    E_INFO("\tFrame Shift:               %d\n", fe->frame_shift);
    E_INFO("\tFFT Size:                  %d\n", fe->fft_size);
    E_INFO("\tLower Frequency:           %g\n",
           fe->mel_fb->lower_filt_freq);
    E_INFO("\tUpper Frequency:           %g\n",
           fe->mel_fb->upper_filt_freq);
    E_INFO("\tNumber of filters:         %d\n", fe->mel_fb->num_filters);
    E_INFO("\tNumber of Overflow Samps:  %d\n", fe->num_overflow_samps);
    E_INFO("\tStart Utt Status:          %d\n", fe->start_flag);
    E_INFO("Will %sremove DC offset at frame level\n",
           fe->remove_dc ? "" : "not ");
    if (fe->dither) {
        E_INFO("Will add dither to audio\n");
        E_INFO("Dither seeded with %d\n", fe->seed);
    }
    else {
        E_INFO("Will not add dither to audio\n");
    }
    if (fe->mel_fb->lifter_val) {
        E_INFO("Will apply sine-curve liftering, period %d\n",
               fe->mel_fb->lifter_val);
    }
    E_INFO("Will %snormalize filters to unit area\n",
           fe->mel_fb->unit_area ? "" : "not ");
    E_INFO("Will %sround filter frequencies to DFT points\n",
           fe->mel_fb->round_filters ? "" : "not ");
    E_INFO("Will %suse double bandwidth in mel filter\n",
           fe->mel_fb->doublewide ? "" : "not ");
}

fe_t *
fe_init_auto()
{
    return fe_init_auto_r(cmd_ln_retain(cmd_ln_get()));
}

fe_t *
fe_init_auto_r(cmd_ln_t *config)
{
    fe_t *fe;

    fe = ckd_calloc(1, sizeof(*fe));
    fe->refcount = 1;

    /* transfer params to front end */
    if (fe_parse_general_params(config, fe) < 0) {
        fe_free(fe);
        return NULL;
    }

    /* compute remaining fe parameters */
    /* We add 0.5 so approximate the float with the closest
     * integer. E.g., 2.3 is truncate to 2, whereas 3.7 becomes 4
     */
    fe->frame_shift = (int32) (fe->sampling_rate / fe->frame_rate + 0.5);
    fe->frame_size = (int32) (fe->window_length * fe->sampling_rate + 0.5);
    fe->prior = 0;
    fe->frame_counter = 0;

    if (fe->frame_size > (fe->fft_size)) {
        E_WARN
            ("Number of FFT points has to be a power of 2 higher than %d\n",
             (fe->frame_size));
        return (NULL);
    }

    if (fe->dither)
        fe_init_dither(fe->seed);

    /* establish buffers for overflow samps and hamming window */
    fe->overflow_samps = ckd_calloc(fe->frame_size, sizeof(int16));
    fe->hamming_window = ckd_calloc(fe->frame_size/2, sizeof(window_t));

    /* create hamming window */
    fe_create_hamming(fe->hamming_window, fe->frame_size);

    /* init and fill appropriate filter structure */
    fe->mel_fb = ckd_calloc(1, sizeof(*fe->mel_fb));

    /* transfer params to mel fb */
    fe_parse_melfb_params(config, fe, fe->mel_fb);
    fe_build_melfilters(fe->mel_fb);
    fe_compute_melcosine(fe->mel_fb);

    /* Create temporary FFT, spectrum and mel-spectrum buffers. */
    /* FIXME: Gosh there are a lot of these. */
    fe->spch = ckd_calloc(fe->frame_size, sizeof(*fe->spch));
    fe->frame = ckd_calloc(fe->fft_size, sizeof(*fe->frame));
    fe->spec = ckd_calloc(fe->fft_size, sizeof(*fe->spec));
    fe->mfspec = ckd_calloc(fe->mel_fb->num_filters, sizeof(*fe->mfspec));

    /* create twiddle factors */
    fe->ccc = ckd_calloc(fe->fft_size / 4, sizeof(*fe->ccc));
    fe->sss = ckd_calloc(fe->fft_size / 4, sizeof(*fe->sss));
    fe_create_twiddle(fe);

    if (cmd_ln_boolean_r(config, "-verbose")) {
        fe_print_current(fe);
    }

    /*** Z.A.B. ***/
    /*** Initialize the overflow buffers ***/
    fe_start_utt(fe);
    return fe;
}

arg_t const *
fe_get_args(void)
{
    return fe_args;
}

cmd_ln_t *
fe_get_config(fe_t *fe)
{
    return fe->config;
}

void
fe_init_dither(int32 seed)
{
    if (seed < 0) {
        E_INFO("You are using the internal mechanism to generate the seed.\n");
#ifdef _WIN32_WCE
        s3_rand_seed(GetTickCount());
#else
        s3_rand_seed((long) time(0));
#endif
    }
    else {
        E_INFO("You are using %d as the seed.\n", seed);
        s3_rand_seed(seed);
    }
}

int32
fe_start_utt(fe_t * fe)
{
    fe->num_overflow_samps = 0;
    memset(fe->overflow_samps, 0, fe->frame_size * sizeof(int16));
    fe->start_flag = 1;
    fe->prior = 0;
    return 0;
}

int
fe_get_output_size(fe_t *fe)
{
    return (int)fe->feature_dimension;
}

void
fe_get_input_size(fe_t *fe, int *out_frame_shift,
                  int *out_frame_size)
{
    if (out_frame_shift)
        *out_frame_shift = fe->frame_shift;
    if (out_frame_size)
        *out_frame_size = fe->frame_size;
}

int32
fe_process_frame(fe_t * fe, int16 const *spch, int32 nsamps, mfcc_t * fr_cep)
{
    fe_read_frame(fe, spch, nsamps);
    return fe_write_frame(fe, fr_cep);
}

int
fe_process_frames(fe_t *fe,
                  int16 const **inout_spch,
                  size_t *inout_nsamps,
                  mfcc_t **buf_cep,
                  int32 *inout_nframes)
{
    int32 frame_count;
    int i, n_overflow, orig_n_overflow;
    int16 const *orig_spch;

    /* In the special case where there is no output buffer, return the
     * number of frames which would be generated. */
    if (buf_cep == NULL) {
        if (*inout_nsamps + fe->num_overflow_samps < (size_t)fe->frame_size)
            *inout_nframes = 0;
        else 
            *inout_nframes = 1
                + ((*inout_nsamps + fe->num_overflow_samps - fe->frame_size)
                   / fe->frame_shift);
        return 0;
    }

    /* Are there not enough samples to make at least 1 frame? */
    if (*inout_nsamps + fe->num_overflow_samps < (size_t)fe->frame_size) {
        if (*inout_nsamps > 0) {
            /* Append them to the overflow buffer. */
            memcpy(fe->overflow_samps + fe->num_overflow_samps,
                   *inout_spch, *inout_nsamps * (sizeof(int16)));
            fe->num_overflow_samps += *inout_nsamps;
            /* Update input-output pointers and counters. */
            *inout_spch += *inout_nsamps;
            *inout_nsamps = 0;
        }
        /* We produced no frames of output, sorry! */
        *inout_nframes = 0;
        return 0;
    }

    /* Can't write a frame?  Then do nothing! */
    if (*inout_nframes < 1) {
        *inout_nframes = 0;
        return 0;
    }

    /* Keep track of the original start of the buffer. */
    orig_spch = *inout_spch;
    orig_n_overflow = fe->num_overflow_samps;
    /* How many frames will we be able to get? */
    frame_count = 1
        + ((*inout_nsamps + fe->num_overflow_samps - fe->frame_size)
           / fe->frame_shift);
    /* Limit it to the number of output frames available. */
    if (frame_count > *inout_nframes)
        frame_count = *inout_nframes;

    /* Start processing, taking care of any incoming overflow. */
    if (fe->num_overflow_samps) {
        int offset = fe->frame_size - fe->num_overflow_samps;

        /* Append start of spch to overflow samples to make a full frame. */
        memcpy(fe->overflow_samps + fe->num_overflow_samps,
               *inout_spch, offset * sizeof(**inout_spch));
        fe_read_frame(fe, fe->overflow_samps, fe->frame_size);
        fe_write_frame(fe, buf_cep[0]);
        /* Update input-output pointers and counters. */
        *inout_spch += offset;
        *inout_nsamps -= offset;
        fe->num_overflow_samps -= fe->frame_shift;
    }
    else {
        fe_read_frame(fe, *inout_spch, fe->frame_size);
        fe_write_frame(fe, buf_cep[0]);
        /* Update input-output pointers and counters. */
        *inout_spch += fe->frame_size;
        *inout_nsamps -= fe->frame_size;
    }
    /* Update the number of remaining frames. */
    --*inout_nframes;

    /* Process all remaining frames. */
    for (i = 1; i < frame_count; ++i) {
        assert(*inout_nsamps >= (size_t)fe->frame_shift);
        assert(*inout_nframes > 0);

        fe_shift_frame(fe, *inout_spch, fe->frame_shift);
        fe_write_frame(fe, buf_cep[i]);
        /* Update input-output pointers and counters. */
        *inout_spch += fe->frame_shift;
        *inout_nsamps -= fe->frame_shift;
        /* Amount of data behind the original input which is still needed. */
        if (fe->num_overflow_samps > 0)
            fe->num_overflow_samps -= fe->frame_shift;
        /* Update number of remaining frames. */
        --*inout_nframes;
    }

    /* How many relevant overflow samples are there left? */
    if (fe->num_overflow_samps <= 0) {
        /* Maximum number of overflow samples past *inout_spch to save. */
        n_overflow = *inout_nsamps;
        if (n_overflow > fe->frame_shift)
            n_overflow = fe->frame_shift;
        fe->num_overflow_samps = fe->frame_size - fe->frame_shift;
        /* Make sure this isn't an illegal read! */
        if (fe->num_overflow_samps > *inout_spch - orig_spch)
            fe->num_overflow_samps = *inout_spch - orig_spch;
        fe->num_overflow_samps += n_overflow;
        if (fe->num_overflow_samps > 0) {
            memcpy(fe->overflow_samps,
                   *inout_spch - (fe->frame_size - fe->frame_shift),
                   fe->num_overflow_samps * sizeof(**inout_spch));
            /* Update the input pointer to cover this stuff. */
            *inout_spch += n_overflow;
            *inout_nsamps -= n_overflow;
        }
    }
    else {
        /* There is still some relevant data left in the overflow buffer. */
        /* Shift existing data to the beginning. */
        memmove(fe->overflow_samps,
                fe->overflow_samps + orig_n_overflow - fe->num_overflow_samps,
                fe->num_overflow_samps * sizeof(*fe->overflow_samps));
        /* Copy in whatever we had in the original speech buffer. */
        n_overflow = *inout_spch - orig_spch + *inout_nsamps;
        if (n_overflow > fe->frame_size - fe->num_overflow_samps)
            n_overflow = fe->frame_size - fe->num_overflow_samps;
        memcpy(fe->overflow_samps + fe->num_overflow_samps,
               orig_spch, n_overflow * sizeof(*orig_spch));
        fe->num_overflow_samps += n_overflow;
        /* Advance the input pointers. */
        if (n_overflow > *inout_spch - orig_spch) {
            n_overflow -= (*inout_spch - orig_spch);
            *inout_spch += n_overflow;
            *inout_nsamps -= n_overflow;
        }
    }

    /* Finally update the frame counter with the number of frames we procesed. */
    *inout_nframes = frame_count;
    return 0;
}

int
fe_process_utt(fe_t * fe, int16 const * spch, size_t nsamps,
               mfcc_t *** cep_block, int32 * nframes)
{
    mfcc_t **cep;
    int rv;

    /* Figure out how many frames we will need. */
    fe_process_frames(fe, NULL, &nsamps, NULL, nframes);
    /* Create the output buffer (it has to exist, even if there are no output frames). */
    if (*nframes)
        cep = (mfcc_t **)ckd_calloc_2d(*nframes, fe->feature_dimension, sizeof(**cep));
    else
        cep = (mfcc_t **)ckd_calloc_2d(1, fe->feature_dimension, sizeof(**cep));
    /* Now just call fe_process_frames() with the allocated buffer. */
    rv = fe_process_frames(fe, &spch, &nsamps, cep, nframes);
    *cep_block = cep;

    return rv;
}


int32
fe_end_utt(fe_t * fe, mfcc_t * cepvector, int32 * nframes)
{
    int32 frame_count;

    /* Process any remaining data. */
    if (fe->num_overflow_samps > 0) {
        fe_read_frame(fe, fe->overflow_samps, fe->num_overflow_samps);
        fe_write_frame(fe, cepvector);
        frame_count = 1;
    }
    else {
        frame_count = 0;
    }

    /* reset overflow buffers... */
    fe->num_overflow_samps = 0;
    fe->start_flag = 0;

    *nframes = frame_count;
    return 0;
}

fe_t *
fe_retain(fe_t *fe)
{
    ++fe->refcount;
    return fe;
}

int
fe_free(fe_t * fe)
{
    if (fe == NULL)
        return 0;
    if (--fe->refcount > 0)
        return fe->refcount;

    /* kill FE instance - free everything... */
    fe_free_2d((void *) fe->mel_fb->mel_cosine);
    if (fe->mel_fb->lifter)
        free(fe->mel_fb->lifter);
    ckd_free(fe->mel_fb->spec_start);
    ckd_free(fe->mel_fb->filt_start);
    ckd_free(fe->mel_fb->filt_width);
    ckd_free(fe->mel_fb->filt_coeffs);
    ckd_free(fe->mel_fb);
    ckd_free(fe->spch);
    ckd_free(fe->frame);
    ckd_free(fe->ccc);
    ckd_free(fe->sss);
    ckd_free(fe->spec);
    ckd_free(fe->mfspec);
    ckd_free(fe->overflow_samps);
    ckd_free(fe->hamming_window);
    ckd_free(fe);

    return 0;
}

/**
 * Convert a block of mfcc_t to float32 (can be done in-place)
 **/
int32
fe_mfcc_to_float(fe_t * fe,
                 mfcc_t ** input, float32 ** output, int32 nframes)
{
    int32 i;

#ifndef FIXED_POINT
    if ((void *) input == (void *) output)
        return nframes * fe->feature_dimension;
#endif
    for (i = 0; i < nframes * fe->feature_dimension; ++i)
        output[0][i] = MFCC2FLOAT(input[0][i]);

    return i;
}

/**
 * Convert a block of float32 to mfcc_t (can be done in-place)
 **/
int32
fe_float_to_mfcc(fe_t * fe,
                 float32 ** input, mfcc_t ** output, int32 nframes)
{
    int32 i;

#ifndef FIXED_POINT
    if ((void *) input == (void *) output)
        return nframes * fe->feature_dimension;
#endif
    for (i = 0; i < nframes * fe->feature_dimension; ++i)
        output[0][i] = FLOAT2MFCC(input[0][i]);

    return i;
}

int32
fe_logspec_to_mfcc(fe_t * fe, const mfcc_t * fr_spec, mfcc_t * fr_cep)
{
#ifdef FIXED_POINT
    fe_spec2cep(fe, fr_spec, fr_cep);
#else                           /* ! FIXED_POINT */
    powspec_t *powspec;
    int32 i;

    powspec = ckd_malloc(fe->mel_fb->num_filters * sizeof(powspec_t));
    for (i = 0; i < fe->mel_fb->num_filters; ++i)
        powspec[i] = (powspec_t) fr_spec[i];
    fe_spec2cep(fe, powspec, fr_cep);
    ckd_free(powspec);
#endif                          /* ! FIXED_POINT */
    return 0;
}

int32
fe_logspec_dct2(fe_t * fe, const mfcc_t * fr_spec, mfcc_t * fr_cep)
{
#ifdef FIXED_POINT
    fe_dct2(fe, fr_spec, fr_cep, 0);
#else                           /* ! FIXED_POINT */
    powspec_t *powspec;
    int32 i;

    powspec = ckd_malloc(fe->mel_fb->num_filters * sizeof(powspec_t));
    for (i = 0; i < fe->mel_fb->num_filters; ++i)
        powspec[i] = (powspec_t) fr_spec[i];
    fe_dct2(fe, powspec, fr_cep, 0);
    ckd_free(powspec);
#endif                          /* ! FIXED_POINT */
    return 0;
}

int32
fe_mfcc_dct3(fe_t * fe, const mfcc_t * fr_cep, mfcc_t * fr_spec)
{
#ifdef FIXED_POINT
    fe_dct3(fe, fr_cep, fr_spec);
#else                           /* ! FIXED_POINT */
    powspec_t *powspec;
    int32 i;

    powspec = ckd_malloc(fe->mel_fb->num_filters * sizeof(powspec_t));
    fe_dct3(fe, fr_cep, powspec);
    for (i = 0; i < fe->mel_fb->num_filters; ++i)
        fr_spec[i] = (mfcc_t) powspec[i];
    ckd_free(powspec);
#endif                          /* ! FIXED_POINT */
    return 0;
}
