/* -*- c-basic-offset: 4; indent-tabs-mode: nil -*- */
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
 * word_fsg.h -- Word-level finite state graph
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
 * Revision 1.1  2006/04/05  20:27:30  dhdfu
 * A Great Reorganzation of header files and executables
 * 
 * Revision 1.2  2006/02/23 05:10:18  arthchan2003
 * Merged from branch SPHINX3_5_2_RCI_IRII_BRANCH: Adaptation of Sphinx 2's FSG search into Sphinx 3
 *
 * Revision 1.1.2.5  2005/07/20 21:15:56  arthchan2003
 * A FSG in Sphinx 2 can be read by word_fsg.c but there is memory leaks using word_fsg_readfile.  Sounds like related to str2word.
 *
 * Revision 1.1.2.4  2005/07/17 05:58:20  arthchan2003
 * Added context-table argument in word_fsg.h
 *
 * Revision 1.1.2.3  2005/07/13 18:39:48  arthchan2003
 * (For Fun) Remove the hmm_t hack. Consider each s2 global functions one-by-one and replace them by sphinx 3's macro.  There are 8 minor HACKs where functions need to be removed temporarily.  Also, there are three major hacks. 1,  there are no concept of "phone" in sphinx3 dict_t, there is only ciphone. That is to say we need to build it ourselves. 2, sphinx2 dict_t will be a bunch of left and right context tables.  This is currently bypass. 3, the fsg routine is using fsg_hmm_t which is just a duplication of CHAN_T in sphinx2, I will guess using hmm_evaluate should be a good replacement.  But I haven't figure it out yet.
 *
 * Revision 1.1.2.2  2005/06/28 07:01:21  arthchan2003
 * General fix of fsg routines to make a prototype of fsg_init and fsg_read. Not completed.  The number of empty functions in fsg_search is now decreased from 35 to 30.
 *
 * Revision 1.1.2.1  2005/06/27 05:26:29  arthchan2003
 * Sphinx 2 fsg mainpulation routines.  Compiled with faked functions.  Currently fended off from users.
 *
 * Revision 1.2  2004/07/23 23:36:34  egouvea
 * Ravi's merge, with the latest fixes in the FSG code, and making the log files generated by FSG, LM, and allphone have the same 'look and feel', with the backtrace information presented consistently
 *
 * Revision 1.6  2004/07/20 20:48:41  rkm
 * Added uttproc_load_fsg()
 *
 * Revision 1.5  2004/07/20 13:40:55  rkm
 * Added FSG get/set start/final state functions
 *
 * Revision 1.1  2004/07/16 00:57:12  egouvea
 * Added Ravi's implementation of FSG support.
 *
 * Revision 1.4  2004/07/15 20:51:07  rkm
 * *** empty log message ***
 *
 * Revision 1.3  2004/06/21 18:14:20  rkm
 * Omitted noise words from FSG if noise penalty = 0
 *
 * Revision 1.2  2004/05/27 14:22:57  rkm
 * FSG cross-word triphones completed (but for single-phone words)
 *
 * Revision 1.1.1.1  2004/03/01 14:30:31  rkm
 *
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
 * Revision 1.1  2004/02/23 15:53:46  rkm
 * Renamed from fst to fsg
 *
 * Revision 1.6  2004/02/19 21:16:54  rkm
 * Added fsg_search.{c,h}
 *
 * Revision 1.5  2004/02/16 21:10:10  rkm
 * *** empty log message ***
 *
 * Revision 1.4  2004/02/12 16:06:03  rkm
 * *** empty log message ***
 *
 * Revision 1.3  2004/02/09 21:19:22  rkm
 * *** empty log message ***
 *
 * Revision 1.2  2004/02/09 17:30:49  rkm
 * *** empty log message ***
 *
 * Revision 1.1  2004/02/03 21:08:05  rkm
 * *** empty log message ***
 *
 * 
 * 13-Dec-2004	M K Ravishankar (rkm@cs.cmu.edu) at Carnegie Mellon
 * 		Started.
 */


#ifndef __S2_WORD_FSG_H__
#define __S2_WORD_FSG_H__


#include <stdio.h>
#include <string.h>
#include <s3types.h>
#include <glist.h>
#include <fsg.h>
#include <kbcore.h>
#include <ctxt_table.h>

#if 0 
#include <fbs.h>
#endif


#ifdef __cplusplus
extern "C" {
#endif
#if 0
/* Fool Emacs. */
}
#endif

/*
 * A single transition in the FSG.
 */
typedef struct word_fsglink_s {
    int32 from_state;
    int32 to_state;
    s3wid_t wid;		/* Word-ID; <0 if epsilon or null transition */
    int32 logs2prob;	/* logs2(transition probability)*lw */
} word_fsglink_t;


/* Access macros */
#define word_fsglink_from_state(l)	((l)->from_state)
#define word_fsglink_to_state(l)	((l)->to_state)
#define word_fsglink_wid(l)		((l)->wid)
#define word_fsglink_logs2prob(l)	((l)->logs2prob)


/*
 * Word level FSG definition.
 * States are simply integers 0..n_state-1.
 * A transition emits a word and has a given probability of being taken.
 * There can also be null or epsilon transitions, with no associated emitted
 * word.
 */
typedef struct word_fsg_s {
    char *name;		/* A unique string identifier for this FSG */
    int32 n_state;	/* #states in FSG */
    int32 start_state;	/* Must be in the range [0..n_state-1] */
    int32 final_state;	/* Must be in the range [0..n_state-1] */
    boolean use_altpron;	/* Whether transitions for alternative pronunciations
				   have been added to the FSG */
    boolean use_filler;	/* Whether silence and noise filler-word transitions
                           have been added at each state */
    float32 lw;		/* Language weight that's been applied to transition
                           logprobs */
    glist_t **trans;	/* trans[i][j] = glist of non-epsilon transitions or
                           links (word_fsglink_t *) from state i to state j,
                           if any; NULL if none. */
    word_fsglink_t ***null_trans;	/* null_trans[i][j] = epsilon or null link
					   from state i to j, if any; NULL if none.
					   (At most one null transition between two
					   given states.) */

    /*
     * Left and right CIphone sets for each state.
     * Left context CIphones for a state S: If word W transitions into S, W's
     * final CIphone is in S's {lc}.  Words transitioning out of S must consider
     * these left context CIphones.
     * Similarly, right contexts for state S: If word W transitions out of S,
     * W's first CIphone is in S's {rc}.  Words transitioning into S must consider
     * these right contexts.
     * 
     * NOTE: Words may transition into and out of S INDIRECTLY, with intermediate
     *   null transitions.
     * NOTE: Single-phone words are difficult; only SILENCE right context is
     *   modelled for them.
     * NOTE: Non-silence filler phones aren't included in these sets.  Filler
     *   words don't use context, and present the SILENCE phone as context to
     *   adjacent words.
     */
    int8 **rc, **lc;

    /**Added by Arthur at 20050627*/
    int32 n_ciphone;
    dict_t* dict;
    mdef_t* mdef;
    tmat_t* tmat;
    ctxt_table_t *ctxt;
} word_fsg_t;

/* Access macros */
#define word_fsg_name(f)		((f)->name)
#define word_fsg_n_state(f)		((f)->n_state)
#define word_fsg_start_state(f)		((f)->start_state)
#define word_fsg_final_state(f)		((f)->final_state)
#define word_fsg_lw(f)			((f)->lw)
#define word_fsg_use_altpron(f)		((f)->use_altpron)
#define word_fsg_use_filler(f)		((f)->use_filler)
#define word_fsg_trans(f,i,j)		((f)->trans[i][j])
#define word_fsg_null_trans(f,i,j)	((f)->null_trans[i][j])
#define word_fsg_lc(f,s)		((f)->lc[s])
#define word_fsg_rc(f,s)		((f)->rc[s])


/*
 * Read a word FSG from the given file and return a pointer to the structure
 * created.  Return NULL if any error occurred.
 * 
 * File format:
 * 
 *   Any number of comment lines; ignored
 *   FSG_BEGIN [<fsgname>]
 *   N <#states>
 *   S <start-state ID>
 *   F <final-state ID>
 *   T <from-state> <to-state> <prob> [<word-string>]
 *   T ...
 *   ... (any number of state transitions)
 *   FSG_END
 *   Any number of comment lines; ignored
 * 
 * The FSG spec begins with the line containing the keyword FSG_BEGIN.
 * It has an optional fsg name string.  If not present, the FSG has the empty
 * string as its name.
 * 
 * Following the FSG_BEGIN declaration is the number of states, the start
 * state, and the final state, each on a separate line.  States are numbered
 * in the range [0 .. <numberofstate>-1].
 * 
 * These are followed by all the state transitions, each on a separate line,
 * and terminated by the FSG_END line.  A state transition has the given
 * probability of being taken, and emits the given word.  The word emission
 * is optional; if word-string omitted, it is an epsilon or null transition.
 * 
 * Comments can also be embedded within the FSG body proper (i.e. between
 * FSG_BEGIN and FSG_END): any line with a # character in col 1 is treated
 * as a comment line.
 * 
 * Other arguments:
 * - use_altpron: if TRUE, if a transition labelled W exists between two
 *   states, all the alternative pronunciations of W (in the dictionary) are
 *   also added as transitions with the same given probability.
 * - use_filler: if TRUE, a separate transition for each silence and noise
 *   filler word in the lexicon is added at each state (loop transition).
 *   However, if noiseword penalty is 0.0, noise words are ignored.
 * - silprob: transition prob for silence word transitions (if use_filler)
 * - fillprob: transition prob for noise word transitions (if use_filler)
 * - lw: language weight, applied as a multiplicative factor to the LOG of
 *   the transition probs.
 * 
 * Return value: a new word_fsg_t structure if the file is successfully
 * read, NULL otherwise.
 */
word_fsg_t *word_fsg_readfile (const char *file,
			       boolean use_altpron, boolean use_filler,
                               kbcore_t *kbc);


/*
 * Like word_fsg_readfile(), but from an already open stream.
 */
word_fsg_t *word_fsg_read (FILE *fp,
			   boolean use_altpron, boolean use_filler,
                           kbcore_t *kbc);


/*
 * Like word_fsg_read(), but from an in-memory structure.
 */
word_fsg_t *word_fsg_load (s2_fsg_t *s2_fsg,
			   boolean use_altpron, boolean use_filler,
                           kbcore_t *kbc);


/*
 * Write the given fsg structure to the given file.
 * (This needs options for writing in format readable by word_fsg_read().)
 */
void word_fsg_writefile (word_fsg_t *fsg, char *file);


/*
 * Like word_fsg_writefile(), but to an already open stream.
 */
void word_fsg_write (word_fsg_t *fsg, FILE *fp);


/*
 * Free the given word FSG
 */
void word_fsg_free(word_fsg_t *);


/*
 * Set the FSG start state (or the final state) to the given state.
 * Return value: -1 if there was any error, otherwise the previous start
 * state (or final state).
 */
int32 word_fsg_set_start_state (word_fsg_t *fsg, int32 new_start_state);
int32 word_fsg_set_final_state (word_fsg_t *fsg, int32 new_final_state);

#ifdef __cplusplus
}
#endif


#endif
