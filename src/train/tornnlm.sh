#!/bin/sh
# split the wiki corpus into three sets: train, valid, test

corpus='../../data/wikipedia/wiki.transcript'
outdir='../../data/wikipedia/rnnlm/'

python wikireader.py $corpus $outdir
