#!/usr/bin/perl
## ====================================================================
##
## Copyright (c) 1996-2000 Carnegie Mellon University.  All rights 
## reserved.
##
## Redistribution and use in source and binary forms, with or without
## modification, are permitted provided that the following conditions
## are met:
##
## 1. Redistributions of source code must retain the above copyright
##    notice, this list of conditions and the following disclaimer. 
##
## 2. Redistributions in binary form must reproduce the above copyright
##    notice, this list of conditions and the following disclaimer in
##    the documentation and/or other materials provided with the
##    distribution.
##
## 3. The names "Sphinx" and "Carnegie Mellon" must not be used to
##    endorse or promote products derived from this software without
##    prior written permission. To obtain permission, contact 
##    sphinx@cs.cmu.edu.
##
## 4. Products derived from this software may not be called "Sphinx"
##    nor may "Sphinx" appear in their names without prior written
##    permission of Carnegie Mellon University. To obtain permission,
##    contact sphinx@cs.cmu.edu.
##
## 5. Redistributions of any form whatsoever must retain the following
##    acknowledgment:
##    "This product includes software developed by Carnegie
##    Mellon University (http://www.speech.cs.cmu.edu/)."
##
## THIS SOFTWARE IS PROVIDED BY CARNEGIE MELLON UNIVERSITY ``AS IS'' AND 
## ANY EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
## THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
## PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY
## NOR ITS EMPLOYEES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
## SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
## LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
## DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
## THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
## (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
## OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
##
## ====================================================================
##
## Author: Ricky Houghton (converted from scripts by Rita Singh)
##


my $index = 0;

# RAH Force passage of config file, or look for it one directory up.
if (lc($ARGV[0]) eq '-cfg') {
    $cfg_file = $ARGV[1];
    if (! -s $cfg_file) {
	print "-cfg specified, but unable to find file $ARGV[1]\n";
	exit -3;
    }
    $index = 2;
    require $cfg_file;
} else {
    $cfg_file = "../sphinx_train.cfg";
    require $cfg_file;
    &ST_LogWarning("-cfg not specified, using the default ../sphinx_train.cfg");
}


my $mdeffn   = "${CFG_BASE_DIR}/model_architecture/${CFG_EXPTNAME}.ci.mdef";
my $hmm_dir  = "${CFG_BASE_DIR}/model_parameters/${CFG_EXPTNAME}.ci_semi";
my $meanfn   = "$hmm_dir/means";
my $varfn    = "$hmm_dir/variances";
my $mixwfn   = "$hmm_dir/mixture_weights";
my $tmp_str = time();
my $tempfn   = "${CFG_BASE_DIR}/tmp/questions.$tmp_str";
#my $questfn  = "${CFG_BASE_DIR}/model_architecture/${CFG_EXPTNAME}.tree_questions";
my $questfn = ${CFG_QUESTION_SET};

my $logdir = "${CFG_LOG_DIR}/05.buildtrees";
mkdir ($logdir,0777) unless -d $logdir;
my $logfile = "$logdir/${CFG_EXPTNAME}.make_questions.log";

#$MAKE_QUEST = "~rsingh/09..sphinx3code/trainer/bin.alpha/make_quests";
my $MAKE_QUEST = "${CFG_BIN_DIR}/make_quests";

&ST_Log ("MODULE: 05 Build Trees\n");
&ST_Log ("\tMake Questions ");
&ST_HTML_Print ("<A HREF=\"$logfile\">Log File</A>\n");

$|=1;
if (open PIPE, "$MAKE_QUEST -moddeffn $mdeffn -meanfn $meanfn -varfn $varfn -mixwfn $mixwfn -npermute 8 -niter 0 -qstperstt 30 -tempfn $tempfn -questfn $questfn -type ${CFG_HMM_TYPE} 2>&1 |") {

    open LOG,">$logfile";
while (<PIPE>) {
    print LOG "$_";
}
close PIPE;
close LOG;
&ST_Log ("\tFinished\n");
$| = 0;
exit 0;
}


&ST_Log ("\tFailure\n");
exit -1
