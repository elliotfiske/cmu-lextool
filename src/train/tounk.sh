#!/bin/bash

#replaceOOV words into <unk>

trainfile=../../data/wikipedia/rnnlm/train
validfile=../../data/wikipedia/rnnlm/valid
testfile=../../data/wikipedia/rnnlm/test

trainfile3=../../data/wikipedia/rnnlm/train3
validfile3=../../data/wikipedia/rnnlm/valid3
testfile3=../../data/wikipedia/rnnlm/test3

vocab=../../data/wikipedia/wiki-50k.vocab

python tounk.py $trainfile $vocab > $trainfile3
python tounk.py $validfile $vocab > $validfile3
python tounk.py $testfile  $vocab > $testfile3
