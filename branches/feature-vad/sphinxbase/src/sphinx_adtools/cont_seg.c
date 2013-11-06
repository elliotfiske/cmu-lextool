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
 * cont_seg.c -- Continuously listen and segment input speech into utterances.
 * 
 * HISTORY
 *
 * 05-Nov-13    Created from adseg and fileseg
 * 
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

//how many times read_audio should return 0
//to terminate reading/recording
#define READ_ATTEMPTS 100

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
	{"-infile",
     ARG_STRING,
     NULL,
     "Name of audio file to use for input."},
    {"-end_sil_len",
     ARG_FLOAT32,
     "1.0",
     "Length of silence to end utterance in seconds."},
	{ NULL, 0, NULL, NULL }
};

static fe_t* fe;
static cmd_ln_t *config;
static int (*read_audio)(int16 *buf, int len);
static ad_rec_t *ad;
static FILE *infile;

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

static int 
read_audio_file(int16 *buf, int len)
{
    if (!infile) {
		E_FATAL("Failed to read audio from file\n");
		return -1;
	}
	return fread(buf, sizeof(int16), len, infile);
}

static int
read_audio_adev(int16 *buf, int len)
{
    if (!ad) {
		E_FATAL("Failed to read audio from mic\n");
		return -1;
	}
	sleep_msec(50);
	return ad_read(ad, buf, len);
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
	/* write pointer 
	   in frames */
	int write_ptr;
	/* amount of frames */
	int frame_num;
	/* frame len in samples */
	int frame_len;
	/* total amount of frames 
	   written into prespeech buf */
	int frame_num_total;
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
	prespch_buf->frame_num_total = 0;
	return prespch_buf;
}

void
prespch_buf_free(prespch_buf_t* prespch_buf)
{
    if (prespch_buf->buf)
	    ckd_free_2d((void **)prespch_buf->buf);
	ckd_free(prespch_buf);
	    
}

/**
 * @brief write audio data into prespch_buf
 * @param [in/out] in - buffer to write, 
                   out - buffer that was substituted
 */
void
prespch_buf_write(prespch_buf_t *prespch_buf, int16* in_buf, int len)
{
	int16 tmp_buf[512];
	memcpy(tmp_buf, prespch_buf->buf[prespch_buf->write_ptr], sizeof(int16)*len);
    memcpy(prespch_buf->buf[prespch_buf->write_ptr], in_buf, sizeof(int16)*len);
	memcpy(in_buf, tmp_buf, sizeof(int16)*len);
	prespch_buf->write_ptr++;
	if (prespch_buf->write_ptr >= prespch_buf->frame_num)
	    //start writing from the beginning
		prespch_buf->write_ptr = 0;
	    prespch_buf->frame_num_total++;
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
	prespch_buf->frame_num_total = 0;
}   

int
prespch_buf_get_nframes(prespch_buf_t *prespch_buf)
{
	return prespch_buf->frame_num_total;
}/* end of prespch_buf declaration */

/*
 * @brief Buffer for frame accumulating
 */
typedef struct frame_buf_s {
	int16 *buf;
	/* write pointer 
	   in samples */
	int write_ptr;
	/* read pointer 
	   in samples */
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
 * Segment input audio data into utterances whenever silence region of given
 * duration is encountered.
 * Utterances are written to files named 0001.raw, 0002.raw, 0003.raw, etc.
 */
void
segment_audio()
{
	frame_buf_t *frame_buf;
	prespch_buf_t *prespch_buf;
	FILE *fp;
	int16 *frame;
	
	int k, uttno, uttlen, sample_rate, read_attempts;
	int start_sil_num, end_sil_num;
	int frame_len, frame_overlap;
	uint8 vad_state, vad_prev_state, is_writing;
	uint8 finish_reading;
	
    int16 raw_buf[2048];
	mfcc_t mfcc_buf[128];
    char file[1024];
	
    start_sil_num = cmd_ln_int_r(config, "-vad_prespeech");
	end_sil_num = (int)(cmd_ln_float_r(config, "-end_sil_len") * 
		cmd_ln_int_r(config, "-frate"));
	sample_rate = (int)cmd_ln_float32_r(config, "-samprate");
	frame_len = cmd_ln_float32_r(config, "-wlen") * sample_rate;
	frame_overlap = sample_rate/cmd_ln_int_r(config, "-frate");
	frame = (int16*)ckd_calloc(frame_len, sizeof(*frame));
	frame_buf = frame_buf_init(4096);
	prespch_buf = prespch_buf_init(start_sil_num, frame_overlap);
	
    /* Forever listen for utterances */
    uttno = 0;
	uttlen = 0;
	vad_state = FALSE;
	vad_prev_state = FALSE;
	is_writing = FALSE;
	finish_reading = FALSE;
	read_attempts = 0;
	fp = NULL;
	if (fe_start_utt(fe) < 0)
	    E_FATAL("Failed to start utterance\n");
    for (;;) {

        /* filling frame buffer, to produce at least one frame */
        while (frame_buf_get_len(frame_buf) < frame_len) {
			if ((k = read_audio(raw_buf, 2048)) < 0)
				E_FATAL("Failed to read audio\n");
			if (k == 0) {
			    read_attempts++;
				if (read_attempts >= READ_ATTEMPTS) {
					//several attempts to read audio failed
					finish_reading = TRUE;
					break;
				}
			} else {
				read_attempts = 0;
			}
			frame_buf_write(frame_buf, raw_buf, k);
		}
		
		if (finish_reading)
			break;

		/* using obtained audio while there is at least frame */
		while (frame_buf_get_len(frame_buf) > frame_len) {
		    frame_buf_read(frame_buf, frame, frame_len, frame_overlap);
            fe_process_frame(fe, frame, frame_len, mfcc_buf);
			vad_state = fe_get_vad_state(fe);
						
			if (vad_state) {
			    if (!vad_prev_state) {
				    //utterance detected
					if (!is_writing) {
					    //creating file for speech segment
						uttno++;
                        sprintf(file, "%04d.raw", uttno);
                        if ((fp = fopen(file, "wb")) == NULL)
                            E_FATAL_SYSTEM("Failed to open '%s' for reading", file);
					    printf("Utterance %04d, logging to %s\n", uttno, file);
					} //in other case end sil specified by user hasn't ended, 
					  //so just continue writing
                    prespch_buf_dump(prespch_buf, fp);
					uttlen += start_sil_num * frame_overlap;
					is_writing = TRUE;
				}
				//we're in the utterance, just dump frame to file
				fwrite(frame, sizeof(*frame), frame_overlap, fp);
				uttlen += frame_overlap;
			} else {
				prespch_buf_write(prespch_buf, frame, frame_overlap);
				if (is_writing) {
				    if (prespch_buf_get_nframes(prespch_buf) > start_sil_num) {
					    //end silence specified by user isn't end yet
						fwrite(frame, sizeof(*frame), frame_overlap, fp);
						uttlen += frame_overlap;
					}
					if (prespch_buf_get_nframes(prespch_buf) > end_sil_num) {
					    //end of utterance detected. File should be closed
					    fclose(fp);
                        printf("\tUtterance %04d = %d samples (%.1fsec)\n\n",
                        uttno, uttlen, (double) uttlen / (double) sample_rate);
					    is_writing = FALSE;
						uttlen = 0;
				    }
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
	
	fe_end_utt(fe, mfcc_buf, &frame_len);
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
    char const *cfg, *infile_path;
	int i;
	int16 buf[2048];
	
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
	
	if ((infile_path = cmd_ln_str_r(config, "-infile")) != NULL) {
	    //reading audio from file
		if ((infile = fopen(infile_path, "rb")) == NULL) {
			E_FATAL("Unable to read audio from [%s]\n", infile_path);
			return 1;
		}
		read_audio = &read_audio_file;
		//skip wav header
		read_audio(buf, 44);
	} else {
		//readinf audio from A/D device
        if ((ad = ad_open_dev(cmd_ln_str_r(config, "-adcdev"),
			(int)cmd_ln_float32_r(config, "-samprate"))) == NULL) {
            E_FATAL("Failed to open audio device\n");
			return 1;
		}
		read_audio = &read_audio_adev;
        printf("Start recording ...\n");
        fflush(stdout);
        if (ad_start_rec(ad) < 0)
           E_FATAL("Failed to start recording\n");
	    //skip empty buffers
	    for (i=0; i<5; i++) {
		    sleep_msec(200);
		    read_audio(buf, 2048);
	    }
		printf("You may speak now\n");
        fflush(stdout);
	}

    fe = fe_init_auto_r(config);
    if (fe == NULL)
        return 1;

    segment_audio();

	if (ad)
		ad_close(ad);
	if (infile)
		fclose(infile);

    fe_free(fe);
    return 0;
}
