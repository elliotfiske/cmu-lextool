import os
import sys


if len(sys.argv) != 5:
        print '\n Usage: python split_grammar.py <input_phoneseg_file> <nbg_key_mapper> <input_wav_file> <out_split_dir> \n'
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
count=0;
for p1 in phonelist.readlines():
	seg_p1 = p1.strip()
	s = seg_p1.split()
	if(s[0].isdigit()):
		if s[3] in cmuphones:
			allphones.append([s[0],s[1],s[3],s[len(s)-1]]);
			count = count + 1;

j=0;
while 1:
	if(j==len(allphones)):
		break;
	phlist=''
	z=0;
	while(z<count):
		if(z==j):
			phlist = phlist+' ('+cmuphones[allphones[z][2]]+')'
		else:
			phlist = phlist+' ('+allphones[z][2]+')'
		z=z+1;
	s_id = "%.2d" % (j+1)
	fname = str(s_id)+'_'+allphones[j][2]
	os.system('cp '+sys.argv[3]+' '+sys.argv[4]+fname+'.wav');
	ph_fname = 'FSG_phrase/JSGF/'+fname+'.jsgf'
	ph = open(ph_fname, "w");
	ph.write('#JSGF V1.0;\n\ngrammar phonelist;\n\npublic <phonelist> = ('+phlist+');\n\n');
	j=j+1;
