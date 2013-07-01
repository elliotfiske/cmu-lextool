%extend Decoder {
  /* Following functions have no bindings:
   * arg_t ps_args
   * TODO: implement reference counting
   * TODO: check Decoder for multiple return values
   * ps_decoder_t *ps_retain
   * fe_t *ps_get_fe - requires fe_t implementation
   * feat_t *ps_get_feat - requires feat_t implementation
   * ps_mllr_t *ps_update_mllr - requires 
   * ngram_model_t *ps_get_lmset
   * ngram_model_t *ps_update_lmset
   * fsg_set_t *ps_get_fsgset
   * fsg_set_t *ps_update_fsgset
   * int ps_decode_senscr
   * int ps_process_cep
   */

  // TODO: underscore names of methods
  // TODO: check for multiple values
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

  int reinit(Config *config) {
    return ps_reinit($self, config);
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
    return ps_lattice_retain(ps_get_lattice($self));
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

  Hypothesis * hyp() {
    char const *hyp, *uttid;
    int32 best_score;
    hyp = ps_get_hyp($self, &best_score, &uttid);

    return new_Hypothesis(hyp, uttid, best_score);
  }

  Nbest * nbest() {
    return new_Nbest(ps_nbest($self, 0, -1, NULL, NULL));
  }

  Segment * seg() {
    int32 best_score;
    return new_Segment(ps_seg_iter($self, &best_score));
  }

  int n_frames() {
    return ps_get_n_frames($self);
  }
}
