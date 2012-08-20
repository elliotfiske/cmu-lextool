import os
import sys
import math

if len(sys.argv) != 3:
        print '\n Usage: python phrase_info.py <input_phonesegdir> <CMU_phonelist> \n'
        sys.exit(1)

os.system("ls " + sys.argv[1] + " > temp.txt");
in_phone = open(sys.argv[2], "r");

cmuphones={}

for j in in_phone.readlines():
        phn = j.strip()
        cmuphones[phn] = phn

ac_scores = []
dur_scores = []
allphones = []

fp = open("temp.txt","r");

count = 1;
for i in fp.readlines():
        f_name = sys.argv[1]+i.strip()
#       print f_name
        phonelist = open(f_name, "r");

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
                                phone_duration = ((((float(s[1])-int(s[0]))+1)*10)+15);
                                log_score =  math.log(1-int(s[2]));
                                if(count==1):
                                        allphones.append([])
                                        allphones[indx].append(s[3]);
                                        ac_scores.append([])
                                        ac_scores[indx].append(log_score)
                                        dur_scores.append([])
                                        dur_scores[indx].append(phone_duration)
                                        continue;
                                ac_scores[indx].append(log_score)
                                dur_scores[indx].append(phone_duration)
        count = count + 1;


count = count - 1;
print 'Phone \t Mean Score \t Std Score \t Max. Z-score \t Mean Duration \t Std Duration \t Max. Z-score'
flag = 0;
for i in allphones:
        mean_score =  sum(ac_scores[flag])/count
	mean_dur = sum(dur_scores[flag])/count
        count2 = 0;
        for k in ac_scores[flag]:
                ac_scores[flag][count2] = pow((ac_scores[flag][count2] - (mean_score)),2)
                dur_scores[flag][count2] = pow((dur_scores[flag][count2] - (mean_dur)),2)
                count2 = count2 +1
        var_score = math.sqrt(sum(ac_scores[flag])/count)
        var_dur = math.sqrt(sum(dur_scores[flag])/count)
        count2 = 0;
        for k in ac_scores[flag]:
                ac_scores[flag][count2] = abs(math.sqrt(ac_scores[flag][count2])/var_score)
                dur_scores[flag][count2] = abs(math.sqrt(dur_scores[flag][count2])/var_dur)
                count2=count2+1

        t_score = max(ac_scores[flag])
        z_score = max(dur_scores[flag])
        print allphones[flag][0]+'\t'+str(mean_score)+'\t'+str(var_score)+'\t'+str(t_score)+'\t'+str(mean_dur)+'\t'+str(var_dur)+'\t'+str(z_score)
        flag = flag + 1

