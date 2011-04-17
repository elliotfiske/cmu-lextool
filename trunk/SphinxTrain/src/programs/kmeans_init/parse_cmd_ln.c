/* ====================================================================
 * Copyright (c) 2004 Carnegie Mellon University.  All rights 
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
/*********************************************************************
 *
 * File: cmd_ln.c
 * 
 * Description: 
 *    This file defines the command line arguments for kmeans_init
 *
 * Author: 
 * 	Eric Thayer (eht@cs.cmu.edu)
 *********************************************************************/
#include "parse_cmd_ln.h"

#include <sphinxbase/cmd_ln.h>
#include <sphinxbase/feat.h>
#include <s3/err.h>

#include <stdio.h>
#include <sys_compat/misc.h>

int
parse_cmd_ln(int argc, char *argv[])
{
  uint32      isHelp;
  uint32      isExample;

    const char helpstr[] =  
"Description: \n\
\n\
   Using the segment dump file generated by external software such as \n\
   agg_seg to initilaize the model.  It performs k-mean clustering to \n\
   create the initial means and variances for s2 hmms. This is an \n\
   important process of initialization of s2 training. ";

    const char examplestr[]=
"Example : \n\
\n\
kmeans_init -gthobj single -stride 1 -ntrial 1 -minratio 0.001 \n\
-ndensity 256 -meanfn $outhmm/means -varfn $outhmm/variances -reest no \n\
-segdmpdirs $segdmpdir -segdmpfn $dumpfile -ceplen 13";

    static arg_t defn[] = {
	{ "-help",
	  ARG_BOOLEAN,
	  "no",
	  "Shows the usage of the tool"},

	{ "-example",
	  ARG_BOOLEAN,
	  "no",
	  "Shows example of how to use the tool"},

	{ "-segdir",
	  ARG_STRING,
	  NULL,
	  "Directory containing the state segmentations" },

	{ "-segext",
	  ARG_STRING,
	  "v8_seg",
	  "Extention of state segmentation files" },

	{ "-omoddeffn",
	  ARG_STRING,
	  NULL,
	  "Model definition of output models" },

	{ "-dmoddeffn",
	  ARG_STRING,
	  NULL,
	  "Model definition used for observation dump" },

	{ "-ts2cbfn",
	  ARG_STRING,
	  NULL,
	  "Tied-state-to-codebook mapping file" },

	{ "-lsnfn",
	  ARG_STRING,
	  NULL,
	  "LSN file name (word transcripts)" },
	  
	{ "-dictfn",
	  ARG_STRING,
	  NULL,
	  "Dictionary file name"},
	  
	{ "-fdictfn",
	  ARG_STRING,
	  NULL,
	  "Filler word dictionary file name"},
	  
	{ "-cbcntfn",
	  ARG_STRING,
	  NULL,
	  "File containing # of times a codebook ID appears in the corpus" },

	{ "-maxcbobs",
	  ARG_INT32,
	  NULL,
	  "Cluster at most this many observations per codebook" },

	{ "-maxtotobs",
	  ARG_INT32,
	  NULL,
	  "Cluster at most approximately this many observations over all codebooks" },

	{ "-featsel",
	  ARG_INT32,
	  NULL,
	  "The feature stream ( 0, 1, ...) to select" },

	/* Defines a corpus */
	{ "-ctlfn",
	  ARG_STRING,
	  NULL,
	  "The training corpus control file" },

	/* Cepstrum file location and extension */
	{ "-cepext",
	  ARG_STRING,
	  ".mfc",
	  "The cepstrum file extension" },
	{ "-cepdir",
	  ARG_STRING,
	  NULL,
	  "The cepstrum data root directory" },

	{ "-segdmpdirs",
	  ARG_STRING_LIST,
	  NULL,
	  "segment dmp directories"},

	{ "-segdmpfn",
	  ARG_STRING,
	  NULL,
	  "segment dmp file"},

	{ "-segidxfn",
	  ARG_STRING,
	  NULL,
	  "segment dmp index file"},

	{ "-fpcachesz",
	  ARG_INT32,
	  "3000",
	  "# of file descriptors to cache for observation dmp files" },
	
	{ "-obscachesz",
	  ARG_INT32,
	  "92",
	  "# of Mbytes cache to use for observations" },

	{ "-ndensity",
	  ARG_INT32,
	  NULL,
	  "# of densities to initialize per tied state per feature" },

	{ "-ntrial",
	  ARG_INT32,
	  "5",
	  "random initialized K-means: # of trials of k-means w/ random initialization from within corpus" },
	
	{ "-minratio",
	  ARG_FLOAT32,
	  "0.01",
	  "K-means: minimum convergence ratio, (p_squerr - squerr) / p_squerr"},

	{ "-maxiter",
	  ARG_INT32,
	  "100",
	  "K-means: maximum # of iterations of updating to apply"},

	{ "-mixwfn",
	  ARG_STRING,
	  NULL,
	  "Output file for mixing weights" },
	
	{ "-meanfn",
	  ARG_STRING,
	  NULL,
	  "Output file for means" },

	{ "-varfn",
	  ARG_STRING,
	  NULL,
	  "Output file for variances" },
	
	{ "-fullvar",
	  ARG_BOOLEAN,
	  "no",
	  "Calculate full covariance matrices" },
	
	{ "-method",
	  ARG_STRING,
	  "rkm",
	  "Initialization method.  Options: rkm | fnkm" },
	
	{ "-reest",
	  ARG_BOOLEAN,
	  "yes",
	  "Reestimate states according to usual definitions assuming vit seg."},
	
	{ "-niter",
	  ARG_INT32,
	  "20",
	  "# of iterations of reestimation to perform per state" },

	{ "-gthobj",
	  ARG_STRING,
	  "state",
	  "Gather what kind of obj {state, phn, frame}" },
	
	{ "-stride",
	  ARG_INT32,
	  "32",
	  "Gather every -stride'th frame" },
	
	{ "-runlen",
	  ARG_INT32,
	  NULL,
	  "# of utts to process from ctl file" },

	{ "-tsoff",
	  ARG_INT32,
	  "0",
	  "Begin tied state reestimation with this tied state" },

	{ "-tscnt",
	  ARG_INT32,
	  NULL,
	  "Reestimate this many tied states (if available)" },

	{ "-tsrngfn",
	  ARG_STRING,
	  NULL,
	  "The range of tied states reestimated expressed as offset,count"},

	{ "-vartiethr",
	  ARG_INT32,
	  "0",		/* i.e. no variance tying based on occurrance count */
	  "Tie variances if # of observations for state exceed this number" },

	cepstral_to_feature_command_line_macro(),
	
	{NULL, 0, NULL, NULL}

    };
    
    cmd_ln_parse(defn, argc, argv, TRUE);

    isHelp    = cmd_ln_int32("-help");
    isExample    = cmd_ln_int32("-example");

    if(isHelp){
      printf("%s\n\n",helpstr);
    }

    if(isExample){
      printf("%s\n\n",examplestr);
    }

    if(isHelp || isExample){
      E_INFO("User asked for help or example.\n");
      exit(0);
    }


    return 0;
}

