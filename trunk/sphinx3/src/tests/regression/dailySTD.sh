#!/bin/sh
# Try to find an executable that can send mail
# Default to sendmail
MAILX=sendmail

# Try to find mhmail
TMPMAIL=`which mhmail 2> /dev/null`
if test z${TMPMAIL} == z; then
# If we failed, try mailx
    TMPMAIL=`which mailx 2> /dev/null`
    if test z${TMPMAIL} == z; then
# If we failed again, try mail
	TMPMAIL=`which mail 2> /dev/null`
	if test z${TMPMAIL} == z; then
# If we failed again, try mailto
	    TMPMAIL=`which mailto 2> /dev/null`
	fi
    fi
fi

# If we found one of the above, use it. Otherwise, keep sendmail
if test z${TMPMAIL} != z; then MAILX=${TMPMAIL};fi

echo "This job was submitted by user $PBS_O_LOGNAME"
echo "This job was submitted to host $PBS_O_HOST"
echo "This job was submitted to queue $PBS_O_QUEUE"
echo "PBS working directory $PBS_O_WORKDIR"
echo "PBS job id $PBS_JOBID"
echo "PBS job name $PBS_JOBNAME"
echo "PBS environment $PBS_ENVIRONMENT"
echo "This script is running on `hostname`"

#Run test. 
cd $PBS_O_WORKDIR

if ! make perf-quick > perf-quick.log 2>&1 ;
 then
    ${MAILX} -s "Quick Performance Test failed at date:$testdate,machine:`hostname`,dir:$PBS_O_WORKDIR" ${S3REGTESTLIST} < perf-quick.log
 fi

if ! make perf-std > perf-std.log 2>&1 ;
 then
    ${MAILX} -s "Standard Performance Test failed at date:$testdate,machine:`hostname`,dir:$PBS_O_WORKDIR" ${S3REGTESTLIST} < perf-std.log 
 fi

#Store the results. 
S3REGTESTLIST='archan@cs.cmu.edu'
testdate=`date -I`
logdir=log.$testdate
resultfn=allresults
mkdir $logdir

cp ./src/tests/performance/*/*.raw ./perf-std.log ./perf-quick.log ./dailySTD.sh.[oe]* $logdir

#Add analysis script at here. 
cat $logdir/*.raw > $resultfn
${MAILX} -s "Results of S3 Standard Regression Test at date:$testdate,machine:`hostname`,dir:$PBS_O_WORKDIR " ${S3REGTESTLIST} < $resultfn
rm $resultfn