#!/usr/bin/bash

java -classpath ../lib/commons-lang-2.0.jar:../lib/sphinx4.jar:../bin/ PostProcessing "$1" "$2" "$3" "$4"
