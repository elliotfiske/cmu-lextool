/* -*- c-basic-offset: 4; indent-tabs-mode: nil -*- */
/* ====================================================================
 * Copyright (c) 1999-2010 Carnegie Mellon University.  All rights
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
 * debug_vad.c - Simple pocketsphinx command-line application to debug
 *               implemented voice activity detection. Takes audio file,
 *               outputs file with vad decisions for each processed frame.
 * 
 * !!!Currently the tool is useless, but in future 
 * "utterance detector" can be developed and tested here
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <sphinxbase/err.h>

#include "pocketsphinx.h"

static const arg_t cont_args_def[] = {
    POCKETSPHINX_OPTIONS,
    /* Argument file. */
    {"-argfile",
     ARG_STRING,
     NULL,
     "Argument file giving extra arguments."},
    {"-infile",
     ARG_STRING,
     NULL,
     "Audio file to transcribe."},
    CMDLN_EMPTY_OPTION
};

static ps_decoder_t *ps;
static cmd_ln_t *config;
static FILE *rawfd;

static void
recognize_from_file()
{
	int n;
	char const *hyp;
	char const *uttid;
	char buffer[1024];
	
    if ((rawfd = fopen(cmd_ln_str_r(config, "-infile"), "rb")) == NULL) {
        E_FATAL_SYSTEM("Failed to open file '%s' for reading",
                       cmd_ln_str_r(config, "-infile"));
    }
    fread(buffer, 1, 44, rawfd); //skip wav header
    if ((n = fread(buffer, sizeof(int16), 
		cmd_ln_int_r(config, "-wlen")*cmd_ln_float32_r(config, "-samprate"), rawfd)) < 0) {
		E_FATAL_SYSTEM("Error reading from '%s'",
           cmd_ln_str_r(config, "-infile"));
	}
    
    
    ps_start_utt(ps, NULL);
    //process first buffer with window length
    ps_process_raw(ps, buffer, n, FALSE, FALSE);
    while ((n = fread(buffer, sizeof(int16), 
			cmd_ln_float32_r(config, "-samprate")/cmd_ln_int_r(config, "-frate"), rawfd)) > 0) {
		//process some data, to compose next frame inside ps
		ps_process_raw(ps, buffer, n, FALSE, FALSE);
	}
	
	ps_end_utt(ps);
    hyp = ps_get_hyp(ps, NULL, &uttid);
    E_INFO("Recognition result: [%s]\n", hyp);
	fclose(rawfd);
}

int
main(int argc, char *argv[])
{
    char const *cfg;

    if (argc == 2) {
        config = cmd_ln_parse_file_r(NULL, cont_args_def, argv[1], TRUE);
    }
    else {
        config = cmd_ln_parse_r(NULL, cont_args_def, argc, argv, FALSE);
    }
    /* Handle argument file as -argfile. */
    if (config && (cfg = cmd_ln_str_r(config, "-argfile")) != NULL) {
        config = cmd_ln_parse_file_r(config, cont_args_def, cfg, FALSE);
    }
    if (config == NULL)
        return 1;

    ps = ps_init(config);
    if (ps == NULL)
        return 1;

    E_INFO("%s COMPILED ON: %s, AT: %s\n\n", argv[0], __DATE__, __TIME__);

    if (cmd_ln_str_r(config, "-infile") == NULL) {
		E_ERROR("Input audio file isn't specified\n");
		return 1;
	}
	
	E_INFO("infile: [%s]\n", cmd_ln_str_r(config, "-infile"));
	recognize_from_file();
    ps_free(ps);
    return 0;
}
