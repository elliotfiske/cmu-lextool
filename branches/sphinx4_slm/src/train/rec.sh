#!/bin/sh
#get recognition results using sphinx4

path="../../data/IEEE"

f=$path/2012March19fixing-the-brain-with-computers.ffmpeg.wav
fn=`basename $f .wav`
java -jar ../../bin/Transcriber.jar $f > $path/$fn.rec

#for f in $path/*.wav
#do
#	fn=`basename $f .wav`
#	java -jar ../../bin/Transcriber.jar $f > $path/$fn.rec
#done
