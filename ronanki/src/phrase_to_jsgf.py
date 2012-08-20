import os
import sys


if len(sys.argv) != 4:
        print '\n Usage: python phrase_to_jsgf.py <input_phoneseg_file> <nbg_key_mapper> <out_jsgf_file> \n'
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
			allphones.append(s[3]);
			count = count + 1;


phlist=''
z=0;
while(z<count):
	phlist = phlist+' ('+cmuphones[allphones[z]]+')'
	z=z+1;
	
ph = open(sys.argv[3], "w");
ph.write('#JSGF V1.0;\n\ngrammar phonelist;\n\npublic <phonelist> = ((SIL)'+phlist+' (SIL));\n\n');

print('please execute below command to get fsg file\n')
print('/home/ronanki/web/source/sphinxbase/bin/sphinx_jsgf2fsg <'+sys.argv[3]+' >'+sys.argv[3][0:len(sys.argv[3])-5]+'.fsg');
