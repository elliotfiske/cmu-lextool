SPHINX3=/home/ronanki/web/source/sphinx3/bin/sphinx3_decode
HMM=/home/ronanki/web/test/GSoC_test/wsj_all_cd30.mllt_cd_cont_4000

$SPHINX3 -hmm ${HMM} -fsg sample.fsg -dict phone.dic -fdict phone.filler -ctl phrase.ctl -cepdir feats -hyp sample.out -mode allphone -op_mode 2



