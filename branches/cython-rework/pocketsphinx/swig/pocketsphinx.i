/* -*- c-basic-offset: 4; indent-tabs-mode: nil -*- */
/* ====================================================================
 * Copyright (c) 2013 Carnegie Mellon University.  All rights
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


%module pocketsphinx

// TODO: use camelcase names for Java, underscore names for Python, see %rename
// TODO: use %newobject
// TODO: create exception handling for the functions returning error codes

%include file.i

#ifdef SWIGJAVA
%include <arrays_java.i>
#endif

%{
#include <sphinxbase/cmd_ln.h>
#include <sphinxbase/jsgf.h>
#include <sphinxbase/fe.h>
#include <sphinxbase/feat.h>
#include <sphinxbase/ngram_model.h>
#include <sphinxbase/err.h>

#include <pocketsphinx.h>

typedef int bool;
#define false 0
#define true 1

typedef cmd_ln_t Config;
typedef jsgf_t Jsgf;
typedef feat_t Feature;
typedef fe_t FrontEnd;
typedef fsg_model_t FsgModel;
typedef ngram_model_t NgramModel;

typedef ps_decoder_t Decoder;
typedef ps_lattice_t Lattice;
%}

%inline {
typedef struct {
  char *hypstr;
  char *uttid;
  int best_score;
} Hypothesis;

typedef struct {
  ps_seg_t *ptr;
  char *word;
  int32 ascr;
  int32 lscr;
  int32 lback;
  int start_frame;
  int end_frame;
} Segment;

typedef struct {
  ps_nbest_t *ptr;
} Nbest;
}

#ifdef SWIGJAVA
/* Special typemap for arrays of audio. */
%typemap(in) (short const *SDATA, size_t NSAMP) {
  $1 = (short const *) JCALL2(GetShortArrayElements, jenv, $input, NULL);
  $2 = JCALL1(GetArrayLength, jenv, $input);
};

%typemap(freearg) (short const *SDATA, size_t NSAMP) {
  JCALL3(ReleaseShortArrayElements, jenv, $input, $1, 0);
}

%typemap(jni) (short const *SDATA, size_t NSAMP) "jshortArray"
%typemap(jtype) (short const *SDATA, size_t NSAMP) "short[]"
%typemap(jstype) (short const *SDATA, size_t NSAMP) "short[]"
%typemap(javain) (short const *SDATA, size_t NSAMP) "$javainput"
#endif

typedef struct {} Config;
typedef struct {} FrontEnd;
typedef struct {} Feature;
typedef struct {} Jsgf;
typedef struct {} FsgModel;
typedef struct {} NgramModel;

typedef struct {} Decoder;
typedef struct {} Lattice;

%include cmd_ln.i
%include fe.i
%include feat.i
%include fsg_model.i
%include jsgf.i
%include ngram_model.i

%include ps_decoder.i
%include ps_lattice.i

%extend Hypothesis {
  Hypothesis(char const *hypstr, char const *uttid, int best_score) {
    Hypothesis *h = ckd_malloc(sizeof *h);
    if (hypstr)
      h->hypstr = ckd_salloc(hypstr);
    if (uttid)
      h->uttid = ckd_salloc(uttid);
    h->best_score = best_score;
    return h;
    
  }

  ~Hypothesis() {
    ckd_free($self->hypstr);
    ckd_free($self->uttid);
    ckd_free($self);
  }
}

#ifdef SWIGPYTHON
%exception next() {
  $action
  if (!arg1->ptr) {
    PyErr_SetString(PyExc_StopIteration, "end of iteration");
    SWIG_fail;
  }
}
#endif

%extend Nbest {
  Nbest(ps_nbest_t *ptr) {
    if (!ptr)
      return NULL;

    Nbest *nbest = ckd_malloc(sizeof *nbest);
    nbest->ptr = ptr;

    return nbest;
  }

  ~Nbest() {
      ps_nbest_free($self->ptr);
      ckd_free($self);
  }
  
#ifdef SWIGPYTHON
  Nbest * __iter__() {
    return $self;
  }

  Nbest * next() {
    $self->ptr = ps_nbest_next($self->ptr);
    return $self;
  }
#endif
  
  Hypothesis * hyp() {
      int32 score;
      const char *hyp = ps_nbest_hyp($self->ptr, &score);
      return new_Hypothesis(hyp, "", score);
  }

  Segment * seg() {
    int32 score;
    return new_Segment(ps_nbest_seg($self->ptr, &score));
  }
}

%extend Segment {
  Segment(ps_seg_t *ptr) {
    if (!ptr)
      return NULL;

    Segment *seg = ckd_malloc(sizeof *seg);
    seg->ptr = ptr;
    seg->word = 0;

    return seg;
  }

  ~Segment() {
    ps_seg_free($self->ptr);
    ckd_free($self->word);
    ckd_free($self);
  }

#ifdef SWIGPYTHON
  Segment * __iter__() {
    return $self;
  }  

  Segment * next() {
    if (($self->ptr = ps_seg_next($self->ptr))) {
      $self->word = ckd_salloc(ps_seg_word($self->ptr));
      ps_seg_prob($self->ptr, &$self->ascr, &$self->lscr, &$self->lback);
      ps_seg_frames($self->ptr, &$self->start_frame, &$self->end_frame);
    }

    return $self;
  }
#endif
}

// Clear exception handler for 'next'.
%exception;

%newobject _datafile_path;

%inline {
/* Static method to set the logging file. */
// TODO: use underscore name
void setLogFile(char const *path)
{
  err_set_logfile(path);
}

// Helper function to locate data files.
const char * _datafile_path(const char *fname)
{
  char *buf = (char *) strcpy(ckd_malloc(1024 * sizeof *buf), DATADIR);
  return strcat(buf, fname);
}

}

%pythoncode {

def _resource_path(filename):
  import os
  local_path = os.path.join('../../../test/data', filename)
  if os.path.exists(local_path):
    return local_path
  else:
    return _datafile_path(filename)

}
