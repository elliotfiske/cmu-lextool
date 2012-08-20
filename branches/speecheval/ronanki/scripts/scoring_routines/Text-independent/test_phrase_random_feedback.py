import os
import sys
import math

if len(sys.argv) != 6:
        print '\n Usage: python test_phrase_random_feedback.py <input_phonesegdir> <transcription.insent> <TIMIT_stats.txt> <CMU_phonelist> <out_file>\n'
        sys.exit(1)

os.system("ls " + sys.argv[1] + " > temp.txt");
in_sentence = open(sys.argv[2], "r");
in_data = open(sys.argv[3], "r");
in_phone = open(sys.argv[4], "r");
ot_file = open(sys.argv[5],"w");

cmuphones={}

for j in in_phone.readlines():
        phn = j.strip()
        cmuphones[phn] = phn

allphones = []
ac_scores = []
words = []
w_score = []

for k in in_data.readlines():
        line = k.strip()
        ph_data = line.split()
        if ph_data[0] in cmuphones:
                allphones.append([ph_data[0],ph_data[1],ph_data[2],ph_data[3],ph_data[4],ph_data[5]])

for k in in_sentence.readlines():
	sent = k.strip()
	wds = sent.split()
	wc=0;
	while(wc<len(wds)):
		words.append(wds[wc]);
		wc = wc+1

word_count = wc-1;

fp = open("temp.txt","r");

count = 1;
wc = 0;
pos = 0;
ph_cnt = 0;
fn_score = 0;
wd_score = 0;
rate = '';
rt_full = 0
for i in fp.readlines():
        f_name = sys.argv[1]+i.strip()
        phonelist = open(f_name, "r");
        score1 = 0
        score2 = 0
        while True:
                p1 = phonelist.readline();
                seg_p1 = p1.strip()
                if seg_p1 =='':
                        break;
                s = seg_p1.split()
                if(s[0].isdigit()):
                        if s[3] in cmuphones:
				indx=0;
				if(s[len(s)-1]=='i'):
					pos = 1;
				elif(s[len(s)-1]=='b'):
					pos = 0;
				else:
					pos = 2;
				while True:
					if(allphones[indx][0]==s[3] and int(allphones[indx][1])==pos):
						break;
					else:
						indx = indx+1;
                                phone_duration = (((int(s[1])-int(s[0]))+1)*10)+15;
                                log_score =  math.log(1-int(s[2]));
                                t_score = abs((log_score - float(allphones[indx][2]))/float(allphones[indx][3]))
                                sgn_score = (phone_duration - float(allphones[indx][4]))/float(allphones[indx][5])
				if(sgn_score<0):
					rate='Fast'
					rt_full = rt_full+1
				else:
					rate='Slow'
					rt_full = rt_full-1
				z_score = abs(sgn_score)
				if(z_score<1):
					rate='Normal'
                                st_score1 = 5 - t_score
                                st_score2 = 5 - z_score
				if(st_score2<0):
					st_score2 = 0
                                score1 = score1 + st_score1
                                score2 = score2 + st_score2
				ph_cnt = ph_cnt + 1
				if(s[len(s)-1]=='e' or s[len(s)-1]=='s'):
					wd_score = (score1 + score2)/ph_cnt
					fn_score = fn_score + wd_score
					ostr = words[wc]+' %.2f ' % wd_score
					ot_file.write(ostr+rate+'\n')
					wc = wc+1
					score1 = 0;
					score2 = 0;
					ph_cnt = 0;

fn_score = fn_score/wc;
ostr = 'Complete_phrase %.2f ' % fn_score
if(rt_full>0):
	rate='Fast'
else:
	rate='Slow'
if(fn_score>=7.5):
	rate='Normal'
ot_file.write(ostr+rate+'\n')
ot_file.close()

