bin_PROGRAMS = mk_s3mixw

mk_s3mixw_SOURCES = mk_s3mixw.c parse_cmd_ln.c parse_cmd_ln.h 

LDADD = \
	$(top_builddir)/src/libs/libcep_feat/libcepfeat.la \
	$(top_builddir)/src/libs/libclust/libclust.la \
	$(top_builddir)/src/libs/libcommon/libcommon.la \
	$(top_builddir)/src/libs/libio/libio.la \
	$(top_builddir)/src/libs/libmllr/libmllr.la \
	$(top_builddir)/src/libs/libmodinv/libmodinv.la


INCLUDES = -I$(top_srcdir)/include

