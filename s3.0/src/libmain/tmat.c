/* ====================================================================
 * Copyright (c) 1996-2000 Carnegie Mellon University.  All rights 
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
 * 3. The names "Sphinx" and "Carnegie Mellon" must not be used to
 *    endorse or promote products derived from this software without
 *    prior written permission. To obtain permission, contact 
 *    sphinx@cs.cmu.edu.
 *
 * 4. Products derived from this software may not be called "Sphinx"
 *    nor may "Sphinx" appear in their names without prior written
 *    permission of Carnegie Mellon University. To obtain permission,
 *    contact sphinx@cs.cmu.edu.
 *
 * 5. Redistributions of any form whatsoever must retain the following
 *    acknowledgment:
 *    "This product includes software developed by Carnegie
 *    Mellon University (http://www.speech.cs.cmu.edu/)."
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
 * tmat.c
 *
 * 
 * HISTORY
 * 
 * 11-Mar-97	M K Ravishankar (rkm@cs.cmu.edu) at Carnegie Mellon University.
 * 		Started based on original S3 implementation.
 */


#include <libutil/libutil.h>
#include <libio/libio.h>
#include <libmisc/libmisc.h>
#include "tmat.h"


#define TMAT_PARAM_VERSION		"1.0"


#if _TMAT_TEST_
void tmat_dump (tmat_t *tmat)
{
    int32 i, src, dst;
    
    for (i = 0; i < tmat->n_tmat; i++) {
	printf ("TMAT %d %d %d\n", i, tmat->n_state, tmat->n_state+1);
	for (src = 0; src < tmat->n_state; src++) {
	    for (dst = 0; dst <= tmat->n_state; dst++)
		printf (" %12d", tmat->tp[i][src][dst]);
	    printf ("\n");
	}
	printf ("\n");
    }
}
#endif


/*
 * Check model tprob matrices that they conform to upper-triangular assumption;
 * i.e. no "backward" transitions allowed.
 */
static int32 tmat_chk_uppertri (tmat_t *tmat)
{
    int32 i, src, dst;
    
    /* Check that each tmat is upper-triangular */
    for (i = 0; i < tmat->n_tmat; i++) {
	for (dst = 0; dst < tmat->n_state; dst++)
	    for (src = dst+1; src < tmat->n_state; src++)
		if (tmat->tp[i][src][dst] > LOGPROB_ZERO)
		    E_FATAL("Tmat %d not upper triangular\n", i);
    }
    
    return 0;
}


tmat_t *tmat_init (char *file_name, float64 tpfloor)
{
    char tmp;
    int32 n_src, n_dst;
    FILE *fp;
    int32 byteswap, chksum_present;
    uint32 chksum;
    float32 **tp;
    int32 i, j, k, tp_per_tmat;
    char **argname, **argval;
    tmat_t *t;
    
    E_INFO("Reading HMM transition probability matrices: %s\n", file_name);

    t = (tmat_t *) ckd_calloc (1, sizeof(tmat_t));

    if ((fp = fopen(file_name, "rb")) == NULL)
	E_FATAL_SYSTEM("fopen(%s,rb) failed\n", file_name);
    
    /* Read header, including argument-value info and 32-bit byteorder magic */
    if (bio_readhdr (fp, &argname, &argval, &byteswap) < 0)
	E_FATAL("bio_readhdr(%s) failed\n", file_name);
    
    /* Parse argument-value list */
    chksum_present = 0;
    for (i = 0; argname[i]; i++) {
	if (strcmp (argname[i], "version") == 0) {
	    if (strcmp(argval[i], TMAT_PARAM_VERSION) != 0)
		E_WARN("Version mismatch(%s): %s, expecting %s\n",
			file_name, argval[i], TMAT_PARAM_VERSION);
	} else if (strcmp (argname[i], "chksum0") == 0) {
	    chksum_present = 1;	/* Ignore the associated value */
	}
    }
    bio_hdrarg_free (argname, argval);
    argname = argval = NULL;
    
    chksum = 0;
    
    /* Read #tmat, #from-states, #to-states, arraysize */
    if ((bio_fread (&(t->n_tmat), sizeof(int32), 1, fp, byteswap, &chksum) != 1) ||
	(bio_fread (&n_src, sizeof(int32), 1, fp, byteswap, &chksum) != 1) ||
	(bio_fread (&n_dst, sizeof(int32), 1, fp, byteswap, &chksum) != 1) ||
	(bio_fread (&i, sizeof(int32), 1, fp, byteswap, &chksum) != 1)) {
	E_FATAL("bio_fread(%s) (arraysize) failed\n", file_name);
    }
    if (t->n_tmat >= MAX_TMATID)
	E_FATAL("%s: #tmat (%d) exceeds limit (%d)\n", file_name, t->n_tmat, MAX_TMATID);
    if (n_dst != n_src+1)
	E_FATAL("%s: #from-states(%d) != #to-states(%d)-1\n", file_name, n_src, n_dst);
    t->n_state = n_src;
    
    if (i != t->n_tmat * n_src * n_dst) {
	E_FATAL("%s: #float32s(%d) doesn't match dimensions: %d x %d x %d\n",
		file_name, i, t->n_tmat, n_src, n_dst);
    }

    /* Allocate memory for tmat data */
    t->tp = (int32 ***) ckd_calloc_3d (t->n_tmat, n_src, n_dst, sizeof(int32));

    /* Temporary structure to read in the float data */
    tp = (float32 **) ckd_calloc_2d (n_src, n_dst, sizeof(float32));

    /* Read transition matrices, normalize and floor them, and convert to logs3 domain */
    tp_per_tmat = n_src * n_dst;
    for (i = 0; i < t->n_tmat; i++) {
	if (bio_fread (tp[0], sizeof(float32), tp_per_tmat, fp,
		       byteswap, &chksum) != tp_per_tmat) {
	    E_FATAL("fread(%s) (arraydata) failed\n", file_name);
	}
	
	/* Normalize and floor */
	for (j = 0; j < n_src; j++) {
	    if (vector_sum_norm (tp[j], n_dst) == 0.0)
		E_ERROR("Normalization failed for tmat %d from state %d\n", i, j);
	    vector_nz_floor (tp[j], n_dst, tpfloor);
	    vector_sum_norm (tp[j], n_dst);

	    /* Convert to logs3.  Take care of special case when tp = 0.0! */
	    for (k = 0; k < n_dst; k++)
		t->tp[i][j][k] = (tp[j][k] == 0.0) ? LOGPROB_ZERO : logs3(tp[j][k]);
	}
    }

    ckd_free_2d ((void **) tp);

    if (chksum_present)
	bio_verify_chksum (fp, byteswap, chksum);

    if (fread (&tmp, 1, 1, fp) == 1)
	E_ERROR("Non-empty file beyond end of data\n");

    fclose(fp);

    E_INFO("Read %d transition matrices of size %dx%d\n",
	   t->n_tmat, t->n_state, t->n_state+1);
    
    tmat_chk_uppertri (t);
    
    return t;
}


#if _TMAT_TEST_
main (int32 argc, char *argv[])
{
    tmat_t *t;
    float64 flr;
    
    if (argc < 3)
	E_FATAL("Usage: %s tmat floor\n", argv[0]);
    if (sscanf (argv[2], "%lf", &flr) != 1)
	E_FATAL("Usage: %s tmat floor\n", argv[0]);

    logs3_init ((float64) 1.0001);
    
    t = tmat_init (argv[1], flr);
    tmat_dump (t);
}
#endif
