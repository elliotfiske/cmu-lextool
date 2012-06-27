#!/bin/sh
#Get WER after you get the recognition result
#Need word_align.py from sphinxtrain
#*.nrm is the reference text, *.rec is the recognition result

for f in IEEE/*.nrm
do
	fn=`basename $f .nrm`
	python word_align.py $f IEEE/$fn.ffmpeg.rec
	python word_align.py $f IEEE/$fn.mpg123.rec
	python word_align.py $f IEEE/$fn.mplayer.rec
done

