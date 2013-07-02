%extend FrontEnd {
  FrontEnd(fe_t *ptr) {
    return ptr;
  }

  ~FrontEnd() {
    fe_free($self);
  }

  int output_size() {
    return fe_get_output_size($self);
  }

  int start_utt() {
    return fe_start_utt($self);
  }

  int process_utt(const int16 *spch, size_t nsamps, mfcc_t ***cep_block) {
    int32 nframes;
    return fe_process_utt($self, spch, nsamps, cep_block, &nframes);
  }

  int end_utt(mfcc_t *out_cepvector) {
    int32 nframes;
    return fe_end_utt($self, out_cepvector, &nframes);
  }

  int process_frame() {
    return fe_get_output_size($self);
  }
}
