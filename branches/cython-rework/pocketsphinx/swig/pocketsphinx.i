/**
 * SphinxBase headers to process:
 *   fe.h
 *   feat.h
 *   fsg_model.h
 *   ngram_model.h
 *  
 * TODO: use camelcase names for Java, underscore names for Python, see %rename
 *       use %newobject
 */
%module pocketsphinx

#ifdef SWIGJAVA
%include <arrays_java.i>
#endif

%{
#include <sphinxbase/jsgf.h>
#include <sphinxbase/err.h>

#include <pocketsphinx.h>

typedef int bool;
#define false 0
#define true 1

typedef jsgf_t Jsgf;
typedef cmd_ln_t Config;
typedef fsg_model_t FsgModel;

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
typedef struct {} Jsgf;
typedef struct {} FsgModel;

typedef struct {} Decoder;
typedef struct {} Lattice;

%include cmd_ln.i
//%include fe.i
//%include "feat.i"
%include fsg_model.i
%include jsgf.i
//%include "ngram_model.i"

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

%inline {
/* Static method to set the logging file. */
// TODO: use underscore name
void setLogFile(char const *path)
{
  err_set_logfile(path);
}
}
