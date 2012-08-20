if [ $# -ne 1 ]
then
	echo "Script invoking error!";
	return 1;
fi

flvfile=$1
wavfile=`echo ${flvfile} | sed "s:.flv:.wav:g"`;

if [ ! -e ${wavfile} ]; then 
	# convert from flv to wav
	/usr/bin/ffmpeg  -y -i ${flvfile} -vn -f wav -ar 16000 -ac 1 ${wavfile} 2>&1

	if [ $? -gt 0 ]; then
		echo "ffmpeg conversion error!";
		return 1;
	fi
fi

# mfc feature extraction
mfcfile=`echo ${wavfile} | sed "s:.wav:.mfc:g"`;

scppath=/home/li-bo/web/datacollection/models

/home/li-bo/web/source/sphinxbase/bin/sphinx_fe -i ${wavfile} -o ${mfcfile} -argfile ${scppath}/feat.params -mswav yes

if [ $? -gt 0 ]; then
	echo "MFCC feature extraction error!";
	return 1;
fi

nosuffix=`echo ${wavfile} | sed "s:.wav::g"`;
filename=`basename ${nosuffix}`;

ctlfile=${nosuffix}.ctl
echo ${nosuffix} > ${ctlfile}

mkdir -p ${nosuffix}{.phonelabdir,.phonesegdir,.statesegdir,.aligndir}

/home/li-bo/web/source/sphinx3/bin/sphinx3_align -hmm ${scppath}/wsj_all_cd30.mllt_cd_cont_4000 -dict ${scppath}/cmu.dic -fdict ${scppath}/phone.filler -ctl ${ctlfile} -insent ${scppath}/sents/${filename}.insent -outsent ${nosuffix}.outsent -phlabdir ${nosuffix}.phonelabdir -phsegdir ${nosuffix}.phonesegdir -stsegdir ${nosuffix}.statesegdir -wdsegdir ${nosuffix}.aligndir

if [ $? -gt 0 ]; then
	echo "Alignment error!";
	return 1;
fi

if [ ! -e ${nosuffix}.phonesegdir/${filename}.phseg ]; then
	echo "No alignment results could be found!";
	return 1;
fi

# edit-distance decoding

/home/li-bo/web/source/sphinx3/bin/sphinx3_decode -hmm ${scppath}/wsj_all_cd30.mllt_cd_cont_4000 -fsg ${scppath}/fsgs/${filename}.fsg -dict ${scppath}/phone.dic -fdict ${scppath}/phone.filler -ctl ${ctlfile} -hyp ${nosuffix}.rec -mode allphone -op_mode 2

if [ $? -gt 0 ]; then
	echo "Decoding error!";
	return 1;
fi

# post-processing to get the accuracy
python ${scppath}/compute_wer.py ${nosuffix}.phonesegdir/${filename}.phseg ${nosuffix}.rec ${nosuffix}.per

if [ $? -gt 0 ]; then
	echo "Computing PER error!";
	return 1;
fi

