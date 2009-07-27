/* -*- c-basic-offset: 4; indent-tabs-mode: nil -*- */
/* ====================================================================
 * Copyright (c) 2006 Carnegie Mellon University.  All rights
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

/* cmdln_macro.h - Command line definitions for PocketSphinx */

#ifndef __PS_CMDLN_MACRO_H__
#define __PS_CMDLN_MACRO_H__

#include <cmd_ln.h>
#include <feat.h>
#include <fe.h>

/** Minimal set of command-line options for PocketSphinx. */
#define POCKETSPHINX_OPTIONS \
    waveform_to_cepstral_command_line_macro(), \
    cepstral_to_feature_command_line_macro(), \
    POCKETSPHINX_ACMOD_OPTIONS, \
        POCKETSPHINX_BEAM_OPTIONS,   \
        POCKETSPHINX_SEARCH_OPTIONS, \
        POCKETSPHINX_DICT_OPTIONS, \
        POCKETSPHINX_NGRAM_OPTIONS, \
        POCKETSPHINX_FSG_OPTIONS, \
        POCKETSPHINX_DEBUG_OPTIONS

/** Options for debugging and logging. */
#define POCKETSPHINX_DEBUG_OPTIONS                      \
    { "-logfn",                                         \
            ARG_STRING,                                 \
            NULL,                                       \
            "File to write log messages in"             \
     },                                                 \
     { "-mfclogdir",                                    \
             ARG_STRING,                                \
             NULL,                                      \
             "Directory to log feature files to"        \
             },                                         \
    { "-rawlogdir",                                     \
            ARG_STRING,                                 \
            NULL,                                       \
            "Directory to log raw audio files to" }

/** Options defining beam width parameters for tuning the search. */
#define POCKETSPHINX_BEAM_OPTIONS                                       \
{ "-beam",                                                              \
      ARG_FLOAT64,                                                      \
      "1e-48",                                                          \
      "Beam width applied to every frame in Viterbi search (smaller values mean wider beam)" }, \
{ "-wbeam",                                                             \
      ARG_FLOAT64,                                                      \
      "7e-29",                                                          \
      "Beam width applied to word exits" },                             \
{ "-pbeam",                                                             \
      ARG_FLOAT64,                                                      \
      "1e-48",                                                          \
      "Beam width applied to phone transitions" },                      \
{ "-lpbeam",                                                            \
      ARG_FLOAT64,                                                      \
      "1e-40",                                                          \
      "Beam width applied to last phone in words" },                    \
{ "-lponlybeam",                                                        \
      ARG_FLOAT64,                                                      \
      "7e-29",                                                          \
      "Beam width applied to last phone in single-phone words" },       \
{ "-fwdflatbeam",                                                       \
      ARG_FLOAT64,                                                      \
      "1e-64",                                                          \
      "Beam width applied to every frame in second-pass flat search" }, \
{ "-fwdflatwbeam",                                                      \
      ARG_FLOAT64,                                                      \
      "7e-29",                                                          \
      "Beam width applied to word exits in second-pass flat search" },  \
{ "-pl_window",                                                         \
      ARG_INT32,                                                        \
      "0",                                                              \
      "Phoneme lookahead window size, in frames" },                     \
{ "-pl_beam",                                                           \
      ARG_FLOAT64,                                                      \
      "1e-10",                                                          \
      "Beam width applied to phone loop search for lookahead" },        \
{ "-pl_pbeam",                                                          \
      ARG_FLOAT64,                                                      \
      "1e-5",                                                           \
      "Beam width applied to phone loop transitions for lookahead" }

/** Options defining other parameters for tuning the search. */
#define POCKETSPHINX_SEARCH_OPTIONS \
{ "-compallsen",                                                                                \
      ARG_BOOLEAN,                                                                              \
      "no",                                                                                     \
      "Compute all senone scores in every frame (can be faster when there are many senones)" }, \
{ "-tst",                                                                                       \
      ARG_BOOLEAN,                                                                              \
      "no",                                                                                     \
      "Run time-switch tree search (a.k.a. Sphinx3 search)" },                                  \
{ "-Nlextree", \
      ARG_INT32, \
      "3", \
      "(tst only) No. of lextrees to be instantiated; entries into them staggered in time" }, \
{ "-epl", \
      ARG_INT32, \
      "3", \
      "(tst only) Entries Per Lextree; #successive entries into one lextree before lextree-entries shifted to the next" }, \
{ "-maxhistpf", \
      ARG_INT32, \
      "100", \
      "(tst only) Max no. of histories to maintain at each frame" }, \
{ "-hmmhistbinsize", \
      ARG_INT32, \
      "5000", \
      "(tst only) Performance histogram: #frames vs #HMMs active; #HMMs/bin in this histogram" }, \
{ "-fwdtree",                                                                                   \
      ARG_BOOLEAN,                                                                              \
      "yes",                                                                                    \
      "Run forward lexicon-tree search (1st pass)" },                                           \
{ "-fwdflat",                                                                                   \
      ARG_BOOLEAN,                                                                              \
      "yes",                                                                                    \
      "Run forward flat-lexicon search over word lattice (2nd pass)" },                         \
{ "-bestpath",                                                                                  \
      ARG_BOOLEAN,                                                                              \
      "yes",                                                                                    \
      "Run bestpath (Dijkstra) search over word lattice (3rd pass)" },                          \
{ "-backtrace",                                                                                 \
      ARG_BOOLEAN,                                                                              \
      "no",                                                                                     \
      "Print results and backtraces to log file." },                                            \
{ "-latsize",                                                                                   \
      ARG_INT32,                                                                                \
      "5000",                                                                                   \
      "Initial backpointer table size" },                                                       \
{ "-maxwpf",                                                                                    \
      ARG_INT32,                                                                                \
      "-1",                                                                                     \
      "Maximum number of distinct word exits at each frame (or -1 for no pruning)" },           \
{ "-maxhmmpf",                                                                                  \
      ARG_INT32,                                                                                \
      "-1",                                                                                     \
      "Maximum number of active HMMs to maintain at each frame (or -1 for no pruning)" },       \
{ "-fwdflatefwid",                                                                              \
      ARG_INT32,                                                                                \
      "4",                                                                     	                \
      "Minimum number of end frames for a word to be searched in fwdflat search" },             \
{ "-fwdflatsfwin",                                                                              \
      ARG_INT32,                                                                                \
      "25",                                                                    	                \
      "Window of frames in lattice to search for successor words in fwdflat search " }

/** Command-line options for finite state grammars. */
#define POCKETSPHINX_FSG_OPTIONS \
    { "-fsg",                                                   \
            ARG_STRING,                                         \
            NULL,                                               \
            "Sphinx format finite state grammar file"},         \
{ "-jsgf",                                                      \
        ARG_STRING,                                             \
        NULL,                                                   \
        "JSGF grammar file" },                                  \
{ "-toprule",                                                   \
        ARG_STRING,                                             \
        NULL,                                                   \
        "Start rule for JSGF (first public rule is default)" }, \
{ "-fsgusealtpron",                                             \
        ARG_BOOLEAN,                                            \
        "yes",                                                  \
        "Add alternate pronunciations to FSG"},                 \
{ "-fsgusefiller",                                              \
        ARG_BOOLEAN,                                            \
        "yes",                                                  \
        "Insert filler words at each state."}

/** Command-line options for statistical language models. */
#define POCKETSPHINX_NGRAM_OPTIONS \
{ "-lm",										\
      ARG_STRING,									\
      NULL,										\
      "Word trigram language model input file" },					\
{ "-lmctl",										\
      ARG_STRING,									\
      NULL,										\
      "Specify a set of language model\n"},						\
{ "-lmname",										\
      ARG_STRING,									\
      "default",									\
      "Which language model in -lmctl to use by default"},				\
{ "-lw",										\
      ARG_FLOAT32,									\
      "6.5",										\
      "Language model probability weight" },						\
{ "-fwdflatlw",										\
      ARG_FLOAT32,									\
      "8.5",										\
      "Language model probability weight for flat lexicon (2nd pass) decoding" },	\
{ "-bestpathlw",									\
      ARG_FLOAT32,									\
      "9.5",										\
      "Language model probability weight for bestpath search" },			\
{ "-ascale",										\
      ARG_FLOAT32,									\
      "20.0",										\
      "Inverse of acoustic model scale for confidence score calculation" },		\
{ "-wip",										\
      ARG_FLOAT32,									\
      "0.65",										\
      "Word insertion penalty" },							\
{ "-nwpen",										\
      ARG_FLOAT32,									\
      "1.0",										\
      "New word transition penalty" },							\
{ "-pip",										\
      ARG_FLOAT32,									\
      "1.0",										\
      "Phone insertion penalty" },							\
{ "-uw",										\
      ARG_FLOAT32,									\
      "1.0",										\
      "Unigram weight" }, 								\
{ "-silprob",										\
      ARG_FLOAT32,									\
      "0.005",										\
      "Silence word transition probability" },						\
{ "-fillprob",										\
      ARG_FLOAT32,									\
      "1e-8",										\
        "Filler word transition probability" }, \
{ "-bghist",   \
      ARG_BOOLEAN, \
      "no", \
      "Bigram-mode: If TRUE only one BP entry/frame; else one per LM state" }, \
{ "-lextreedump", \
      ARG_INT32, \
      "0", \
      "Whether to dump the lextree structure to stderr (for debugging), 1 for Ravi's format, 2 for Dot format, Larger than 2 will be treated as Ravi's format" }

/** Command-line options for dictionaries. */
#define POCKETSPHINX_DICT_OPTIONS \
    { "-dict",							\
      REQARG_STRING,						\
      NULL,							\
      "Main pronunciation dictionary (lexicon) input file" },	\
    { "-fdict",							\
      ARG_STRING,						\
      NULL,							\
      "Noise word pronunciation dictionary input file" },	\
    { "-dictcase",						\
      ARG_BOOLEAN,						\
      "no",							\
      "Dictionary is case sensitive (NOTE: case insensitivity applies to ASCII characters only)" },	\
    { "-maxnewoov",						\
      ARG_INT32,						\
      "20",							\
      "Maximum new OOVs that can be added at run time" },	\
    { "-usewdphones",						\
      ARG_BOOLEAN,						\
      "no",							\
      "Use within-word phones only" }

/** Command-line options for acoustic modeling */
#define POCKETSPHINX_ACMOD_OPTIONS \
{ "-hmm",                                                                       \
      ARG_STRING,                                                               \
      NULL,                                                                     \
      "Directory containing acoustic model files."},                            \
{ "-featparams",                                                                \
      ARG_STRING,                                                               \
      NULL,                                                                     \
      "File containing feature extraction parameters."},                        \
{ "-mdef",                                                                      \
      ARG_STRING,                                                               \
      NULL,                                                                     \
      "Model definition input file" },                                          \
{ "-tmat",                                                                      \
      ARG_STRING,                                                               \
      NULL,                                                                     \
      "HMM state transition matrix input file" },                               \
{ "-tmatfloor",                                                                 \
      ARG_FLOAT32,                                                              \
      "0.0001",                                                                 \
      "HMM state transition probability floor (applied to -tmat file)" },       \
{ "-mean",                                                                      \
      ARG_STRING,                                                               \
      NULL,                                                                     \
      "Mixture gaussian means input file" },                                    \
{ "-var",                                                                       \
      ARG_STRING,                                                               \
      NULL,                                                                     \
      "Mixture gaussian variances input file" },                                \
{ "-varfloor",                                                                  \
      ARG_FLOAT32,                                                              \
      "0.0001",                                                                 \
      "Mixture gaussian variance floor (applied to data from -var file)" },     \
{ "-mixw",                                                                      \
      ARG_STRING,                                                               \
      NULL,                                                                     \
      "Senone mixture weights input file (uncompressed)" },                     \
{ "-mixwfloor",                                                                 \
      ARG_FLOAT32,                                                              \
      "0.0000001",                                                              \
      "Senone mixture weights floor (applied to data from -mixw file)" },       \
{ "-sendump",                                                                   \
      ARG_STRING,                                                               \
      NULL,                                                                     \
      "Senone dump (compressed mixture weights) input file" },                  \
{ "-mllr",                                                                      \
      ARG_STRING,                                                               \
      NULL,                                                                     \
      "MLLR transformation to apply to means and variances" },                  \
{ "-mmap",                                                                      \
      ARG_BOOLEAN,                                                              \
      "yes",                                                                    \
      "Use memory-mapped I/O (if possible) for model files" },                  \
{ "-ds",                                                                        \
      ARG_INT32,                                                                \
      "1",                                                                      \
      "Frame GMM computation downsampling ratio" },                             \
{ "-topn",                                                                      \
      ARG_INT32,                                                                \
      "4",                                                                      \
      "Maximum number of top Gaussians to use in scoring." },                   \
{ "-topn_beam",                                                                 \
      ARG_STRING,                                                               \
      "0",                                                                     \
      "Beam width used to determine top-N Gaussians (or a list, per-feature)" },\
{ "-kdtree",                                                                    \
      ARG_STRING,                                                               \
      NULL,                                                                     \
      "kd-Tree file for Gaussian selection" },                                  \
{ "-kdmaxdepth",                                                                \
      ARG_INT32,                                                                \
      "0",                                                                      \
      "Maximum depth of kd-Trees to use" },                                     \
{ "-kdmaxbbi",                                                                  \
      ARG_INT32,                                                                \
      "-1",                                                                     \
      "Maximum number of Gaussians per leaf node in kd-Trees" },                \
{ "-logbase",                                                                   \
      ARG_FLOAT32,                                                              \
      "1.0001",                                                                 \
      "Base in which all log-likelihoods calculated" }

#define CMDLN_EMPTY_OPTION { NULL, 0, NULL, NULL }

#endif /* __PS_CMDLN_MACRO_H__ */
