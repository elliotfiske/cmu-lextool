from collections import defaultdict
import sys

if len(sys.argv) == 1:
    print "No file was given"
    print "Usage: abbv_pp.py asr_output_file"
    sys.exit()

abb_list = open('abbreviations', 'r')

d = defaultdict(list)

for line in abb_list:
    s = line.split('\t')
    desc_list = s[1].replace('\n','').split(';')
    for desc in desc_list:
        d[desc.lower()] = s[0]


ars_o = open(sys.argv[1], 'r')
ars_po = open(sys.argv[1] + "_processed", 'w')

for line in ars_o:
    for key in d.keys():
        line = line.replace(key, d[key])

    ars_po.write(line)
