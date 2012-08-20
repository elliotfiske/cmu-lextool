import os
import sys

if len(sys.argv) != 4:
        print '\n Usage: python binary_features.py <input_label_file> <input_feature_stream> <window-shift> >  <output_binary_features> \n'
        sys.exit(1)

phonelist = open(sys.argv[1], "r");
in_feature = open(sys.argv[2], "r");

allphones = []

for i in in_feature.readlines():
	allphones.append(i.strip())


pv = 0;
rf=0;
ws = sys.argv[3];
for p1 in phonelist.readlines():
	seg_p1 = p1.strip()
	s = seg_p1.split()
	if(len(s)>1):
		rf=(float(s[0])*1000/int(ws))
		k=0;
		while 1:
			ix = allphones[k].index(' ');
			if(s[2]==allphones[k][0:ix]):
				break;
			else:
				k=k+1;
		no_frames = int(rf) - pv
		pv = int(rf);
		count = 0;
		while(count<no_frames):
			print allphones[k][ix+1:len(allphones[k])]
			count = count+1
