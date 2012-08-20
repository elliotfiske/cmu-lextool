import os
import sys

if len(sys.argv) != 5:
        print '\n Usage: python binary_features.py <input_label_file> <input_feature_stream> <window-shift> <input_mcep_file> > <output_binary_features> \n'
        sys.exit(1)

phonelist = open(sys.argv[1], "r");
in_feature = open(sys.argv[2], "r");

allphones = []

for i in in_feature.readlines():
	allphones.append(i.strip())


pv = 0;
rf=0;
tc=0;
ws = sys.argv[3];
for p1 in phonelist.readlines():
	seg_p1 = p1.strip()
	s = seg_p1.split()
	if(len(s)>1):
		rf=(float(s[0])*1000/int(ws))
		k=0;
		while k<40:
			ix = allphones[k].index(' ');
			if(s[2]==allphones[k][0:ix]):
				break;
			else:
				k=k+1;
		if(k==40):
			k=39;
		no_frames = int(rf) - pv
		pv = int(rf);
		count = 0;
		while(count<no_frames):
			print allphones[k][ix+1:len(allphones[k])]
			count = count+1
			tc = tc + 1
			
in_mcep = open(sys.argv[4],'r');
mc = in_mcep.readlines();
mc_length = len(mc);
while(tc<mc_length):
	print allphones[k][ix+1:len(allphones[k])]
	tc=tc+1
