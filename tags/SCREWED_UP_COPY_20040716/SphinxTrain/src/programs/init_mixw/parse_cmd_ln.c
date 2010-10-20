/* ====================================================================
 * Copyright (c) 1995-2000 Carnegie Mellon University.  All rights 
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
 * 4. Redistributions of any form whatsoever must retain the following
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
/*********************************************************************
 *
 * File: parse_cmd_ln.c
 * 
 * Description: 
 * 
 * Author: 
 * 	Eric H. Thayer (eht@cs.cmu.edu)
 *********************************************************************/

#include "parse_cmd_ln.h"

#include <s3/cmd_ln.h>
#include <s3/s3.h>

#include <sys_compat/misc.h>
#include <sys_compat/file.h>

#include <stdio.h>
#include <assert.h>

/* defines, parses and (partially) validates the arguments
   given on the command line */

int
parse_cmd_ln(int argc, char *argv[])
{
    static arg_def_t defn[] = {
	{ "-src_moddeffn",
	  CMD_LN_STRING,
	  CMD_LN_NO_VALIDATION,
	  CMD_LN_NO_DEFAULT,
	  "The source model definition file name"},
	{ "-src_ts2cbfn",
	  CMD_LN_STRING,
	  CMD_LN_NO_VALIDATION,
	  CMD_LN_NO_DEFAULT,
	  "The source state definition file name"},
	{ "-src_mixwfn",
	  CMD_LN_STRING,
	  CMD_LN_NO_VALIDATION,
	  CMD_LN_NO_DEFAULT,
	  "The source mixing weight file name"},
	{ "-src_meanfn",
	  CMD_LN_STRING,
	  CMD_LN_NO_VALIDATION,
	  CMD_LN_NO_DEFAULT,
	  "The source mean file name"},
	{ "-src_varfn",
	  CMD_LN_STRING,
	  CMD_LN_NO_VALIDATION,
	  CMD_LN_NO_DEFAULT,
	  "The source variance file name"},
	{ "-src_tmatfn",
	  CMD_LN_STRING,
	  CMD_LN_NO_VALIDATION,
	  CMD_LN_NO_DEFAULT,
	  "The source transition matrix file name"},

	{ "-dest_moddeffn",
	  CMD_LN_STRING,
	  CMD_LN_NO_VALIDATION,
	  CMD_LN_NO_DEFAULT,
	  "The destination model definition file name"},
	{ "-dest_ts2cbfn",
	  CMD_LN_STRING,
	  CMD_LN_NO_VALIDATION,
	  CMD_LN_NO_DEFAULT,
	  "The destination state definition file name"},
	{ "-dest_mixwfn",
	  CMD_LN_STRING,
	  CMD_LN_NO_VALIDATION,
	  CMD_LN_NO_DEFAULT,
	  "The destination mixing weight file name"},
	{ "-dest_meanfn",
	  CMD_LN_STRING,
	  CMD_LN_NO_VALIDATION,
	  CMD_LN_NO_DEFAULT,
	  "The destination mean file name"},
	{ "-dest_varfn",
	  CMD_LN_STRING,
	  CMD_LN_NO_VALIDATION,
	  CMD_LN_NO_DEFAULT,
	  "The destination variance file name"},
	{ "-dest_tmatfn",
	  CMD_LN_STRING,
	  CMD_LN_NO_VALIDATION,
	  CMD_LN_NO_DEFAULT,
	  "The destination transition matrix file name"},

	{ "-feat",
	  CMD_LN_STRING,
	  CMD_LN_NO_VALIDATION,
	  "1s_12c_12d_3p_12dd",
	  "Derived feature computation to use"},
	{ "-ceplen",
	  CMD_LN_INT32,
	  CMD_LN_NO_VALIDATION,
	  "13",
	  "Size of the input feature vector length"},
	{ NULL, CMD_LN_UNDEF, CMD_LN_NO_VALIDATION, CMD_LN_NO_DEFAULT, NULL }
    };

    cmd_ln_define(defn);

    if (argc == 1) {
	cmd_ln_print_definitions();
	exit(1);
    }

    cmd_ln_parse(argc, argv);

    if (cmd_ln_validate() == FALSE) {
	/* one or more command line arguments were
	   deemed invalid */
	exit(1);
    }

    cmd_ln_print_configuration();

    return 0;
}

/*
 * Log record.  Maintained by RCS.
 *
 * $Log$
 * Revision 1.3  2001/04/05  20:02:31  awb
 * *** empty log message ***
 * 
 * Revision 1.2  2000/09/29 22:35:14  awb
 * *** empty log message ***
 *
 * Revision 1.1  2000/09/24 21:38:31  awb
 * *** empty log message ***
 *
 * Revision 1.2  97/07/16  11:36:22  eht
 * *** empty log message ***
 * 
 * Revision 1.1  1995/10/05  12:54:02  eht
 * Initial revision
 *
 * Revision 1.2  1995/09/07  19:59:50  eht
 * Include defn for TRUE/FALSE
 *
 * Revision 1.1  1995/06/02  20:55:11  eht
 * Initial revision
 *
 *
 */