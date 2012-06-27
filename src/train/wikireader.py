#!/usr/bin/python
#load wikipedia corpus, remove blank line, split it into different part: train, valid, test

import sys

def load(wiki, out):
	f = open(wiki)
	fout_train = open(out+"train","w")
	fout_test = open(out+"test","w")
	fout_valid = open(out+"valid","w")
	nline = 0
	ndoc = 0
	line = f.readline()
	while line:
		line = line.rstrip()
		if len(line) == 0:
			ndoc = ndoc + 1
		else:
			nline = (nline + 1)%10
			if nline < 8:
				fout_train.write(line+"\n")
			elif nline == 8:
				fout_test.write(line+"\n")
			else:
				fout_valid.write(line+"\n")
		line = f.readline()
	f.close()
	print "ndoc=%d" % ndoc
	
	fout.close()
	fout_train.close()
	fout_test.close()
	fout_valid.close()
	
if len(sys.argv) > 2:
	wiki = sys.argv[1]
	outdir = sys.argv[2]
else:
	wiki = '/media/PRIVATE/loop/project/workspace/sphinx4/data/wiki/wiki_corpus.txt'
	outdir = '/media/PRIVATE/loop/project/workspace/sphinx4/data/wiki/rnnlm/'
	
load(wiki,outdir)

if __name__ == '__main__':
	p = 'This is a sentence.  This is an excited sentence! And do you think this is a question?'

	#sentences = splitParagraphIntoSentences(p)
	#for s in sentences:
	#	print s.strip()
