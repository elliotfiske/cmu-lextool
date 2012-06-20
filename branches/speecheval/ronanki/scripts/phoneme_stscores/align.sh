SPHINX3_ALIGN=./sphinx3/bin/sphinx3_align # path to sphinx3_align in sphinx3 directory
HMM=./wsj_all_cd30.mllt_cd_cont_4000 # Download acoustic models from net and place it in your working directory

# -hmm - Acoustic Models (currently using WSJ)
# -dic - complete dictionary 
# -fdict- basic filler file
# -ctl - file ontaining all input files without extension
# -insent - input utterances to be aligned with wave files
# - feats - input feats directory
# -stsegdir - output directory for each frame acoustic scores
# -phsegdir - output directory for phoneme acoustic scores
# -wdsegdir - output directory for word acoustic scores
# -phlabdir - output directory for phoneme labels
# - outsent - output phonemes

$SPHINX3_ALIGN -hmm ${HMM} -dict cmu.dic -fdict phone.filler -ctl phone.ctl -insent phone.insent -cepdir feats -phsegdir phonesegdir -phlabdir phonelabdir -outsent phone.outsent -stsegdir statesegdir -wdsegdir wordsegdir

