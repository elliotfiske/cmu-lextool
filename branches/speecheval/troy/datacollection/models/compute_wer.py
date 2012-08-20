#!/usr/bin/env python

import string, sys, numpy

if len(sys.argv)!=4:
	print 'Usage: script [alignfile][recfile][resfile]'
	sys.exit(1)

alnfile=sys.argv[1]
recfile=sys.argv[2]
outfile=sys.argv[3]

alnphn=[]
fin=open(alnfile)
fin.readline()
while True:
	sr=fin.readline()
	if sr=='':break
	sr=sr.strip()
	if sr.startswith('Total'):break
	lst=sr.split(' ')
	inflst=[]
	for itm in lst:
		if itm!='':inflst.append(itm)
	#print inflst
	if inflst[3]!='SIL' and not inflst[3].startswith('+'):
		alnphn.append(inflst[3])
fin.close()

fin=open(recfile)
sr=(fin.readline()).strip()
sr=(sr[:sr.find('(')]).strip()
recphn=sr.split(' ')
fin.close()

costs=numpy.zeros([len(alnphn)+1, len(recphn)+1])
for ii in range(len(recphn)):
	for jj in range(len(alnphn)):
		cc=0
		if costs[jj,ii]<costs[jj+1,ii]:
			cc=costs[jj,ii]
		else:
			cc=costs[jj+1,ii]
		
		if recphn[ii] != alnphn[jj]:
			cc+=1
		
		costs[jj+1,ii+1]=cc
fout=open(outfile,'w')
print >>fout, costs[len(alnphn), len(recphn)]/len(alnphn)
fout.close()
	
	
		

