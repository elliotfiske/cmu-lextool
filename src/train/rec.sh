#!/bin/sh
#get recognition results using sphinx4

for f in IEEE/*.wav
do
	fn=`basename $f .wav`
	java -jar ../bin/Transcriber.jar $f > IEEE/$fn.rec
done
