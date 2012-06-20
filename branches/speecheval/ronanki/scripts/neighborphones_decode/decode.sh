SPHINX3=/home/ronanki/web/source/sphinx3/bin/sphinx3_decode
HMM=../test/GSoC_test/wsj_all_cd30.mllt_cd_cont_4000

$SPHINX3 -hmm ${HMM} -fsg phrase1.fsg -dict phone.dic -fdict phone.filler -ctl single_phone.ctl -cepdir feats -hyp single_phone.out -mode allphone -op_mode 2



