#/bin/sh

datadir='../../data/wikipedia/rnnlm/'

cp $datadir/train $datadir/train2
cat $datadir/valid >> $datadir/train2
