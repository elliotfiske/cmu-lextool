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
 * fsg_search.c -- Search structures for FSM decoding.
 * 
 * **********************************************
 * CMU ARPA Speech Project
 *
 * Copyright (c) 2004 Carnegie Mellon University.
 * ALL RIGHTS RESERVED.
 * **********************************************
 * 
 * HISTORY
 *
 * $Log: fsg_search.c,v $
 * Revision 1.2  2006/02/23 05:10:18  arthchan2003
 * Merged from branch SPHINX3_5_2_RCI_IRII_BRANCH: Adaptation of Sphinx 2's FSG search into Sphinx 3
 *
 * Revision 1.1.2.10  2006/01/16 18:20:46  arthchan2003
 * Remove junks in the code, change the reporting from printf to log_hypstr.
 *
 * Revision 1.1.2.9  2005/08/02 21:14:34  arthchan2003
 * Removed comments and change sen to senscr.
 *
 * Revision 1.1.2.8  2005/07/26 02:20:39  arthchan2003
 * merged hyp_t with srch_hyp_t.
 *
 * Revision 1.1.2.7  2005/07/24 19:34:46  arthchan2003
 * Removed search_hyp_t, used srch_hyp_t instead
 *
 * Revision 1.1.2.6  2005/07/24 01:34:54  arthchan2003
 * Mode 2 is basically running. Still need to fix function such as resulting and build the correct utterance ID
 *
 * Revision 1.1.2.5  2005/07/20 21:18:30  arthchan2003
 * FSG can now be read, srch_fsg_init can now be initialized, psubtree can be built. Sounds like it is time to plug in other function pointers.
 *
 * Revision 1.1.2.4  2005/07/17 05:44:32  arthchan2003
 * Added dag_write_header so that DAG header writer could be shared between 3.x and 3.0. However, because the backtrack pointer structure is different in 3.x and 3.0. The DAG writer still can't be shared yet.
 *
 * Revision 1.1.2.3  2005/07/13 18:39:48  arthchan2003
 * (For Fun) Remove the hmm_t hack. Consider each s2 global functions one-by-one and replace them by sphinx 3's macro.  There are 8 minor HACKs where functions need to be removed temporarily.  Also, there are three major hacks. 1,  there are no concept of "phone" in sphinx3 dict_t, there is only ciphone. That is to say we need to build it ourselves. 2, sphinx2 dict_t will be a bunch of left and right context tables.  This is currently bypass. 3, the fsg routine is using fsg_hmm_t which is just a duplication of CHAN_T in sphinx2, I will guess using hmm_evaluate should be a good replacement.  But I haven't figure it out yet.
 *
 * Revision 1.1.2.2  2005/06/28 07:01:20  arthchan2003
 * General fix of fsg routines to make a prototype of fsg_init and fsg_read. Not completed.  The number of empty functions in fsg_search is now decreased from 35 to 30.
 *
 * Revision 1.1.2.1  2005/06/27 05:26:29  arthchan2003
 * Sphinx 2 fsg mainpulation routines.  Compiled with faked functions.  Currently fended off from users.
 *
 * Revision 1.2  2004/07/23 23:36:34  egouvea
 * Ravi's merge, with the latest fixes in the FSG code, and making the log files generated by FSG, LM, and allphone have the same 'look and feel', with the backtrace information presented consistently
 *
 * Revision 1.16  2004/07/20 13:40:55  rkm
 * Added FSG get/set start/final state functions
 *
 * Revision 1.15  2004/07/16 19:55:28  rkm
 * Added state information to hypothesis, and added uttproc_get_fsg_final_state function
 * 
 * Revision 1.1  2004/07/16 00:57:11  egouvea
 * Added Ravi's implementation of FSG support.
 *
 * Revision 1.14  2004/07/07 22:30:35  rkm
 * *** empty log message ***
 *
 * Revision 1.13  2004/07/07 13:56:33  rkm
 * Added reporting of (acoustic score - best senone score)/frame
 *
 * Revision 1.12  2004/06/25 14:49:08  rkm
 * Optimized size of history table and speed of word transitions by maintaining only best scoring word exits at each state
 *
 * Revision 1.11  2004/06/24 14:52:58  rkm
 * *** empty log message ***
 *
 * Revision 1.10  2004/06/23 20:32:16  rkm
 * *** empty log message ***
 *
 * Revision 1.9  2004/06/22 15:36:12  rkm
 * Added partial result handling in FSG mode
 *
 * Revision 1.8  2004/06/21 20:01:23  rkm
 * Added #history entries to log
 *
 * Revision 1.7  2004/06/18 17:11:53  rkm
 * *** empty log message ***
 *
 * Revision 1.6  2004/06/16 19:25:29  rkm
 * *** empty log message ***
 *
 * Revision 1.5  2004/06/16 19:07:33  rkm
 * *** empty log message ***
 *
 * Revision 1.4  2004/06/16 18:45:54  rkm
 * *** empty log message ***
 *
 * Revision 1.3  2004/06/16 18:32:28  rkm
 * Minor logformat change
 *
 * Revision 1.2  2004/05/27 14:22:57  rkm
 * FSG cross-word triphones completed (but for single-phone words)
 *
 * Revision 1.12  2004/03/02 04:10:14  rkm
 * FSG bugfix: need to get senscores every utt
 *
 * Revision 1.11  2004/03/01 19:28:54  rkm
 * *** empty log message ***
 *
 * Revision 1.10  2004/02/27 21:01:25  rkm
 * Many bug fixes in multiple FSGs
 *
 * Revision 1.9  2004/02/27 17:01:01  rkm
 * *** empty log message ***
 *
 * Revision 1.8  2004/02/27 16:15:13  rkm
 * Added FSG switching
 *
 * Revision 1.7  2004/02/27 15:05:21  rkm
 * *** empty log message ***
 *
 * Revision 1.6  2004/02/26 15:35:50  rkm
 * *** empty log message ***
 *
 * Revision 1.5  2004/02/26 14:48:20  rkm
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
 * Revision 1.2  2004/02/23 15:09:50  rkm
 * *** empty log message ***
 *
 * Revision 1.1  2004/02/19 21:16:54  rkm
 * Added fsg_search.{c,h}
 *
 * 
 * 18-Feb-2004	M K Ravishankar (rkm@cs.cmu.edu) at Carnegie Mellon
 * 		Started.
 */


#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <err.h>
#include <ckd_alloc.h>
#include <fsg_search.h>
#include <kb.h>
#include <fsg.h>
#include <dict.h>
#include <logs3.h>

#include <srch.h>
#include <ascr.h>

#define FSG_SEARCH_IDLE		0
#define FSG_SEARCH_BUSY		1

/* Turn this on for detailed debugging dump */
#define __FSG_DBG__		0
#define __FSG_DBG_CHAN__	0


fsg_search_t *
fsg_search_init(word_fsg_t * fsg, void *srch)
{
    fsg_search_t *search;
    float32 lw;
    int32 pip, wip;
    srch_t *s;


    /* Now we do the dance for initializing search */

    s = (srch_t *) srch;

    search = (fsg_search_t *) ckd_calloc(1, sizeof(fsg_search_t));
    search->fsg = fsg;

    if (fsg) {
        search->fsglist = glist_add_ptr(NULL, (void *) fsg);
        search->lextree = fsg_lextree_init(fsg, search->hmmctx);
    }
    else {
        search->fsglist = NULL;
        search->lextree = NULL;
    }

    assert(s->kbc->mdef);

    /* Transfer all member variables */
    search->mdef = s->kbc->mdef;
    search->dict = s->kbc->dict;
    search->tmat = s->kbc->tmat;
    search->am_score_pool = s->ascr;

    search->hmmctx = hmm_context_init(mdef_n_emit_state(search->mdef),
				      search->tmat->tp, NULL,
				      search->mdef->sseq);

    search->n_ci_phone = mdef_n_ciphone(s->kbc->mdef);

    /* Intialize the search history object */
    search->history = fsg_history_init(fsg, search->n_ci_phone);

    /* Initialize the active lists */
    search->pnode_active = NULL;
    search->pnode_active_next = NULL;

    search->frame = -1;

    search->hyp = NULL;

    search->state = FSG_SEARCH_IDLE;

    search->beam = s->beam->hmm;
    search->pbeam = s->beam->ptrans;
    search->wbeam = s->beam->word;
    search->isUsealtpron = cmd_ln_int32("-fsgusealtpron");
    search->isUseFiller = cmd_ln_int32("-fsgusefiller");
    search->isBacktrace = cmd_ln_int32("-backtrace");
    search->matchfp = s->matchfp;
    search->matchsegfp = s->matchsegfp;
    search->senscale = s->ascale;

    lw = s->kbc->fillpen->lw;
    pip = (int32) (logs3(cmd_ln_float32("-phonepen")) * lw);
    wip = s->kbc->fillpen->wip;

    E_INFO("FSG(beam: %d, pbeam: %d, wbeam: %d; wip: %d, pip: %d)\n",
           search->beam, search->pbeam, search->wbeam, wip, pip);
    return search;
}


word_fsg_t *
fsg_search_fsgname_to_fsg(fsg_search_t * search, char *name)
{
    gnode_t *gn;
    word_fsg_t *fsg;

    for (gn = search->fsglist; gn; gn = gnode_next(gn)) {
        fsg = (word_fsg_t *) gnode_ptr(gn);
        if (strcmp(name, word_fsg_name(fsg)) == 0)
            return fsg;
    }

    return NULL;
}


boolean
fsg_search_add_fsg(fsg_search_t * search, word_fsg_t * fsg)
{
    word_fsg_t *oldfsg;

    /* Check to make sure search is in a quiescent state */
    if (search->state != FSG_SEARCH_IDLE) {
        E_ERROR("Attempt to switch FSG inside an utterance\n");
        return FALSE;
    }

    /* Make sure no existing FSG has the same name as the given one */
    oldfsg = fsg_search_fsgname_to_fsg(search, word_fsg_name(fsg));
    if (oldfsg) {
        E_ERROR("FSG name '%s' already exists\n", word_fsg_name(fsg));
        return FALSE;
    }

    search->fsglist = glist_add_ptr(search->fsglist, (void *) fsg);
    return TRUE;
}


boolean
fsg_search_del_fsg(fsg_search_t * search, word_fsg_t * fsg)
{
    gnode_t *gn, *prev, *next;
    word_fsg_t *oldfsg;

    /* Check to make sure search is in a quiescent state */
    if (search->state != FSG_SEARCH_IDLE) {
        E_ERROR("Attempt to switch FSG inside an utterance\n");
        return FALSE;
    }

    /* Search fsglist for the given fsg */
    prev = NULL;
    for (gn = search->fsglist; gn; gn = next) {
        oldfsg = (word_fsg_t *) gnode_ptr(gn);
        next = gnode_next(gn);
        if (oldfsg == fsg) {
            /* Found the FSG to be deleted; remove it from fsglist */
            if (prev)
                prev->next = next;
            else
                search->fsglist = next;

            myfree((char *) gn, sizeof(gnode_t));

            /* If this was the currently active FSG, also delete other stuff */
            if (search->fsg == fsg) {
                fsg_lextree_free(search->lextree);
                search->lextree = NULL;

                fsg_history_set_fsg(search->history, NULL);

                search->fsg = NULL;
            }

            E_INFO("Deleting FSG '%s'\n", word_fsg_name(fsg));

            word_fsg_free(fsg);

            return TRUE;
        }
        else
            prev = gn;
    }

    E_WARN("FSG '%s' to be deleted not found\n", word_fsg_name(fsg));

    return TRUE;
}


boolean
fsg_search_del_fsg_byname(fsg_search_t * search, char *name)
{
    word_fsg_t *fsg;

    fsg = fsg_search_fsgname_to_fsg(search, name);
    if (!fsg) {
        E_WARN("FSG name '%s' to be deleted not found\n", name);
        return TRUE;
    }
    else
        return fsg_search_del_fsg(search, fsg);
}


boolean
fsg_search_set_current_fsg(fsg_search_t * search, char *name)
{
    word_fsg_t *fsg;

    /* Check to make sure search is in a quiescent state */
    if (search->state != FSG_SEARCH_IDLE) {
        E_ERROR("Attempt to switch FSG inside an utterance\n");
        return FALSE;
    }

    fsg = fsg_search_fsgname_to_fsg(search, name);
    if (!fsg) {
        E_ERROR("FSG '%s' not known; cannot make it current\n", name);
        return FALSE;
    }

    /* Free the old lextree */
    if (search->lextree)
        fsg_lextree_free(search->lextree);

    /* Allocate new lextree for the given FSG */
    search->lextree = fsg_lextree_init(fsg, search->hmmctx);

    /* Inform the history module of the new fsg */
    fsg_history_set_fsg(search->history, fsg);

    search->fsg = fsg;
    return TRUE;
}


void
fsg_search_free(fsg_search_t * search)
{
    hmm_context_free(search->hmmctx);
}


static void
hmm_sen_active(hmm_t * hmm, ascr_t * a, mdef_t * m)
{
    int32 i;
    s3senid_t *senp;
    s3ssid_t ssid;
    senp = NULL;

    if (hmm_frame(hmm) > 0) {
	ssid = hmm_nonmpx_ssid(hmm);
        senp = m->sseq[ssid];

        for (i = 0; i < hmm_n_emit_state(hmm); i++) {
            /*Get the senone sequence id */
	    if (senp[i] == -1)
		continue;
            a->sen_active[senp[i]] = 1;
        }
    }
}

void
fsg_search_sen_active(fsg_search_t * search)
{
    gnode_t *gn;
    fsg_pnode_t *pnode;
    hmm_t *hmm;

    assert(search->am_score_pool);
    ascr_clear_sen_active(search->am_score_pool);

    for (gn = search->pnode_active; gn; gn = gnode_next(gn)) {
        pnode = (fsg_pnode_t *) gnode_ptr(gn);
        hmm = fsg_pnode_hmmptr(pnode);
        assert(hmm_frame(hmm) == search->frame);

        hmm_sen_active(hmm, search->am_score_pool, search->mdef);
    }
}



/*
 * Evaluate all the active HMMs.
 * (Executed once per frame.)
 */
void
fsg_search_hmm_eval(fsg_search_t * search)
{
    gnode_t *gn;
    fsg_pnode_t *pnode;
    hmm_t *hmm;
    int32 bestscore;
    int32 n;

    bestscore = (int32) 0x80000000;

    if (!search->pnode_active) {
        E_ERROR("Frame %d: No active HMM!!\n", search->frame);
        return;
    }

    hmm_context_set_senscore(search->hmmctx, search->am_score_pool->senscr);
    for (n = 0, gn = search->pnode_active; gn; gn = gnode_next(gn), n++) {
        pnode = (fsg_pnode_t *) gnode_ptr(gn);
        hmm = fsg_pnode_hmmptr(pnode);
        assert(hmm_frame(hmm) == search->frame);

	hmm_vit_eval(hmm);

        if (bestscore < hmm_bestscore(hmm))
            bestscore = hmm_bestscore(hmm);
    }

#if __FSG_DBG__
    E_INFO("[%5d] %6d HMM; bestscr: %11d\n", search->frame, n, bestscore);
#endif
    search->n_hmm_eval += n;

    if (n > fsg_lextree_n_pnode(search->lextree))
        E_FATAL("PANIC! Frame %d: #HMM evaluated(%d) > #PNodes(%d)\n",
                search->frame, n, fsg_lextree_n_pnode(search->lextree));

    search->bestscore = bestscore;
}


static void
fsg_search_pnode_trans(fsg_search_t * search, fsg_pnode_t * pnode)
{
    fsg_pnode_t *child;
    hmm_t *hmm;

    assert(pnode);
    assert(!fsg_pnode_leaf(pnode));
    hmm = fsg_pnode_hmmptr(pnode);

    for (child = fsg_pnode_succ(pnode);
         child; child = fsg_pnode_sibling(child)) {
        if (fsg_psubtree_pnode_enter(child,
                                     hmm_out_score(hmm),
                                     search->frame + 1,
				     hmm_out_history(hmm))) {
            search->pnode_active_next =
                glist_add_ptr(search->pnode_active_next, (void *) child);
        }
    }
}


static void
fsg_search_pnode_exit(fsg_search_t * search, fsg_pnode_t * pnode)
{
    hmm_t *hmm;
    word_fsglink_t *fl;
    dict_t *dict;
    int32 wid, endwid;
    fsg_pnode_ctxt_t ctxt;

    assert(pnode);
    assert(fsg_pnode_leaf(pnode));

    hmm = fsg_pnode_hmmptr(pnode);
    fl = fsg_pnode_fsglink(pnode);
    assert(fl);

    dict = search->dict;
    endwid = dict_basewid(dict, dict_finishwid(dict));

    wid = word_fsglink_wid(fl);
    assert(wid >= 0);

#if __FSG_DBG__
    E_INFO("[%5d] Exit(%08x) %10d(score) %5d(pred)\n",
           search->frame, (int32) pnode,
           hmm->score[search->n_state_hmm - 1],
           hmm->history[search->n_state_hmm - 1]);
#endif

    /*
     * Check if this is filler or single phone word; these do not model right
     * context (i.e., the exit score applies to all right contexts).
     */
    if (dict_filler_word(dict, wid) ||
        (wid == endwid) || (dict_pronlen(dict, wid) == 1)) {
        /* Create a dummy context structure that applies to all right contexts */
        fsg_pnode_add_all_ctxt(&ctxt);

        /* Create history table entry for this word exit */
        fsg_history_entry_add(search->history,
                              fl,
                              search->frame,
			      hmm_out_score(hmm),
			      hmm_out_history(hmm),
                              pnode->ci_ext, ctxt);

    }
    else {
        /* Create history table entry for this word exit */
        fsg_history_entry_add(search->history,
                              fl,
                              search->frame,
			      hmm_out_score(hmm),
			      hmm_out_history(hmm),
                              pnode->ci_ext, pnode->ctxt);
    }
}


/*
 * (Beam) prune the just evaluated HMMs, determine which ones remain
 * active, which ones transition to successors, which ones exit and
 * terminate in their respective destination FSM states.
 * (Executed once per frame.)
 */
void
fsg_search_hmm_prune_prop(fsg_search_t * search)
{
    gnode_t *gn;
    fsg_pnode_t *pnode;
    hmm_t *hmm;
    int32 thresh, word_thresh, phone_thresh;

    assert(search->pnode_active_next == NULL);

    thresh = search->bestscore + search->beam;
    phone_thresh = search->bestscore + search->pbeam;
    word_thresh = search->bestscore + search->wbeam;

    for (gn = search->pnode_active; gn; gn = gnode_next(gn)) {
        pnode = (fsg_pnode_t *) gnode_ptr(gn);
        hmm = fsg_pnode_hmmptr(pnode);

        if (hmm_bestscore(hmm) >= thresh) {
            /* Keep this HMM active in the next frame */
            if (hmm_frame(hmm) == search->frame) {
                hmm_frame(hmm) = search->frame + 1;
                search->pnode_active_next =
                    glist_add_ptr(search->pnode_active_next,
                                  (void *) pnode);
            }
            else {
                assert(hmm_frame(hmm) == search->frame + 1);
            }

            if (!fsg_pnode_leaf(pnode)) {
                if (hmm_out_score(hmm) >= phone_thresh) {
                    /* Transition out of this phone into its children */
                    fsg_search_pnode_trans(search, pnode);
                }
            }
            else {
                if (hmm_out_score(hmm) >= word_thresh) {
                    /* Transition out of leaf node into destination FSG state */
                    fsg_search_pnode_exit(search, pnode);
                }
            }
        }
    }
}


/*
 * Propagate newly created history entries through null transitions.
 */
static void
fsg_search_null_prop(fsg_search_t * search)
{
    int32 bpidx, n_entries, thresh, newscore;
    fsg_hist_entry_t *hist_entry;
    word_fsglink_t *l;
    int32 s, d;
    word_fsg_t *fsg;

    fsg = search->fsg;
    thresh = search->bestscore + search->wbeam; /* Which beam really?? */

    n_entries = fsg_history_n_entries(search->history);

    for (bpidx = search->bpidx_start; bpidx < n_entries; bpidx++) {
        hist_entry = fsg_history_entry_get(search->history, bpidx);

        l = fsg_hist_entry_fsglink(hist_entry);

        /* Destination FSG state for history entry */
        s = l ? word_fsglink_to_state(l) : word_fsg_start_state(fsg);

        /*
         * Check null transitions from d to all other states.  (Only need to
         * propagate one step, since FSG contains transitive closure of null
         * transitions.)
         */
        for (d = 0; d < word_fsg_n_state(fsg); d++) {
            l = word_fsg_null_trans(fsg, s, d);

            if (l) {            /* Propagate history entry through this null transition */
                newscore =
                    fsg_hist_entry_score(hist_entry) +
                    word_fsglink_logs2prob(l);

                if (newscore >= thresh) {
                    fsg_history_entry_add(search->history, l,
                                          fsg_hist_entry_frame(hist_entry),
                                          newscore,
                                          bpidx,
                                          fsg_hist_entry_lc(hist_entry),
                                          fsg_hist_entry_rc(hist_entry));
                }
            }
        }
    }
}


/*
 * Perform cross-word transitions; propagate each history entry created in this
 * frame to lextree roots attached to the target FSG state for that entry.
 */
static void
fsg_search_word_trans(fsg_search_t * search)
{
    int32 bpidx, n_entries;
    fsg_hist_entry_t *hist_entry;
    word_fsglink_t *l;
    int32 score, d;
    fsg_pnode_t *root;
    int32 lc, rc;

    n_entries = fsg_history_n_entries(search->history);

    for (bpidx = search->bpidx_start; bpidx < n_entries; bpidx++) {
        hist_entry = fsg_history_entry_get(search->history, bpidx);
        assert(hist_entry);
        score = fsg_hist_entry_score(hist_entry);
        assert(search->frame == fsg_hist_entry_frame(hist_entry));

        l = fsg_hist_entry_fsglink(hist_entry);

        /* Destination state for hist_entry */
        d = l ? word_fsglink_to_state(l) : word_fsg_start_state(search->
                                                                fsg);

        lc = fsg_hist_entry_lc(hist_entry);

        /* Transition to all root nodes attached to state d */
        for (root = fsg_lextree_root(search->lextree, d);
             root; root = root->sibling) {
            rc = root->ci_ext;

            if ((root->ctxt.bv[lc >> 5] & (1 << (lc & 0x001f))) &&
                (hist_entry->rc.bv[rc >> 5] & (1 << (rc & 0x001f)))) {
                /*
                 * Last CIphone of history entry is in left-context list supported by
                 * target root node, and
                 * first CIphone of target root node is in right context list supported
                 * by history entry;
                 * So the transition can go ahead.
                 */

                if (fsg_psubtree_pnode_enter
                    (root, score, search->frame + 1, bpidx)) {
                    /* Newly activated node; add to active list */
                    search->pnode_active_next =
                        glist_add_ptr(search->pnode_active_next,
                                      (void *) root);
#if __FSG_DBG__
                    E_INFO
                        ("[%5d] WordTrans bpidx[%d] -> pnode[%08x] (activated)\n",
                         search->frame, bpidx, (int32) root);
#endif
                }
                else {
#if __FSG_DBG__
                    E_INFO("[%5d] WordTrans bpidx[%d] -> pnode[%08x]\n",
                           search->frame, bpidx, (int32) root);
#endif
                }
            }
        }
    }
}


void
fsg_search_frame_fwd(fsg_search_t * search)
{
    gnode_t *gn;
    fsg_pnode_t *pnode;
    hmm_t *hmm;

    search->bpidx_start = fsg_history_n_entries(search->history);

    /* Evaluate all active pnodes (HMMs) */
    fsg_search_hmm_eval(search);

    /*
     * Prune and propagate the HMMs evaluated; create history entries for
     * word exits.  The words exits are tentative, and may be pruned; make
     * the survivors permanent via fsg_history_end_frame().
     */
    fsg_search_hmm_prune_prop(search);
    fsg_history_end_frame(search->history);

    /*
     * Propagate new history entries through any null transitions, creating
     * new history entries, and then make the survivors permanent.
     */
    fsg_search_null_prop(search);
    fsg_history_end_frame(search->history);

    /*
     * Perform cross-word transitions; propagate each history entry across its
     * terminating state to the root nodes of the lextree attached to the state.
     */
    fsg_search_word_trans(search);

    /*
     * We've now come full circle, HMM and FSG states have been updated for
     * the next frame.
     * Update the active lists, deactivate any currently active HMMs that
     * did not survive into the next frame
     */
    for (gn = search->pnode_active; gn; gn = gnode_next(gn)) {
        pnode = (fsg_pnode_t *) gnode_ptr(gn);
        hmm = fsg_pnode_hmmptr(pnode);

        if (hmm_frame(hmm) == search->frame) {
            /* This HMM NOT activated for the next frame; reset it */
            fsg_psubtree_pnode_deactivate(pnode);
        }
        else {
            assert(hmm_frame(hmm) == (search->frame + 1));
        }
    }

    /* Free the currently active list */
    glist_free(search->pnode_active);

    /* Make the next-frame active list the current one */
    search->pnode_active = search->pnode_active_next;
    search->pnode_active_next = NULL;

    /* End of this frame; ready for the next */
    (search->frame)++;
}


static void
fsg_search_hyp_free(fsg_search_t * search)
{
    srch_hyp_t *hyp, *nexthyp;

    for (hyp = search->hyp; hyp; hyp = nexthyp) {
        nexthyp = hyp->next;
        ckd_free(hyp);
    }
    search->hyp = NULL;
}


/*
 * Set all HMMs to inactive, clear active lists, initialize FSM start
 * state to be the only active node.
 * (Executed at the start of each utterance.)
 */
void
fsg_search_utt_start(fsg_search_t * search)
{
    int32 silcipid;
    fsg_pnode_ctxt_t ctxt;

    assert(search->mdef);
    silcipid = mdef_silphone(search->mdef);

    /* Initialize EVERYTHING to be inactive */
    assert(search->pnode_active == NULL);
    assert(search->pnode_active_next == NULL);

    fsg_lextree_utt_start(search->lextree);
    fsg_history_utt_start(search->history);

    /* Dummy context structure that allows all right contexts to use this entry */
    fsg_pnode_add_all_ctxt(&ctxt);

    /* Create dummy history entry leading to start state */
    search->frame = -1;
    search->bestscore = 0;
    fsg_history_entry_add(search->history,
                          NULL, -1, 0, -1, silcipid, ctxt);
    search->bpidx_start = 0;

    /* Propagate dummy history entry through NULL transitions from start state */
    fsg_search_null_prop(search);

    /* Perform word transitions from this dummy history entry */
    fsg_search_word_trans(search);

    /* Make the next-frame active list the current one */
    search->pnode_active = search->pnode_active_next;
    search->pnode_active_next = NULL;

    (search->frame)++;

    fsg_search_hyp_free(search);

    search->n_hmm_eval = 0;

    search->state = FSG_SEARCH_BUSY;
}


static void
fsg_search_hyp_dump(fsg_search_t * search, FILE * fp)
{
    /* Print backtrace */
    log_hyp_detailed(fp, search->hyp, search->uttid, "FSG", "fsg",
                     search->senscale);
}


#if 0
/* Fill in hyp_str in search.c; filtering out fillers and null trans */
static void
fsg_search_hyp_filter(fsg_search_t * search)
{
    srch_hyp_t *hyp, *filt_hyp, *head;
    int32 i;
    int32 startwid, finishwid;
    int32 altpron;
    dict_t *dict;


    dict = search->dict;
    filt_hyp = search->filt_hyp;
    startwid = dict_basewid(dict, dict_startwid(dict));
    finishwid = dict_basewid(dict, dict_finishwid(dict));
    dict = search->dict;
    altpron = search->isUsealtpron;

    i = 0;
    head = 0;
    for (hyp = search->hyp; hyp; hyp = hyp->next) {
        if ((hyp->id < 0) ||
            (hyp->id == startwid) || (hyp->id >= finishwid))
            continue;

        /* Copy this hyp entry to filtered result */
        filt_hyp = (srch_hyp_t *) ckd_calloc(1, sizeof(srch_hyp_t));

        filt_hyp->word = hyp->word;
        filt_hyp->id = hyp->id;
        filt_hyp->type = hyp->type;
        filt_hyp->sf = hyp->sf;
        filt_hyp->ascr = hyp->ascr;
        filt_hyp->lscr = hyp->lscr;
        filt_hyp->pscr = hyp->pscr;
        filt_hyp->cscr = hyp->cscr;
        filt_hyp->fsg_state = hyp->fsg_state;
        filt_hyp->next = head;
        head = filt_hyp;
        /*
           filt_hyp[i] = *hyp;
         */

        /* Replace specific word pronunciation ID with base ID */
        if (!altpron) {
            filt_hyp->id = dict_basewid(dict, filt_hyp->id);
        }

        i++;
        if ((i + 1) >= HYP_SZ)
            E_FATAL
                ("Hyp array overflow; increase HYP_SZ in fsg_search.h\n");
    }

    filt_hyp->id = -1;          /* Sentinel */
    search->filt_hyp = filt_hyp;
}

#endif


void
fsg_search_history_backtrace(fsg_search_t * search,
                             boolean check_fsg_final_state)
{
    word_fsg_t *fsg;
    fsg_hist_entry_t *hist_entry;
    word_fsglink_t *fl;
    int32 bestscore, bestscore_finalstate, besthist_finalstate, besthist;
    int32 bpidx, score, frm, last_frm;
    srch_hyp_t *hyp, *head;

    /* Free any existing search hypothesis */
    fsg_search_hyp_free(search);
    search->ascr = 0;
    search->lscr = 0;

    fsg = search->fsg;

    /* Find most recent bestscoring history entry */
    bpidx = fsg_history_n_entries(search->history) - 1;
    if (bpidx > 0) {
        hist_entry = fsg_history_entry_get(search->history, bpidx);
        last_frm = frm = fsg_hist_entry_frame(hist_entry);
        assert(frm < search->frame);
    }
    else {
        hist_entry = NULL;
        last_frm = frm = -1;
    }

    if ((bpidx <= 0) || (last_frm < 0)) {
        /* Only the dummy root entry, or null transitions from it, exist */
        if (check_fsg_final_state) {
            E_WARN("Empty utterance: %s\n", search->uttid);
        }

        return;
    }

    if (check_fsg_final_state) {
        if (frm < (search->frame - 1)) {
            E_WARN
                ("No history entry in the final frame %d; using last entry at frame %d\n",
                 search->frame - 1, frm);
        }
    }

    /*
     * Find best history entry, as well as best entry leading to FSG final state
     * in final frame.
     */
    bestscore = bestscore_finalstate = (int32) 0x80000000;
    besthist = besthist_finalstate = -1;

    while (frm == last_frm) {
        fl = fsg_hist_entry_fsglink(hist_entry);
        score = fsg_hist_entry_score(hist_entry);

        if (word_fsglink_to_state(fl) == word_fsg_final_state(fsg)) {
            if (score > bestscore_finalstate) {
                bestscore_finalstate = score;
                besthist_finalstate = bpidx;
            }
        }

        if (score > bestscore) {
            bestscore = score;
            besthist = bpidx;
        }

        --bpidx;
        if (bpidx < 0)
            break;

        hist_entry = fsg_history_entry_get(search->history, bpidx);
        frm = fsg_hist_entry_frame(hist_entry);
    }

    if (check_fsg_final_state) {
        if (besthist_finalstate > 0) {
            /*
             * Final state entry found; discard the plain best entry.
             * (Policy decision!  Is this the right thing to do??)
             */
            if (bestscore > bestscore_finalstate)
                E_INFO
                    ("Best score (%d) > best final state score (%d); but using latter\n",
                     bestscore, bestscore_finalstate);

            bestscore = bestscore_finalstate;
            besthist = besthist_finalstate;
        }
        else
            E_ERROR
                ("Final state not reached; backtracing from best scoring entry\n");
    }

    /* Backtrace through the search history, starting from besthist */
    head = NULL;
    for (bpidx = besthist; bpidx > 0;) {
        hist_entry = fsg_history_entry_get(search->history, bpidx);

        hyp = (srch_hyp_t *) ckd_calloc(1, sizeof(srch_hyp_t));

        if (fsg_history_entry_hyp_extract
            (search->history, bpidx, hyp, search->dict) <= 0)
            E_FATAL("fsg_history_entry_hyp_extract() returned <= 0\n");
        hyp->next = head;
        head = hyp;

        search->lscr += hyp->lscr;
        search->ascr += hyp->ascr;

        bpidx = fsg_hist_entry_pred(hist_entry);
    }
    search->hyp = head;

}


/*
 * Cleanup at the end of each utterance.
 */
void
fsg_search_utt_end(fsg_search_t * search)
{
    gnode_t *gn;
    fsg_pnode_t *pnode;
    hmm_t *hmm;
    int32 n_hist;
    FILE *latfp;
    char file[4096];

    /* Write history table if needed */

    if (cmd_ln_str("-bptbldir")) {
        sprintf(file, "%s/%s.hist", cmd_ln_str("-bptbldir"),
                search->uttid);
        if ((latfp = fopen(file, "w")) == NULL)
            E_ERROR("fopen(%s,w) failed\n", file);
        else {
            fsg_history_dump(search->history, search->uttid, latfp,
                             search->dict);
            fclose(latfp);
        }
    }

    /*
     * Backtrace through Viterbi history to get the best recognition.
     * First check if the final state has been reached; otherwise just use
     * the best scoring state.
     */
    fsg_search_history_backtrace(search, TRUE);

    /*  fsg_search_hyp_filter(search); */

    if (search->isBacktrace)
        fsg_search_hyp_dump(search, stdout);

    printf("\nFSGSRCH: ");
    log_hypstr(stdout, search->hyp, search->uttid, 0,
               search->ascr + search->lscr, search->dict);
    fflush(stdout);

    if (search->matchfp)
        log_hypstr(search->matchfp, search->hyp, search->uttid, 0,
                   search->ascr + search->lscr, search->dict);

    if (search->matchsegfp)
        E_WARN("Option -hypsegfp is not implemented in FSG mode yet.\n");

    n_hist = fsg_history_n_entries(search->history);
    fsg_history_reset(search->history);

    fsg_lextree_utt_end(search->lextree);

    /* Deactivate all nodes in the current and next-frame active lists */
    for (gn = search->pnode_active; gn; gn = gnode_next(gn)) {
        pnode = (fsg_pnode_t *) gnode_ptr(gn);
        hmm = fsg_pnode_hmmptr(pnode);

        fsg_psubtree_pnode_deactivate(pnode);
    }
    for (gn = search->pnode_active_next; gn; gn = gnode_next(gn)) {
        pnode = (fsg_pnode_t *) gnode_ptr(gn);
        hmm = fsg_pnode_hmmptr(pnode);

        fsg_psubtree_pnode_deactivate(pnode);
    }

    glist_free(search->pnode_active);
    search->pnode_active = NULL;
    glist_free(search->pnode_active_next);
    search->pnode_active_next = NULL;

    /* Do NOT reset search->frame, or search->hyp */

    search->state = FSG_SEARCH_IDLE;

    E_INFO("Utt %s: %d frames, %d HMMs evaluated, %d history entries\n\n",
           search->uttid, search->frame, search->n_hmm_eval, n_hist);

    /* Sanity check */
    if (search->n_hmm_eval >
        fsg_lextree_n_pnode(search->lextree) * search->frame) {
        E_ERROR
            ("SANITY CHECK #HMMEval(%d) > %d (#HMMs(%d)*#frames(%d)) FAILED\n",
             search->n_hmm_eval,
             fsg_lextree_n_pnode(search->lextree) * search->frame,
             fsg_lextree_n_pnode(search->lextree), search->frame);
    }
}


int32
fsg_search_get_start_state(fsg_search_t * search)
{
    if ((!search) || (!search->fsg))
        return -1;
    return word_fsg_start_state(search->fsg);
}


int32
fsg_search_get_final_state(fsg_search_t * search)
{
    if ((!search) || (!search->fsg))
        return -1;
    return word_fsg_final_state(search->fsg);
}


int32
fsg_search_set_start_state(fsg_search_t * search, int32 state)
{
    if (!search)
        return -1;

    if (search->state != FSG_SEARCH_IDLE) {
        E_ERROR("Attempt to switch FSG start state inside an utterance\n");
        return -1;
    }

    return (word_fsg_set_start_state(search->fsg, state));
}


int32
fsg_search_set_final_state(fsg_search_t * search, int32 state)
{
    if (!search)
        return -1;

    if (search->state != FSG_SEARCH_IDLE) {
        E_ERROR("Attempt to switch FSG start state inside an utterance\n");
        return -1;
    }

    return (word_fsg_set_final_state(search->fsg, state));
}
