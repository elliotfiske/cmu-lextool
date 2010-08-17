/* ====================================================================
 * Copyright (c) 1999-2001 Carnegie Mellon University.  All rights
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
 * cont_ad.c -- Continuous A/D listening and silence filtering module.
 * 
 * HISTORY
 * 
 * 23-Oct-98	M K Ravishankar (rkm@cs.cmu.edu) at Carnegie Mellon University
 * 		Small change in the way the noiselevel is updated in find_thresh().
 * 
 * 26-Aug-98	M K Ravishankar (rkm@cs.cmu.edu) at Carnegie Mellon University
 * 		Separated computation of "frame power" into a separate low-level
 * 		function.
 * 
 * 13-Jul-98	M K Ravishankar (rkm@cs.cmu.edu) at Carnegie Mellon University
 * 		Modified to allow frame size to depend on audio sampling rate.
 * 
 * 01-Jul-98	M K Ravishankar (rkm@cs.cmu.edu) at Carnegie Mellon University
 * 		Changed CONT_AD_DELTA_SPEECH back to 20.
 * 
 * 30-Jun-98	M K Ravishankar (rkm@cs.cmu.edu) at Carnegie Mellon University
 * 		Changed CONT_AD_DELTA_SPEECH from 10 to 15.
 * 		Added FILE* argument to cont_ad_powhist_dump().
 * 
 * 19-Jun-98	M K Ravishankar (rkm@cs.cmu.edu) at Carnegie Mellon University
 * 		Changed CONT_AD_DELTA_SPEECH from 20 to 10, to increase sensitivity
 * 		to very short utterances.
 * 
 * 16-Jan-98	Paul Placeway (pwp@cs.cmu.edu) at Carnegie Mellon University
 * 		Changed to use dB instead of the weird power measure.
 * 		Changed analysis window size, tuned default settings of most
 * 		parameters to make the system less sensitive to noise, changed
 * 		the histogram update frequency and decay to make the system
 * 		adapt more rapidly to changes in the environment.
 * 		Added cont_ad_set_params() and cont_ad_get_params().
 * 
 * 28-Jul-96	M K Ravishankar (rkm@cs.cmu.edu) at Carnegie Mellon University
 * 		Added FRMPOW2SIGLVL, max_siglvl(), and cont_ad_t.siglvl.
 * 		Changed min signal energy/frame to CONT_AD_SPF.
 * 
 * 27-Jun-96	M K Ravishankar (rkm@cs.cmu.edu) at Carnegie Mellon University
 * 		Added the option for cont_ad_read to return -1 on EOF.
 * 
 * 21-Jun-96	M K Ravishankar (rkm@cs.cmu.edu) at Carnegie Mellon University
 * 		Added cont_ad_set_thresh().
 * 		Bugfix: n_other is recomputed after updating thresholds.
 * 
 * 20-Jun-96	M K Ravishankar (rkm@cs.cmu.edu) at Carnegie Mellon University
 * 		Separated thresholds for speech and silence.
 * 		Fixed bug in moving analysis window upon transition to speech state.
 * 
 * 17-Jun-96	M K Ravishankar (rkm@cs.cmu.edu) at Carnegie Mellon University
 * 		Created, based loosely on Steve Reed's original implementation.
 */

/*
 * This module is intended to be interposed as a filter between any raw A/D source and the
 * application to remove silence regions.  It is initialized with a raw A/D source function
 * (during the cont_ad_init call).  Filtered A/D data can be read by the application using
 * the cont_ad_read function.  This module assumes that the A/D source function supplies an
 * endless stream of data.  The application is responsible for setting up the A/D source,
 * turning recording on and off as it desires.  It is also responsible for invoking the
 * cont_ad_read function frequently enough to avoid buffer overruns and dropping A/D data.
 * This continuous listening module has an internal buffer of about 4 sec.
 * 
 * This module must be initialized and calibrated at first (cont_ad_init and cont_ad_calib
 * functions).  Raw samples are grouped into frames, the signal power in each frame is
 * computed and accumulated in a histogram.  The module is always in one of two states:
 * SILENCE or SPEECH.  Transitions between the two states are detected by looking for a
 * contiguous window of several frames that is predominantly of the other type.  The type
 * is determined by comparing frame power to either of two thresholds, thresh_sil and
 * thresh_speech, as appropriate for the current state.  These thresholds are set from the
 * first peak in the low-end of the power histogram, and are updated every few seconds.
 * Separate thresholds are used to provide some hysteresis.
 * 
 * The module maintains a linked list of speech (non-silence) segments not yet read by the
 * application.  The cont_ad_read function returns speech data, if any available, by
 * following this list.  It also updates an "absolute" timestamp at the end of the
 * cont_ad_read operation.  The timestamp indicates the total #samples of A/D data read
 * until this point, including data discarded as silence frames.  The application is
 * responsible for using this timestamp to make any policy decisions regarding utterance
 * boundaries or whatever.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include <s3types.h>
#include "ad.h"
#include "cont_ad.h"
#include <err.h>


#ifndef _ABS
#define _ABS(x) ((x) >= 0 ? (x) : -(x))
#endif


/* Various parameters, including defaults for many cont_ad_t member variables */

#define CONT_AD_ADFRMSIZE	256	/* #Frames of internal A/D buffer maintained */

#define CONT_AD_POWHISTSIZE	98	/* #Powhist bins: ~ FRMPOW(65536^2*CONT_AD_SPF) */
/* Maximum level is 96.3 dB full-scale; 97 for safety, plus 1 for zero-based */

#define CONT_AD_THRESH_UPDATE	100	/* Update thresholds approx every so many frames */
	/* PWP: update was 200 frames, or 3.2 seconds.  Now about every 1.6 sec. */

#define CONT_AD_ADAPT_RATE	0.2	/* Interpolation of new and old noiselevel */

#define CONT_AD_SPS             16000

#define CONT_AD_DEFAULT_NOISE	30	/* Default background noise power level */
#define CONT_AD_DELTA_SIL	10	/* Initial default for cont_ad_t.delta_sil */
#define CONT_AD_DELTA_SPEECH	17	/* Initial default for cont_ad_t.delta_speech */
#define CONT_AD_MIN_NOISE	2	/* Expected minimum background noise level */
#define CONT_AD_MAX_NOISE	70	/* Maximum background noise level */

#define CONT_AD_HIST_INERTIA	3	/* Used in decaying the power histogram */

#define CONT_AD_WINSIZE		21	/* Analysis window for state transitions */
				/* rkm had 16 */

#define CONT_AD_SPEECH_ONSET	9	/* Min #speech frames in analysis window for
					   SILENCE -> SPEECH state transition */
/*
 * SReed had 100 ms == 6.25 fr contiguous; rkm had 9 (out of 16+10) with a
 * lower threshold.
 */

#define CONT_AD_SIL_ONSET	18	/* Min #silence frames in analysis window for
					   SPEECH -> SILENCE state transition
					   MUST BE <= CONT_AD_WINSIZE */
/*
 * SReed had 400 ms == 25 fr contiguous; rkm had 14 out of 16
 */

#define CONT_AD_LEADER		5	/* On transition to SPEECH state, so many frames
					   BEFORE window included in speech data (>0) */
				/* SReed had 200 ms == 12.5 fr; rkm had 5 */

#define CONT_AD_TRAILER		10	/* On transition to SILENCE state, so many frames
					   of silence included in speech data (>0).
					   NOTE: Ensure (0 < TRAILER+LEADER <= WINSIZE) */
				/* SReed had 100 ms == 6.25 fr; rkm had 10 */


void cont_ad_powhist_dump (FILE *fp, cont_ad_t *r)
{
    int32 i, j;
    
    fprintf (fp, "PowHist:\n");
    for (i = 0, j = 0; i < CONT_AD_POWHISTSIZE; i++) {
      if (r->pow_hist[i] > 0) {
	fprintf (fp, "\t%3d %6d\n", i, r->pow_hist[i]);
	j = i;
      }
    }
    
    fprintf (fp, "PH[%7.2f]:", (double)(r->tot_frm * r->spf)/(double)(r->sps));
    for (i = 0; i <= j; i++)
      fprintf (fp, " %2d", r->pow_hist[i]);
    fprintf (fp, "\n");
    
    fflush (fp);
}


/*
 * Compute frame power.  Interface deliberately kept low level to allow arbitrary
 * users to call this function with appropriate data.
 */
int32 cont_ad_frame_pow (int16 *buf, int32 *prev, int32 spf)
{
    double sumsq, v;
    int32 i;
    int32 p;
    
    sumsq = 0.0;
    p = *prev;
    for (i = 0; i < spf; i++) {
	v = (double) (buf[i] - p);
	sumsq += v*v;
	p = buf[i];
    }
    *prev = p;
    
    if (sumsq < spf)	/* Make sure FRMPOW(sumsq) >= 0 */
	sumsq = spf;

    /*
     * PWP: Units changed to dB
     *
     * Now the units of measurement of an input sample are volts (really!),
     * so the power in dB is p = 20*log10(samp).  Further, we want the RMS
     * (root-mean-squared) average power across the frame.
     *
     * "sumsq" is the sum of the sum of the squares, so we want
     *
     *   i = 20 * log10( sqrt ( sumsq / n_samps) )
     *
     * (Stephen Reed's code actually had 
     *    i = 20 * log10( sqrt (sumsq) / n_samps )
     *  but this only produced an additive error.)
     *
     * i = 20 * log10( sqrt ( sumsq / n_samps) )
     *   = 20 * log10( ( sumsq / n_samps) ^ 0.5 )
     *   = 20 * log10( ( sumsq / n_samps) ) * 0.5 )
     *   = 10 * log10( ( sumsq / n_samps) )
     *   = 10 * ( log10( sumsq) - log10(n_samps) )
     */
    i = (int32) ((10.0 * (log10(sumsq) - log10((double) spf))) + 0.5);
    if (i < 0) i = 0;		/* trim lower bound again to be safe. */
    assert (i < 97);

    return (i);
}


/*
 * Classify frame (id=frm, starting at sample position s) as sil/nonsil.  Classification
 * done in isolation, independent of any other frame, based only on power histogram.
 */
static void compute_frame_pow (cont_ad_t *r, int32 frm)
{
    int32 i;
    
    i = cont_ad_frame_pow (r->adbuf + (frm * r->spf), &(r->prev_sample), r->spf);

    r->frm_pow[frm] = (char) i;
    (r->pow_hist[i])++;
    r->thresh_update--;
}


/* PWP: $$$ check this */
/*
 * PWP: in SReed's code, decay was done by zeroing the histogram,
 * i.e. no history.
 */
static void decay_hist (cont_ad_t *r)
{
    int32 i;
    
    for (i = 0; i < CONT_AD_POWHISTSIZE; i++)
	r->pow_hist[i] -= (r->pow_hist[i] >> CONT_AD_HIST_INERTIA);
}


/*
 * Find silence threshold from power histogram.
 */
static int32 find_thresh (cont_ad_t *r)
{
    int32 i, j, max, th;
    int32 old_noise_level, old_thresh_sil, old_thresh_speech;

    if (!r->auto_thresh)
      return 0;

    /*
     * Find smallest non-zero histogram entry, but starting at some minimum power.
     * Power lower than CONT_AD_MIN_NOISE indicates bad A/D input (eg, mic off...).
     * Too high a minimum power is also bad.
     */
    for (i = r->min_noise; (i < CONT_AD_POWHISTSIZE) && (r->pow_hist[i] == 0); i++);
    if (i > r->max_noise)	/* Bad signal? */
	return -1;

    /* PWP: Hmmmmm.... SReed's code looks over the lower 20 dB */
    /* PWP: 1/14/98  Made to work like Stephen Reed's code */

    /* This method of detecting the noise level is VERY unsatisfactory */
    max = 0;
    for (j = i, th = i;
	 (j < CONT_AD_POWHISTSIZE) && (j < i+20); j++) { /* PWP: was i+6, which was 9 dB */
	if (max < r->pow_hist[j]) {
	    max = r->pow_hist[j];
	    th = j;
	}
    }

    /* "Don't change the threshold too fast" */
    old_noise_level = r->noise_level;
    old_thresh_sil = r->thresh_sil;
    old_thresh_speech = r->thresh_speech;
#if 0
    if ( _ABS(r->noise_level - th) >= 10 ) {
	if (th > r->noise_level)
	    r->noise_level += ((th - r->noise_level) / 2);
	else
	    r->noise_level -= ((r->noise_level - th) / 2);
    } else {
	r->noise_level = th;
    }
#else
    /*
     * RKM: The above is odd; if (diff >= 10) += diff/2; else += diff??
     * This is discontinuous.  Instead, adapt based on the adapt_rate parameter.
     */
    /* r->noise_level = (int32) (th * r->adapt_rate + r->noise_level * (1.0 - r->adapt_rate)); */
    r->noise_level = (int32) (r->noise_level + r->adapt_rate * (th - r->noise_level) + 0.5);
#endif

    /* update thresholds */
    r->thresh_sil = r->noise_level + r->delta_sil;
    r->thresh_speech = r->noise_level + r->delta_speech;

    if (r->logfp) {
      fprintf (r->logfp, "%7.2fs %8df: NoisePeak: %d, Noiselevel: %d -> %d, Th-Sil: %d -> %d, Th-Sp: %d -> %d\n",
	       (double)(r->tot_frm * r->spf)/(double)(r->sps), r->tot_frm,
	       th,
	       old_noise_level, r->noise_level,
	       old_thresh_sil, r->thresh_sil, old_thresh_speech, r->thresh_speech);
      
      cont_ad_powhist_dump (r->logfp, r);
      
      fflush (r->logfp);
    }
    
    /*
     * PWP: in SReed's original, he cleared the histogram here.
     * I can't fathom why.
     */

    return 0;
}


/*
 * Main silence/speech region detection routine.  If currently in
 * SILENCE state, switch to SPEECH state if a window (r->winsize)
 * of frames is mostly non-silence.  If in SPEECH state, switch to
 * SILENCE state if the window is mostly silence.
 */
static void boundary_detect (cont_ad_t *r, int32 frm)
{
    spseg_t *seg;
    int32 f;
    
    assert (r->n_other >= 0);

    r->win_validfrm++;
    if (r->tail_state == CONT_AD_STATE_SIL) {
	if (r->frm_pow[frm] >= r->thresh_speech)
	    r->n_other++;
    } else {
        if (r->frm_pow[frm] <= r->thresh_sil)
	    r->n_other++;
    }
    
    if (r->logfp) {
      fprintf (r->logfp, "%7.2fs %8d[%3d]f: P: %2d, N: %2d, T+: %2d, T-: %2d, #O: %2d, %s\n",
	       (double)(r->tot_frm * r->spf)/(double)(r->sps),
	       r->tot_frm, frm,
	       r->frm_pow[frm],
	       r->noise_level, r->thresh_speech, r->thresh_sil,
	       r->n_other,
	       (r->tail_state == CONT_AD_STATE_SIL) ? "--" : "Sp");
    }
    
    if (r->win_validfrm < r->winsize)	/* Not reached full analysis window size */
	return;
    assert (r->win_validfrm == r->winsize);

    if (r->tail_state == CONT_AD_STATE_SIL) {	/* Currently in SILENCE state */
	if (r->n_frm >= r->winsize + r->leader) {
	    if (r->n_other >= r->speech_onset) {
		/* Speech detected; create speech segment description */
		seg = malloc (sizeof(*seg));
		
		seg->startfrm = r->win_startfrm - r->leader;
		if (seg->startfrm < 0)
		    seg->startfrm += CONT_AD_ADFRMSIZE;
		seg->nfrm = r->leader + r->winsize;
		seg->next = NULL;

		if (! r->spseg_head)
		    r->spseg_head = seg;
		else
		    r->spseg_tail->next = seg;
		r->spseg_tail = seg;
		
		r->tail_state = CONT_AD_STATE_SPEECH;
		
		if (r->logfp) {
		  int32 n;
		  
		  /* Where (in absolute time) this speech segment starts */
		  n = frm - seg->startfrm;
		  if (n < 0)
		    n += CONT_AD_ADFRMSIZE;
		  n = r->tot_frm - n - 1;
		  
		  fprintf (r->logfp, "%7.2fs %8d[%3d]f: Sil -> Sp detect; seg start: %7.2fs %8d\n",
			   (double)(r->tot_frm * r->spf)/(double)(r->sps),
			   r->tot_frm, frm,
			   (double)(n * r->spf)/(double)(r->sps), n);
		}
		
		/* Now in SPEECH state; want to look for silence from end of this window */
		r->win_validfrm = 1;
		r->win_startfrm = frm;

		/* Count #sil frames remaining in reduced window (of 1 frame) */
		r->n_other = (r->frm_pow[frm] <= r->thresh_sil) ? 1 : 0;
	    }
	}
    } else {
	if (r->n_other >= r->sil_onset) {
	    /* End of speech detected; speech->sil transition */
	    r->spseg_tail->nfrm += r->trailer;
	    
	    r->tail_state = CONT_AD_STATE_SIL;
		
	    if (r->logfp) {
	      int32 n;
	      
	      /* Where (in absolute time) this speech segment ends */
	      n = r->spseg_tail->startfrm + r->spseg_tail->nfrm - 1;
	      if (n >= CONT_AD_ADFRMSIZE)
		n -= CONT_AD_ADFRMSIZE;
	      n = frm - n;
	      if (n < 0)
		n += CONT_AD_ADFRMSIZE;
	      n = r->tot_frm - n;
	      
	      fprintf (r->logfp, "%7.2fs %8d[%3d]f: Sp -> Sil detect; seg end: %7.2fs %8d\n",
		       (double)(r->tot_frm * r->spf)/(double)(r->sps),
		       r->tot_frm, frm,
		       (double)(n * r->spf)/(double)(r->sps), n);
	    }
	    
	    /* Now in SILENCE state; start looking for speech trailer+leader frames later */
	    r->win_validfrm -= (r->trailer + r->leader - 1);
	    r->win_startfrm += (r->trailer + r->leader - 1);
	    if (r->win_startfrm >= CONT_AD_ADFRMSIZE)
		r->win_startfrm -= CONT_AD_ADFRMSIZE;
	    
	    /* Count #speech frames remaining in reduced window */
	    r->n_other = 0;
	    for (f = r->win_startfrm;; ) {
		if (r->frm_pow[f] >= r->thresh_speech)
		    r->n_other++;
		
		if (f == frm)
		    break;
		
		f++;
		if (f >= CONT_AD_ADFRMSIZE)
		    f = 0;
	    }
	} else {
	    /* In speech state, and staying there; add this frame to segment */
	    r->spseg_tail->nfrm++;
	}
    }
    
    /*
     * Get rid of oldest frame in analysis window.  Not quite correct;
     * thresholds could have changed over the window; should preserve
     * the original speech/silence label for the frame and undo it.  Later..
     */
    if (r->tail_state == CONT_AD_STATE_SIL) {
	if (r->frm_pow[r->win_startfrm] >= r->thresh_speech) {
	  if (r->n_other > 0)
	    r->n_other--;
	}
    } else {
	if (r->frm_pow[r->win_startfrm] <= r->thresh_sil) {
	  if (r->n_other > 0)
	    r->n_other--;
	}
    }
    r->win_validfrm--;
    r->win_startfrm++;
    if (r->win_startfrm >= CONT_AD_ADFRMSIZE)
	r->win_startfrm = 0;
    
    if (r->logfp)
      fflush (r->logfp);
}


static int32 max_siglvl (cont_ad_t *r, int32 startfrm, int32 nfrm)
{
    int32 siglvl, i, f;

    siglvl = 0;
    if (nfrm > 0) {
	for (i = 0, f = startfrm; i < nfrm; i++, f++) {
	    if (f >= CONT_AD_ADFRMSIZE)
		f -= CONT_AD_ADFRMSIZE;
	    if (r->frm_pow[f] > siglvl)
		siglvl = r->frm_pow[f];
	}
    }
    return siglvl;
}


#if 0
/*
 * RKM(2005/01/31): Where did this come from?  If needed, it should be called
 * cont_ad_get_audio_data.
 */
void get_audio_data(cont_ad_t *r, int16 *buf, int32 max) {
}
#endif


static void cont_ad_read_log (cont_ad_t *r, int32 retval)
{
  spseg_t *seg;
  
  fprintf (r->logfp, "return from cont_ad_read() -> %d:\n", retval);
  fprintf (r->logfp, "\tstate: %d\n", r->state);
  fprintf (r->logfp, "\tread_ts: %d (%.2fs)\n",
	   r->read_ts, (float32)r->read_ts / (float32)r->sps);
  fprintf (r->logfp, "\tseglen: %d (%.2fs)\n",
	   r->seglen, (float32)r->seglen / (float32)r->sps);
  fprintf (r->logfp, "\tsiglvl: %d\n", r->siglvl);
  fprintf (r->logfp, "\theadfrm: %d\n", r->headfrm);
  fprintf (r->logfp, "\tn_frm: %d\n", r->n_frm);
  fprintf (r->logfp, "\tn_sample: %d\n", r->n_sample);
  fprintf (r->logfp, "\twin_startfrm: %d\n", r->win_startfrm);
  fprintf (r->logfp, "\twin_validfrm: %d\n", r->win_validfrm);
  fprintf (r->logfp, "\tnoise_level: %d\n", r->noise_level);
  fprintf (r->logfp, "\tthresh_sil: %d\n", r->thresh_sil);
  fprintf (r->logfp, "\tthresh_speech: %d\n", r->thresh_speech);
  fprintf (r->logfp, "\tn_other: %d\n", r->n_other);
  fprintf (r->logfp, "\ttail_state: %d\n", r->tail_state);
  fprintf (r->logfp, "\ttot_frm: %d\n", r->tot_frm);
  
  fprintf (r->logfp, "\tspseg:");
  for (seg = r->spseg_head; seg; seg = seg->next)
    fprintf (r->logfp, " %d[%d]", seg->startfrm, seg->nfrm);
  fprintf (r->logfp, "\n");
  
  fflush (r->logfp);
}


/*
 * Copy data from r->adbuf[sf], for nf frames, into buf.
 * All length checks must have been completed before this call; hence, this
 * function will copy exactly the specified number of frames.
 * 
 * Return value: Index of frame just after the segment copied, possibly wrapped
 * around to 0.
 */
static int32 buf_copy (cont_ad_t *r, int32 sf, int32 nf, int16 *buf)
{
  int32 f, l;
  
  assert ((sf >= 0) && (sf < CONT_AD_ADFRMSIZE));
  assert (nf >= 0);
  
  if (sf + nf > CONT_AD_ADFRMSIZE) {
    /* Amount to be copied wraps around adbuf; copy in two stages */
    f = CONT_AD_ADFRMSIZE - sf;
    l = (f * r->spf);
    memcpy (buf, r->adbuf + (sf * r->spf), l * sizeof(int16));
    
    if (r->logfp) {
      fprintf (r->logfp, "return %d speech frames [%d..%d]; %d samples\n",
	       f, sf, sf + f - 1, l);
    }
    
    buf += l;
    sf = 0;
    nf -= f;
  }

  if (nf > 0) {
    l = (nf * r->spf);
    memcpy (buf, r->adbuf + (sf * r->spf), l * sizeof(int16));
    
    if (r->logfp) {
      fprintf (r->logfp, "return %d speech frames [%d..%d]; %d samples\n",
	       nf, sf, sf + nf - 1, l);
    }
  }
  
  if ((sf+nf) >= CONT_AD_ADFRMSIZE) {
    assert ((sf+nf) == CONT_AD_ADFRMSIZE);
    return 0;
  } else
    return (sf+nf);
}


/*
 * Main function called by the application to filter out silence regions.
 * Maintains a linked list of speech segments pointing into r->adbuf and feeds
 * data to application from them.
 */
int32 cont_ad_read (cont_ad_t *r, int16 *buf, int32 max)
{
    int32 head, tail, tailfrm, flen, len, retval, newstate;
    int32 i, f, l;
    spseg_t *seg;
    int num_to_copy = 0, num_left = max;
    
    if ((r == NULL) || (buf == NULL))
      return -1;
    
    if (max < r->spf) {
      E_ERROR("cont_ad_read requires buffer of at least %d samples\n", r->spf);
      return -1;
    }
    
    if (r->logfp) {
      fprintf (r->logfp, "cont_ad_read(,, %d)\n", max);
      fflush (r->logfp);
    }
    
    /*
     * First read as much of raw A/D as possible and available.  adbuf is not
     * really a circular buffer, so may have to read in two steps for wrapping
     * around.
     */
    head = r->headfrm * r->spf;
    tail = head + r->n_sample;
    len = r->n_sample - (r->n_frm * r->spf);	/* #partial frame samples at the tail */
    assert ((len >= 0) && (len < r->spf));
    
    if ((tail < r->adbufsize) && (! r->eof)) {
      if (r->adfunc != NULL) {
	if ((l = (*(r->adfunc))(r->ad, r->adbuf+tail, r->adbufsize - tail)) < 0) {
	  r->eof = 1;
	  l = 0;
	}
      } else {
	/*
	 * RKM(2005/02/01): Why would r->adfunc == NULL?  Where would this object
	 * get its raw data from if r->adfunc == NULL?  Why is data getting copied
	 * into r->adbuf from buf?  Where is this behavior documented?
	 * If max >> num_to_copy, the 2nd memcpy below can have overlapping src and
	 * dst regions, which isn't allowed by memcpy.
	 * This block of code doesn't make sense.
	 */
	num_to_copy = r->adbufsize - tail;
	num_left -= num_to_copy;
	if (num_to_copy > max) {
	  num_to_copy = max;
	  num_left = 0;
	}
	memcpy(r->adbuf+tail, buf, num_to_copy*sizeof(int16));
	memcpy(buf, buf+num_to_copy, num_left*sizeof(int16));
	l = num_to_copy;
      }
      
      if ((l > 0) && r->rawfp) {
	fwrite (r->adbuf+tail, sizeof(int16), l, r->rawfp);
	fflush (r->rawfp);
      }
      
      tail += l;
      len += l;
      r->n_sample += l;
    }
    if ((tail >= r->adbufsize) && (! r->eof)) {
      tail -= r->adbufsize;
      if (tail < head) {
	if (r->adfunc != NULL) {
	  if ((l = (*(r->adfunc))(r->ad, r->adbuf+tail, head - tail)) < 0) {
	    r->eof = 1;
	    l = 0;
	  }
	} else {
	  /* RKM(2005/02/01): See comment above */
	  num_to_copy = head-tail;
	  if (num_to_copy > num_left)
	    num_to_copy = num_left;
	  memcpy(r->adbuf+tail, buf, num_to_copy*sizeof(int16));
	  l = num_to_copy;
	}
	
	if ((l > 0) && r->rawfp) {
	  fwrite (r->adbuf+tail, sizeof(int16), l, r->rawfp);
	  fflush (r->rawfp);
	}
	
	tail += l;
	len += l;
	r->n_sample += l;
      }
    }  

    /* Compute frame power for unprocessed+new data and find speech/silence boundaries */
    tailfrm = (r->headfrm + r->n_frm);	/* Next free frame slot to be filled */
    if (tailfrm >= CONT_AD_ADFRMSIZE)
	tailfrm -= CONT_AD_ADFRMSIZE;
    for (; len >= r->spf; len -= r->spf) {
	compute_frame_pow (r, tailfrm);
	r->n_frm++;
	r->tot_frm++;
	
	/*
	 * Find speech/sil state change, if any.  Also, if staying in speech state
	 * add this frame to current speech segment.
	 */
	boundary_detect (r, tailfrm);
	
	if (++tailfrm >= CONT_AD_ADFRMSIZE)
	    tailfrm = 0;

	/* RKM: 2004-06-23: Moved this block from outside the enclosing loop */
	/* Update thresholds if time to do so */
	if (r->thresh_update <= 0) {
	  find_thresh (r);
	  decay_hist (r);
	  r->thresh_update = CONT_AD_THRESH_UPDATE;
	  
#if 1
	  /*
	   * Since threshold has been updated, recompute r->n_other.
	   * (RKM: Is this really necessary?  Comment out??)
	   */
	  r->n_other = 0;
	  if (r->tail_state == CONT_AD_STATE_SIL) {
	    for (i = r->win_validfrm, f = r->win_startfrm; i > 0; --i) {
	      if (r->frm_pow[f] >= r->thresh_speech)
		r->n_other++;
	      
	      f++;
	      if (f >= CONT_AD_ADFRMSIZE)
		f = 0;
	    }
	  } else {
	    for (i = r->win_validfrm, f = r->win_startfrm; i > 0; --i) {
	      if (r->frm_pow[f] <= r->thresh_sil)
		r->n_other++;
	      
	      f++;
	      if (f >= CONT_AD_ADFRMSIZE)
		f = 0;
	    }
	  }
#endif
	}
    }
    
    /*
     * If eof on input data source, cleanup the final segment.
     */
    if (r->eof) {
      if (r->tail_state == CONT_AD_STATE_SPEECH) {
	/*
	 * Still inside a speech segment when input data got over.  Absort any
	 * remaining frames into the final speech segment.
	 */
	assert (r->spseg_tail != NULL);
	
	/* Absorb frames still in analysis window into final speech seg */
	assert ((r->win_validfrm >= 0) && (r->win_validfrm < r->winsize));
	r->spseg_tail->nfrm += r->win_validfrm;
	
	r->tail_state = CONT_AD_STATE_SIL;
      }
      
      r->win_startfrm += r->win_validfrm;
      if (r->win_startfrm >= CONT_AD_ADFRMSIZE)
	r->win_startfrm -= CONT_AD_ADFRMSIZE;
      r->win_validfrm = 0;
      r->n_other = 0;
    }
    
    /*
     * At last ready to copy speech data, if any, into caller's buffer.  Raw
     * speech data is segmented into alternating speech and silence segments.
     * But any single call to cont_ad_read will never cross a speech/silence
     * boundary.
     */
    seg = r->spseg_head;	/* first speech segment available, if any */
    
    if ((seg == NULL) || (r->headfrm != seg->startfrm)) {
        /*
	 * Either no speech data available, or inside a silence segment.  Find
	 * length of silence segment.
	 */
	if (seg == NULL) {
	    assert (r->tail_state == CONT_AD_STATE_SIL);
	    
	    flen = (r->eof) ? r->n_frm : r->n_frm - (r->winsize + r->leader - 1);
	    if (flen < 0)
	      flen = 0;
	} else {
	    flen = seg->startfrm - r->headfrm;
	    if (flen < 0)
		flen += CONT_AD_ADFRMSIZE;
	}
	
	if (r->rawmode) {
	  /* Restrict silence segment to user buffer size, integral #frames */
	  f = max / r->spf;
	  if (flen > f)
	    flen = f;
	}
	
	newstate = CONT_AD_STATE_SIL;
    } else {
        flen = max / r->spf;	/* truncate read-size to integral #frames */
	if (flen > seg->nfrm)
	    flen = seg->nfrm;	/* truncate further to this segment size */
	
	newstate = CONT_AD_STATE_SPEECH;
    }
    
    len = flen * r->spf;	/* #samples being consumed */
    
    r->siglvl = max_siglvl (r, r->headfrm, flen);
    
    if ((newstate == CONT_AD_STATE_SIL) && (! r->rawmode)) {
      /* Skip silence data */
      r->headfrm += flen;
      if (r->headfrm >= CONT_AD_ADFRMSIZE)
	r->headfrm -= CONT_AD_ADFRMSIZE;
      
      retval = 0;		/* #samples being copied/returned */
    } else {
      /* Copy speech/silence(in rawmode) data */
      r->headfrm = buf_copy (r, r->headfrm, flen, buf);
      
      retval = len;		/* #samples being copied/returned */
    }
    
    r->n_frm -= flen;
    r->n_sample -= len;
    assert ((r->n_frm >= 0) && (r->n_sample >= 0));
    assert (r->win_validfrm <= r->n_frm);
    
    if (r->state == newstate)
      r->seglen += len;
    else
      r->seglen = len;
    r->state = newstate;
    
    if (newstate == CONT_AD_STATE_SPEECH) {
      seg->startfrm = r->headfrm;
      seg->nfrm -= flen;
      
      /* Free seg if empty and not recording into it */
      if ((seg->nfrm == 0) && (seg->next || (r->tail_state == CONT_AD_STATE_SIL))) {
	r->spseg_head = seg->next;
	if (seg->next == NULL)
	  r->spseg_tail = NULL;
	free (seg);
      }
    }
    
    /* Update timestamp.  Total raw A/D read - those remaining to be consumed */
    r->read_ts = (r->tot_frm - r->n_frm) * r->spf;
    
    if (retval == 0)
	retval = (r->eof && (r->spseg_head == NULL)) ? -1 : 0;
    
    if (r->logfp)
      cont_ad_read_log (r, retval);
    
    return retval;
}


/*
 * Calibrate input channel for silence threshold.
 */
int32 cont_ad_calib (cont_ad_t *r)
{
    int32 i, f, s, k, len, tailfrm;

    if (r == NULL)
      return -1;
    
    /* clear histogram */
    for (i = 0; i < CONT_AD_POWHISTSIZE; i++)
	r->pow_hist[i] = 0;
    tailfrm = r->headfrm + r->n_frm;
    if (tailfrm >= CONT_AD_ADFRMSIZE)
	tailfrm -= CONT_AD_ADFRMSIZE;
    s = (tailfrm * r->spf);
    
    for (f = 0; f < (CONT_AD_POWHISTSIZE<<1); f++) {
	len = r->spf;
	while (len > 0) {
	  /*Trouble */
	    if ((k = (*(r->adfunc))(r->ad, r->adbuf+s, len)) < 0)
		return -1;
	    len -= k;
	    s += k;
	}
	s -= r->spf;

	compute_frame_pow (r, tailfrm);
    }

    r->thresh_update = CONT_AD_THRESH_UPDATE;
    
    return (find_thresh (r));
}


int32 cont_ad_calib_loop (cont_ad_t *r, int16 *buf, int32 max)
{
    int32 i, s, len, tailfrm;
    static int32 finished = 1;
    static int32 f = 0;

    if (finished) {
      finished = 0;
      f = 0;

      /* clear histogram */
      for (i = 0; i < CONT_AD_POWHISTSIZE; i++)
	r->pow_hist[i] = 0;
      
    }

    tailfrm = r->headfrm + r->n_frm;
    if (tailfrm >= CONT_AD_ADFRMSIZE)
      tailfrm -= CONT_AD_ADFRMSIZE;
    s = (tailfrm * r->spf);
    
    len = r->spf;
    for (; f < (CONT_AD_POWHISTSIZE<<1); f++) {
      if (max < len)
	return 1;
      memcpy (r->adbuf+s, buf, len*sizeof(int16));
      max -= len;
      memcpy (buf, buf+len, max*sizeof(int16));
      
      compute_frame_pow (r, tailfrm);
    }
    
    finished = 1;
    return (find_thresh (r));
}


/* PWP 1/14/98 -- modified for compatibility with old code */
int32 cont_ad_set_thresh (cont_ad_t *r, int32 sil, int32 speech)
{
    if (r == NULL)
      return -1;
    
    if ((sil < 0) || (speech < 0)) {
	fprintf(stderr, "cont_ad_set_thresh: invalid threshold arguments: %d, %d\n",
		sil, speech);
	return -1;
    }
    r->delta_sil = (3 * sil) / 2;
    r->delta_speech = (3 * speech) / 2;
    
    return 0;
}


/*
 * PWP 1/14/98 -- set the changable params.
 *
 *   delta_sil, delta_speech, min_noise, and max_noise are in dB,
 *   winsize, speech_onset, sil_onset, leader and trailer are in frames of
 *   16 ms length (256 samples @ 16kHz sampling).
 */
int32 cont_ad_set_params (cont_ad_t *r, int32 delta_sil, int32 delta_speech,
			  int32 min_noise, int32 max_noise,
			  int32 winsize, int32 speech_onset, int32 sil_onset,
			  int32 leader, int32 trailer,
			  float32 adapt_rate)
{
    if ((delta_sil < 0) || (delta_speech < 0) || (min_noise < 0) || (max_noise < 0)) {
	E_ERROR("threshold arguments: " "%d, %d, %d, %d must all be >=0\n",
		delta_sil, delta_speech, min_noise, max_noise);
	return -1;
    }

    if ((speech_onset > winsize) || (speech_onset <= 0) || (winsize <= 0)) {
	E_ERROR("speech_onset, %d, must be <= winsize, %d, and both >0\n",
		speech_onset, winsize);
	return -1;
    }
    
    if ((sil_onset > winsize) || (sil_onset <= 0) || (winsize <= 0)) {
	E_ERROR("sil_onset, %d, must be <= winsize, %d, and both >0\n",
		sil_onset, winsize);
	return -1;
    }
    
    if (((leader + trailer) > winsize) || (leader <= 0) || (trailer <= 0)) {
	E_ERROR("leader, %d, plus trailer, %d, must be <= winsize, %d, and both >0\n",
		leader, trailer, winsize);
	return -1;
    }

    if ((adapt_rate < 0.0) || (adapt_rate > 1.0)) {
      E_ERROR("adapt_rate, %e; must be in range 0..1\n", adapt_rate);
      return -1;
    }
    
    if (r == NULL)
      return -1;
    
    r->delta_sil = delta_sil;
    r->delta_speech = delta_speech;
    r->min_noise = min_noise;
    r->max_noise = max_noise;

    r->winsize = winsize;
    r->speech_onset = speech_onset;
    r->sil_onset = sil_onset;
    r->leader = leader;
    r->trailer = trailer;
    
    r->adapt_rate = adapt_rate;
    
    if (r->win_validfrm >= r->winsize)
	r->win_validfrm = r->winsize - 1;

    return 0;
}


/*
 * PWP 1/14/98 -- get the changable params.
 *
 *   delta_sil, delta_speech, min_noise, and max_noise are in dB,
 *   winsize, speech_onset, sil_onset, leader and trailer are in frames of
 *   16 ms length (256 samples @ 16kHz sampling).
 */
int32 cont_ad_get_params (cont_ad_t *r, int32 *delta_sil, int32 *delta_speech,
			  int32 *min_noise, int32 *max_noise,
			  int32 *winsize, int32 *speech_onset, int32 *sil_onset,
			  int32 *leader, int32 *trailer,
			  float32 *adapt_rate)
{
    if (!delta_sil || !delta_speech || !min_noise || !max_noise || !winsize
	|| !speech_onset || !sil_onset || !leader || !trailer || !adapt_rate) {
	fprintf(stderr, "cont_ad_get_params: some param slots are NULL\n");
	return (-1);
    }

    if (r == NULL)
      return -1;
    
    *delta_sil = r->delta_sil;
    *delta_speech = r->delta_speech;
    *min_noise = r->min_noise;
    *max_noise = r->max_noise;

    *winsize = r->winsize;
    *speech_onset = r->speech_onset;
    *sil_onset = r->sil_onset;
    *leader = r->leader;
    *trailer = r->trailer;
    
    *adapt_rate = r->adapt_rate;
    
    return 0;
}


/*
 * Reset, discarded any accumulated speech.
 */
int32 cont_ad_reset (cont_ad_t *r)
{
    spseg_t *seg;
    
    if (r == NULL)
      return -1;
    
    while (r->spseg_head) {
	seg = r->spseg_head;
	r->spseg_head = seg->next;
	free (seg);
    }
    r->spseg_tail = NULL;
    
    r->headfrm = 0;
    r->n_frm = 0;
    r->n_sample = 0;
    r->win_startfrm = 0;
    r->win_validfrm = 0;
    r->n_other = 0;

    r->tail_state = CONT_AD_STATE_SIL;
    
    return 0;
}


int32 cont_ad_close (cont_ad_t *cont)
{
    if (cont == NULL)
      return -1;
    
    cont_ad_reset (cont);	/* Frees any remaining speech segments */
    
    free (cont->adbuf);
    free (cont->pow_hist);
    free (cont->frm_pow);
    free (cont);
    
    return 0;
}


int32 cont_ad_detach (cont_ad_t *c)
{
    if (c == NULL)
      return -1;
    
    c->ad = NULL;
    c->adfunc = NULL;
    return 0;
}


int32 cont_ad_attach (cont_ad_t *c, ad_rec_t *a, int32 (*func)(ad_rec_t *, int16 *, int32))
{
    if (c == NULL)
      return -1;
    
    c->ad = a;
    c->adfunc = func;
    c->eof = 0;
    
    return 0;
}


int32 cont_set_thresh(cont_ad_t *r, int32 silence, int32 speech) {
  int32 i, f;
  
  r->thresh_speech = speech;
  r->thresh_sil = silence;
  
  /* Since threshold has been updated, recompute r->n_other */
  r->n_other = 0;
  if (r->tail_state == CONT_AD_STATE_SIL) {
    for (i = r->win_validfrm, f = r->win_startfrm; i > 0; --i) {
      if (r->frm_pow[f] >= r->thresh_speech)
	r->n_other++;
      
      f++;
      if (f >= CONT_AD_ADFRMSIZE)
	f = 0;
    }
  } else if (r->tail_state == CONT_AD_STATE_SPEECH) {
    for (i = r->win_validfrm, f = r->win_startfrm; i > 0; --i) {
      if (r->frm_pow[f] <= r->thresh_sil)
	r->n_other++;
      
      f++;
      if (f >= CONT_AD_ADFRMSIZE)
	f = 0;
    }
  }
  
  return 0;
}


/*
 * Set the file pointer for dumping the raw input audio stream.
 */
int32 cont_ad_set_rawfp (cont_ad_t *r, FILE *fp)
{
    if (r == NULL)
      return -1;
    
    r->rawfp = fp;
    return 0;
}


/*
 * Set the file pointer for logging cont_ad progress.
 */
int32 cont_ad_set_logfp (cont_ad_t *r, FILE *fp)
{
    if (r == NULL)
      return -1;
    
    r->logfp = fp;
    return 0;
}


/*
 * One-time initialization.
 */
cont_ad_t *cont_ad_init (ad_rec_t *a, int32 (*func)(ad_rec_t *, int16 *, int32))
{
    cont_ad_t *r;
    
    if ((r = malloc (sizeof(*r))) == NULL) {
	perror("allocation of cont_ad_t failed");
	return NULL;
    }

    r->ad = a;
    r->adfunc = func;
    r->eof = 0;
    r->rawmode = 0;
    
    if (a != NULL) 
      r->sps = a->sps;
    else
      r->sps = CONT_AD_SPS;

    /* Set samples/frame such that when sps=16000, spf=256 */
    r->spf = (r->sps * 256) / CONT_AD_SPS;
    r->adbufsize = CONT_AD_ADFRMSIZE * r->spf;

    if ((r->adbuf = malloc (r->adbufsize * sizeof(*r->adbuf))) == NULL) {
	perror("allocation of audio buffer failed");
	free (r);
	return NULL;
    }
    if ((r->pow_hist = calloc (CONT_AD_POWHISTSIZE, sizeof(*r->pow_hist))) == NULL) {
	perror("allocation of power history buffer failed");
	free (r->adbuf);
	free (r);
	return NULL;
    }
    if ((r->frm_pow = calloc (CONT_AD_ADFRMSIZE, sizeof(*r->frm_pow))) == NULL) {
	perror("allocation of frame power buffer failed");
	free (r->pow_hist);
	free (r->adbuf);
	free (r);
	return NULL;
    }
    
    r->state = CONT_AD_STATE_SIL;
    r->read_ts = 0;
    r->seglen = 0;
    r->siglvl = 0;
    r->prev_sample = 0;
    r->tot_frm = 0;
    r->noise_level = CONT_AD_DEFAULT_NOISE;
    
    r->auto_thresh = 1;
    r->delta_sil = CONT_AD_DELTA_SIL;
    r->delta_speech = CONT_AD_DELTA_SPEECH;
    r->min_noise = CONT_AD_MIN_NOISE;
    r->max_noise = CONT_AD_MAX_NOISE;
    r->winsize = CONT_AD_WINSIZE;
    r->speech_onset = CONT_AD_SPEECH_ONSET;
    r->sil_onset = CONT_AD_SIL_ONSET;
    r->leader = CONT_AD_LEADER;
    r->trailer = CONT_AD_TRAILER;

    r->thresh_sil = r->noise_level + r->delta_sil;
    r->thresh_speech = r->noise_level + r->delta_speech;
    r->thresh_update = CONT_AD_THRESH_UPDATE;
    r->adapt_rate = CONT_AD_ADAPT_RATE;
    
    r->tail_state = CONT_AD_STATE_SIL;

    r->spseg_head = NULL;
    r->spseg_tail = NULL;
    
    r->rawfp = NULL;
    r->logfp = NULL;
    
    cont_ad_reset (r);
    
    return r;
}


cont_ad_t *cont_ad_init_rawmode (ad_rec_t *a,
				 int32 (*func)(ad_rec_t *, int16 *, int32))
{
    cont_ad_t *r;

    r = cont_ad_init (a, func);
    r->rawmode = 1;
    
    return r;
}
