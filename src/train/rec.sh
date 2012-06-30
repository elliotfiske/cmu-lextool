#!/bin/sh
#get recognition results using sphinx4

path="../../data/IEEE"

for f in $path/*.wav
do
	fn=`basename $f .wav`
	java -jar ../../bin/Transcriber.jar $f > $path/$fn.rec
done
