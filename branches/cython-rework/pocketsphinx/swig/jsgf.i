%extend Jsgf {
  Jsgf(Jsgf *parent) {
    return jsgf_grammar_new(parent);
  }

  Jsgf(const char *path, Jsgf *parent) {
    return jsgf_parse_file(path, parent);
  }

  ~Jsgf() {
    jsgf_grammar_free($self);
  }

  const char * name() {
    return jsgf_grammar_name($self);
  }
}
