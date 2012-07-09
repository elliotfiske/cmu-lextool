#!/usr/bin/python
#replace OOV words into <unk>
import sys

tag = "<unk>"

def tounk(f_in, vocab):
	fin = open(f_in)
	
	#read vocab 
	dict = {}
	fvoc = open(vocab)
	word = fvoc.readline()
	while word:
		word = word.rstrip()
		if len(word) == 0:
			pass
		elif word[0] == '#': #skip comments
			pass
		else:
			if dict.has_key(word):
				dict[word] = dict[word] + 1
			else:
				dict[word] = 1
		word = fvoc.readline()
	fvoc.close()
	
	#read each line to check whether the word is in the vocab
	nline = 0
	line = fin.readline()
	while line:
		line = line.rstrip()
		if len(line) == 0:
			pass
		else:
			words = line.split()
			for word in words:
				if dict.has_key(word):
					print word,
				else:#out of dict words
					print tag,
			print
		line = fin.readline()
		
	fin.close()

f_in = sys.argv[1]
vocab = sys.argv[2]
	
tounk(f_in, vocab)

