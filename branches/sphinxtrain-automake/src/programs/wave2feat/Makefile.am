bin_PROGRAMS = wave2feat

wave2feat_SOURCES = cmd_ln_defn.h fe.h fe_interface.c fe_internal.h fe_sigproc.c fe_warp_affine.c fe_warp_affine.h fe_warp.c fe_warp.h fe_warp_inverse_linear.c fe_warp_inverse_linear.h fe_warp_piecewise_linear.c fe_warp_piecewise_linear.h parse_cmd_ln.c parse_cmd_ln.h wave2feat.c wave2feat.h 

LDADD = \
	$(top_builddir)/src/libs/libcep_feat/libcepfeat.la \
	$(top_builddir)/src/libs/libclust/libclust.la \
	$(top_builddir)/src/libs/libcommon/libcommon.la \
	$(top_builddir)/src/libs/libio/libio.la \
	$(top_builddir)/src/libs/libmllr/libmllr.la \
	$(top_builddir)/src/libs/libmodinv/libmodinv.la


INCLUDES = -I$(top_srcdir)/include

