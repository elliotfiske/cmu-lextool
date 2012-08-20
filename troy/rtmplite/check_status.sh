#!/bin/bash

##################################
# Put a crontab for this script
#

# pidfile contains the rtmp process id in the first line
pidfile=/home/li-bo/.rtmp_pid
# rtmp.py folder
exefile=/home/li-bo/tools/rtmplite/rtmp.py
# recording data root
dataroot=/home/li-bo/web/data/

# Only if the $pidfile exists, will we check the status.
# No $pidfile means we don't want to start the rtmp service yet.
# Thus everytime after starting the rtmp service, do create the $pidfile with
#	its process id inside.
if [ -e "$pidfile" ]
then
	#echo `/bin/date` "The rtmp service should be up!"
	
	# check whether the process is running
	rtmppid=`/usr/bin/head -n 1 ${pidfile} | /usr/bin/awk '{print $1}'`;
	#echo "Old pid: ${rtmppid}"
	
	# restart the process if not running
	if [ ! -d /proc/${rtmppid} ]
	then
		/usr/bin/python ${exefile} -r ${dataroot} &
		rtmppid=$!
		echo "${rtmppid}" > ${pidfile}
		echo `/bin/date` "### rtmp process restarted with pid: ${rtmppid}"
	fi
fi
