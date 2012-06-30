#!/bin/sh
#convert mp3 to wav using three different methods: mplayer, mpg123, ffmpeg
#you need to install these tools before run it

path="../../data/IEEE"

for f in $path/*.mp3
do
	fn=`basename $f .mp3`
	mplayer -really-quiet -noconsolecontrols -nojoystick -nolirc -nomouseinput -vo null -vc null -ao      		pcm:file=tmp.wav -af format=s16le,channels=1,resample=16000:0:2 $f
	
	sox tmp.wav -b 16 $path/$fn.mplayer.wav
	rm tmp.wav

	mpg123 -w tmp.wav $f 
	sox tmp.wav -b 16 $path/$fn.mpg123.wav
	rm tmp.wav

	ffmpeg -i $f tmp.wav
	sox tmp.wav -b 16 $path/$fn.ffmpeg.wav
	rm tmp.wav
done
