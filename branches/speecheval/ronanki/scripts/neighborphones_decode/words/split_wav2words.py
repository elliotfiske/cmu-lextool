import os
import sys

# program takes sample wav file as input and splits in to words as wav files

if len(sys.argv) != 5:
        print '\n Usage: python split_wav2words.py <input_phoneseg_file> <nbg_key_mapper> <input_wav_file> <out_split_dir> \n'
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
wc=0;
k=0;
while 1:
	if(j==len(allphones)):
		break;
	else:
		if(allphones[j][3]=='e'):
			wc = wc + 1
			print "phone "+allphones[j][2]
			print "word count : "+str(wc)
		else:
			print "phone "+allphones[j][2]
			j=j+1;
			continue;
	st_ind = k;
	fn_ind = j;
	ph_start =  (((float(allphones[st_ind][0])-1)*10)+1)/1000;
	ph_end = ((float(allphones[fn_ind][1])*10)+15)/1000 - ph_start;
	while 1:
		if(k>j):
			break;
		z = st_ind;
		count = fn_ind;
		phlist=''
		while(z<=count):
			if(z==k):
				phlist = phlist+' ('+cmuphones[allphones[z][2]]+')'
			else:
				phlist = phlist+' ('+allphones[z][2]+')'
			z=z+1;
		s_id = "%.2d" % (k+1)
		fname = str(s_id)+'_'+allphones[k][2]
		os.system('sox '+sys.argv[3]+' '+sys.argv[4]+'/'+fname+'.wav trim '+str(ph_start)+' '+str(ph_end));
		ph_fname = 'FSG_words/JSGF/'+fname+'.jsgf'
		ph = open(ph_fname, "w");
		ph.write('#JSGF V1.0;\n\ngrammar phonelist;\n\npublic <phonelist> = ('+phlist+');\n\n');
		k = k + 1;

	j=j+1;
