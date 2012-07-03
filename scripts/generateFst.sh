#!/usr/bin/bash

#Usage: sh ./generateFST.sh -text t -lm lm_path -path output_path

java -classpath ../lib/commons-lang-2.0.jar:../lib/sphinx4.jar:../bin/ GenerateFSA "$1" "$2" "$3" "$4" "$5" "$6"
