import os
import sys

if len(sys.argv) != 4:
	print '\n Usage: python convert2_ngbphones.py <input_phone_list> <key_mapping_file> <output_neighbouring_phone_list> \n' 
        sys.exit(1) 

in_phone = open(sys.argv[1], "r");
in_key = open(sys.argv[2], "r");
out_phone = open(sys.argv[3], "w");


cmu_phones = {}

while True:
	p1 = in_key.readline();
	key_phone = p1.strip()
	if key_phone =='':
		break;
	s = key_phone.split()
	if s[0] not in cmu_phones:
		cmu_phones[s[0]]=s[2]

for k in in_phone.readlines():
	n1 = k.split()
	for i in n1:
		if i in cmu_phones:
			n1[n1.index(i)] = cmu_phones[i]
	for i in n1:
		if '(' not in i:
			out_phone.write('('+i+') ')
		else:
			out_phone.write(i)
			
	out_phone.write('\n')
