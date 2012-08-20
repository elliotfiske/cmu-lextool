import os
import sys
from subprocess import Popen, PIPE


if len(sys.argv) != 3:
        print '\n Usage: python decode_phrase.py <input_split_ctl_file> <output_phone_file> \n'
        sys.exit(1)

fp = open(sys.argv[1],"r");
fp1 = open(sys.argv[2],"w");

count = 0
for i in fp.readlines():
	fname = i.strip()
	count = count + 1
	lno = str(count)+','+str(count)+'p' 
	os.system('sed -n '+lno+' '+sys.argv[1]+' > single_phone.ctl')
	fsg_file = 'FSG_phrase/FSG/'+fname+'.fsg'
	cmd = '/home/ronanki/web/source/sphinx3/bin/sphinx3_decode -hmm ../test/GSoC_test/wsj_all_cd30.mllt_cd_cont_4000 -fsg '+fsg_file+' -dict phone.dic -fdict phone.filler -ctl single_phone.ctl -cepdir split_feats -hyp single_phone.out -op_mode 2'
	os.system(cmd)
	stdout = Popen('cat single_phone.out', shell=True, stdout=PIPE).stdout
	output = stdout.read()
	fp1.write(output)



