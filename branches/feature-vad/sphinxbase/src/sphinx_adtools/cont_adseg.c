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
    {"-utt_end_sil",
     ARG_FLOAT32,
     "1.0",
     "Length of silence to end utterance in seconds."}
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
 * Segment raw A/D input data into utterances whenever silence region of given
 * duration is encountered.
 * Utterances are written to files named 0001.raw, 0002.raw, 0003.raw, etc.
 */
void
record_segments()
{
    ad_rec_t *ad;
    int32 i, k, uttno, uttlen, sample_rate;
	uint8 is_speech, vad_state, is_writing;
    int16 ad_buf[2048];
	int16 frame_buf[4096];
	mfcc_t mfcc_buf[128];
	int16 *frame;
	int frame_read_ptr, frame_wrt_ptr;
    FILE *fp;
    char file[1024];
	
	int utt_start_sil, utt_end_sil;
	int end_sil_num;
	int frame_len, frame_overlap;
	int16** prespch_buf;
	int prespch_wrt_ptr;
	
    prespch_wrt_ptr = 0;
	frame_read_ptr = 0;
	frame_wrt_ptr = 0;
	end_sil_num = 0;
    utt_start_sil = cmd_ln_int_r(config, "-vad_prespeech");
	utt_end_sil = (int)(cmd_ln_float_r(config, "-utt_end_sil") *
	                            cmd_ln_int_r(config, "-frate"));
	sample_rate = (int)cmd_ln_float32_r(config, "-samprate");
	frame_len = cmd_ln_float32_r(config, "-wlen") * sample_rate;
	frame_overlap = sample_rate/cmd_ln_int_r(config, "-frate");
	prespch_buf = (int16 **)
            ckd_calloc_2d(utt_start_sil, frame_overlap, sizeof(**prespch_buf));
	frame = (int16*)ckd_calloc(frame_len, sizeof(*frame));
	
    /* Open raw A/D device */
    if ((ad = ad_open_dev(cmd_ln_str_r(config, "-adcdev"),sample_rate)) == NULL)
        E_FATAL("Failed to open audio device\n");

    printf("Start recording ...\n");
    fflush(stdout);
    if (ad_start_rec(ad) < 0)
        E_FATAL("Failed to start recording\n");
	for (i=0; i<5; i++) {
		//skip zero silence that comes from audio device
		sleep_msec(100);
		ad_read(ad, ad_buf, 2048);
	}
    
    /* Forever listen for utterances */
    printf("You may speak now\n");
    fflush(stdout);
    uttno = 0;
    if (fe_start_utt(fe) < 0)
	    E_FATAL("Failed to start utterance\n");
	is_speech = 0;
	vad_state = 0;
	is_writing = 0;
    for (;;) {

        //filling frame buffer, to produce at least one frame
        while (frame_wrt_ptr < frame_len) {
			if ((k = ad_read(ad, ad_buf, 2048)) < 0)
				E_FATAL("Failed to read audio\n");
			memcpy(&frame_buf[frame_wrt_ptr], ad_buf, sizeof(*ad_buf)*k);
			frame_wrt_ptr += k;
		}
		
		//using obtained audio while more will be needed
		while (frame_wrt_ptr - frame_read_ptr > frame_len) {
		    memcpy(frame, &frame_buf[frame_read_ptr], sizeof(*frame)*frame_len);
			frame_read_ptr += frame_overlap;
            fe_process_frame(fe, frame, frame_len, mfcc_buf);
			is_speech = fe_get_vad_state(fe);
			if (is_speech)
				end_sil_num = 0;
			if (is_writing && !is_speech) {
				end_sil_num++;
				is_speech = 1;
				if (end_sil_num >= utt_end_sil) {
				    end_sil_num = 0;
					is_speech = 0;
				}
			}
			if (is_speech) {
			    if (!vad_state) {
				    //utterance detected. file should be created, prespeech dumped
					uttno++;
                    sprintf(file, "%04d.raw", uttno);
                    if ((fp = fopen(file, "wb")) == NULL)
                        E_FATAL_SYSTEM("Failed to open '%s' for reading", file);
					for (i=prespch_wrt_ptr; i<utt_start_sil; i++)
					    fwrite(prespch_buf[i], sizeof(int16), frame_overlap, fp);
					for (i=0; i<prespch_wrt_ptr; i++)
					    fwrite(prespch_buf[i], sizeof(int16), frame_overlap, fp);
					prespch_wrt_ptr = 0;
					uttlen = utt_start_sil * frame_overlap;
					printf("Utterance %04d, logging to %s\n", uttno, file);
					is_writing = 1;
				}
				//we're in the utterance, just dump frame to file
				fwrite(frame, sizeof(*frame), frame_overlap, fp);
				uttlen += frame_overlap;
			} else {
			    memcpy(prespch_buf[prespch_wrt_ptr], frame, sizeof(*frame)*frame_overlap);
				prespch_wrt_ptr++;
				if (prespch_wrt_ptr >= utt_start_sil) {
				    //start writing from the beginning
				    prespch_wrt_ptr = 0;
			    }
				if (is_writing) {
				     //end of utterance detected. File should be closed
					 fclose(fp);
                     printf("\tUtterance %04d = %d samples (%.1fsec)\n\n",
                        uttno, uttlen, (double) uttlen / (double) sample_rate);
					is_writing = 0;
				}
			}
			vad_state = is_speech;
		}
		
		//move rest of frame_buf to the begin
		memmove(frame_buf, &frame_buf[frame_read_ptr], sizeof(*frame_buf)*(frame_wrt_ptr-frame_read_ptr));
		frame_wrt_ptr -= frame_read_ptr;
		frame_read_ptr = 0;
		
    }
	
	if (is_writing) {
		fclose(fp);
        printf("\tUtterance %04d = %d samples (%.1fsec)\n\n",
                uttno, uttlen, (double) uttlen / (double) sample_rate);
	}
	
    ad_stop_rec(ad);
    ad_close(ad);
    if (prespch_buf)
        ckd_free_2d((void **)prespch_buf);
	if (frame)
	    ckd_free((void*)frame);
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
