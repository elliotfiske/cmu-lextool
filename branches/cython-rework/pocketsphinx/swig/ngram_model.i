%extend NgramModel {
  NgramModel(ngram_model_t *ptr) {
    return ptr;
  }

  ~NgramModel() {
    ngram_model_free($self);
  }

  int write(const char *path, ngram_file_type_t ftype) {
    return ngram_model_write($self, path, ftype);
  }

  // TODO: make static
  ngram_file_type_t str_to_type(const char *str) {
    return ngram_str_to_type(str);
  }

  // TODO: make static
  const char *type_to_str(int type) {
    return ngram_type_to_str(type);
  }

  int recode(const char *from, const char *to) {
    return ngram_model_recode($self, from, to);
  }

  int casefold(int kase) {
    return ngram_model_casefold($self, kase);
  }

  int32 size() {
    return ngram_model_get_size($self);
  }

  int32 add_word(const char *word, float32 weight) {
    return ngram_model_add_word($self, word, weight);
  }

  int32 add_class(const char *c, float32 w, char **words,
    const float32 *weights, int32 nwords)
  {
    return ngram_model_add_class($self, c, w, words, weights, nwords);
  }
}
