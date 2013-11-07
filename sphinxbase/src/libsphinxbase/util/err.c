/* -*- c-basic-offset: 4; indent-tabs-mode: nil -*- */
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
/**
 * @file err.c
 * @brief Somewhat antiquated logging and error interface.
 */

#include "config.h"

#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

#include "sphinxbase/err.h"
#include "sphinxbase/prim_type.h"

static FILE*  logfp = NULL;
static int    logfp_disabled = FALSE;

static int sphinx_debug_level;

#if   defined __ANDROID__
static err_cb_f err_cb = err_logcat_cb;
#elif defined _WIN32_WCE
static err_cb_f err_cb = err_wince_cb;
#else
static err_cb_f err_cb = err_logfp_cb;
#endif
static void* err_user_data;

void
err_msg(err_lvl_t lvl, const char *path, long ln, const char *fmt, ...)
{
    char msg[1024];
    char *fname;
    va_list ap;

    if (!err_cb)
        return;

    va_start(ap, fmt);
    vsnprintf(msg, sizeof msg, fmt, ap);
    va_end(ap);

    if (path) {
        fname = strdup(path);
        if (lvl == ERR_INFO)
            err_cb(err_user_data, lvl, "%s(%ld): %s", basename(fname), ln, msg);
        else
    	    err_cb(err_user_data, lvl, "\"%s\", line %ld: %s", basename(fname), ln, msg);
        free(fname);
    } else {
        err_cb(err_user_data, lvl, "%s", msg);
    }
}

#if   defined __ANDROID__
void
err_logcat_cb(void *user_data, err_lvl_t lvl, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    __android_log_vprint(lvl, "cmusphinx", fmt, ap);
    va_end(ap);
}
#elif defined _WIN32_WCE
void
err_wince_cb(void *user_data, err_lvl_t lvl, const char *fmt, ...)
{
    char msg[1024];
    WCHAR *wmsg;
    size_t size;
    va_list ap;

    va_start(ap, fmt);
    _vsnprintf(msg, sizeof(msg), fmt, ap);
    va_end(ap);

    size = mbstowcs(NULL, msg, 0) + 1;
    wmsg = ckd_calloc(size, sizeof(*wmsg));
    mbstowcs(wmsg, msg, size);

    OutputDebugStringW(wmsg);
    ckd_free(wmsg);
}
#else
void
err_logfp_cb(void *user_data, err_lvl_t lvl, const char *fmt, ...)
{
    static const char *err_prefix[ERR_MAX] = {
        "DEBUG", "INFO", "WARN", "ERROR", "FATAL"
    };

    FILE *fp = err_get_logfp();
    if (!fp)
        return;
    
    va_list ap;
    fprintf(fp, "%s: ", err_prefix[lvl]);
    va_start(ap, fmt);
    vfprintf(fp, fmt, ap);
    va_end(ap);
}
#endif

int
err_set_logfile(const char *path)
{
    FILE *newfp, *oldfp;

    if ((newfp = fopen(path, "a")) == NULL)
        return -1;
    oldfp = err_get_logfp();
    err_set_logfp(newfp);
    if (oldfp != NULL && oldfp != stdout && oldfp != stderr)
        fclose(oldfp);
    return 0;
}

void
err_set_logfp(FILE *stream)
{
    if (stream == NULL) {
	logfp_disabled = TRUE;
	logfp = NULL;
	return;
    }    
    logfp_disabled = FALSE;
    logfp = stream;
    return;
}

FILE *
err_get_logfp(void)
{
    if (logfp_disabled)
	return NULL;
    if (logfp == NULL)
	return stderr;

    return logfp;
}

int
err_set_debug_level(int level)
{
    int prev = sphinx_debug_level;
    sphinx_debug_level = level;
    return prev;
}

int
err_get_debug_level(void)
{
    return sphinx_debug_level;
}

void
err_set_callback(err_cb_f cb, void* user_data)
{
    err_cb = cb;
    err_user_data= user_data;
}
