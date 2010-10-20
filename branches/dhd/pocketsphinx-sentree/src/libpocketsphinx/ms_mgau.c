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
 * ms_mgau.c -- Essentially a wrapper that wrap up gauden and
 * senone. It supports multi-stream. 
 *
 *
 * **********************************************
 * CMU ARPA Speech Project
 *
 * Copyright (c) 1997 Carnegie Mellon University.
 * ALL RIGHTS RESERVED.
 * **********************************************
 * HISTORY
 * $Log$
 * Revision 1.2  2006/02/22  16:56:01  arthchan2003
 * Merged from SPHINX3_5_2_RCI_IRII_BRANCH: Added ms_mgau.[ch] into the trunk. It is a wrapper of ms_gauden and ms_senone
 * 
 * Revision 1.1.2.4  2005/09/25 18:55:19  arthchan2003
 * Added a flag to turn on and off precomputation.
 *
 * Revision 1.1.2.3  2005/08/03 18:53:44  dhdfu
 * Add memory deallocation functions.  Also move all the initialization
 * of ms_mgau_model_t into ms_mgau_init (duh!), which entails removing it
 * from decode_anytopo and friends.
 *
 * Revision 1.1.2.2  2005/08/02 21:05:38  arthchan2003
 * 1, Added dist and mgau_active as intermediate variable for computation. 2, Added ms_cont_mgau_frame_eval, which is a multi stream version of GMM computation mainly s3.0 family of tools. 3, Fixed dox-doc.
 *
 * Revision 1.1.2.1  2005/07/20 19:37:09  arthchan2003
 * Added a multi-stream cont_mgau (ms_mgau) which is a wrapper of both gauden and senone.  Add ms_mgau_init and model_set_mllr.  This allow eliminating 600 lines of code in decode_anytopo/align/allphone.
 *
 *
 *
 */

/* Local headers. */
#include "ms_mgau.h"
#include "senscr.h"

ms_mgau_model_t *
ms_mgau_init(cmd_ln_t *config, logmath_t *lmath)
{
    /* Codebooks */
    int32 i;
    ms_mgau_model_t *msg;
    gauden_t *g;
    senone_t *s;
    mgau2sen_t *m2s;

    msg = (ms_mgau_model_t *) ckd_calloc(1, sizeof(ms_mgau_model_t));


    msg->g = NULL;
    msg->s = NULL;

    msg->g = gauden_init(cmd_ln_str_r(config, "-mean"),
			 cmd_ln_str_r(config, "-var"),
			 cmd_ln_float32_r(config, "-varfloor"),
			 lmath);
    msg->s = senone_init(msg->g,
			 cmd_ln_str_r(config, "-mixw"), NULL,
			 cmd_ln_float32_r(config, "-mixwfloor"), lmath);

    g = ms_mgau_gauden(msg);
    s = ms_mgau_senone(msg);

    /* Verify senone parameters against gauden parameters */
    if (s->n_feat != g->n_feat)
        E_FATAL("#Feature mismatch: gauden= %d, senone= %d\n", g->n_feat,
                s->n_feat);
    if (s->n_cw != g->n_density)
        E_FATAL("#Densities mismatch: gauden= %d, senone= %d\n",
                g->n_density, s->n_cw);
    if (s->n_gauden > g->n_mgau)
        E_FATAL("Senones need more codebooks (%d) than present (%d)\n",
                s->n_gauden, g->n_mgau);
    if (s->n_gauden < g->n_mgau)
        E_ERROR("Senones use fewer codebooks (%d) than present (%d)\n",
                s->n_gauden, g->n_mgau);
    /* Initialize mapping from mixture Gaussian to senones */
    msg->mgau2sen =
        (mgau2sen_t **) ckd_calloc(g->n_mgau, sizeof(mgau2sen_t *));
    for (i = 0; i < s->n_sen; i++) {
        m2s = (mgau2sen_t *) ckd_calloc(1, sizeof(mgau2sen_t));
        m2s->sen = i;
        m2s->next = msg->mgau2sen[s->mgau[i]];
        msg->mgau2sen[s->mgau[i]] = m2s;
    }

    msg->topn = cmd_ln_int32_r(config, "-topn");
    E_INFO("The value of topn: %d\n", msg->topn);
    if (msg->topn == 0 || msg->topn > msg->g->n_density) {
        E_WARN
            ("-topn argument (%d) invalid or > #density codewords (%d); set to latter\n",
             msg->topn, msg->g->n_density);
        msg->topn = msg->g->n_density;
    }

    msg->dist = (gauden_dist_t ***)
        ckd_calloc_3d(g->n_mgau, g->n_feat, msg->topn,
                      sizeof(gauden_dist_t));
    msg->mgau_active = ckd_calloc(g->n_mgau, sizeof(int8));

    return msg;
}

void
ms_mgau_free(ms_mgau_model_t * msg)
{
    if (msg == NULL)
        return;

    gauden_free(msg->g);
    senone_free(msg->s);
    ckd_free_3d((void *) msg->dist);
    ckd_free(msg->mgau_active);
    ckd_free(msg);
}

int32
ms_cont_mgau_frame_eval(ms_mgau_model_t * msg,
			int16 *senscr,
			int32 *senone_active,
			int32 n_senone_active,
                        mfcc_t ** feat,
			int32 frame,
			int32 compallsen,
			int32 *bestidx)
{
    int32 gid;
    int32 i;
    int32 topn;
    int32 best;
    gauden_t *g;
    senone_t *sen;

    topn = ms_mgau_topn(msg);
    g = ms_mgau_gauden(msg);
    sen = ms_mgau_senone(msg);

    /*
     * Evaluate gaussian density codebooks and senone scores for input codeword.
     * Evaluate only active codebooks and senones. (ignore compallsen...)
     */

    /* Flag all active mixture-gaussian codebooks */
    for (gid = 0; gid < g->n_mgau; gid++)
        msg->mgau_active[gid] = 0;

    for (i = 0; i < n_senone_active; i++) {
	int32 s = senone_active[i];
	msg->mgau_active[sen->mgau[s]] = 1;
    }

    /* Compute topn gaussian density values (for active codebooks) */
    for (gid = 0; gid < g->n_mgau; gid++) {
	if (msg->mgau_active[gid])
            gauden_dist(g, gid, topn, feat, msg->dist[gid]);
    }

    best = (int32) 0x7fffffff;
    *bestidx = -1;
    for (i = 0; i < n_senone_active; i++) {
	int32 s = senone_active[i];
	senscr[s] = senone_eval(sen, s, msg->dist[sen->mgau[s]], topn);
	if (best > senscr[s]) {
	    best = senscr[s];
	    *bestidx = s;
	}
    }

    /* Normalize senone scores */
    for (i = 0; i < n_senone_active; i++) {
	int32 s = senone_active[i];
	senscr[s] -= best;
    }

    return best;
}