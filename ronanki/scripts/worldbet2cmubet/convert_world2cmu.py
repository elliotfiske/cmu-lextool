import os
import sys


if len(sys.argv) != 4: 
	print '\n Usage: convert_world2cmu.py <input_worldbet_phone> <input_key_map> <output_cmubet_phone> \n' 
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
		cmu_phones[s[0]]=s[1]

for k in in_phone.readlines():
	n1 = k.split()
	flag = 0;
	for i in n1:
		if i in cmu_phones:
			n1[flag] = cmu_phones[i]
		elif ',' in i:
			if i.split(',')[0] in cmu_phones:
				j=i.split(',')[0]
				n1[flag] = cmu_phones[j]+'|';
		flag = flag + 1;
		
	len_n1 = len(n1)
	count = 1;
	for i in n1:
		out_phone.write(i)
		if count<3:
			count = count + 1
			out_phone.write(' ')
	out_phone.write('\n')
