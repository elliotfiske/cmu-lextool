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
## 4. Redistributions of any form whatsoever must retain the following
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
#*************************************************************************
# This script runs the prunetree and tiestate scripts 
#*************************************************************************
#
#  Author: Alan W Black (awb@cs.cmu.edu)
#

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

my $scriptdir = "scripts_pl/06.prunetree";

my $logdir = "$CFG_LOG_DIR/06.prunetree";
mkdir ($logdir,0777) unless -d $logdir;

$| = 1; # Turn on autoflushing
&ST_Log ("MODULE: 06 Prune Trees)\n");
&ST_Log ("    Cleaning up old log files...\n");
system ("/bin/rm -f $logdir/*");

# Build all triphone model
my $logfile = "$logdir/$CFG_EXPTNAME.build.alltriphones.mdef.log";
$MAKE_MDEF = "$CFG_BIN_DIR/mk_mdef_gen";
$modarchdir          = "$CFG_BASE_DIR/model_architecture";
$ALLTRIPHONESMDDEF = "$modarchdir/$CFG_EXPTNAME.alltriphones.mdef";
$phonefile           = "$modarchdir/$CFG_EXPTNAME.phonelist";

system ("$MAKE_MDEF -phnlstfn $phonefile -oalltphnmdef $ALLTRIPHONESMDDEF -dictfn $CFG_DICTIONARY -fdictfn $CFG_FILLERDICT -n_state_pm $CFG_STATESPERHMM 2>$logfile");

system("$scriptdir/prunetree.pl -cfg $cfg_file $CFG_N_TIED_STATES");
system("$scriptdir/tiestate.pl -cfg $cfg_file $CFG_N_TIED_STATES");

