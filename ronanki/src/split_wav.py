import os
import sys


if len(sys.argv) != 5:
        print '\n Usage: python split_wav.py <input_phoneseg_file> <complete_phone_list> <input_wav_file> <out_split_dir> \n'
        sys.exit(1)

phonelist = open(sys.argv[1], "r");
in_phone = open(sys.argv[2], "r");

cmuphones = {}

for i in in_phone.readlines():
	phn = i.strip()
	cmuphones[phn]=phn;
j=0
for p1 in phonelist.readlines():
	seg_p1 = p1.strip()
	s = seg_p1.split()
	if(s[0].isdigit()):
		print s[3]
		if s[3] in cmuphones:
			ph_start =  (((float(s[0])-1)*10)+1)/1000;
			ph_end = ((float(s[1])*10)+15)/1000 - ph_start;
			j = j+1;
			s_id = "%.2d" % (j)
			fname = str(s_id)+'_'+s[3] 
			os.system('sox '+sys.argv[3]+' '+sys.argv[4]+fname+'.wav trim '+str(ph_start)+' '+str(ph_end));
