/* -*- c-basic-offset: 4; indent-tabs-mode: nil -*- */
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

/* KB.C - for compile_kb
 * 
 * 02-Dec-2004	M K Ravishankar (rkm@cs.cmu.edu) at Carnegie Mellon University
 * 		Added acoustic score weight (applied only to S3 continuous
 * 		acoustic models).
 * 
 * 22-Nov-2004	M K Ravishankar (rkm@cs.cmu.edu) at Carnegie Mellon University
 * 		Incorporated continuous acoustic model handling.
 * 
 * 06-Aug-2004	M K Ravishankar (rkm@cs.cmu.edu) at Carnegie Mellon University
 * 		Added phonetp (phone transition probs matrix) for use in
 * 		allphone search.
 * 
 * 27-May-97  M K Ravishankar (rkm@cs.cmu.edu) at Carnegie-Mellon University
 * 		Included Bob Brennan's personaldic handling (similar to 
 *              oovdic).
 * 
 * 09-Dec-94	M K Ravishankar (rkm@cs) at Carnegie Mellon University
 * 		Cleaned up kb() interface; got rid of fwd-only, astar-mode
 * 		etc.
 * 
 * Revision 8.6  94/10/11  12:33:49  rkm
 * Minor changes.
 * 
 * Revision 8.5  94/07/29  11:52:10  rkm
 * Removed lmSetParameters() call; that is now part of lm_3g.c.
 * Added lm_init_oov() call to LM module.
 * Added ilm_init() call to ILM module.
 * 
 * Revision 8.4  94/05/19  14:19:12  rkm
 * Commented out computePhraseLMProbs().
 * 
 * Revision 8.3  94/04/14  14:38:01  rkm
 * Added OOV words sub-dictionary.
 * 
 * Revision 8.1  94/02/15  15:08:13  rkm
 * Derived from v7.  Includes multiple start symbols for the LISTEN
 * project.  Includes multiple LMs for grammar switching.
 * 
 * Revision 6.15  94/02/11  13:15:18  rkm
 * Initial revision (going into v7).
 * Added multiple start symbols for the LISTEN project.
 * 
 * Revision 6.14  94/01/31  16:35:17  rkm
 * Moved check for use of 8/16-bit senones on HPs to after pconf().
 * 
 * Revision 6.13  94/01/07  17:48:13  rkm
 * Added option to use trigrams in forward pass (simple implementation).
 * 
 * Revision 6.12  94/01/05  16:04:20  rkm
 * *** empty log message ***
 * 
 * Revision 6.11  94/01/05  16:02:17  rkm
 * Placed senone probs compression under conditional compilation.
 * 
 * Revision 6.10  93/12/05  17:25:46  rkm
 * Added -8bsen option and necessary datastructures.
 * 
 * Revision 6.9  93/12/04  16:24:11  rkm
 * Added check for use of -16bsen if compiled with _HPUX_SOURCE.
 * 
 * Revision 6.8  93/11/15  12:20:39  rkm
 * Added -ilmusesdarpalm flag.
 * 
 * Revision 6.7  93/11/03  12:42:35  rkm
 * Added -16bsen option to compress senone probs to 16 bits.
 * 
 * Revision 6.6  93/10/27  17:29:45  rkm
 * *** empty log message ***
 * 
 * Revision 6.5  93/10/15  14:59:13  rkm
 * Bug-fix in call to create_ilmwid_map.
 * 
 * Revision 6.4  93/10/13  16:48:16  rkm
 * Added ilm_init call to Roni's ILM and whatever else is needed.
 * Added option to process A* only (in which case phoneme-level
 * files are not loaded).  Added bigram_only argument to lm_read
 * call.
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#if defined(WIN32) && !defined(GNUWINCE) && !defined(CYGWIN)
#include <fcntl.h>
#include <io.h>
#else
#include <unistd.h>
#include <sys/param.h>
#endif

#include "s2types.h"
#include "basic_types.h"
#include "search_const.h"
#include "list.h"
#include "err.h"
#include "ckd_alloc.h"
#include "c.h"
#include "pio.h"
#include "log.h"
#include "dict.h"
#include "msd.h"
#include "smmap4f.h"
#include "lm.h"
#include "lmclass.h"
#include "lm_3g.h"
#include "s2_semi_mgau.h"
#include "kb.h"
#include "phone.h"
#include "fbs.h"
#include "mdef.h"
#include "tmat.h"
#include "search.h"
#include "cmd_ln.h"

/* Dictionary. */
dictT *word_dict;

/* Transition matrices. */
tmat_t *tmat;

/* Phone transition LOG(probability) matrix */
int32 **phonetp;

/* S3 model definition */
bin_mdef_t *mdef;

/* S2 fast SCGMM computation object */
s2_semi_mgau_t *semi_mgau;

/* Model file names */
char *hmmdir, *mdeffn, *meanfn, *varfn, *mixwfn, *tmatfn, *sendumpfn;

void
kbAddGrammar(char const *fileName, char const *grammarName)
{
    lmSetStartSym(cmd_ln_str("-lmstartsym"));
    lmSetEndSym(cmd_ln_str("-lmendsym"));
    lm_read(fileName, grammarName,
            cmd_ln_float32("-lw"),
            cmd_ln_float32("-uw"),
            cmd_ln_float32("-wip"));
}

static void
kb_init_lmclass_dictwid(lmclass_t cl)
{
    lmclass_word_t w;
    int32 wid;

    for (w = lmclass_firstword(cl); lmclass_isword(w);
         w = lmclass_nextword(cl, w)) {
        wid = kb_get_word_id(lmclass_getword(w));
        lmclass_set_dictwid(w, wid);
    }
}


static void
phonetp_load_file(char *file, int32 ** tp)
{
    FILE *fp;
    char line[16384], p1[4096], p2[4096];
    int32 i, j, k, n;

    E_INFO("Reading phone transition counts file '%s'\n", file);
    fp = myfopen(file, "r");

    while (fgets(line, sizeof(line), fp) != NULL) {
        if (line[0] == '#')
            continue;

        k = sscanf(line, "%s %s %d", p1, p2, &n);
        if ((k != 0) && (k != 3))
            E_FATAL("Expecting 'srcphone dstphone count'; found:\n%s\n",
                    line);

        i = phone_to_id(p1, TRUE);
        j = phone_to_id(p2, TRUE);
        if ((i == NO_PHONE) || (j == NO_PHONE))
            E_FATAL("Unknown src or dst phone: %s or %s\n", p1, p2);
        if (n < 0)
            E_FATAL("Phone transition count cannot be < 0:\n%s\n", line);

        tp[i][j] = n;
    }

    fclose(fp);
}

static void
lm_init(void)
{
    char *lm_ctl_filename = cmd_ln_str("-lmctlfn");
    char *lm_file_name = cmd_ln_str("-lm");
    char *lm_start_sym = cmd_ln_str("-lmstartsym");
    char *lm_end_sym = cmd_ln_str("-lmendsym");

    lmSetStartSym(lm_start_sym);
    lmSetEndSym(lm_end_sym);

    /*
     * Read control file describing multiple LMs, if specified.
     * File format (optional stuff is indicated by enclosing in []):
     * 
     *   [{ LMClassFileName LMClassFilename ... }]
     *   TrigramLMFileName LMName [{ LMClassName LMClassName ... }]
     *   TrigramLMFileName LMName [{ LMClassName LMClassName ... }]
     *   ...
     * (There should be whitespace around the { and } delimiters.)
     * 
     * This is an extension of the older format that had only TrigramLMFilenName
     * and LMName pairs.  The new format allows a set of LMClass files to be read
     * in and referred to by the trigram LMs.  (Incidentally, if one wants to use
     * LM classes in a trigram LM, one MUST use the -lmctlfn flag.  It is not
     * possible to read in a class-based trigram LM using the -lm flag.)
     * 
     * No "comments" allowed in this file.
     */
    if (lm_ctl_filename) {
        FILE *ctlfp;
        char lmfile[4096], lmname[4096], str[4096];
        lmclass_set_t lmclass_set;
        lmclass_t *lmclass, cl;
        int32 n_lmclass, n_lmclass_used;

        lmclass_set = lmclass_newset();

        E_INFO("Reading LM control file '%s'\n", lm_ctl_filename);

        ctlfp = myfopen(lm_ctl_filename, "r");
        if (fscanf(ctlfp, "%s", str) == 1) {
            if (strcmp(str, "{") == 0) {
                /* Load LMclass files */
                while ((fscanf(ctlfp, "%s", str) == 1)
                       && (strcmp(str, "}") != 0))
                    lmclass_set = lmclass_loadfile(lmclass_set, str);

                if (strcmp(str, "}") != 0)
                    E_FATAL("Unexpected EOF(%s)\n", lm_ctl_filename);

                if (fscanf(ctlfp, "%s", str) != 1)
                    str[0] = '\0';
            }
        }
        else
            str[0] = '\0';

        /* Fill in dictionary word id information for each LMclass word */
        for (cl = lmclass_firstclass(lmclass_set);
             lmclass_isclass(cl);
             cl = lmclass_nextclass(lmclass_set, cl)) {
            kb_init_lmclass_dictwid(cl);
        }

        /* At this point if str[0] != '\0', we have an LM filename */
        n_lmclass = lmclass_get_nclass(lmclass_set);
        lmclass = ckd_calloc(n_lmclass, sizeof(lmclass_t));

        /* Read in one LM at a time */
        while (str[0] != '\0') {
            strcpy(lmfile, str);
            if (fscanf(ctlfp, "%s", lmname) != 1)
                E_FATAL("LMname missing after LMFileName '%s'\n",
                        lmfile);

            n_lmclass_used = 0;

            if (fscanf(ctlfp, "%s", str) == 1) {
                if (strcmp(str, "{") == 0) {
                    /* LM uses classes; read their names */
                    while ((fscanf(ctlfp, "%s", str) == 1) &&
                           (strcmp(str, "}") != 0)) {
                        if (n_lmclass_used >= n_lmclass)
                            E_FATAL
                                ("Too many LM classes specified for '%s'\n",
                                 lmfile);
                        lmclass[n_lmclass_used] =
                            lmclass_get_lmclass(lmclass_set, str);
                        if (!
                            (lmclass_isclass(lmclass[n_lmclass_used])))
                            E_FATAL("LM class '%s' not found\n", str);
                        n_lmclass_used++;
                    }
                    if (strcmp(str, "}") != 0)
                        E_FATAL("Unexpected EOF(%s)\n",
                                lm_ctl_filename);

                    if (fscanf(ctlfp, "%s", str) != 1)
                        str[0] = '\0';
                }
            }
            else
                str[0] = '\0';

            if (n_lmclass_used > 0)
                lm_read_clm(lmfile, lmname,
                            cmd_ln_float32("-lw"),
                            cmd_ln_float32("-uw"),
                            cmd_ln_float32("-wip"),
                            lmclass, n_lmclass_used);
            else
                lm_read(lmfile, lmname,
                        cmd_ln_float32("-lw"),
                        cmd_ln_float32("-uw"),
                        cmd_ln_float32("-wip"));
        }

        fclose(ctlfp);
    }

    /* Read "base" LM file, if specified */
    if (lm_file_name) {
        lmSetStartSym(lm_start_sym);
        lmSetEndSym(lm_end_sym);
        lm_read(lm_file_name, "",
                cmd_ln_float32("-lw"),
                cmd_ln_float32("-uw"),
                cmd_ln_float32("-wip"));

        /* Make initial OOV list known to this base LM */
        lm_init_oov();
    }
}

static void
dict_init(void)
{
    E_INFO("Reading dict file [%s]\n", cmd_ln_str("-dict"));
    word_dict = dict_new();
    if (dict_read(word_dict, cmd_ln_str("-dict"), cmd_ln_str("-pdict"),
                  cmd_ln_str("-fdict"), !cmd_ln_boolean("-usewdphones"))) {
        exit(-1);
    }
}

static void
phonetp_init(int32 num_ci_phones)
{
    int i, j, n, logp;
    float32 p, uptp;
    float32 pip = cmd_ln_float32("-pip");
    float32 ptplw = cmd_ln_float32("-ptplw");
    float32 uptpwt = cmd_ln_float32("-uptpwt");

    phonetp =
        (int32 **) ckd_calloc_2d(num_ci_phones, num_ci_phones,
                                 sizeof(int32));
    if (cmd_ln_str("-phonetp")) {
        /* Load phone transition counts file */
        phonetp_load_file(cmd_ln_str("-phonetp"), phonetp);
    }
    else {
        /* No transition probs file specified; use uniform probs */
        for (i = 0; i < num_ci_phones; i++) {
            for (j = 0; j < num_ci_phones; j++) {
                phonetp[i][j] = 1;
            }
        }
    }
    /* Convert counts to probs; smooth; convert to LOG-probs; apply lw/pip */
    for (i = 0; i < num_ci_phones; i++) {
        n = 0;
        for (j = 0; j < num_ci_phones; j++)
            n += phonetp[i][j];
        assert(n >= 0);

        if (n == 0) {           /* No data here, use uniform probs */
            p = 1.0 / (float32) num_ci_phones;
            p *= pip;           /* Phone insertion penalty */
            logp = (int32) (LOG(p) * ptplw);

            for (j = 0; j < num_ci_phones; j++)
                phonetp[i][j] = logp;
        }
        else {
            uptp = 1.0 / (float32) num_ci_phones;       /* Uniform prob trans prob */

            for (j = 0; j < num_ci_phones; j++) {
                p = ((float32) phonetp[i][j] / (float32) n);
                p = ((1.0 - uptpwt) * p) + (uptpwt * uptp);     /* Smooth */
                p *= pip;       /* Phone insertion penalty */

                phonetp[i][j] = (int32) (LOG(p) * ptplw);
            }
        }
    }
}

char *
string_join(const char *base, ...)
{
    va_list args;
    size_t len;
    const char *c;
    char *out;

    va_start(args, base);
    len = strlen(base);
    while ((c = va_arg(args, const char *)) != NULL) {
        len += strlen(c);
    }
    len++;
    va_end(args);

    out = ckd_calloc(len, 1);
    va_start(args, base);
    strcpy(out, base);
    while ((c = va_arg(args, const char *)) != NULL) {
        strcat(out, c);
    }
    va_end(args);

    return out;
}

void
kb_init(void)
{
    int32 num_phones, num_ci_phones;

    /* Get acoustic model filenames */
    if ((hmmdir = cmd_ln_str("-hmm")) != NULL) {
        FILE *tmp;

        mdeffn = string_join(hmmdir, "/mdef", NULL);
        meanfn = string_join(hmmdir, "/means", NULL);
        varfn = string_join(hmmdir, "/variances", NULL);
        mixwfn = string_join(hmmdir, "/mixture_weights", NULL);
        tmatfn = string_join(hmmdir, "/transition_matrices", NULL);

        /* Special case - make sure this one actually exists */
        sendumpfn = string_join(hmmdir, "/sendump", NULL);
        if ((tmp = fopen(sendumpfn, "rb")) == NULL) {
            ckd_free(sendumpfn);
            sendumpfn = NULL;
        }
        else {
            fclose(tmp);
        }
    }
    if (cmd_ln_str("-mdef"))
        mdeffn = cmd_ln_str("-mdef");
    if (cmd_ln_str("-mean"))
        meanfn = cmd_ln_str("-mean");
    if (cmd_ln_str("-var"))
        varfn = cmd_ln_str("-var");
    if (cmd_ln_str("-mixw"))
        mixwfn = cmd_ln_str("-mixw");
    if (cmd_ln_str("-tmat"))
        tmatfn = cmd_ln_str("-tmat");
    if (cmd_ln_str("-sendump"))
        sendumpfn = cmd_ln_str("-sendump");

    /* Read model definition. */
    if (mdeffn == NULL)
        E_FATAL("Must specify -mdeffn or -hmm\n");
    if ((mdef = bin_mdef_read(mdeffn)) == NULL)
        E_FATAL("Failed to read model definition from %s\n", mdeffn);
    num_ci_phones = bin_mdef_n_ciphone(mdef);
    num_phones = bin_mdef_n_phone(mdef);

    dict_init();

    lm_init();

    /* Read acoustic model files. */
    if ((meanfn == NULL)
        || (varfn == NULL)
        || (tmatfn == NULL))
        E_FATAL("No mean/var/tmat files specified\n");

    /* Read transition matrices. */
    tmat = tmat_init(tmatfn, cmd_ln_float32("-tmatfloor"), TRUE);

    /* Read the acoustic models. */
    E_INFO("Initializing SCGMM computation module\n");
    semi_mgau = s2_semi_mgau_init(meanfn,
                                  varfn,
                                  cmd_ln_float32("-varfloor"),
                                  mixwfn,
                                  cmd_ln_float32("-mixwfloor"),
                                  cmd_ln_int32("-topn"));
    if (cmd_ln_str("-kdtree"))
        s2_semi_mgau_load_kdtree(semi_mgau,
                                 cmd_ln_str("-kdtree"),
                                 cmd_ln_int32("-kdmaxdepth"),
                                 cmd_ln_int32("-kdmaxbbi"));
    semi_mgau->dcep80msWeight = FLOAT2MFCC(cmd_ln_float32("-dcep80msweight"));
    semi_mgau->ds_ratio = cmd_ln_int32("-dsratio");

    /*
     * Create phone transition logprobs matrix
     */
    phonetp_init(num_ci_phones);
}

char *
kb_get_senprob_dump_file(void)
{
    return sendumpfn;
}

int32
kb_get_word_id(char const *word)
{
    return (dict_to_id(word_dict, word));
}

char *
kb_get_word_str(int32 wid)
{
    return (word_dict->dict_list[wid]->word);
}

int32
kb_dict_maxsize(void)
{
    return (hash_table_size(word_dict->dict));
}
