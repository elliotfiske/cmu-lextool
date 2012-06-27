#!/usr/bin/bash
lm=$1
dic=$2
filler=$3

java -classpath ../lib/commons-lang-2.0.jar:../lib/sphinx4.jar:../bin/ Main $lm $dic $filler
