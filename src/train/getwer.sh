#!/bin/sh
#Get WER after you get the recognition result
#Need word_align.py from sphinxtrain
#*.nrm is the reference text, *.rec is the recognition result

path="../../data/IEEE"

#Get WER from ffmpeg result
for f in $path/*.nrm
do
	fn=`basename $f .nrm`
	python word_align.py $f $path/$fn.ffmpeg.rec
done

#Get WER from mpg123 result
for f in $path/*.nrm
do
	fn=`basename $f .nrm`
	python word_align.py $f $path/$fn.mpg123.rec
done

#Get WER from mplayer result
for f in $path/*.nrm
do
	fn=`basename $f .nrm`
	python word_align.py $f $path/$fn.mplayer.rec
done

