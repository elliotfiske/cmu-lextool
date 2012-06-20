#!/bin/sh

for f in IEEE/*.nrm
do
	fn=`basename $f .nrm`
	python word_align.py $f IEEE/$fn.ffmpeg.rec
	python word_align.py $f IEEE/$fn.mpg123.rec
	python word_align.py $f IEEE/$fn.mplayer.rec
done

