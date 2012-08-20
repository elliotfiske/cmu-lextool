import os
import sys

if len(sys.argv) != 5:
        print '\n Usage: python extract_mfc2pf.py <input_ctl_file> <input_lab_dir> <input_mfc_dir> <output_feats_dir> \n'
        sys.exit(1)

fp = open(sys.argv[1], 'r');

for i in fp.readlines():
	fname = i.strip();
	cmd = 'python binary_features.py '+sys.argv[2]+'/'+fname+'.lab feature_stream 10 '+sys.argv[3]+'/'+fname+'.mfc > '+sys.argv[4]+'/'+fname+'.pf';
	os.system(cmd);
