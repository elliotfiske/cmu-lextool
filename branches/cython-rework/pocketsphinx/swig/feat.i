%extend Feature {
  Feature(feat_t *ptr) {
    return ptr;
  }

  ~Feature() {
    feat_free($self);
  }

  int32 s2mfc2feat(const char *path, const char *cepext,
    int32 start_frame, int32 end_frame)
  {
    mfcc_t ***feat;
    return feat_s2mfc2feat($self->ptr, path, NULL, cepext, start_frame,
      end_frame, feat, -1);
  }
};
