import os
import sys


os.system('ls '+sys.argv[1]+' > temp')
fp = open(temp,"r");
out_tr = open('TIMIT_prompt.txt',"w");

for i in fp.readlines():
	f_name = i.strip()
	tp = open(sys.argv[2]+f_name,"r");
	for j in tp.readlines():
		in_str = j.strip()
	out_str = in_str[8:len(in_str)-1]+' ('f_name[0:len(f_name)-5]+')'
	out_tr.write(out_str)
	tp.close()
	
