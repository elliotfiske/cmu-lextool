if BUILD_SWIG

# TODO: use AC_SUBST for LIBADD and CPPFLAGS
# TODO: take SWIG_DIR and SWIG_FILES out to swig/Makefile

SWIG_DIR := $(top_srcdir)/swig

SWIG_FILES := \
	pocketsphinx.i \
	cmd_ln.i \
	#fe.i \
	#feat.i \
	#jsgf.i \
	#ngram_model.i \
	ps_decoder.i \
	ps_lattice.i

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
													

pocketsphinx_wrap.c: $(addprefix $(SWIG_DIR)/, $(SWIG_FILES))
	$(SWIG) -I$(top_srcdir)/include -outdir . -o $@ -python $<

clean-local :
	$(RM) pocketsphinx_wrap.c pocketsphinx.py

endif
