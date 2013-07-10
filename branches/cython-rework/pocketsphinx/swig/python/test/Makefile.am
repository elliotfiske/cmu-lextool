examplesdir = $(datadir)/@PACKAGE@/examples/python
dist_examples_DATA = \
	config_test.py \
	decoder_test.py

LIBPATH = $(top_builddir)/swig/python
TESTS_ENVIRONMENT = PYTHONPATH=$(LIBPATH):$(LIBPATH)/.libs $(PYTHON)

TESTS = $(dist_examples_DATA)
