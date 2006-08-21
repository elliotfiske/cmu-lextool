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
## Author: David Huggins-Daines
##

use strict;
use File::Copy;
use File::Basename;
use File::Spec::Functions;
use File::Path;

use lib catdir(dirname($0), updir(), 'lib');
use SphinxTrain::Config;
use SphinxTrain::Util;

my ($iter, $n_parts) = @ARGV;
$iter = 1 unless defined $iter;
$n_parts = (defined($ST::CFG_NPART) ? $ST::CFG_NPART : 1) unless defined $n_parts;

my $modeldir  = "$ST::CFG_BASE_DIR/model_parameters";
mkdir ($modeldir,0777) unless -d $modeldir;

$| = 1; # Turn on autoflushing
my $logdir = "$ST::CFG_LOG_DIR/07.sa_train";
my $return_value = 0;

unless (defined($ST::CFG_SPEAKERLIST) and -r $ST::CFG_SPEAKERLIST) {
    Log("MODULE: 07 Training Speaker Adaptive models\n");
    Log("    Skipped (no speaker list defined in config\n");
    exit 0;
}

my $hmmdir = "$modeldir/${ST::CFG_EXPTNAME}.sat_$ST::CFG_DIRLABEL";

# We have to clean up and run flat initialize if it is the first iteration
if ($iter == 1) {
    Log ("MODULE: 07 Training Speaker Adaptive models\n");
    Log ("    Cleaning up directories: accumulator...");
    rmtree($ST::CFG_BWACCUM_DIR, 0, 1);
    mkdir ($ST::CFG_BWACCUM_DIR,0777);
    Log ("logs...");
    rmtree($logdir, 0, 1);
    mkdir ($logdir,0777);
    Log ("qmanager...");
    rmtree ($ST::CFG_QMGR_DIR, 0, 1);
    mkdir ($ST::CFG_QMGR_DIR,0777);
    Log ("models...\n");
    rmtree($hmmdir, 0, 1);
    mkdir ($hmmdir,0777);

    exit ($return_value) if ($return_value);
}

# Read list of speakers
open SPEAKER, "<$ST::CFG_SPEAKERLIST" or die "Failed to open $ST::CFG_SPEAKERLIST: $!";
chomp(my @speakers = <SPEAKER>);
close SPEAKER;

my @deps;
# Run baum_welch twice:
# First to calculate MLLR transform based on previous SA model
# (this is actually kind of bogus, but whatever...)
Log("    Accumulating for speaker MLLR transforms...");
foreach (@speakers) {
    push @deps, [$_ => LaunchScript("bw.$iter.si.$_", ['baum_welch.pl', $iter, $_, 'si'])];
}
# Now estimate and transform means
foreach (@deps) {
    my ($speaker, $job) = @$_;
    WaitForScript($job);

    # Run mllr_solve
    Log(" $speaker");
    my $inmodeldir = ($iter == 1) ?
	catfile($ST::CFG_MODEL_DIR,
		"${ST::CFG_EXPTNAME}.cd_${ST::CFG_DIRLABEL}_${ST::CFG_N_TIED_STATES}")
	    : $hmmdir;
    my $mllrfile = catfile($hmmdir, "$ST::CFG_EXPTNAME.$speaker.mllr");
    my $mllrmeans = catfile($hmmdir, "$ST::CFG_EXPTNAME.$speaker.means");
    my $logfile  = "$logdir/${ST::CFG_EXPTNAME}.$iter-$speaker.mllr_solve.log";
    my $rv = RunTool
	('mllr_solve', $logfile, 0,
	 -outmllrfn => $mllrfile,
	 -accumdir => catfile($ST::CFG_BASE_DIR,
			      'bwaccumdir', "${ST::CFG_EXPTNAME}_buff_${speaker}"),
	 -meanfn => catfile($inmodeldir, 'means'),
	 -varfn => catfile($inmodeldir, 'variances'));
    exit $rv if $rv;

    my $rv = RunTool
	('mllr_transform', $logfile, 0,
	 -inmeanfn => catfile($inmodeldir, 'means'),
	 -outmeanfn => $mllrmeans,
	 -mllrmat => $mllrfile,
	 -varfn => catfile($inmodeldir, 'variances'));
    exit $rv if $rv;
}

# Second to train new SA model with speaker transforms
@deps = ();
Log("\n    Training Speaker-Adaptive model...\n");
foreach (@speakers) {
    push @deps, LaunchScript("bw.$iter.sa.$_", ['baum_welch.pl', $iter, $_, 'sa'])
}
LaunchScript("norm.$iter", ['norm_and_launchbw.pl', $iter, $n_parts], \@deps);

# For the first iteration (i.e. the one that was called from the
# command line or a parent script), wait until completion or error
if ($iter == 1) {
    $return_value = WaitForConvergence($logdir);
}
exit $return_value;
