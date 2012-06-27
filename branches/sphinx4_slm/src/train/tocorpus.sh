#!/bin/sh
#Convert raw wikipedia files to a corpus, including splitting into sentences, removing punctiontations, converting to uppercase

./tocorpus.pl < wiki.txt > wiki_corpus.txt
