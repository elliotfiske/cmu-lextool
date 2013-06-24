if BUILD_SWIG

# TODO: fix broken parallel build

pkgpython_PYTHON = __init__.py pocketsphinx.py
pkgpyexec_LTLIBRARIES = _pocketsphinx.la
_pocketsphinx_la_SOURCES = pocketsphinx_wrap.c ../pocketsphinx.i
_pocketsphinx_la_CPPFLAGS = -I$(top_srcdir)/../sphinxbase/include \
														-I$(top_srcdir)/include \
														$(PYTHON_CPPFLAGS)
_pocketsphinx_la_LDFLAGS = -module
_pocketsphinx_la_LIBADD = $(top_srcdir)/src/libpocketsphinx/libpocketsphinx.la \
													$(top_srcdir)/../sphinxbase/src/libsphinxbase/libsphinxbase.la

pocketsphinx_wrap.c: ../pocketsphinx.i
	$(SWIG) -I$(top_srcdir)/include -outdir . -o $@ -python $<

clean-local :
	$(RM) pocketsphinx_wrap.c pocketsphinx.py

endif
