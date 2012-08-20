import os
import sys
import math

if len(sys.argv) != 4:
        print '\n Usage: python calc_ref_data_position.py <input_phonesegdir> <complete_phone_list> <output_data_file> \n'
        sys.exit(1)

os.system("ls " + sys.argv[1] + " > temp.txt");
in_phone = open(sys.argv[2], "r");
out_phone = open(sys.argv[3], "w");

cmuphones = []
allphones = []

fp = open("temp.txt","r");

for i in fp.readlines():
        f_name = sys.argv[1]+i.strip()
        phonelist = open(f_name, "r");

        while True:
                p1 = phonelist.readline();
                seg_p1 = p1.strip()
                if seg_p1 =='':
                        break;
                s = seg_p1.split()
                if(s[0].isdigit()):
                        phone_duration = (((int(s[1])-int(s[0]))+1)*10)+15;
                        log_score =  math.log(1-int(s[2]));
                        allphones.append([s[3],log_score,phone_duration,s[len(s)-1]])

for j in in_phone.readlines():
	phn = j.strip()
	cmuphones.append([phn,0,0,0,0,0])

print 'phone \t Position \t Mean Score \t Std. Score \t Mean duration \t Std. duration \t count'
count=0;
i=0
while 1:
	if i==len(cmuphones):
		break
	else:
		ph=cmuphones[i][0]
		j=0
		mean_score=[]
		mean_dur=[]
		count=[]
		acs_scores = []
		dur_scores = []
		for cnt in range(0,3):
			acs_scores.append([])
			dur_scores.append([])
			count.append(0)
			mean_score.append(0)
			mean_dur.append(0)
		while 1:
			if j==len(allphones):
				break
			if ph==allphones[j][0]:
				if(allphones[j][3]=='b'):
					acs_scores[0].append(float(allphones[j][1]))
					dur_scores[0].append(float(allphones[j][2]))
					count[0] = count[0] + 1
				elif(allphones[j][3]=='i'):
					acs_scores[1].append(float(allphones[j][1]))
					dur_scores[1].append(float(allphones[j][2]))
					count[1] = count[1] + 1
				else:
					acs_scores[2].append(float(allphones[j][1]))
					dur_scores[2].append(float(allphones[j][2]))
					count[2] = count[2] + 1
			j=j+1
		for cnt in range(0,3):
			mean_score[cnt]=sum(acs_scores[cnt])
			mean_dur[cnt]=sum(dur_scores[cnt])
			diff_arr=acs_scores[cnt]
			diff_arr1=dur_scores[cnt]
			flag=0;
			while(flag<count[cnt]):
				diff_arr[flag] = diff_arr[flag] - float(mean_score[cnt]/count[cnt])
				diff_arr1[flag] = diff_arr1[flag] - float(mean_dur[cnt]/count[cnt])
				flag=flag+1
			if count[cnt]==0:
				count[cnt]=1
			k=0
			while 1:
				if k==len(diff_arr):
					break
				diff_arr[k]=pow(diff_arr[k],2)
				diff_arr1[k]=pow(diff_arr1[k],2)
				k=k+1
			cmuphones[i][1]=str(float(mean_score[cnt]/count[cnt]))
			cmuphones[i][2]=str(math.sqrt(sum(diff_arr)/count[cnt]))
			cmuphones[i][3]=str(float(mean_dur[cnt]/count[cnt]))
			cmuphones[i][4]=str(math.sqrt(sum(diff_arr1)/count[cnt]))
			cmuphones[i][5]=str(count[cnt])
			print str(cmuphones[i][0])+"\t"+str(cnt)+"\t"+str(cmuphones[i][1])+"\t"+str(cmuphones[i][2])+"\t"+str(cmuphones[i][3])+"\t"+str(cmuphones[i][4])+"\t"+str(cmuphones[i][5])
		i=i+1		

out_phone.write('Phone \t Mean Score \t Std Score \t Mean Duration(in ms)\n')
for i in cmuphones:
	out_phone.write("%s\n" % i)
