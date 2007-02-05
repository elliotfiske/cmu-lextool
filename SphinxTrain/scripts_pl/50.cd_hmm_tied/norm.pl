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
## This work was supported in part by funding from the Defense Advanced
## Research Projects Agency and the National Science Foundation of the
## United States of America, and the CMU Sphinx Speech Consortium.
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
## Author: Ricky Houghton
##

use strict;
use File::Copy;
use File::Basename;
use File::Spec::Functions;
use File::Path;

use lib catdir(dirname($0), updir(), 'lib');
use SphinxTrain::Config;
use SphinxTrain::Util;

#*****************************************************************************
#  Baum-welch is done in several parts. This script gathers the results of
#  all those parts and then computes the discrete probability distributions
#  associated with all the states. It also computes the transition matrices.
#****************************************************************************

$| = 1; # Turn on autoflushing

die "USAGE: $0 <ngau> <iter>" if @ARGV != 2;
my ($n_gau, $iter) = @ARGV;

my $modelname="${ST::CFG_EXPTNAME}.cd_${ST::CFG_DIRLABEL}_$ST::CFG_N_TIED_STATES";
my $processpart="50.cd_hmm_tied";

my $bwaccumdir 	     = "${ST::CFG_BASE_DIR}/bwaccumdir";
opendir(ACCUMDIR, $bwaccumdir)
    or die "Could not open $bwaccumdir: $!";
my @bwaccumdirs = map catdir($bwaccumdir, $_),
    grep /^${ST::CFG_EXPTNAME}_buff_/, readdir(ACCUMDIR);
closedir(ACCUMDIR);
my $hmmdir              = "${ST::CFG_BASE_DIR}/model_parameters/$modelname";
mkdir ($hmmdir,0777);
my $means               = "$hmmdir/means";
my $variances           = "$hmmdir/variances";
my $mixture_weights     = "$hmmdir/mixture_weights";
my $transition_matrices = "$hmmdir/transition_matrices";

if ($iter == 1) {
    my $mdeffile = catfile($ST::CFG_BASE_DIR, 'model_architecture',
			   "${ST::CFG_EXPTNAME}.$ST::CFG_N_TIED_STATES.mdef");
    # Copy the mdef and fillerdict files into the new HMM directory
    copy($mdeffile, catfile($hmmdir, 'mdef'))
	or die "Failed to copy $mdeffile to $hmmdir/mdef: $!";
    copy($ST::CFG_FILLERDICT, catfile($hmmdir, 'noisedict'))
	or die "Failed to copy $ST::CFG_FILLERDICT to $hmmdir/noisedict: $!";
}

my  $logdir              = "${ST::CFG_LOG_DIR}/$processpart";
mkdir ($logdir,0777);
my $logfile 	     = "$logdir/${ST::CFG_EXPTNAME}.${n_gau}.${iter}.norm.log";

copy "$ST::CFG_GIF_DIR/green-ball.gif", "$ST::CFG_BASE_DIR/.50.norm.${n_gau}.$iter.state.gif";
HTML_Print ("\t" . ImgSrc("$ST::CFG_BASE_DIR/.50.norm.${n_gau}.$iter.state.gif") . " ");   
Log ("    Normalization for iteration: $iter ");
HTML_Print (FormatURL("$logfile", "Log File") . " ");

# if there is an LDA transformation, use it
my @feat;
if (defined($ST::CFG_LDA_TRANSFORM) and -r $ST::CFG_LDA_TRANSFORM) {
    @feat = (-feat => '1s_c', -ceplen => $ST::CFG_LDA_DIMENSION);
}
else {
    @feat = (-feat => $ST::CFG_FEATURE, -ceplen => $ST::CFG_VECTOR_LENGTH);
}
my $return_value = RunTool
    ('norm', $logfile, 0,
     -accumdir => @bwaccumdirs,
     -mixwfn => $mixture_weights,
     -tmatfn => $transition_matrices,
     -meanfn => $means,
     -varfn => $variances,
     -fullvar => $ST::CFG_FULLVAR,
     @feat
    );

if ($return_value) {
  copy "$ST::CFG_GIF_DIR/red-ball.gif", "$ST::CFG_BASE_DIR/.50.norm.${n_gau}.$iter.state.gif";
  LogError ("\tFailed to start norm \n");
}
exit ($return_value);
