#!/usr/bin/bash

lm_path=$1
fst_path=$2

java -classpath ../lib/commons-lang-2.0.jar:../lib/sphinx4.jar:../bin/ CreateLmFSA $lm_path $fst_path