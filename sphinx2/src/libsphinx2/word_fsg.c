/* ====================================================================
 * Copyright (c) 1999-2004 Carnegie Mellon University.  All rights
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
 * word_fsg.c -- Finite state LM handling
 * 
 * **********************************************
 * CMU ARPA Speech Project
 *
 * Copyright (c) 2003 Carnegie Mellon University.
 * ALL RIGHTS RESERVED.
 * **********************************************
 * 
 * HISTORY
 * 
 * $Log$
 * Revision 1.1  2004/07/16  00:57:12  egouvea
 * Added Ravi's implementation of FSG support.
 * 
 * Revision 1.6  2004/07/12 18:47:43  rkm
 * *** empty log message ***
 *
 * Revision 1.5  2004/06/21 18:16:12  rkm
 * Omitted noise words from FSG if noise penalty = 0
 *
 * Revision 1.4  2004/06/21 18:12:19  rkm
 * *** empty log message ***
 *
 * Revision 1.3  2004/06/21 18:09:17  rkm
 * *** empty log message ***
 *
 * Revision 1.2  2004/05/27 14:22:57  rkm
 * FSG cross-word triphones completed (but for single-phone words)
 *
 * Revision 1.1.1.1  2004/03/01 14:30:30  rkm
 *
 *
 * Revision 1.8  2004/02/27 19:33:01  rkm
 * *** empty log message ***
 *
 * Revision 1.7  2004/02/27 16:15:13  rkm
 * Added FSG switching
 *
 * Revision 1.6  2004/02/27 15:05:21  rkm
 * *** empty log message ***
 *
 * Revision 1.5  2004/02/26 15:35:50  rkm
 * *** empty log message ***
 *
 * Revision 1.4  2004/02/26 01:14:48  rkm
 * *** empty log message ***
 *
 * Revision 1.3  2004/02/25 15:08:19  rkm
 * *** empty log message ***
 *
 * Revision 1.2  2004/02/24 18:13:05  rkm
 * Added NULL transition handling
 *
 * Revision 1.1  2004/02/23 15:53:45  rkm
 * Renamed from fst to fsg
 *
 * Revision 1.6  2004/02/19 21:16:54  rkm
 * Added fsg_search.{c,h}
 *
 * Revision 1.5  2004/02/17 21:11:49  rkm
 * *** empty log message ***
 *
 * Revision 1.4  2004/02/16 21:10:10  rkm
 * *** empty log message ***
 *
 * Revision 1.3  2004/02/09 21:19:18  rkm
 * *** empty log message ***
 *
 * Revision 1.2  2004/02/09 17:30:20  rkm
 * *** empty log message ***
 *
 * Revision 1.1  2004/02/03 21:08:05  rkm
 * *** empty log message ***
 *
 * 
 * 13-Jan-2004	M K Ravishankar (rkm@cs.cmu.edu) at Carnegie Mellon
 * 		Started.
 */


#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <s2types.h>
#include <CM_macros.h>
#include <err.h>
#include <str2words.h>
#include <ckd_alloc.h>
#include <kb.h>
#include <phone.h>
#include <dict.h>
#include <log.h>
#include <word_fsg.h>


#define __FSG_DBG__		0


#define WORD_FSG_MAX_WORDPTR	4096

#define WORD_FSG_BEGIN_DECL		"FSG_BEGIN"
#define WORD_FST_BEGIN_DECL		"FST_BEGIN"
#define WORD_FSG_END_DECL		"FSG_END"
#define WORD_FST_END_DECL		"FST_END"
#define WORD_FSG_N_DECL			"N"
#define WORD_FSG_NUM_STATES_DECL	"NUM_STATES"
#define WORD_FSG_S_DECL			"S"
#define WORD_FSG_START_STATE_DECL	"START_STATE"
#define WORD_FSG_F_DECL			"F"
#define WORD_FSG_FINAL_STATE_DECL	"FINAL_STATE"
#define WORD_FSG_T_DECL			"T"
#define WORD_FSG_TRANSITION_DECL	"TRANSITION"
#define WORD_FSG_COMMENT_CHAR		'#'


static int32 nextline_str2words (FILE *fp, int32 *lineno,
				 char **wordptr, int32 max_ptr)
{
  char line[16384];
  int32 n;
  
  for (;;) {
    if (fgets (line, sizeof(line), fp) == NULL)
      return -1;
    
    (*lineno)++;
    
    if (line[0] != WORD_FSG_COMMENT_CHAR) {	/* Skip comment lines */
      if ((n = str2words(line, wordptr, max_ptr)) < 0)
	E_FATAL("Line[%d] too long\n", *lineno);
      
      if (n > 0)				/* Skip blank lines */
	break;
    }
  }
  
  return n;
}


/*
 * Add the given transition to the FSG transition matrix.  Duplicates (i.e.,
 * two transitions between the same states, with the same word label) are
 * flagged and only the highest prob retained.
 */
static void word_fsg_trans_add (word_fsg_t *fsg,
				int32 from, int32 to, int32 logp,
				int32 wid)
{
  word_fsglink_t *link;
  gnode_t *gn;
  
  /* Check for duplicate link (i.e., link already exists with label=wid) */
  for (gn = fsg->trans[from][to]; gn; gn = gnode_next(gn)) {
    link = (word_fsglink_t *) gnode_ptr(gn);
    
    if (link->wid == wid) {
#if 0
      E_WARN("Duplicate transition %d -> %d ('%s'); highest prob kept\n",
	     from, to, kb_get_word_str(wid));
#endif
      if (link->logs2prob < logp)
	link->logs2prob = logp;
      return;
    }
  }
  
  /* Create transition object */
  link = (word_fsglink_t *) ckd_calloc (1, sizeof(word_fsglink_t));
  link->from_state = from;
  link->to_state = to;
  link->logs2prob = logp;
  link->wid = wid;
  
  fsg->trans[from][to] = glist_add_ptr(fsg->trans[from][to], (void *)link);
}


/*
 * Link word_fsg_trans_add, but for a null transition between the given
 * states.  Also, there can be at most one null transition between the
 * given states; duplicates are flagged and only the best prob retained.
 * Transition probs must be <= 1 (i.e., logprob <= 0).
 * Return value: 1 if a new transition was added, 0 if the prob of an existing
 * transition was upgraded; -1 if nothing was changed.
 */
static int32 word_fsg_null_trans_add (word_fsg_t *fsg,
				     int32 from, int32 to, int32 logp)
{
  word_fsglink_t *link;
  
  /* Check for transition probability */
  if (logp > 0) {
    E_FATAL("Null transition prob must be <= 1.0 (state %d -> %d)\n",
	    from, to);
  }
  
  /* Self-loop null transitions (with prob <= 1.0) are redundant */
  if (from == to)
    return -1;
  
  /* Check for a duplicate link; if found, keep the higher prob */
  link = fsg->null_trans[from][to];
  if (link) {
    assert (link->wid < 0);
#if 0
    E_WARN("Duplicate null transition %d -> %d; highest prob kept\n",
	   from, to);
#endif
    if (link->logs2prob < logp) {
      link->logs2prob = logp;
      return 0;
    } else
      return -1;
  }
  
  /* Create null transition object */
  link = (word_fsglink_t *) ckd_calloc (1, sizeof(word_fsglink_t));
  link->from_state = from;
  link->to_state = to;
  link->logs2prob = logp;
  link->wid = -1;
  
  fsg->null_trans[from][to] = link;
  
  return 1;
}


/*
 * Obtain transitive closure of NULL transitions in the given FSG.  (Initial
 * list of such transitions is given.)
 * Return value: Updated list of null transitions.
 */
static glist_t word_fsg_null_trans_closure (word_fsg_t *fsg,
					    glist_t nulls)
{
  gnode_t *gn1, *gn2;
  boolean updated;
  word_fsglink_t *tl1, *tl2;
  int32 k, n;
  
  E_INFO("Computing transitive closure for null transitions\n");

  /*
   * Probably not the most efficient closure implementation, in general, but
   * probably reasonably efficient for a sparse null transition matrix.
   */
  n = 0;
  do {
    updated = FALSE;
    
    for (gn1 = nulls; gn1; gn1 = gnode_next(gn1)) {
      tl1 = (word_fsglink_t *) gnode_ptr(gn1);
      assert (tl1->wid < 0);
      
      for (gn2 = nulls; gn2; gn2 = gnode_next(gn2)) {
	tl2 = (word_fsglink_t *) gnode_ptr(gn2);
	
	if (tl1->to_state == tl2->from_state) {
	  k = word_fsg_null_trans_add (fsg,
				       tl1->from_state,
				       tl2->to_state,
				       tl1->logs2prob + tl2->logs2prob);
	  if (k >= 0) {
	    updated = TRUE;
	    if (k > 0) {
	      nulls = glist_add_ptr (nulls, (void *) fsg->null_trans[tl1->from_state][tl2->to_state]);
	      n++;
	    }
	  }
	}
      }
    }
  } while (updated);
  
  E_INFO("%d null transitions added\n", n);
  
  return nulls;
}


/*
 * Add silence and noise filler words to the FSG.
 * Return the number of transitions added..
 */
static int32 word_fsg_add_filler (word_fsg_t *fsg,
				  float32 silprob, float32 fillprob)
{
  dictT *dict;
  int32 src;
  int32 wid, silwid, n_word;
  int32 n_trans;
  int32 logsilp, logfillp;
  
  E_INFO("Adding filler words to FSG\n");
  
  assert (fsg);
  
  dict = kb_get_word_dict();
  silwid = kb_get_silence_word_id();
  n_word = kb_get_num_words();

  logsilp = (int32) (LOG(silprob) * fsg->lw);
  logfillp = (int32) (LOG(fillprob) * fsg->lw);
  
  /*
   * Add silence and filler word self-loop transitions to each state.
   * NOTE: No check to see if these words already exist in FSG!
   */
  n_trans = 0;
  if (silwid >= 0) {
    for (src = 0; src < fsg->n_state; src++) {
      word_fsg_trans_add (fsg, src, src, logsilp, silwid);
      n_trans++;
      
      if (fillprob > 0.0) {
	/* Add other filler (noise) words */
	for (wid = silwid+1; wid < n_word; wid++) {
	  word_fsg_trans_add (fsg, src, src, logfillp, wid);
	  n_trans++;
	}
      }
    }
  }

  return n_trans;
}


/*
 * Compute the left and right context CIphone sets for each state.
 * (Needed for building the phone HMM net using cross-word triphones.  Invoke
 * after computing null transitions closure.)
 */
static void word_fsg_lc_rc (word_fsg_t *fsg)
{
  int32 s, d, i, j;
  int32 n_ci;
  gnode_t *gn;
  word_fsglink_t *l;
  int32 silcipid;
  int32 endwid;
  dictT *dict;
  int32 len;
  
  dict = kb_get_word_dict();
  silcipid = kb_get_silence_ciphone_id();
  endwid = kb_get_word_id (kb_get_lm_end_sym());
  n_ci = phoneCiCount();
  if (n_ci > 127) {
    E_FATAL("#phones(%d) > 127; cannot use int8** for word_fsg_t.{lc,rc}\n",
	    n_ci);
  }

  /*
   * fsg->lc[s] = set of left context CIphones for state s.  Similarly, rc[s]
   * for right context CIphones.
   */
  fsg->lc = (int8 **) ckd_calloc_2d (fsg->n_state, n_ci+1, sizeof(int8));
  fsg->rc = (int8 **) ckd_calloc_2d (fsg->n_state, n_ci+1, sizeof(int8));
  
  for (s = 0; s < fsg->n_state; s++) {
    for (d = 0; d < fsg->n_state; d++) {
      for (gn = fsg->trans[s][d]; gn; gn = gnode_next(gn)) {
	l = (word_fsglink_t *) gnode_ptr(gn);
	assert (l->wid >= 0);
	
	/*
	 * Add the first CIphone of l->wid to the rclist of state s, and
	 * the last CIphone to lclist of state d.
	 * (Filler phones are a pain to deal with.  There is no direct
	 * marking of a filler phone; but only filler words are supposed to
	 * use such phones, so we use that fact.  HACK!!  FRAGILE!!)
	 */
	if (dict_is_filler_word(dict, l->wid) || (l->wid == endwid)) {
	  /* Filler phone; use silence phone as context */
	  fsg->rc[s][silcipid] = 1;
	  fsg->lc[d][silcipid] = 1;
	} else {
	  len = dict_pronlen(dict, l->wid);
	  fsg->rc[s][dict_ciphone(dict, l->wid, 0)] = 1;
	  fsg->lc[d][dict_ciphone(dict, l->wid, len-1)] = 1;
	}
      }
    }
  }

  /*
   * Add SIL phone to the lclist of start state and rclist of final state.
   */
  fsg->lc[fsg->start_state][silcipid] = 1;
  fsg->rc[fsg->final_state][silcipid] = 1;
  
  /*
   * Propagate lc and rc lists past null transitions.  (Since FSG contains
   * null transitions closure, no need to worry about a chain of successive
   * null transitions.  Right??)
   */
  for (s = 0; s < fsg->n_state; s++) {
    for (d = 0; d < fsg->n_state; d++) {
      l = fsg->null_trans[s][d];
      if (l) {
	/*
	 * lclist(d) |= lclist(s), because all the words ending up at s, can
	 * now also end at d, becoming the left context for words leaving d.
	 */
	for (i = 0; i < n_ci; i++)
	  fsg->lc[d][i] |= fsg->lc[s][i];
	/*
	 * Similarly, rclist(s) |= rclist(d), because all the words leaving d
	 * can equivalently leave s, becoming the right context for words
	 * ending up at s.
	 */
	for (i = 0; i < n_ci; i++)
	  fsg->rc[s][i] |= fsg->rc[d][i];
      }
    }
  }
  
  /* Convert the bit-vector representation into a list */
  for (s = 0; s < fsg->n_state; s++) {
    j = 0;
    for (i = 0; i < n_ci; i++) {
      if (fsg->lc[s][i]) {
	fsg->lc[s][j] = i;
	j++;
      }
    }
    fsg->lc[s][j] = -1;	/* Terminate the list */

    j = 0;
    for (i = 0; i < n_ci; i++) {
      if (fsg->rc[s][i]) {
	fsg->rc[s][j] = i;
	j++;
      }
    }
    fsg->rc[s][j] = -1;	/* Terminate the list */
  }
}


word_fsg_t *word_fsg_read (FILE *fp,
			   boolean use_altpron, boolean use_filler,
			   float32 silprob, float32 fillprob,
			   float32 lw)
{
  word_fsg_t *word_fsg;
  char *wordptr[WORD_FSG_MAX_WORDPTR];	/* ptrs to words in an input line */
  int32 lineno;
  int32 n, i, j;
  int32 n_trans, n_null_trans, n_alt_trans, n_filler_trans, n_unk;
  float32 p;
  dictT *dict;
  int32 wid;
  int32 logp;
  glist_t nulls;
  
  if (lw <= 0.0)
    E_WARN("Unusual language-weight value: %.3e\n", lw);

  dict = kb_get_word_dict();
  
  lineno = 0;
  
  /* Scan upto FSG_BEGIN header */
  for (;;) {
    n = nextline_str2words(fp, &lineno, wordptr, WORD_FSG_MAX_WORDPTR);
    if (n < 0) {
      E_ERROR("%s declaration missing\n", WORD_FSG_BEGIN_DECL);
      return NULL;
    }
    
    if ((strcmp (wordptr[0], WORD_FSG_BEGIN_DECL) == 0) ||
	(strcmp (wordptr[0], WORD_FST_BEGIN_DECL) == 0)) {
      if (n > 2) {
	E_ERROR("Line[%d]: malformed FSM BEGIN delcaration\n", lineno);
	return NULL;
      }
      break;
    }
  }
  
  /* FSG_BEGIN found */
  word_fsg = (word_fsg_t *) ckd_calloc (1, sizeof(word_fsg_t));
  word_fsg->name = ckd_salloc((n == 2) ? wordptr[1] : "");
  word_fsg->n_state = 0;
  word_fsg->use_altpron = use_altpron;
  word_fsg->use_filler = use_filler;
  word_fsg->lw = lw;
  word_fsg->trans = NULL;
  word_fsg->null_trans = NULL;
  word_fsg->lc = NULL;
  word_fsg->rc = NULL;
  
  /* Read #states */
  n = nextline_str2words(fp, &lineno, wordptr, WORD_FSG_MAX_WORDPTR);
  if ((n != 2) ||
      ((strcmp (wordptr[0], WORD_FSG_N_DECL) != 0) &&
       (strcmp (wordptr[0], WORD_FSG_NUM_STATES_DECL) != 0)) ||
      (sscanf (wordptr[1], "%d", &(word_fsg->n_state)) != 1) ||
      (word_fsg->n_state <= 0)) {
    E_ERROR("Line[%d]: #states declaration line missing or malformed\n",
	    lineno);
    goto parse_error;
  }

  /* Read start state */
  n = nextline_str2words(fp, &lineno, wordptr, WORD_FSG_MAX_WORDPTR);
  if ((n != 2) ||
      ((strcmp (wordptr[0], WORD_FSG_S_DECL) != 0) &&
       (strcmp (wordptr[0], WORD_FSG_START_STATE_DECL) != 0)) ||
      (sscanf (wordptr[1], "%d", &(word_fsg->start_state)) != 1) ||
      (word_fsg->start_state < 0) ||
      (word_fsg->start_state >= word_fsg->n_state)) {
    E_ERROR("Line[%d]: start state declaration line missing or malformed\n",
	    lineno);
    goto parse_error;
  }
  
  /* Read final state */
  n = nextline_str2words(fp, &lineno, wordptr, WORD_FSG_MAX_WORDPTR);
  if ((n != 2) ||
      ((strcmp (wordptr[0], WORD_FSG_F_DECL) != 0) &&
       (strcmp (wordptr[0], WORD_FSG_FINAL_STATE_DECL) != 0)) ||
      (sscanf (wordptr[1], "%d", &(word_fsg->final_state)) != 1) ||
      (word_fsg->final_state < 0) ||
      (word_fsg->final_state >= word_fsg->n_state)) {
    E_ERROR("Line[%d]: final state declaration line missing or malformed\n",
	    lineno);
    goto parse_error;
  }
  
  /* Allocate non-epsilon transition matrix array */
  word_fsg->trans = (glist_t **) ckd_calloc_2d (word_fsg->n_state,
						word_fsg->n_state,
						sizeof(glist_t));
  /* Allocate epsilon transition matrix array */
  word_fsg->null_trans = (word_fsglink_t ***)
    ckd_calloc_2d(word_fsg->n_state, word_fsg->n_state,
		  sizeof(word_fsglink_t *));
  
  /* Read transitions */
  n_null_trans = 0;
  n_alt_trans = 0;
  n_filler_trans = 0;
  n_unk = 0;
  nulls = NULL;
  for (n_trans = 0;; n_trans++) {
    n = nextline_str2words(fp, &lineno, wordptr, WORD_FSG_MAX_WORDPTR);
    if (n <= 0) {
      E_ERROR("Line[%d]: transition or FSM END declaration line expected\n",
	      lineno);
      goto parse_error;
    }
    
    if ((strcmp (wordptr[0], WORD_FSG_END_DECL) == 0) ||
	(strcmp (wordptr[0], WORD_FST_END_DECL) == 0)) {
      break;
    }
    
    if ((strcmp (wordptr[0], WORD_FSG_T_DECL) == 0) ||
	(strcmp (wordptr[0], WORD_FSG_TRANSITION_DECL) == 0)) {
      if (((n != 4) && (n != 5)) ||
	  (sscanf (wordptr[1], "%d", &i) != 1) ||
	  (sscanf (wordptr[2], "%d", &j) != 1) ||
	  (sscanf (wordptr[3], "%f", &p) != 1) ||
	  (i < 0) || (i >= word_fsg->n_state) ||
	  (j < 0) || (j >= word_fsg->n_state) ||
	  (p <= 0.0) || (p > 1.0)) {
	E_ERROR("Line[%d]: transition spec malformed; Expecting: from-state to-state trans-prob [word]\n", lineno);
	goto parse_error;
      }
    } else {
      E_ERROR("Line[%d]: transition or FSM END declaration line expected\n",
	      lineno);
      goto parse_error;
    }
    
    /* Convert prob to logs2prob and apply language weight */
    logp = (int32) (LOG(p) * lw);
    
    /* Check if word is in dictionary */
    if (n > 4) {
      wid = kb_get_word_id(wordptr[4]);
      if (wid < 0) {
	E_ERROR("Line[%d]: Unknown word '%s'\n", lineno, wordptr[4]);
	n_unk++;
      } else if (use_altpron) {
	wid = dictid_to_baseid(dict, wid);
	assert (wid >= 0);
      }
    } else
      wid = -1;		/* Null transition */
    
    /* Add transition to word_fsg structure */
    if (wid < 0) {
      if (word_fsg_null_trans_add (word_fsg, i, j, logp) == 1) {
	n_null_trans++;
	nulls = glist_add_ptr (nulls, (void *) word_fsg->null_trans[i][j]);
      }
    } else {
      word_fsg_trans_add (word_fsg, i, j, logp, wid);
      
      /* Add transitions for alternative pronunciations, if any */
      if (use_altpron) {
	for (wid = dict_next_alt(dict, wid);
	     wid >= 0;
	     wid = dict_next_alt(dict, wid)) {
	  word_fsg_trans_add (word_fsg, i, j, logp, wid);
	  n_alt_trans++;
	  n_trans++;
	}
      }
    }
  }
  
  /* Add silence and noise filler word transitions if specified */
  if (use_filler) {
    n_filler_trans = word_fsg_add_filler (word_fsg, silprob, fillprob);
    n_trans += n_filler_trans;
  }
  
  E_INFO("FSG: %d states, %d transitions (%d null, %d alt, %d filler,  %d unknown)\n",
	 word_fsg->n_state, n_trans,
	 n_null_trans, n_alt_trans, n_filler_trans, n_unk);
  
#if __FSG_DBG__
  E_INFO("FSG before NULL closure:\n");
  word_fsg_write (word_fsg, stdout);
#endif

  /* Null transitions closure */
  nulls = word_fsg_null_trans_closure (word_fsg, nulls);
  glist_free (nulls);
  
#if __FSG_DBG__
  E_INFO("FSG after NULL closure:\n");
  word_fsg_write (word_fsg, stdout);
#endif
  
  /* Compute left and right context CIphone lists for each state */
  word_fsg_lc_rc (word_fsg);
  
#if __FSG_DBG__
  E_INFO("FSG after lc/rc:\n");
  word_fsg_write (word_fsg, stdout);
#endif
  
  return word_fsg;
  
 parse_error:
  word_fsg_free (word_fsg);
  return NULL;
}


word_fsg_t *word_fsg_readfile (char *file,
			       boolean use_altpron, boolean use_filler,
			       float32 silprob, float32 fillprob,
			       float32 lw)
{
  FILE *fp;
  word_fsg_t *fsg;
  
  E_INFO("Reading FSG file '%s' (altpron=%d, filler=%d, lw=%.2f, silprob=%.2e, fillprob=%.2e)\n",
	 file, use_altpron, use_filler, lw, silprob, fillprob);
  
  if ((fp = fopen(file, "r")) == NULL) {
    E_ERROR("fopen(%s,r) failed\n", file);
    return NULL;
  }
  
  fsg = word_fsg_read (fp,
		       use_altpron, use_filler,
		       silprob, fillprob, lw);
  
  fclose (fp);
  
  return fsg;
}


void word_fsg_free (word_fsg_t *fsg)
{
  int32 i, j;
  gnode_t *gn;
  word_fsglink_t *tl;
  
  for (i = 0; i < fsg->n_state; i++) {
    for (j = 0; j < fsg->n_state; j++) {
      /* Free all non-null transitions between states i and j */
      for (gn = fsg->trans[i][j]; gn; gn = gnode_next(gn)) {
	tl = (word_fsglink_t *) gnode_ptr(gn);
	ckd_free((void *) tl);
      }
      
      glist_free(fsg->trans[i][j]);
      
      /* Free any null transition i->j */
      ckd_free ((void *) fsg->null_trans[i][j]);
    }
  }
  
  ckd_free_2d ((void **) fsg->trans);
  ckd_free_2d ((void **) fsg->null_trans);
  ckd_free ((void *) fsg->name);
  
  if (fsg->lc)
    ckd_free_2d ((void **) fsg->lc);
  if (fsg->rc)
    ckd_free_2d ((void **) fsg->rc);
  
  ckd_free ((void *) fsg);
}


void word_fsg_write (word_fsg_t *fsg, FILE *fp)
{
  time_t tp;
  int32 i, j;
  gnode_t *gn;
  word_fsglink_t *tl;
  
  assert (fsg);
  
  time(&tp);
  if (tp > 0)
    fprintf (fp, "%c WORD-FSG; %s\n", WORD_FSG_COMMENT_CHAR,
	     ctime(&tp));
  else
    fprintf (fp, "%c WORD-FSG\n", WORD_FSG_COMMENT_CHAR);
  fprintf (fp, "%s\n", WORD_FSG_BEGIN_DECL);
  
  fprintf (fp, "%c #states\n", WORD_FSG_COMMENT_CHAR);
  fprintf (fp, "%s %d\n", WORD_FSG_NUM_STATES_DECL, fsg->n_state);
  
  fprintf (fp, "%c start-state\n", WORD_FSG_COMMENT_CHAR);
  fprintf (fp, "%s %d\n", WORD_FSG_START_STATE_DECL, fsg->start_state);

  fprintf (fp, "%c final-state\n", WORD_FSG_COMMENT_CHAR);
  fprintf (fp, "%s %d\n", WORD_FSG_FINAL_STATE_DECL, fsg->final_state);
  
  fprintf (fp, "%c transitions\n", WORD_FSG_COMMENT_CHAR);
  fprintf (fp, "%c from-state to-state logs2prob*lw word-ID\n",
	   WORD_FSG_COMMENT_CHAR);
  for (i = 0; i < fsg->n_state; i++) {
    for (j = 0; j < fsg->n_state; j++) {
      /* Print non-null transitions */
      for (gn = fsg->trans[i][j]; gn; gn = gnode_next(gn)) {
	tl = (word_fsglink_t *) gnode_ptr(gn);
	
	fprintf (fp, "%c %d %d %d %d\n",
		 WORD_FSG_COMMENT_CHAR,
		 tl->from_state, tl->to_state, tl->logs2prob, tl->wid);
	fprintf (fp, "%s %d %d %.3e %s\n",
		 WORD_FSG_TRANSITION_DECL,
		 tl->from_state, tl->to_state,
		 EXP(tl->logs2prob / fsg->lw),
		 (tl->wid < 0) ? "" : kb_get_word_str(tl->wid));
      }
      
      /* Print null transitions */
      tl = fsg->null_trans[i][j];
      if (tl) {
	fprintf (fp, "%c %d %d %d\n",
		 WORD_FSG_COMMENT_CHAR,
		 tl->from_state, tl->to_state, tl->logs2prob);
	fprintf (fp, "%s %d %d %.3e\n",
		 WORD_FSG_TRANSITION_DECL,
		 tl->from_state, tl->to_state,
		 EXP(tl->logs2prob / fsg->lw));
      }
    }
  }
  
  /* Print lc/rc vectors */
  if (fsg->lc && fsg->rc) {
    for (i = 0; i < fsg->n_state; i++) {
      fprintf (fp, "%c LC[%d]:", WORD_FSG_COMMENT_CHAR, i);
      for (j = 0; fsg->lc[i][j] >= 0; j++)
	fprintf (fp, " %s", phone_from_id(fsg->lc[i][j]));
      fprintf (fp, "\n");
      
      fprintf (fp, "%c RC[%d]:", WORD_FSG_COMMENT_CHAR, i);
      for (j = 0; fsg->rc[i][j] >= 0; j++)
	fprintf (fp, " %s", phone_from_id(fsg->rc[i][j]));
      fprintf (fp, "\n");
    }
  }
  
  fprintf (fp, "%c\n", WORD_FSG_COMMENT_CHAR);
  fprintf (fp, "%s\n", WORD_FSG_END_DECL);
  
  fflush (fp);
}


void word_fsg_writefile (word_fsg_t *fsg, char *file)
{
  FILE *fp;
  
  assert (fsg);
  
  E_INFO("Writing FSG file '%s'\n", file);
  
  if ((fp = fopen(file, "w")) == NULL) {
    E_ERROR("fopen(%s,r) failed\n", file);
    return;
  }
  
  word_fsg_write (fsg, fp);
  
  fclose (fp);
}
