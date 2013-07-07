if BUILD_SWIG

# TODO: use AC_SUBST for LIBADD and CPPFLAGS
# TODO: use conditional to determine sphinxbase.{so,la} location

SUBDIRS = test

SWIG_DIR = $(top_srcdir)/swig

SWIG_FILES = \
	$(SWIG_DIR)/pocketsphinx.i \
	$(SWIG_DIR)/cmd_ln.i \
	$(SWIG_DIR)/fe.i \
	$(SWIG_DIR)/feat.i \
	$(SWIG_DIR)/jsgf.i \
	$(SWIG_DIR)/fsg_model.i \
	$(SWIG_DIR)/ngram_model.i \
	$(SWIG_DIR)/ps_decoder.i \
	$(SWIG_DIR)/ps_lattice.i

CLEANFILES = pocketsphinx_wrap.c pocketsphinx.py

pkgpython_PYTHON = __init__.py 
nodist_pkgpython_PYTHON = pocketsphinx.py
pkgpyexec_LTLIBRARIES = _pocketsphinx.la
nodist__pocketsphinx_la_SOURCES = pocketsphinx_wrap.c
_pocketsphinx_la_CPPFLAGS = \
	-I$(sphinxbase)/include \
	-I$(top_srcdir)/include \
	$(PYTHON_CPPFLAGS)
_pocketsphinx_la_LDFLAGS = -module
_pocketsphinx_la_LIBADD = \
	-lsphinxbase \
	$(top_builddir)/src/libpocketsphinx/libpocketsphinx.la

pocketsphinx_wrap.c: $(SWIG_FILES)
	$(SWIG) -I$(top_srcdir)/include -outdir . -o $@ -python $<

endif
