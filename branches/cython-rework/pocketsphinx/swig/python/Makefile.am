if BUILD_SWIG

# TODO: use AC_SUBST for LIBADD and CPPFLAGS
# TODO: take SWIG_DIR and SWIG_FILES out to swig/Makefile
# TODO: fix dependencies

SWIG_DIR = $(top_srcdir)/swig

SWIG_FILES = \
	$(SWIG_DIR)/pocketsphinx.i \
	$(SWIG_DIR)/cmd_ln.i \
	$(SWIG_DIR)/jsgf.i \
	$(SWIG_DIR)/fsg_model.i \
	$(SWIG_DIR)/ps_decoder.i \
	$(SWIG_DIR)/ps_lattice.i

pkgpython_PYTHON = __init__.py 
nodist_pkgpython_PYTHON = pocketsphinx.py
pkgpyexec_LTLIBRARIES = _pocketsphinx.la
nodist__pocketsphinx_la_SOURCES = pocketsphinx_wrap.c
_pocketsphinx_la_CPPFLAGS = \
	-I$(sphinxbase)/include \
	-I$(top_srcdir)/include \
	-I$(SWIG_DIR) \
	$(PYTHON_CPPFLAGS)
_pocketsphinx_la_LDFLAGS = -module
_pocketsphinx_la_LIBADD = \
	-lsphinxbase \
	$(top_srcdir)/src/libpocketsphinx/libpocketsphinx.la
													

pocketsphinx_wrap.c: $(SWIG_FILES)
	$(SWIG) -I$(top_srcdir)/include -outdir . -o $@ -python $<

clean-local :
	$(RM) pocketsphinx_wrap.c pocketsphinx.py

endif
