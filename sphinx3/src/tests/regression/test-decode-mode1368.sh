#!/bin/sh
. testfuncs.sh

tmpout="test-decode-mode1368.out"

echo "DECODE MODE 1368 (DO NOTHING MODE) TEST"
echo "This matches the current decoding routine call sequence with the default behaviour"

margs="-mdef $hub4am/hub4opensrc.6000.mdef \
-fdict $an4lm/filler.dict \
-dict $an4lm/an4.dict \
-mean $hub4am/means \
-var $hub4am/variances \
-mixw $hub4am/mixture_weights \
-tmat $hub4am/transition_matrices \
-ctl $an4lm/an4.ctl \
-cepdir $an4lm/ \
-agc none \
-varnorm no \
-cmn current \
-subvqbeam 1e-02 \
-epl 4 \
-fillprob 0.02 \
-feat 1s_c_d_dd \
-lw 9.5 \
-maxwpf 1 \
-beam 1e-40 \
-pbeam 1e-30 \
-wbeam 1e-20 \
-maxhmmpf 1500 \
-wend_beam 1e-1 \
-ci_pbeam 1e-5 \
-ds 2 \
-tighten_factor 0.4 \
-op_mode 1368"

lmargs="-lm $an4lm/an4.ug.lm.DMP"

clsargs="-lmctlfn $an4lm/an4.ug.cls.lmctl \
-ctl_lm  $an4lm/an4.ctl_lm" 

if run_program sphinx3_decode $margs $lmargs  > $tmpout 2>&1; then
    pass "DECODE MODE 1368 (DO NOTHING MODE) test"
else
    fail "DECODE MODE 1368 (DO NOTHING MODE) test"
fi
