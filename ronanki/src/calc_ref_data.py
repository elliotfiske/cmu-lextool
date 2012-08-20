import os
import sys
import math

if len(sys.argv) != 4:
        print '\n Usage: python calc_ref_data.py <input_phonesegdir> <complete_phone_list> <output_data_file> \n'
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
	cmuphones.append([phn,0,0,0,0])

print 'phone \t Mean Score \t Std. Score \t Mean duration \t count'
count=0;
i=0
while 1:
	if i==len(cmuphones):
		break
	else:
		ph=cmuphones[i][0]
		j=0
		res1=0
		res2=0
		count=0
		acs_scores = []
		while 1:
			if j==len(allphones):
				break
			if ph==allphones[j][0]:
				acs_scores.append(float(allphones[j][1]))
				res1=res1+float(allphones[j][1])
				res2=res2+float(allphones[j][2])
				count=count+1
			j=j+1
		flag=0;
		diff_arr=acs_scores
		while(flag<count):
			diff_arr[flag] = acs_scores[flag] - float(res1/count)
			flag=flag+1
		if count==0:
			count=1
		k=0
		while 1:
			if k==len(diff_arr):
				break
			diff_arr[k]=pow(diff_arr[k],2)
			k=k+1
		cmuphones[i][1]=str(float(res1/count))
		cmuphones[i][2]=str(math.sqrt(sum(diff_arr)/count))
		cmuphones[i][3]=str(float(res2/count))
		cmuphones[i][4]=str(count)
		print str(cmuphones[i][0])+"\t"+str(cmuphones[i][1])+"\t"+str(cmuphones[i][2])+"\t"+str(cmuphones[i][3])+"\t"+str(cmuphones[i][4])
	i=i+1		

out_phone.write('Phone \t Mean Score \t Std Score \t Mean Duration(in ms)\n')
for i in cmuphones:
	out_phone.write("%s\n" % i)
