if BUILD_SWIG_PYTHON

all-local:  _pocketsphinx.so

install-exec-local: pymod-build-stamp
	$(PYTHON) setup.py install \
		--prefix $(DESTDIR)$(prefix) $(SETUPTOOLS_CRAP)

uninstall-local:

clean-local:
#	$(PYTHON) setup.py clean --all
	$(RM) pocketsphinx.py  pocketsphinx.pyc  \
		  pocketsphinx_wrap.c  pocketsphinx_wrap.o _pocketsphinx.so

pymod-build-stamp: $(srcdir)/pocketsphinx.c
# This is dumb, but distutils is completely incapable of VPATH building
	touch $@

# TODO using the installed pocketsphinx for now...
_pocketsphinx.so: pocketsphinx_wrap.o
	$(CC) -shared -o $@ pocketsphinx_wrap.o
	
pocketsphinx_wrap.o: pocketsphinx_wrap.c
	$(CC) -fpic -c pocketsphinx_wrap.c -o $@ \
		-I../../include -I../../../sphinxbase/include \
		`pkg-config --cflags python` 

pocketsphinx_wrap.c: ../pocketsphinx.i
	swig -I../../include  -python $<
	
endif

