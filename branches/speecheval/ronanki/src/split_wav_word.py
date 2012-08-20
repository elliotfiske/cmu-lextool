import os
import sys


if len(sys.argv) != 5:
        print '\n Usage: python split_wav_word.py <input_phoneseg_file> <CMUphone_list> <input_wav_file> <out_split_dir> \n'
        sys.exit(1)

phonelist = open(sys.argv[1], "r");
in_phone = open(sys.argv[2], "r");

allphones = []
cmuphones = {}

for i in in_phone.readlines():
	phn = i.strip()
	cmuphones[phn]=phn;

for p1 in phonelist.readlines():
	seg_p1 = p1.strip()
	s = seg_p1.split()
	if(s[0].isdigit()):
		if s[3] in cmuphones:
			allphones.append([s[0],s[1],s[3],s[len(s)-1]]);

j=0;
wc=0;
st_ind=0;
while 1:
	if(j==len(allphones)):
		break;
	else:
		if(allphones[j][3]=='e' or allphones[j][3]=='s'):
			wc = wc + 1
			print "word count : "+str(wc)
		else:
			j=j+1;
			continue;
	fn_ind=j;
	ph_start =  (((float(allphones[st_ind][0])-1)*10)+1)/1000;
	ph_end = ((float(allphones[fn_ind][1])*10)+15)/1000 - ph_start;
	fname = 'word_'+str(wc)
	os.system('sox '+sys.argv[3]+' '+sys.argv[4]+fname+'.wav trim '+str(ph_start)+' '+str(ph_end));
	j=j+1
	st_ind=j;
