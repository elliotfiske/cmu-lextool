#!/bin/bash

rnnpath=/media/PRIVATE/loop/project/workspace/sphinx4/src/train/rnnlm/rnnlm-0.3e
trainfile=/media/PRIVATE/loop/project/workspace/sphinx4/data/wiki/rnnlm/train
#trainfile=/media/PRIVATE/loop/project/workspace/sphinx4/src/train/rnnlm/simple-examples/data/ptb.train.txt
validfile=/media/PRIVATE/loop/project/workspace/sphinx4/data/wiki/rnnlm/valid
#validfile=/media/PRIVATE/loop/project/workspace/sphinx4/src/train/rnnlm/simple-examples/data/ptb.valid.txt
testfile=/media/PRIVATE/loop/project/workspace/sphinx4/data/wiki/rnnlm/test
#testfile=/media/PRIVATE/loop/project/workspace/sphinx4/src/train/rnnlm/simple-examples/data/ptb.test.txt
rnnmodel=/media/PRIVATE/loop/project/workspace/sphinx4/data/wiki/rnnlm/wiki.model.hidden100.class100.rnnlm
temp=../temp

hidden_size=100
class_size=100
bptt_steps=4

#################################
# CHECK FOR 'rnnlm' AND 'convert'
#################################

if [ ! -e $rnnpath/rnnlm ]; then
    make clean -C $rnnpath
    make -C $rnnpath
fi

if [ ! -e $rnnpath/rnnlm ]; then
    echo "Cannot compile rnnlm tool";
    exit
fi

if [ ! -e $rnnpath/convert ]; then
    gcc $rnnpath/convert.c -O2 -o $rnnpath/convert
fi

#################################################
# IF MODEL FILE ALREADY EXISTS, IT WILL BE ERASED
#################################################

if [ -e $rnnmodel ]; then
    rm $rnnmodel
fi

if [ -e $rnnmodel.output.txt ]; then
    rm $rnnmodel.output.txt
fi

################################
# TRAINING OF RNNLM HAPPENS HERE
################################

time $rnnpath/rnnlm -train $trainfile -valid $validfile -rnnlm $rnnmodel -hidden $hidden_size -rand-seed 1 -debug 2 -class $class_size -bptt $bptt_steps -bptt-block 10

##################################################
# MODELS ARE COMBINED HERE, PERPLEXITY IS REPORTED
##################################################

time $rnnpath/rnnlm -rnnlm $rnnmodel -test $testfile
