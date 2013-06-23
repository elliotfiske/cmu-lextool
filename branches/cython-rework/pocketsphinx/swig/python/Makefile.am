if BUILD_SWIG

# TODO: fix broken parallel build

pkgpython_PYTHON = __init__.py pocketsphinx.py
pkgpyexec_LTLIBRARIES = _pocketsphinx.la
_pocketsphinx_la_SOURCES = pocketsphinx_wrap.c ../pocketsphinx.i
_pocketsphinx_la_CFLAGS = -I../../../sphinxbase/include -I../../include \
													`pkg-config --cflags python`
_pocketsphinx_la_LDFLAGS = -module
_pocketsphinx_la_LIBADD = ../../src/libpocketsphinx/libpocketsphinx.la \
													../../../sphinxbase/src/libsphinxbase/libsphinxbase.la

pocketsphinx_wrap.c: ../pocketsphinx.i
	$(SWIG) -I../../include -outdir . -o $@ -python $<

clean-local :
	$(RM) pocketsphinx_wrap.c pocketsphinx.py

endif
