pkglibexec_PROGRAMS = mk_s2sendump

mk_s2sendump_SOURCES = another_mdef.h another_s3types.h another_senone.h bio.c bio.h feat.c feat.h logs3.c logs3.h mk_s2sendump.c parse_cmd_ln.c parse_cmd_ln.h s3types.h 

LDADD = \
	$(top_builddir)/src/libs/libcep_feat/libcepfeat.la \
	$(top_builddir)/src/libs/libclust/libclust.la \
	$(top_builddir)/src/libs/libcommon/libcommon.la \
	$(top_builddir)/src/libs/libio/libio.la \
	$(top_builddir)/src/libs/libmllr/libmllr.la \
	$(top_builddir)/src/libs/libmodinv/libmodinv.la


INCLUDES = -I$(top_srcdir)/include

