from itertools import izip
from pocketsphinx import _resource_path, Config, Decoder

decoder = Decoder(Config())
decoder.decodeRaw(open(_resource_path('goforward.raw'), 'rb'))
hypothesis = decoder.hyp()

print 'Best hypothesis: ', hypothesis.best_score, hypothesis.hypstr
print 'Best hypothesis segments: ', list([seg.word for seg in decoder.seg()])

print 'Best 10 hypothesis: '
for best, i in izip(decoder.nbest(), range(10)):
	print best.hyp().best_score, best.hyp().hypstr
