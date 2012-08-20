import os
import sys
import math

if len(sys.argv) != 4:
        print '\n Usage: python test_phrase.py <input_phonesegdir> <input_phrase_stats.txt> <CMU_phonelist> \n'
        sys.exit(1)

os.system("ls " + sys.argv[1] + " > temp.txt");
in_data = open(sys.argv[2], "r");
in_phone = open(sys.argv[3], "r");

cmuphones={}

for j in in_phone.readlines():
        phn = j.strip()
        cmuphones[phn] = phn

allphones = []
ac_scores = []

for k in in_data.readlines():
        line = k.strip()
        ph_data = line.split()
        if ph_data[0] in cmuphones:
                allphones.append([ph_data[0],ph_data[1],ph_data[2],ph_data[3],ph_data[4]])


fp = open("temp.txt","r");

count = 1;
for i in fp.readlines():
        f_name = sys.argv[1]+i.strip()
        phonelist = open(f_name, "r");
        score1 = 0
        score2 = 0
        indx = -1;
        while True:
                p1 = phonelist.readline();
                seg_p1 = p1.strip()
                if seg_p1 =='':
                        break;
                s = seg_p1.split()
                if(s[0].isdigit()):
                        if s[3] in cmuphones:
				indx = indx+1;
                                phone_duration = (((int(s[1])-int(s[0]))+1)*10)+15;
                                log_score =  math.log(1-int(s[2]));
                                t_score = abs((log_score - float(allphones[indx][1]))/float(allphones[indx][2]))
                                z_score = abs((phone_duration - float(allphones[indx][3]))/float(allphones[indx][4]))
                                st_score1 = 5 - t_score
                                st_score2 = 5 - z_score
				if(st_score2<0):
					st_score2 = 0
                                score1 = score1 + st_score1
                                score2 = score2 + st_score2
                                print allphones[indx][0],st_score1,st_score2

print '\nMean Acoustic Score = '+str(score1/(indx+1))
print 'Mean Duration Score = '+str(score2/(indx+1))
print '\nTotal Score (out of 10) = '+str(score1/(indx+1) + score2/(indx+1))

