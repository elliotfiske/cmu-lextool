/***********************************************
 * CMU ARPA Speech Project
 *
 * Copyright (c) 2000 Carnegie Mellon University.
 * ALL RIGHTS RESERVED.
 * **********************************************
 * 
 * HISTORY
 * 
 * 30-Dec-2000  Rita Singh (rsingh@cs.cmu.edu) at Carnegie Mellon University
 * Created
 */

#include <stdio.h>
#include <libutil/libutil.h>

#include "cmd_ln_args.h"

static arg_t arg[] = {
    { "-logbase",
      ARG_FLOAT32,
      "1.0003",
      "Base in which all log-likelihoods calculated" },
#if 0
    /* Commented out; must be s3_1x39 */
    { "-feat",
      ARG_STRING,
      NULL,
      "Feature type: Must be s3_1x39 / s2_4x / cep_dcep[,%d] / cep[,%d] / %d,%d,...,%d" },
#endif
    { "-lminmemory",
      ARG_INT32,
      "0",
      "Load language model into memory (default: use disk cache for lm"},
    { "-log3table",
      ARG_INT32,
      "1",
      "Determines whether to use the log3 table or to compute the values at run time."},
    { "-vqeval",
      ARG_INT32,
      "3",
      "How many vectors should be analyzed by VQ when building the shortlist. It speeds up the decoder, but at a cost."},

    { "-cmn",
      ARG_STRING,
      "current",
      "Cepstral mean normalization scheme (default: Cep -= mean-over-current-sentence(Cep))" },
    { "-varnorm",
      ARG_STRING,
      "no",
      "Variance normalize each utterance (yes/no; only applicable if CMN is also performed)" },
    { "-agc",
      ARG_STRING,
      "max",
      "Automatic gain control for c0 ('max' or 'none'); (max: c0 -= max-over-current-sentence(c0))" },
    { "-mdef",
      REQARG_STRING,
      NULL,
      "Model definition input file" },
    { "-dict",
      REQARG_STRING,
      NULL,
      "Pronunciation dictionary input file" },
    { "-fdict",
      REQARG_STRING,
      NULL,
      "Filler word pronunciation dictionary input file" },
#if 0
    /* Commented out; not supported */
    { "-compsep",
      ARG_STRING,
      "",	/* Default: No compound word (NULL separator char) */
      "Separator character between components of a compound word (NULL if none)" },
#endif
    { "-lm",
      REQARG_STRING,
      NULL,
      "Word trigram language model input file" },
    { "-fillpen",
      ARG_STRING,
      NULL,
      "Filler word probabilities input file" },
    { "-silprob",
      ARG_FLOAT32,
      "0.1",
      "Default silence word probability" },
    { "-fillprob",
      ARG_FLOAT32,
      "0.1",
      "Default non-silence filler word probability" },
    { "-lw",
      ARG_FLOAT32,
      "8.5",
      "Language weight" },
    { "-wip",
      ARG_FLOAT32,
      "0.7",
      "Word insertion penalty" },
    { "-uw",
      ARG_FLOAT32,
      "0.7",
      "Unigram weight" },
    { "-mean",
      REQARG_STRING,
      NULL,
      "Mixture gaussian means input file" },
    { "-var",
      REQARG_STRING,
      NULL,
      "Mixture gaussian variances input file" },
    { "-varfloor",
      ARG_FLOAT32,
      "0.0001",
      "Mixture gaussian variance floor (applied to data from -var file)" },
    { "-mixw",
      REQARG_STRING,
      NULL,
      "Senone mixture weights input file" },
    { "-mixwfloor",
      ARG_FLOAT32,
      "0.0000001",
      "Senone mixture weights floor (applied to data from -mixw file)" },
    { "-subvq",
      ARG_STRING,
      NULL,
      "Sub-vector quantized form of acoustic model" },
    { "-tmat",
      REQARG_STRING,
      NULL,
      "HMM state transition matrix input file" },
    { "-tmatfloor",
      ARG_FLOAT32,
      "0.0001",
      "HMM state transition probability floor (applied to -tmat file)" },
    { "-Nlextree",
      ARG_INT32,
      "3",
      "No. of lextrees to be instantiated; entries into them staggered in time" },
    { "-epl",
      ARG_INT32,
      "3",
      "Entries Per Lextree; #successive entries into one lextree before lextree-entries shifted to the next" },
    { "-subvqbeam",
      ARG_FLOAT64,
      "3.0e-3",
      "Beam selecting best components within each mixture Gaussian [0(widest)..1(narrowest)]" },
    { "-beam",
      ARG_FLOAT64,
      "1.0e-55",
      "Beam selecting active HMMs (relative to best) in each frame [0(widest)..1(narrowest)]" },
    { "-pbeam",
      ARG_FLOAT64,
      "1.0e-50",
      "Beam selecting HMMs transitioning to successors in each frame [0(widest)..1(narrowest)]" },
    { "-wbeam",
      ARG_FLOAT64,
      "1.0e-35",
      "Beam selecting word-final HMMs exiting in each frame [0(widest)..1(narrowest)]" },
    { "-ctl",
      ARG_STRING,
      NULL,
      "Control file listing utterances to be processed" },
    { "-utt",
      ARG_STRING,
      NULL,
      "Utterance file to be processed (-ctlcount argument times)" },
    { "-ctloffset",
      ARG_INT32,
      "0",
      "No. of utterances at the beginning of -ctl file to be skipped" },
    { "-ctlcount",
      ARG_INT32,
      "1000000000",	/* A big number to approximate the default: "until EOF" */
      "No. of utterances to be processed (after skipping -ctloffset entries)" },
    { "-cepdir",
      ARG_STRING,
      NULL,
      "Input cepstrum files directory (prefixed to filespecs in control file)" },
    { "-bptbldir",
      ARG_STRING,
      NULL,
      "Directory in which to dump word Viterbi back pointer table (for debugging)" },
    { "-outlatdir",
      ARG_STRING,
      NULL,
      "Directory in which to dump word lattices" },
    { "-outlatoldfmt",
      ARG_INT32,
      "1",
      "Whether to dump lattices in old format" },
    { "-latext",
      ARG_STRING,
      "lat.gz",
      "Filename extension for lattice files (gzip compressed, by default)" },
    { "-hmmdump",
      ARG_INT32,
      "0",
      "Whether to dump active HMM details to stderr (for debugging)" },
    { "-lextreedump",
      ARG_INT32,
      "0",
      "Whether to dump the lextree structure to stderr (for debugging)" },
    { "-maxwpf",
      ARG_INT32,
      "20",
      "Max no. of distinct word exits to maintain at each frame" },
    { "-maxhistpf",
      ARG_INT32,
      "100",
      "Max no. of histories to maintain at each frame" },
    { "-bghist",
      ARG_INT32,
      "0",
      "Bigram-mode: If TRUE only one BP entry/frame; else one per LM state" },
    { "-maxhmmpf",
      ARG_INT32,
      "20000",
      "Max no. of active HMMs to maintain at each frame; approx." },
    { "-hmmhistbinsize",
      ARG_INT32,
      "5000",
      "Performance histogram: #frames vs #HMMs active; #HMMs/bin in this histogram" },
    { "-ptranskip",
      ARG_INT32,
      "0",
      "Use wbeam for phone transitions every so many frames (if >= 1)" },
    { "-hypseg",
      ARG_STRING,
      NULL,
      "Recognition result file, with word segmentations and scores" },
    { "-treeugprob",
      ARG_INT32,
      "1",
      "If TRUE (non-0), Use unigram probs in lextree" },
    { "-maxhyplen",
      ARG_INT32,
      "1000",
      "Maximum number of words in a partial hypothesis (for block decoding)" },
    { "-maxcepvecs",
      ARG_INT32,
      "256",
      "Maximum number of cepstral vectors that can be obtained from a single sample buffer" },
    { "-samprate",
      ARG_INT32,
      "8000",
      "Sampling rate (only 8K and 16K currently supported)" },
    
    { NULL, ARG_INT32, NULL, NULL }
};

static char **liveargs = NULL;		/* RAH, make global so we can free it later */

void  parse_args_file(char *live_args)
{
  /*    static char **liveargs; */	/* RAH, 4.17.01 */
    static int32 nliveargs;
    int32 nargs, maxarglen;
    char  *argline, *targ; 
    FILE *fp;

    if ((fp = fopen(live_args,"r")) == NULL)
	E_FATAL("Unable to open arguments file %s for reading\n",live_args);

    argline = (char*) ckd_calloc(10000,sizeof(char)); /* Longest line allowed */
    nargs = 1;
    maxarglen = 0;
    while (fgets(argline,10000,fp) != NULL){
        if ((targ = strtok(argline," \t\n")) == NULL)
            continue; /* Empty line in argfile */
      if ((int32) strlen(targ) > maxarglen) maxarglen = strlen(targ);
	nargs++; 

        while ((targ = strtok(NULL," \t\n")) != NULL){
	if ((int32) strlen(targ) > maxarglen) maxarglen = strlen(targ);
	    nargs++; 
	}
    }
    rewind(fp);

    nliveargs = nargs;
    liveargs = (char**) ckd_calloc_2d(nargs,maxarglen+1,sizeof(char));

    nargs = 1;
    while (fgets(argline,10000,fp) != NULL){
        if ((targ = strtok(argline," \t\n")) == NULL)
            continue; /* Empty line in argfile */

        strcpy(liveargs[nargs++],targ);
        while ((targ = strtok(NULL," \t\n")) != NULL){
            strcpy(liveargs[nargs++],targ);
	}
    }
    fclose(fp);

    assert(nargs == nliveargs);
    free(argline);

    cmd_ln_parse(arg, nliveargs, liveargs);

    return;
}

/* RAH, 4.17.01, free memory that was allocated above */
void parse_args_free()
{
  cmd_ln_free();		/* Free stuff allocated in cmd_ln_parse */
  ckd_free_2d ((void **) liveargs);
}
