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

use File::Path;

my $index = 0;
if (lc($ARGV[0]) eq '-cfg') {
    $cfg_file = $ARGV[1];
    $index = 2;
} else {
    $cfg_file = "etc/sphinx_train.cfg";
}

if (! -s "$cfg_file") {
    print ("unable to find default configuration file, use -cfg file.cfg or create etc/sphinx_train.cfg for default\n");
    exit -3;
}
require $cfg_file;
require "$CFG_SCRIPT_DIR/util/utils.pl";

#***************************************************************************
# This script launches all the ci - continuous training jobs in the proper
# order. First it cleans up the directories, then launches the 
# flat initialization, and the baum-welch and norm jobs for the required
# number of iterations. Within each iteration it launches as many baumwelch
# jobs as the number of parts we wish to split the training into.
#***************************************************************************

my $iter = 1;
if (($#ARGV >= ($index))) {
   $iter= $ARGV[$index];
}
my $n_parts = ($CFG_NPART) ? $CFG_NPART : 1;
if (($#ARGV >= ($index+1))) {
   $n_part= $ARGV[$index+1];
}

$| = 1; # Turn on autoflushing
my $scriptdir = "${CFG_SCRIPT_DIR}/04.cd_schmm_untied";
my $logdir = "${CFG_LOG_DIR}/04.cd_schmm_untied";
mkdir ("$logdir",0777) unless -d $logdir;

#Read npart_untied from variables.def

if ($iter == 1) {
   # Clean up junk from earlier runs
   &ST_Log ("MODULE: 04 Training Context Dependent models\n");
   &ST_Log ("    Cleaning up directories: accumulator...");

    rmtree ($CFG_BWACCUM_DIR) unless ! -d $CFG_BWACCUM_DIR;
    mkdir ($CFG_BWACCUM_DIR,0777);

    &ST_Log ("logs...");
    rmtree ($logdir);
    mkdir ($logdir,0777) unless -d $logdir;

    &ST_Log ("\n");
    # For the first iteration Flat initialize models.
    $return_value = &Initialize();
    exit ($return_value) if ($return_value);
}

# The parallel version can't really cope with the huge disk read/writes
# so we always make this serial
if (0 && $MC && $n_parts > 1)
{
    # multi-processor version -- assumes ssh machine works
    for ($i=1; $i<=$n_parts; $i++)
    {
        $job_command = "\"$scriptdir/baum_welch.pl\" -cfg \"$cfg_file\" $iter $i $n_parts";
#	print $job_command."\n";
	open rrr,"scripts_pl/mc/mc_run.pl $job_command |";
	while ($line = <rrr>)
	{
	    chomp($line);
#	    print "mc_run: ".$line."\n";
	    @fff=split(/\s+/,$line);
            if ($fff[0] eq "MC")
            {
  	       $job_name = $fff[1];
	       last;
	    }
#            print "waiting for mc_run to say something\n";
	    sleep 3;
	}
        close rrr;
	if ($job_name eq "no_job")
	{
	    print "waiting for machine for part $i (of $n_parts) of iteration $iter\n";
	    sleep 30;
	    $i = $i-1;
	}
	else
	{
	    print "running part $i (of $n_parts) of iteration $iter on $job_name \n";
	    $parts[$i] = $job_name;
	    sleep 30; # to offset the runs
	}
    }
    # Wait for them all to finish
    $jobs_still_todo = 1;
    while ($jobs_still_todo)
    {
	$jobs_still_todo = 0;
	for ($i=1; $i<=$n_parts; $i++)
	{
	    if ( -f $parts[$i] )
	    {
		print "waiting for part $i (of $n_parts) of iteration $iter on ".$parts[$i]."\n";
		$jobs_still_todo = 1;
	    }
	}
	sleep 30;
    }
    print "iteration $iter: $n_parts completed\n";
}
else
{
    # Call baum_welch with iter part and n_part, 
    # once done call norm_and_lauchbw.pl
    for ($i=1; $i<=$n_parts; $i++)
    {
	system ("perl \"$scriptdir/baum_welch.pl\" -cfg \"$cfg_file\" $iter $i $n_parts");
    }
}
system ("perl \"$scriptdir/norm_and_launchbw.pl\" -cfg \"$cfg_file\" $iter $n_parts");

exit 0;

sub Initialize () {
  my $cihmmdir = "${CFG_BASE_DIR}/model_parameters/${CFG_EXPTNAME}.ci_${CFG_DIRLABEL}";
  my $cdhmmdir = "${CFG_BASE_DIR}/model_parameters/${CFG_EXPTNAME}.cd_${CFG_DIRLABEL}_untied";
  mkdir ($cdhmmdir,0777) unless -d $cdhmmdir;

  my $logdir  =  "${CFG_LOG_DIR}/04.cd_schmm_untied";
  mkdir ($logdir,0777) unless -d $logdir;
  my $logfile = "$logdir/${CFG_EXPTNAME}.copycitocd.log";

  &ST_Log ("    Initialization Copy CI to CD ");
  &ST_HTML_Print (&ST_FormatURL("$logfile", "Log File") . " ");

  my $COPY_CI_TO_CD = "${CFG_BIN_DIR}/init_mixw";

  $cmd = "\"$COPY_CI_TO_CD\" -src_moddeffn \"${CFG_BASE_DIR}/model_architecture/${CFG_EXPTNAME}.ci.mdef\" -src_ts2cbfn ${CFG_HMM_TYPE} -src_mixwfn \"$cihmmdir/mixture_weights\" -src_meanfn \"$cihmmdir/means\" -src_varfn \"$cihmmdir/variances\" -src_tmatfn \"$cihmmdir/transition_matrices\" -dest_moddeffn \"${CFG_BASE_DIR}/model_architecture/${CFG_EXPTNAME}.untied.mdef\" -dest_ts2cbfn ${CFG_HMM_TYPE} -dest_mixwfn \"$cdhmmdir/mixture_weights\" -dest_meanfn \"$cdhmmdir/means\" -dest_varfn \"$cdhmmdir/variances\" -dest_tmatfn \"$cdhmmdir/transition_matrices\" -feat ${CFG_FEATURE} -ceplen ${CFG_VECTOR_LENGTH}";

  return (RunTool($cmd, $logfile, 0));
}

