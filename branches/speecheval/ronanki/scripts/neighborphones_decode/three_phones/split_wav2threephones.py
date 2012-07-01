import os
import sys

# Program takes sample input wav file and splits into three consecutive phones as a split wav file
# sample .phseg file required for boundary detections

if len(sys.argv) != 5:
        print '\n Usage: python split_wav2threephones.py <input_phoneseg_file> <nbg_key_mapper> <input_wav_file> <out_split_dir> \n'
        sys.exit(1)

phonelist = open(sys.argv[1], "r");
in_key = open(sys.argv[2], "r");

cmuphones = {}
allphones = []

while True:
	p1 = in_key.readline();
	key_phone = p1.strip()
	if key_phone =='':
		break;
	s = key_phone.split()
	if s[0] not in cmuphones:
		cmuphones[s[0]]=s[2]

for p1 in phonelist.readlines():
	seg_p1 = p1.strip()
	s = seg_p1.split()
	if(s[0].isdigit()):
		if s[3] in cmuphones:
			allphones.append([s[0],s[1],s[3],s[len(s)-1]]);

j=0;
while 1:
	if(j==len(allphones)):
		break;
	else:
		if(allphones[j][3]=='b'):
			print "begin "+allphones[j][2]
			if(allphones[j+1][3]=='b'):
				ph_start =  (((float(allphones[j][0])-1)*10)+1)/1000;
				ph_end = ((float(allphones[j][1])*10)+15)/1000 - ph_start;
				phlist = '('+cmuphones[allphones[j][2]]+')'
			elif(allphones[j+1][3]=='e'):
				ph_start =  (((float(allphones[j][0])-1)*10)+1)/1000;
				ph_end = ((float(allphones[j+1][1])*10)+15)/1000 - ph_start;
				phlist = '('+cmuphones[allphones[j][2]]+') ('+allphones[j+1][2]+')'
			else:
				ph_start =  (((float(allphones[j][0])-1)*10)+1)/1000;
				ph_end = ((float(allphones[j+2][1])*10)+15)/1000 - ph_start;
				phlist = '('+cmuphones[allphones[j][2]]+') ('+allphones[j+1][2]+') ('+allphones[j+2][2]+')'
		elif(allphones[j][3]=='e'):
			print "end "+allphones[j][2]
			if(allphones[j-1][3]=='b'):
				ph_start =  (((float(allphones[j-1][0])-1)*10)+1)/1000;
				ph_end = ((float(allphones[j][1])*10)+15)/1000 - ph_start;
				phlist = '('+allphones[j-1][2]+') ('+cmuphones[allphones[j][2]]+')'
			else:
				ph_start =  (((float(allphones[j-2][0])-1)*10)+1)/1000;
				ph_end = ((float(allphones[j][1])*10)+15)/1000 - ph_start;
				phlist = '('+allphones[j-2][2]+') ('+allphones[j-1][2]+') ('+cmuphones[allphones[j][2]]+')'
		else:
			print "middle "+allphones[j][2]
			ph_start =  (((float(allphones[j-1][0])-1)*10)+1)/1000;
			ph_end = ((float(allphones[j+1][1])*10)+15)/1000 - ph_start;
			phlist = '('+allphones[j-1][2]+') ('+cmuphones[allphones[j][2]]+') ('+allphones[j+1][2]+')'

	s_id = "%.2d" % (j+1)
	fname = str(s_id)+'_'+allphones[j][2] 
	os.system('sox '+sys.argv[3]+' '+sys.argv[4]+'/'+fname+'.wav trim '+str(ph_start)+' '+str(ph_end));
	j=j+1;
	ph_fname = 'FSG_3phoneme/JSGF/'+fname+'.jsgf'
	ph = open(ph_fname, "w");
	ph.write('#JSGF V1.0;\n\ngrammar phonelist;\n\npublic <phonelist> = ('+phlist+');\n\n');

