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
#**************************************************************************
# This script reads off the leaves of the pruned trees and creates an mdef
# file
#**************************************************************************


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


die "USAGE: $0 <iteration number>" if (($#ARGV != ($index)));

my $n_tied_states = $ARGV[$index];

my $untied_mdef_file = "$CFG_BASE_DIR/model_architecture/$CFG_EXPTNAME.alltriphones.mdef";
my $prunedtreedir = "$CFG_BASE_DIR/trees/$CFG_EXPTNAME.$n_tied_states";
my $tied_mdef_file = "$CFG_BASE_DIR/model_architecture/$CFG_EXPTNAME.$n_tied_states.mdef";

my $logdir = "$CFG_LOG_DIR/06.prunetree";
mkdir ($logdir,0777) unless -d $logdir;
my $logfile = "$logdir/$CFG_EXPTNAME.tiestate.$n_tied_states.log";

my $TIESTATE = "$CFG_BIN_DIR/tiestate";

system("$TIESTATE -imoddeffn $untied_mdef_file -omoddeffn $tied_mdef_file -treedir $prunedtreedir -psetfn $CFG_QUESTION_SET 2>$logfile");

