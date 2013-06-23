#!/usr/bin/env python


import pocketsphinx as ps



#some dumb test for checking during developent

c = ps.Config()
decoder = ps.Decoder(c)

f = open("../../test/data/goforward.raw", "rb")
samp_num = decoder.decodeRaw(f)
print "------DECODED SAMPLES:" , samp_num
hyp = decoder.getHyp()
print "hyp", hyp
print "------RECOGNIZED TEXT:", hyp.hypstr
print "------UTTID:", hyp.uttid
print "------BEST_SCORE:", hyp.best_score
#help (hyp)
