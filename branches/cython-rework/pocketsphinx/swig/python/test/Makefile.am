TESTS = \
	config_test.py \
	decoder_test.py \
	lattice_test.py

examplesdir = $(datadir)/@PACKAGE@/examples/python
dist_examples_DATA = \
	$(TESTS) \
	decode_memory.py

LIBPATH = $(top_builddir)/swig/python
TESTS_ENVIRONMENT = PYTHONPATH=$(LIBPATH):$(LIBPATH)/.libs $(PYTHON)
