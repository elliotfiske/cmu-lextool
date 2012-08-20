#!/usr/bin/env python

import string

phnsr='AA AE AH AO AW AY B CH D DH EH ER EY F G HH IH IY JH K L M N NG OW OY P R S SH T TH UH UW V W Y Z ZH';
phnlst=phnsr.split(' ')
assert(len(phnlst)==39)

fout=open('data.txt','w')
#print >>fout, "INSERT INTO phrasestats (phraseid, phnpos, phnid, acomean, acostd, acomax, durmean, durstd, durmaxz)"
#print >>fout, "VALUES"
for ii in range(1,4):
	fin=open('phrase'+str(ii)+'.stats')
	fin.readline();# title line
	pos=0
	while True:
		sr=fin.readline();
		if sr=='':break
		sr=sr.strip()
		lst=sr.split('\t')
		if len(lst)<7:
			print 'Data format error!'
			exit(1)
		kk=0
		while lst[kk]=='':kk+=1
		phn=lst[kk]
		kk+=1
		while lst[kk]=='':kk+=1
		acomean=float(lst[kk])
		kk+=1
		while lst[kk]=='':kk+=1
		acostd=float(lst[kk])
		kk+=1
		while lst[kk]=='':kk+=1
		acomaxz=float(lst[kk])
		kk+=1
		while lst[kk]=='':kk+=1
		durmean=float(lst[kk])
		kk+=1
		while lst[kk]=='':kk+=1
		durstd=float(lst[kk])
		kk+=1
		while lst[kk]=='':kk+=1
		durmaxz=float(lst[kk])
		
		print >>fout, str(ii)+","+str(pos)+","+str(phnlst.index(phn))+","+str(acomean)+","+str(acostd)+","+str(acomaxz)+","+str(durmean)+","+str(durstd)+","+str(durmaxz);
		pos+=1
	fin.close()
fout.close()
		
		
		
	
	
