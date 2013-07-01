%extend FsgModel {
  FsgModel(fsg_model_t *ptr) {
    return ptr;
  }

  ~FsgModel() {
    fsg_model_free($self);
  }

  int word_id(const char *word) {
    return fsg_model_word_id($self, word);
  }

  int word_add(const char *word) {
    return fsg_model_word_add($self, word);
  }

  void trans_add(int32 from, int32 to, int32 logp, int32 wid) {
    fsg_model_trans_add($self, from, to, logp, wid);
  }

  int32 null_trans_add(int32 from, int32 to, int32 logp) {
    return fsg_model_null_trans_add($self, from, to, logp);
  }

  int32 tag_trans_add(int32 from, int32 to, int32 logp, int32 wid) {
    return fsg_model_tag_trans_add($self, from, to, logp, wid);
  }

  int add_silence(const char *silword, int state, int32 silprob) {
    return fsg_model_add_silence($self, silword, state, silprob); 
  }

  int add_alt(const char *baseword, const char *altword) {
    return fsg_model_add_alt($self, baseword, altword);
  }
}
