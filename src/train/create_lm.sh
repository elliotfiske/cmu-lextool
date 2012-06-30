#/bin/sh
#create a Language model using CMULM tool from wikipedia data

Task='wiki'											#The task name
V='65k'													#vocabulary size, "$Task-$V" will be the output language model name
dV=65000
Train='../../data/wikipedia/rnnlm/train2'		#train data file path
Test='../../data/wikipedia/rnnlm/test'			#test data file path, use to get the perplexity
outdir='../../data/wikipedia'
cutoff_2=2												#cutoff for bigram
cutoff_3=2												#cutoff for trigram

# Create vocabulary
#cat $Train | text2wfreq | wfreq2vocab -top $dV > $outdir/$Task-$V.vocab

# Create trigram
#cat $Train | text2idngram -vocab $outdir/$Task-$V.vocab > $outdir/$Task-$V.idngram

# Convert to binary LM
#idngram2lm -idngram $outdir/$Task-$V.idngram -vocab $outdir/$Task-$V.vocab -arpa $outdir/$Task-$V-$cutoff_2$cutoff_3.arpa -calc_mem -cutoffs $cutoff_2 $cutoff_3

#rm $outdir/$Task-$V.idngram

# Calc perplexity
echo "-----------------------------------------------------------------" > log.txt
echo "LM: $outdir/$Task-$V-$cutoff_2$cutoff_3.arpa" >> log.txt
echo "perplexity -text $Test" | evallm -arpa $outdir/$Task-$V-$cutoff_2$cutoff_3.arpa >> log.txt

#convert to DMP form, doesn't work now
# sphinx_lm_convert -i $outdir/$Task-$V-$cutoff_2$cutoff_3.arpa -o $outdir/$Task-$V-$cutoff_2$cutoff_3.lm.DMP
