%module pocketsphinx
#ifdef SWIGJAVA
%include <arrays_java.i>
#endif
%{
#include <pocketsphinx.h>
#include <sphinxbase/err.h>

/* Typedefs to make Java-esque class names. */
typedef struct cmd_ln_s Config;
typedef struct ps_decoder_s Decoder;
typedef struct ps_lattice_s Lattice;

typedef int bool;
#define false 0
#define true 1

/* Auxiliary objects used to return multiple values. */
typedef struct {
  char *hypstr;
  char *uttid;
  int best_score;
} Hypothesis;

typedef struct {
  ps_nbest_t *ptr;
} Nbest;

typedef struct {
  struct ps_seg_s *ps_seg;
  char const *word;
  int start_frame;
  int end_frame;
  int32 ascr;
  int32 lscr;
  int32 lback;
} Segment;

typedef struct {
  Segment *item;
} SegmentIterator;
%}

typedef struct {
  char *hypstr;
  char *uttid;
  int best_score;
} Hypothesis;

typedef struct {
  ps_nbest_t *ptr;
} Nbest;

typedef struct {
  struct ps_seg_s *ps_seg;
  char const *word;
  int start_frame;
  int end_frame;
  int32 ascr;
  int32 lscr;
  int32 lback;
} Segment;

typedef struct {
  Segment *item;
} SegmentIterator;

/* These are opaque types but we have to "define" them for SWIG. */
typedef struct {} Config;
typedef struct {} Lattice;
typedef struct {} Decoder;

#ifdef SWIGPYTHON
/* Converts a PyFile instance to a stdio FILE* */
%typemap(in) FILE* {
    if ( PyFile_Check($input) ){
        $1 = PyFile_AsFile($input);
    } else {
        PyErr_SetString(PyExc_TypeError, "$1_name must be a file type.");
        return NULL;
    }
}
#endif

#ifdef SWIGJAVA
/* Special typemap for arrays of audio. */
%typemap(in) (short const *SDATA, size_t NSAMP) {
  $1 = (short const *) JCALL2(GetShortArrayElements, jenv, $input, NULL);
  $2 = JCALL1(GetArrayLength, jenv, $input);
};

%typemap(freearg) (short const *SDATA, size_t NSAMP) {
  JCALL3(ReleaseShortArrayElements, jenv, $input, $1, 0);
};

%typemap(jni) (short const *SDATA, size_t NSAMP) "jshortArray"
%typemap(jtype) (short const *SDATA, size_t NSAMP) "short[]"
%typemap(jstype) (short const *SDATA, size_t NSAMP) "short[]"
%typemap(javain) (short const *SDATA, size_t NSAMP) "$javainput"
#endif

%extend Hypothesis {
  Hypothesis(char const *hypstr, char const *uttid, int best_score) {
    Hypothesis *h = ckd_calloc(1, sizeof(*h));
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
};

%extend Config {
  Config() {
    Config *c = cmd_ln_init(NULL, ps_args(), FALSE, NULL);
    return c;
  }
  Config(char const *file) {
    Config *c = cmd_ln_parse_file_r(NULL, ps_args(), file, FALSE);
    return c;
  }
  ~Config() {
    cmd_ln_free_r($self);
  }
  void setBoolean(char const *key, bool val) {
    cmd_ln_set_boolean_r($self, key, val);
  }
  void setInt(char const *key, int val) {
    cmd_ln_set_int_r($self, key, val);
  }
  void setFloat(char const *key, double val) {
    cmd_ln_set_float_r($self, key, val);
  }
  void setString(char const *key, char const *val) {
    cmd_ln_set_str_r($self, key, val);
  }
  bool exists(char const *key) {
    return cmd_ln_exists_r($self, key);
  }
  bool getBoolean(char const *key) {
    return cmd_ln_boolean_r($self, key);
  }
  int getInt(char const *key) {
    return cmd_ln_int_r($self, key);
  }
  double getFloat(char const *key) {
    return cmd_ln_float_r($self, key);
  }
  char const *getString(char const *key) {
    return cmd_ln_str_r($self, key);
  }
};

%extend Segment {
  Segment(ps_seg_t *ps_seg) {
    Segment *seg = ckd_calloc(1, sizeof *seg);
    seg->ps_seg = ps_seg;
    seg->word = ps_seg_word(ps_seg);
    ps_seg_frames(ps_seg, &seg->start_frame, &seg->end_frame);
    ps_seg_prob(ps_seg, &seg->ascr, &seg->lscr, &seg->lback);

    return seg;
  }

  ~Segment() {
    ps_seg_free($self->ps_seg);
    ckd_free($self);
  }
};

%extend SegmentIterator {
  SegmentIterator(Decoder *d) {
    int32 best_score;
    SegmentIterator *iter = ckd_calloc(1, sizeof(*iter));
    iter->item = new_Segment(ps_seg_iter(d, &best_score));

    return iter;
  }

  SegmentIterator(Nbest *nbest) {
    int32 score;
    SegmentIterator *iter = ckd_calloc(1, sizeof(*iter));
    iter->item = new_Segment(ps_nbest_seg(nbest->ptr, &score));

    return iter;
  }

  ~SegmentIterator() {
    ckd_free($self);
  }

  bool next() {
    $self->item = new_Segment(ps_seg_next($self->item->ps_seg));
    return $self->item != NULL;
  }
};

%extend Lattice {
  Lattice(Decoder *d) {
    Lattice *lattice = ps_get_lattice(d);
    return lattice;
  }
};

%extend Nbest {
  Nbest(Decoder *d) {
      Nbest *nbest = ckd_calloc(1, sizeof(*nbest));
      nbest->ptr = ps_nbest(d, 0, -1, NULL, NULL);
      return nbest;
  }

  ~Nbest() {
      ps_nbest_free($self->ptr);
      ckd_free($self);
  }
  
  bool next() {
      $self->ptr = ps_nbest_next($self->ptr);
      return $self->ptr != NULL;
  }
  
  Hypothesis* hyp() {
      const char* hyp;
      int32 score;
      hyp = ps_nbest_hyp($self->ptr, &score);
      return new_Hypothesis(hyp, "", score);
  }

  SegmentIterator * seg() {
    return new_SegmentIterator($self);
  }
};

%extend Decoder {
  /* Following functions have no bindings:
   * ps_args
   * TODO: implement reference counting
   * ps_decoder_t *ps_retain
   * logmath_t *ps_get_logmath
   * fe_t *ps_get_fe
   * feat_t *ps_get_feat
   * ps_mllr_t *ps_update_mllr
   * ngram_model_t *ps_get_lmset
   * ngram_model_t *ps_update_lmset
   * fsg_set_t *ps_get_fsgset
   * fsg_set_t *ps_update_fsgset
   * int ps_decode_senscr
   * int ps_process_cep
   * int ps_get_n_frames
   * char const *ps_get_hyp_final
   * int32 ps_get_prob
   */
  Decoder() {
    Decoder *d = ps_init(cmd_ln_init(NULL, ps_args(), FALSE, NULL));
    return d;
  }

  Decoder(Config *c) {
    Decoder *d = ps_init(c);
    return d;
  }

  ~Decoder() {
    ps_free($self);
  }  

  int loadDict(char const *fdict, char const *ffilter, char const *format) {
    return ps_load_dict($self, fdict, ffilter, format);
  }

  int saveDict(char const *dictfile, char const *format) {
    return ps_save_dict($self, dictfile, format);
  }

  int addWord(char const *word, char const *phones, int update) {
    return ps_add_word($self, word, phones, update);
  }

  Lattice * getLattice() {
    return new_Lattice($self);
  }

  int reinit(Config *config) {
    return ps_reinit($self, config);
  }

  Config *getConfig() {
    return cmd_ln_retain(ps_get_config($self));
  }

  int startUtt() {
    return ps_start_utt($self, NULL);
  }

  int startUtt(char const *uttid) {
    return ps_start_utt($self, uttid);
  }

  char const *getUttid() {
    return ps_get_uttid($self);
  }

  int endUtt() {
    return ps_end_utt($self);
  }

  int processRaw(
    const short const *SDATA, size_t NSAMP, bool no_search, bool full_utt) {
    return ps_process_raw($self, SDATA, NSAMP, no_search, full_utt);
  }

  int decodeRaw(FILE *f) {
    return ps_decode_raw($self, f, 0, -1);
  }

  Hypothesis *getHyp() {
    char const *hyp, *uttid;
    int32 best_score;
    hyp = ps_get_hyp($self, &best_score, &uttid);
    return new_Hypothesis(hyp, uttid, best_score);
  }
};

%inline {
  /* Static method to set the logging file. */
  void setLogFile(char const *path) {
    err_set_logfile(path);
  }
};

