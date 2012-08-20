if [ $# -ne 1 ]
then
	echo "Script invoking error!";
	return 1;
fi

flvfile=$1
nosuffix=`echo ${flvfile} | sed "s:.flv::g"`;
filename=`basename ${nosuffix}`;

if [ ! -e ${nosuffix}.wav ]; then

	# convert from flv to wav
	/usr/bin/ffmpeg  -y -i ${flvfile} -vn -f wav -ar 16000 -ac 1 ${nosuffix}.wav 2>&1

	if [ $? -gt 0 ]; then
		echo "ffmpeg conversion error!";
		return 1;
	fi
fi

# mfc feature extraction
scppath=/home/li-bo/web/datacollection/models

/home/li-bo/web/source/sphinxbase/bin/sphinx_fe -i ${nosuffix}.wav -o ${nosuffix}.mfc -argfile ${scppath}/feat.params -mswav yes

if [ $? -gt 0 ]; then
	echo "MFCC feature extraction error!";
	return 1;
fi

# do alignment
echo ${nosuffix} > ${nosuffix}.ctl

mkdir -p ${nosuffix}{.phonelabdir,.phonesegdir,.statesegdir,.aligndir}

/home/li-bo/web/source/sphinx3/bin/sphinx3_align -hmm ${scppath}/wsj_all_cd30.mllt_cd_cont_4000 -dict ${scppath}/cmu.dic -fdict ${scppath}/phone.filler -ctl ${nosuffix}.ctl -insent ${scppath}/sents/${filename}.insent -outsent ${nosuffix}.outsent -phlabdir ${nosuffix}.phonelabdir -phsegdir ${nosuffix}.phonesegdir -stsegdir ${nosuffix}.statesegdir -wdsegdir ${nosuffix}.aligndir

if [ $? -gt 0 ]; then
	echo "Alignment error!";
	return 1;
fi

if [ ! -e ${nosuffix}.phonesegdir/${filename}.phseg ]; then
	echo "No alignment results could be found!";
	return 1;
fi

# generate scores [acoustic score, and duration score]
python ${scppath}/test_phrase.py ${nosuffix}.phonesegdir/${filename}.phseg ${scppath}/stats/${filename}.stats ${scppath}/CMUphones_list ${nosuffix}.sco

if [ $? -gt 0 ]; then
	echo "Score generation error!";
	return 1;
fi


