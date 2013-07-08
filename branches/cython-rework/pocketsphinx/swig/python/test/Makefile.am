examplesdir = $(prefix)/share/@PACKAGE@/examples/python
dist_examples_DATA = \
	config_test.py \
	decoder_test.py

TESTS_ENVIRONMENT = PYTHONPATH=..:../.libs $(PYTHON) 

TESTS = $(dist_examples_DATA)
