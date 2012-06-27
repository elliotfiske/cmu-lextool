#!/bin/sh
#split the corpus file into three parts: train, test, and valid

python wikireader.py wiki_corpus.txt ./rnnlm/
