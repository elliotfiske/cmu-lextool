if BUILD_SWIG

SUBDIRS = . test

SWIG_FLAGS = -Werror -I$(top_srcdir)/include -I$(sphinxbase)/swig

SWIG_DIR = $(top_srcdir)/swig

SWIG_FILES = \
	$(SWIG_DIR)/pocketsphinx.i \
	$(SWIG_DIR)/ps_decoder.i \
	$(SWIG_DIR)/ps_lattice.i

if BUILD_DOXYGEN
SWIG_FLAGS += -DHAS_DOC -I$(top_builddir)/doc
SWIG_FILES += $(top_builddir)/doc/pydoc.i
endif

CLEANFILES = pocketsphinx_wrap.c pocketsphinx.py pocketsphinx.pyc

pkgpython_PYTHON = __init__.py 
nodist_pkgpython_PYTHON = pocketsphinx.py
pkgpyexec_LTLIBRARIES = _pocketsphinx.la
nodist__pocketsphinx_la_SOURCES = pocketsphinx_wrap.c
_pocketsphinx_la_CPPFLAGS = \
	$(PYTHON_CPPFLAGS) \
	-I$(sphinxbase)/include \
	-I$(top_srcdir)/include \
	-D_DATADIR=\"$(datadir)/@PACKAGE@\" \
	-Wno-unused-value \
	-Werror
_pocketsphinx_la_LDFLAGS = -module
_pocketsphinx_la_LIBADD = \
	-lsphinxbase \
	$(top_builddir)/src/libpocketsphinx/libpocketsphinx.la

pocketsphinx_wrap.c: $(SWIG_FILES)
	 $(SWIG) $(SWIG_FLAGS) -outdir . -o $@ -python $<

endif
