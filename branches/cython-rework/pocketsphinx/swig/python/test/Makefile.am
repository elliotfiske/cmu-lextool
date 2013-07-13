TESTS = \
	config_test.py \
	decoder_test.py \
	lattice_test.py \
	lm_test.py

examplesdir = $(datadir)/@PACKAGE@/examples/python
dist_examples_DATA = \
	$(TESTS)

LIBPATH = $(top_builddir)/swig/python
TESTS_ENVIRONMENT = \
	PYTHONPATH=$(LIBPATH):$(LIBPATH)/.libs \
	MODELDIR=$(top_srcdir)/model \
	DATADIR=$(top_srcdir)/test/data \
	$(PYTHON)

CLEANFILES = goforward.lat goforward.htk
