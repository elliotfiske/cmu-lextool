/*
 * senone.h -- Weights associated with each mixture gaussian.
 *
 * **********************************************
 * CMU ARPA Speech Project
 *
 * Copyright (c) 1997 Carnegie Mellon University.
 * ALL RIGHTS RESERVED.
 * **********************************************
 * 
 * HISTORY
 * 
 * $Log$
 * Revision 1.1  2000/04/24  09:07:29  lenzo
 * Import s3.0.
 * 
 * 
 * 19-Feb-97	M K Ravishankar (rkm@cs.cmu.edu) at Carnegie Mellon University.
 * 		Started based on original S3 implementation.
 */


#ifndef _LIBMAIN_SENONE_H_
#define _LIBMAIN_SENONE_H_


#include "s3types.h"


typedef uint8 senprob_t;	/* Senone logprob, truncated to 8 bits */

/*
 * Mixture weights for a group of senones that share a single mixture Gaussian.  Split
 * into two structures: mixw_t and mgau2sen_t.  mgau2sen_t identifies the set of senones
 * that share each mixture Gaussian codebook.  But since a codebook consists of multiple
 * feature streams, with a separate mixture Gaussian of independent size for each stream,
 * the actual mixing weights for each stream are given by mixw_t.
 */

typedef struct {
    int32 n_wt;		/* #mixture weights (== parent mixture Gaussian mgau_t.n_mean) */
    senprob_t **prob;	/* prob[senone][mixture-component]; #row: parent mgau2sen_t.n_sen
			   and #col: n_wt */
} mixw_t;

typedef struct {
    int32 n_sen;	/* #Senones sharing this codebook */
    s3senid_t *sen;	/* List of senones sharing this codebook */
    mixw_t *feat_mixw;	/* Mixture weights for these senones for each feature */
} mgau2sen_t;

/* The main senones data structure */
typedef struct {
    int32 n_sen;	/* Total #senones */
    int32 n_feat;	/* #Features */
    int32 n_mgau;	/* #Parent codebooks (mixture Gaussians for multiple features
			   count as one codebook) */
    int32 shift;	/* LSB bits truncated from original logs3 value */
    mgau2sen_t *mgau2sen;	/* Set of senones for each parent codebook */
    int32 *sen2mgau;		/* Unique parent codebook for each senone */
    int32 *mgau2sen_idx;	/* mgau2sen_idx[s] = index into mgau2sen.sen[]
				   for senone s */
} senone_t;


/* Access macros */
#define senone_n_sen(s)		((s)->n_sen)
#define senone_n_mgau(s)	((s)->n_mgau)
#define senone_n_stream(s)	((s)->n_feat)


/*
 * Load a set of senones (mixing weights and mixture gaussian codebook mappings) from
 * the given files.  Normalize weights for each codebook, apply the given floor, convert
 * PDF values to logs3 domain and quantize to 8-bits.
 * Return value: pointer to senone structure created.  Caller MUST NOT change its contents.
 */
senone_t *senone_init (char *mixwfile,		/* In: mixing weights file */
		       char *mgau_mapfile,	/* In: file specifying mapping from each
						   senone to mixture gaussian codebook.
						   If NULL all senones map to codebook 0 */
		       float64 mixwfloor);	/* In: Floor value for senone weights */

/*
 * Compute senone scores for all senones sharing the given mixture Gaussian.  The computed
 * scores are ACCUMULATED into senscr (because of the need to accumulate over multiple
 * feature streams).  (Scores are computed in logs3 domain.)
 */
void senone_eval_all (senone_t *s,	/* In: Senone structure */
		      int32 m,		/* In: Parent mgau for which senones evaluated */
		      int32 f,		/* In: Feature stream for which evaluated */
		      int32 *dist,	/* In: Gaussian density values to be weighted */
		      int32 *valid,	/* In: Optional list of valid indexes in dist[] */
		      int32 n_dist,	/* In: #density values in dist[] if valid is NULL,
					   or in valid[] otherwise */
		      int32 *senscr);	/* In/Out: Senone scores array (one entry/senone,
					   allocated by caller).  Computed senone scores
					   accumulated into this array */

/*
 * Like senone_eval_all above, but computed only for a single given senone.  Furthermore,
 * the weighted result is not accumulated anywhere automatically.  Rather, it's the
 * return value.
 */
int32 senone_eval (senone_t *s,		/* In: Senone structure */
		   int32 sid,		/* In: Senone id for which score is computed */
		   int32 f,		/* In: Feature stream under consideration */
		   int32 *dist,		/* In: Gaussian density values to be weighted */
		   int32 *valid,	/* In: #density values in dist[] if valid is NULL,
					   or in valid[] otherwise */
		   int32 n_dist);	/* In: #density values in dist above */

/*
 * Mark the given array of senone-ids as active in the given bitvector.
 */
void senone_set_active (bitvec_t flags,	/* In/Out: Bit-vector to be marked */
			s3senid_t *sen,	/* In: Array of senone IDs to be activated */
			int32 n_sen);	/* In: No. of elements in the above ID array */

/*
 * Return the cumulative scaling applied to senone scores in the given segment.
 */
int32 senone_get_senscale (int32 *sc,	/* In: sc[f] = scaling applied in frame f */
			   int32 sf,	/* In: Start frame of segment (inclusive) */
			   int32 ef);	/* In: End frame of segment (inclusive) */

#endif
