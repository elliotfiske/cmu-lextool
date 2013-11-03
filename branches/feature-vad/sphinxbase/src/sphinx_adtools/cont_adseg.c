/* -*- c-basic-offset: 4; indent-tabs-mode: nil -*- */
/* ====================================================================
 * Copyright (c) 2013 Carnegie Mellon University.  All rights
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
/*
 * cont_adseg.c -- Continuously listen and segment input speech into utterances.
 * 
 * HISTORY
 *
 * 02-Nov-13    Refactored to use new snr-based VAD
 * 
 * 27-Jun-96	M K Ravishankar (rkm@cs.cmu.edu) at Carnegie Mellon University
 * 		Created.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include <sphinxbase/prim_type.h>
#include <sphinxbase/ad.h>
#include <sphinxbase/fe.h>
#include <sphinxbase/cmd_ln.h>
#include <sphinxbase/ckd_alloc.h>
#include <sphinxbase/err.h>

static const arg_t cont_args_def[] = {
    waveform_to_cepstral_command_line_macro(),
    /* Argument file. */
    {"-argfile",
     ARG_STRING,
     NULL,
     "Argument file giving extra arguments."},
    {"-adcdev",
     ARG_STRING,
     NULL,
     "Name of audio device to use for input."},
    {"-end_sil_num",
     ARG_FLOAT32,
     "1.0",
     "Length of silence to end utterance in seconds."},
	{ NULL, 0, NULL, NULL }
};

static fe_t* fe;
static cmd_ln_t *config;

/* Sleep for specified msec */
static void
sleep_msec(int32 ms)
{
#if (defined(WIN32) && !defined(GNUWINCE)) || defined(_WIN32_WCE)
    Sleep(ms);
#else
    /* ------------------- Unix ------------------ */
    struct timeval tmo;

    tmo.tv_sec = 0;
    tmo.tv_usec = ms * 1000;

    select(0, NULL, NULL, NULL, &tmo);
#endif
}

/*
 * @brief Buffer for prespeech audio storage.
 *        VAD in fe triggers only after N speech frames,
 *        which are processed from internal buffer as cepstral
 *        coefs. To keep this audio, it should be stored 
 *        externaly.
 */
typedef struct prespch_buf_s {
    int16 **buf;
	int write_ptr;
	int frame_num;
	int frame_len;
} prespch_buf_t;

prespch_buf_t*
prespch_buf_init(int frame_num, int frame_len)
{
    prespch_buf_t *prespch_buf;
	prespch_buf = (prespch_buf_t *) ckd_calloc(1, sizeof(prespch_buf_t));
    prespch_buf->buf = (int16 **)ckd_calloc_2d(frame_num, frame_len, sizeof(int16));
	prespch_buf->frame_num = frame_num;
	prespch_buf->frame_len = frame_len;
	prespch_buf->write_ptr = 0;
	return prespch_buf;
}

void
prespch_buf_free(prespch_buf_t* prespch_buf)
{
    if (prespch_buf->buf)
	    ckd_free_2d((void **)prespch_buf->buf);
	ckd_free(prespch_buf);
	    
}

void
prespch_buf_write(prespch_buf_t *prespch_buf, int16* in_buf, int len)
{
    memcpy(prespch_buf->buf[prespch_buf->write_ptr], in_buf, sizeof(int16)*len);
	prespch_buf->write_ptr++;
	if (prespch_buf->write_ptr >= prespch_buf->frame_num)
	    //start writing from the beginning
		prespch_buf->write_ptr = 0;
}

void
prespch_buf_dump(prespch_buf_t *prespch_buf, FILE *fp)
{
    int i;
	
    for (i=prespch_buf->write_ptr; i<prespch_buf->frame_num; i++)
	    fwrite(prespch_buf->buf[i], sizeof(int16), prespch_buf->frame_len, fp);
	for (i=0; i<prespch_buf->write_ptr; i++)
	    fwrite(prespch_buf->buf[i], sizeof(int16), prespch_buf->frame_len, fp);
	prespch_buf->write_ptr = 0;
}   /* end of prespch_buf declaration */

/*
 * @brief Buffer for frame accumulating
 */
typedef struct frame_buf_s {
	int16 *buf;
	int write_ptr;
	int read_ptr;
} frame_buf_t;

frame_buf_t*
frame_buf_init(int len) 
{
    frame_buf_t *frame_buf;
	frame_buf = (frame_buf_t *) ckd_calloc(1, sizeof(frame_buf_t));
    frame_buf->buf = (int16*)ckd_calloc(len, sizeof(int16));
    frame_buf->write_ptr = 0;
    frame_buf->read_ptr = 0;
	return frame_buf;
}

void
frame_buf_free(frame_buf_t *frame_buf)
{
    if (frame_buf->buf)
        ckd_free(frame_buf->buf);
	ckd_free(frame_buf);
}

void
frame_buf_read(frame_buf_t *frame_buf, int16* out_buf, int len, int shift)
{
    memcpy(out_buf, &frame_buf->buf[frame_buf->read_ptr], sizeof(int16)*len);
	frame_buf->read_ptr += shift;
}

void
frame_buf_write(frame_buf_t *frame_buf, int16* in_buf, int len)
{
    memcpy(&frame_buf->buf[frame_buf->write_ptr], in_buf, sizeof(int16)*len);
	frame_buf->write_ptr += len;
}

void
frame_buf_reset(frame_buf_t *frame_buf)
{
    memmove(frame_buf->buf, &frame_buf->buf[frame_buf->read_ptr], 
	        sizeof(int16)*(frame_buf->write_ptr-frame_buf->read_ptr));
    frame_buf->write_ptr -= frame_buf->read_ptr;
    frame_buf->read_ptr = 0;
}

int
frame_buf_get_len(frame_buf_t *frame_buf)
{
    return (frame_buf->write_ptr - frame_buf->read_ptr);
} /* end of frame_buf declaration */

/*
 * Segment raw A/D input data into utterances whenever silence region of given
 * duration is encountered.
 * Utterances are written to files named 0001.raw, 0002.raw, 0003.raw, etc.
 */
void
record_segments()
{
    ad_rec_t *ad;
	frame_buf_t *frame_buf;
	prespch_buf_t *prespch_buf;
	FILE *fp;
	int16 *frame;
	
	int i, k, uttno, uttlen, sample_rate;
	int start_sil_num, end_sil_num, end_sil;
	int frame_len, frame_overlap;
	uint8 vad_state, vad_prev_state, is_writing;
	
    int16 ad_buf[2048];
	mfcc_t mfcc_buf[128];
    char file[1024];
	
    start_sil_num = cmd_ln_int_r(config, "-vad_prespeech");
	end_sil_num = (int)(cmd_ln_float_r(config, "-end_sil_num") *
	                            cmd_ln_int_r(config, "-frate"));
	sample_rate = (int)cmd_ln_float32_r(config, "-samprate");
	frame_len = cmd_ln_float32_r(config, "-wlen") * sample_rate;
	frame_overlap = sample_rate/cmd_ln_int_r(config, "-frate");
	frame = (int16*)ckd_calloc(frame_len, sizeof(*frame));
	frame_buf = frame_buf_init(4096);
	prespch_buf = prespch_buf_init(start_sil_num, frame_overlap);
	
    /* Open raw A/D device */
    if ((ad = ad_open_dev(cmd_ln_str_r(config, "-adcdev"),sample_rate)) == NULL)
        E_FATAL("Failed to open audio device\n");

    printf("Start recording ...\n");
    fflush(stdout);
    if (ad_start_rec(ad) < 0)
        E_FATAL("Failed to start recording\n");
	/* skip empty buffers */
	for (i=0; i<5; i++) {
		sleep_msec(200);
		k=ad_read(ad, ad_buf, 2048);
	}
    
    /* Forever listen for utterances */
    uttno = 0;
	uttlen = 0;
	vad_state = 0;
	vad_prev_state = 0;
	is_writing = 0;
	end_sil = 0;
	fp = NULL;
	if (fe_start_utt(fe) < 0)
	    E_FATAL("Failed to start utterance\n");
	printf("You may speak now\n");
    fflush(stdout);
    for (;;) {

        /* filling frame buffer, to produce at least one frame */
        while (frame_buf_get_len(frame_buf) < frame_len) {
			if ((k = ad_read(ad, ad_buf, 2048)) < 0)
				E_FATAL("Failed to read audio\n");
			frame_buf_write(frame_buf, ad_buf, k);
		}
		
		/* using obtained audio while there is at least frame */
		while (frame_buf_get_len(frame_buf) > frame_len) {
		    frame_buf_read(frame_buf, frame, frame_len, frame_overlap);
            fe_process_frame(fe, frame, frame_len, mfcc_buf);
			vad_state = fe_get_vad_state(fe);
			
			/* Waiting for silence of length specified by user */
			if (vad_state)
				end_sil = 0;
			if (is_writing && !vad_state) {
				end_sil++;
				vad_state = 1;
				if (end_sil >= end_sil_num) {
				    end_sil = 0;
					vad_state = 0;
				}
			}
			
			if (vad_state) {
			    if (!vad_prev_state) {
				    //utterance detected. file should be created, prespeech dumped
					uttno++;
                    sprintf(file, "%04d.raw", uttno);
                    if ((fp = fopen(file, "wb")) == NULL)
                        E_FATAL_SYSTEM("Failed to open '%s' for reading", file);
                    prespch_buf_dump(prespch_buf, fp);
					uttlen = start_sil_num * frame_overlap;
					printf("Utterance %04d, logging to %s\n", uttno, file);
					is_writing = 1;
				}
				//we're in the utterance, just dump frame to file
				fwrite(frame, sizeof(*frame), frame_overlap, fp);
				uttlen += frame_overlap;
			} else {
			    prespch_buf_write(prespch_buf, frame, frame_overlap);
				if (is_writing) {
				     //end of utterance detected. File should be closed
					 fclose(fp);
                     printf("\tUtterance %04d = %d samples (%.1fsec)\n\n",
                        uttno, uttlen, (double) uttlen / (double) sample_rate);
					is_writing = 0;
				}
			}
			vad_prev_state = vad_state;
		}
		frame_buf_reset(frame_buf);		
    }
	
	if (is_writing) {
		fclose(fp);
        printf("\tUtterance %04d = %d samples (%.1fsec)\n\n",
                uttno, uttlen, (double) uttlen / (double) sample_rate);
	}
	
    ad_stop_rec(ad);
    ad_close(ad);
	if (frame)
	    ckd_free((void*)frame);
	if (frame_buf)
	    frame_buf_free(frame_buf);
    if (prespch_buf)
        prespch_buf_free(prespch_buf);
}

int
main(int argc, char *argv[])
{
    char const *cfg;
    if (argc == 2) {
        config = cmd_ln_parse_file_r(NULL, cont_args_def, argv[1], TRUE);
    }
    else {
        config = cmd_ln_parse_r(NULL, cont_args_def, argc, argv, FALSE);
	}
    /* Handle argument file as -argfile. */
    if (config && (cfg = cmd_ln_str_r(config, "-argfile")) != NULL) {
        config = cmd_ln_parse_file_r(config, cont_args_def, cfg, FALSE);
    }
    if (config == NULL)
        return 1;
	
    fe = fe_init_auto_r(config);
    if (fe == NULL)
        return 1;

    record_segments();

    fe_free(fe);
    return 0;
}
