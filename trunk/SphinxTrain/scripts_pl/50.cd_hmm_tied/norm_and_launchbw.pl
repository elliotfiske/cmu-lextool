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
## Modified: Rita Singh, 27 Nov 2000
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

#*******************************************************************
#*******************************************************************
$| = 1; # Turn on autoflushing

die "USAGE: $0 <ngau> <iter> <n_parts>" if (@ARGV != 3);

my ($n_gau, $iter, $n_parts) = @ARGV;

my $processname="50.cd_hmm_tied";

my $logdir ="$ST::CFG_LOG_DIR/$processname";
mkdir ($logdir,0777) unless -d $logdir;
my $log = "$logdir/${ST::CFG_EXPTNAME}.$n_gau.$iter.norm.log";

# Check the number and list of parts done. Compute avg likelihood per frame
my $num_done = 0; my $tot_lkhd = 0; my $tot_frms = 0;
my @done;
for (my $i=1;$i<=$n_parts;$i++){
    $done[$i] = 0;
    my $input_log = "${logdir}/${ST::CFG_EXPTNAME}.${n_gau}.${iter}-${i}.bw.log";
    next if (! -s $input_log);
    open LOG,$input_log;
    while (<LOG>) {
        if (/.*(Counts saved to).*/) {
            $num_done++;
            $done[$i] = 1;
        }
        if (/.*(overall>).*/){
            my (undef, undef, $nfrms, undef, undef, $lkhd) = split(/ /);
            $tot_lkhd = $tot_lkhd + $lkhd;
            $tot_frms = $tot_frms + $nfrms;
        }
    }
    close LOG;
}

if ($num_done != $n_parts) {
    open OUTPUT,">$log";
    print OUTPUT "Only $num_done parts of $n_parts of Baum Welch were successfully completed\n";
    print "Parts ";
    for (my $i=1;$i<=$n_parts;$i++) {
        print OUTPUT "$i " if ($done[$i] == 0);
    }
    print OUTPUT "failed to run!\n";
    close OUTPUT;
    exit (0);
}

if ($tot_frms == 0) {
    open OUTPUT,">$log";
    print OUTPUT "Baum welch ran successfully for only 0 frames! Aborting..\n";
    close OUTPUT;
    exit (0);
}

my $lkhd_per_frame = $tot_lkhd/$tot_frms;

my $previter = $iter - 1;
my $prev_norm = "${logdir}/${ST::CFG_EXPTNAME}.${n_gau}.${previter}.norm.log";
if (! -s $prev_norm) {
    # Either iter == 1 or we are starting from an intermediate iter value
    RunScript('norm.pl', $n_gau, $iter);
    open OUTPUT, ">> $log";
    print OUTPUT "Current Overall Likelihood Per Frame = $lkhd_per_frame\n";
    close OUTPUT;
    Launch_BW($n_gau, $iter);
    exit (0);
}

# Find the likelihood from the previous iteration
open LOG, $prev_norm; my $prevlkhd = -99999999;
while (<LOG>) {
   if (/.*(Current Overall Likelihood Per Frame).*/){
      (undef,undef,undef,undef,undef,undef,$prevlkhd) = split(/ /);
   }
}
close LOG;

if ($prevlkhd == -99999999) {
    # Some error with previous norm.log. Continue Baum Welch
    RunScript('norm.pl', $n_gau, $iter);
    open OUTPUT, ">> $log";
    print OUTPUT "Current Overall Likelihood Per Frame = $lkhd_per_frame\n";
    close OUTPUT;
    Launch_BW($n_gau, $iter);
    exit (0);
}

my ($convg_ratio, $absprev);
if ($prevlkhd == 0) {
    $convg_ratio = 0;
    $convg_ratio = 1 if ($lkhd_per_frame > 0);
    $convg_ratio = -1 if ($lkhd_per_frame < 0);
}
else {
    $absprev = $prevlkhd;
    $absprev = -$absprev if ($prevlkhd < 0);
    $convg_ratio = ($lkhd_per_frame - $prevlkhd)/$absprev;
}
RunScript('norm.pl', $n_gau, $iter);

open OUTPUT, ">> $log";
print OUTPUT "Current Overall Likelihood Per Frame = $lkhd_per_frame\n";
if (defined($convg_ratio)) {
    print OUTPUT "Convergence ratio = $convg_ratio\n";
}

if ($convg_ratio < 0) {
    print OUTPUT "*WARNING*: NEGATIVE CONVERGENCE RATIO! CHECK YOUR DATA AND TRASNCRIPTS\n";
    print "*WARNING*: NEGATIVE CONVERGENCE RATIO AT ITER ${iter}! CHECK BW AND NORM LOGFILES\n";
}
close OUTPUT;

if ($convg_ratio > $ST::CFG_CONVERGENCE_RATIO && $iter >= $ST::CFG_MAX_ITERATIONS) {
    Launch_SplitGaussian($n_gau);
    open OUTPUT, ">> $log";
    print OUTPUT "Maximum desired iterations $ST::CFG_MAX_ITERATIONS performed. Terminating training iteration\n";
    print OUTPUT "******************************TRAINING COMPLETE*************************\n";
    my $date = localtime;
    print OUTPUT "$date\n";
    print "Maximum desired iterations $ST::CFG_MAX_ITERATIONS performed. Terminating CD training\n";
    close OUTPUT;
    exit (0);
}

if ($convg_ratio > $ST::CFG_CONVERGENCE_RATIO) {
    Launch_BW($n_gau, $iter);
    exit (0);
}
else {
    print "        Current Overall Likelihood Per Frame = $lkhd_per_frame\n";
    Launch_SplitGaussian($n_gau);
    open OUTPUT, ">> $log";
    print OUTPUT "Likelihoods have converged! Baum Welch training completed\!\n";
    print OUTPUT "******************************TRAINING COMPLETE*************************\n";
    my $date = localtime;
    print OUTPUT "$date\n";
    close OUTPUT;
    exit (0);
}

sub Launch_BW {
    my ($n_gau, $iter) = @_;
    my $newiter = $iter + 1;
    print "        Current Overall Likelihood Per Frame = $lkhd_per_frame\n";
    print "        Convergence Ratio = $convg_ratio\n" if defined $convg_ratio;
    RunScript('slave_convg.pl', $n_gau, $newiter, $n_parts);
}

sub Launch_SplitGaussian {
    my $n_gau = shift;

    if ($n_gau < $ST::CFG_FINAL_NUM_DENSITIES && $ST::CFG_HMM_TYPE ne ".semi.") {
# Do stuff
	my $n_split;
        if ($ST::CFG_FINAL_NUM_DENSITIES >= 2 * $n_gau) {
	    $n_split = $n_gau;
        } else {
	    $n_split = $ST::CFG_FINAL_NUM_DENSITIES - $n_gau;
        }
        RunScript('split_gaussians.pl', $n_gau, $n_split);

        Launch_BW($n_gau + $n_split, 0);
	exit 0;
      } elsif ($n_gau == $ST::CFG_FINAL_NUM_DENSITIES) {
# If we finally reached the desired number of densities, we still have
# to copy the models to their final destination. The split gaussian
# script below will do just that
        RunScript('split_gaussians.pl', $ST::CFG_FINAL_NUM_DENSITIES, 0);
      }
}
