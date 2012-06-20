for f in IEEE/*.mp3
do
	fn=`basename $f .mp3`
	mplayer -really-quiet -noconsolecontrols -nojoystick -nolirc -nomouseinput -vo null -vc null -ao      		pcm:file=tmp.wav -af format=s16le,channels=1,resample=16000:0:2 $f
	
	sox tmp.wav -b 16 ./IEEE/$fn.mplayer.wav
	rm tmp.wav

	mpg123 -w tmp.wav $f 
	sox tmp.wav -b 16 ./IEEE/$fn.mpg123.wav
	rm tmp.wav

	ffmpeg -i $f tmp.wav
	sox tmp.wav -b 16 ./IEEE/$fn.ffmpeg.wav
	rm tmp.wav
done
