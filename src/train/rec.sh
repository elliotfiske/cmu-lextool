#!/bin/sh

for f in IEEE/*.wav
do
	fn=`basename $f .wav`
	java -jar ../bin/Transcriber.jar $f > IEEE/$fn.rec
done
