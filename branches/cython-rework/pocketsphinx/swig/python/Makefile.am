if BUILD_SWIG

# TODO: fix broken parallel build
# TODO: use AC_SUBST for LIBADD and CPPFLAGS

pkgpython_PYTHON = __init__.py 
nodist_pkgpython_PYTHON = pocketsphinx.py
pkgpyexec_LTLIBRARIES = _pocketsphinx.la
nodist__pocketsphinx_la_SOURCES = pocketsphinx_wrap.c
_pocketsphinx_la_CPPFLAGS = -I$(sphinxbase)/include -I$(top_srcdir)/include \
														$(PYTHON_CPPFLAGS)
_pocketsphinx_la_LDFLAGS = -module
_pocketsphinx_la_LIBADD = -lsphinxbase
													$(top_srcdir)/src/libpocketsphinx/libpocketsphinx.la
													

pocketsphinx_wrap.c: ../pocketsphinx.i
	$(SWIG) -I$(top_srcdir)/include -outdir . -o $@ -python $<

clean-local :
	$(RM) pocketsphinx_wrap.c pocketsphinx.py

endif
