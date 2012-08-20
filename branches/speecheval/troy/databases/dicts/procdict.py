#!/usr/bin/env python

import string, sys

phnlst=[]
fin=open('phonemes')
while True:
	sr=fin.readline()
	if sr=='':break
	sr=sr.strip()
	if sr!='':phnlst.append(sr)
fin.close()

promap={}
wrdlst=[]
fin=open('cmudict.0.7a')
cnt=0
while True:
	sr=fin.readline()
	if sr=='':break
	sr=(sr.strip()).lower()
	if sr[0] >= 'a' and sr[0] <='z':
		print cnt
		cnt+=1
		wrd=(sr[:sr.find(' ')]).strip()
		if '(' in wrd:
			wrd=(wrd[:wrd.find('(')]).strip()
		phn=(sr[sr.find(' '):]).strip()
		
		if wrd not in promap.keys():
			promap[wrd]=[]
			wrdlst.append(wrd)
		phn=string.replace(phn, '0', '')
		phn=string.replace(phn, '1', '')
		phn=string.replace(phn, '2', '')
		promap[wrd].append(phn.upper())
fin.close()

fout=open('d01_words_data.dat','w')
for ii in range(len(wrdlst)):
	print >>fout, '"{0}",{1}'.format(wrdlst[ii],str(len(promap[wrdlst[ii]])))
fout.close()

fout=open('d02_pronuciations_data.dat', 'w')
for ii in range(len(wrdlst)):
	wordid=ii+1
	for jj in range(len(promap[wrdlst[ii]])):
		proid=jj
		
		sr=promap[wrdlst[ii]][jj]
		lst=sr.split(' ')
		pos=0
		for itm in lst:
			if itm != '':
				phnid=phnlst.index(itm)+1
				print >>fout, str(wordid)+","+str(proid)+","+str(phnid)+","+str(pos)
				pos+=1
fout.close()
		
