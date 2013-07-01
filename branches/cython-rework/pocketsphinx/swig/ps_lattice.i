%extend Lattice {
  Lattice(ps_lattice_t *ptr) {
    return ptr;
  }

  ~Lattice() {
    ps_lattice_free($self);
  }

  int write(char const *path) {
    return ps_lattice_write($self, path);
  }

  int write_htk(char const *path) {
    return ps_lattice_write_htk($self, path);
  }
}
