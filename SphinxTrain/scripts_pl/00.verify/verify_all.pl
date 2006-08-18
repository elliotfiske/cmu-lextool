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
## Author: Ricky Houghton (converted from scripts by Rita Singh)
## Author: David Huggins-Daines (converted from scripts by Rita Singh)
##
use strict;
use File::Copy;
use File::Basename;
use File::Spec::Functions;

use lib catdir(dirname($0), updir(), 'lib');
use SphinxTrain::Config;
use SphinxTrain::Util;

$| = 1;				# Turn on autoflushing
my $ret_value = 0;

# My test files for OS case sensitivity
my $lowercase_file = "tmp_case_sensitive_test";
my $uppercase_file = "TMP_CASE_SENSITIVE_TEST";
# Just in case, clean up both cases
unlink $uppercase_file;
unlink $lowercase_file;
# Create file with lowercase name
open (TEST, ">$lowercase_file");
close(TEST);
# Now, try to open with uppercase name
my $is_case_sensitive;
if (open(TEST, "<$uppercase_file")) {
# If successful, the OS is case insensitive, and we have to check for
# phones in a case insensitive manner
    $is_case_sensitive = 0;
    close(TEST);
    Log("O.S. is case insensitive (\"A\" == \"a\").\n" .
	   "Phones will be treated as case insensitive.\n");
} else {
# If unsuccessful, the OS is case sensitive, and we have to check for
# phones in a case sensitive manner
    $is_case_sensitive = 1;
    Log("O.S. is case sensitive (\"A\" != \"a\").\n" .
	   "Phones will be treated as case sensitive.\n");
}
# Clean up the mess
unlink $lowercase_file;
unlink $uppercase_file;

Log ("MODULE: 00 verify training files\n");

# PHASE 1: Check to see if the phones in the dictionary are listed in the phonelist file
# PHASE 2: Check to make sure there are not duplicate entries in the dictionary
my %phonelist_hash;
{
    open DICT,"$ST::CFG_DICTIONARY" or die "Can not open the dictionary ($ST::CFG_DICTIONARY)";

    my %dict_phone_hash = ();
    my %dict_hash = ();

    Log ("    Phase 1: DICT - Checking to see if the dict and filler dict agrees with the phonelist file\n");
    # This is rather ugly, but it's late and I'm just trying to get the pieces together
    # Clean it up later

    # Read the dictionary and stick phones into dict_phone_hash
    my $counter =0;
    while (<DICT>) {
	if (/^(\S+)\s(.*)$/) {
	    $dict_hash{$1}++;
	    my $phonetic = $2;
	    # Aggregate the non-space characters and store the results
	    # in @phone
	    my @phones = ($phonetic =~ m/(\S+)/g);
	    for my $phone (@phones) {
	      if ($is_case_sensitive) {
		$dict_phone_hash{$phone}++;
	      } else {
		$dict_phone_hash{uc($phone)}++;
	      }
	    }
	}
	$counter++;
    }
    close DICT;

    open DICT,"$ST::CFG_FILLERDICT" or die "Can not open filler dict ($ST::CFG_FILLERDICT)\n";
    while (<DICT>) {
	if (/^(\S+)\s(.*)$/) {
	    $dict_hash{$1}++;
	    my $phonetic = $2;
	    my @phones = ($phonetic =~ m/(\S+)/g);
	    for my $phone (@phones) {
	      if ($is_case_sensitive) {
		$dict_phone_hash{$phone}++;
	      } else {
		$dict_phone_hash{uc($phone)}++;
	      }
	    }
	}
	$counter++;
    }
    close DICT;

    # Read the phonelist and stick phones into phonelist_hash
    open PHONE,"$ST::CFG_RAWPHONEFILE" or die "Can not open phone list ($ST::CFG_RAWPHONEFILE)\n";
    my $has_SIL = 0;
    while (<PHONE>) {
	chomp;
	if (m/\s/) {
	  my $status = 'FAILED';
	  $ret_value = -1;
	  LogWarning("Phone \"$_\" has extra white spaces\n")
	}
	$has_SIL = 1 if m/^SIL$/;
	if ($is_case_sensitive) {
	  $phonelist_hash{$_} = 0;
	} else {
	  $phonelist_hash{uc($_)} = 0;
	}
    }
    close PHONE;

    unless ($has_SIL) {
	my $status = 'FAILED';
	$ret_value = -1;
	LogWarning ("The phonelist ($ST::CFG_RAWPHONEFILE) does not define the phone SIL (required!)\n");
      }
    
    my @keys = keys %dict_phone_hash;
    Log ("        Found $counter words using $#keys phones\n");
    
    my $status = 'passed';
    for my $key (sort (keys %dict_phone_hash)){
	if (defined($phonelist_hash{$key})) {
	    $phonelist_hash{$key} = 1;
	} else {
	    my $status = 'FAILED';
	    $ret_value = -1;
	    copy("$ST::CFG_GIF_DIR/red-ball.gif", "$ST::CFG_BASE_DIR/.00.1.state.gif");
	    LogWarning ("This phone ($key) occurs in the dictionary ($ST::CFG_DICTIONARY), but not in the phonelist ($ST::CFG_RAWPHONEFILE)\n");
	}
    }

    for my $key (sort (keys %phonelist_hash)) {
      if ($phonelist_hash{$key} == 0) {
	    my $status = 'FAILED';
	    $ret_value = -1;
	    copy("$ST::CFG_GIF_DIR/red-ball.gif", "$ST::CFG_BASE_DIR/.00.1.state.gif");
	    LogWarning ("This phone ($key) occurs in the phonelist ($ST::CFG_RAWPHONEFILE), but not in the dictionary ($ST::CFG_DICTIONARY)\n");
	}
    }

    HTML_Print ("\t\t<font color=\"$ST::CFG_OKAY_COLOR\"> $status </font>\n") if ($status eq 'passed');
    HTML_Print ("\t\t<font color=\"$ST::CFG_ERROR_COLOR\"> $status </font>\n") if ($status eq 'FAILED');
#    Log("\t\t$status\n");

    Log("    Phase 2: DICT - Checking to make sure there are not duplicate entries in the dictionary\n");
    my $duplicate_status = 'passed';
    for my $key (keys %dict_hash) {
	if ($dict_hash{$key} > 1) {
	    $ret_value = -2;
	    $duplicate_status = 'FAILED';
	    copy("$ST::CFG_GIF_DIR/red-ball.gif", "$ST::CFG_BASE_DIR/.00.2.state.gif");
	    LogWarning("This word ($key) has duplicate entries in ($ST::CFG_DICTIONARY)\n");
	}
    }
#    Log ("\t\t$duplicate_status\n");
    HTML_Print ("\t\t<font color=\"$ST::CFG_OKAY_COLOR\"> $duplicate_status </font>\n") if($duplicate_status eq 'passed');
    HTML_Print ("\t\t<font color=\"$ST::CFG_ERROR_COLOR\"> $duplicate_status </font>\n") if($duplicate_status eq 'FAILED');
}


# Check to make sure .ctl file is roughly of correct format
# 3.) Check that each utterance specified in the .ctl file has a positive length
#     Verify that the files listed are available and are not of size 0
# 4.) Check number of lines in the transcript and in ctl - they should be the same\n";
{
    my ($status,@ctl_lines,$ctl_line,$file,$start,$end,$number_ctl_lines,$number_lines_transcript);
    
    open CTL,"$ST::CFG_LISTOFFILES" or die "Can not open listoffiles ($ST::CFG_LISTOFFILES)";
    @ctl_lines = <CTL>;		# We are going to iterate over this several times
    close CTL;

    # 3.) Check that each utterance specified in the .ctl file has a positive length
    #     Verify that the files listed are available and are not of size 0

    Log("    Phase 3: CTL - Check general format; utterance length (must be positive); files exist\n");
    $status = 'passed';
    my $estimated_training_data = 0;
    for $ctl_line (@ctl_lines) {
        chomp($ctl_line);
	# Accept: filename int int possible_comment
	if ($ctl_line =~ m/(.+)\s(\d+)\s(\d+).*/) {
	    $file = $1;
	    $start = $2;
	    $end = $3;
	    if ((defined $start) and (defined $file)) {
		if ($end <= $start) {
		    warn "Utterance length is <= 0: $start -> $end ($ctl_line)";
		    $status = 'FAILED';
		    $ret_value = -3;
		}

		if (! -s "$ST::CFG_FEATFILES_DIR/$file.$ST::CFG_FEATFILE_EXTENSION") {
		    $ret_value = -4;
		    $status = 'FAILED';
		    LogWarning ("This file, $ST::CFG_FEATFILES_DIR/$file.$ST::CFG_FEATFILE_EXTENSION, does not exist\n");
		}
	    }
	} else {
	    # Accepts only the file name and possible comment on line by itself..no start/send markers
	    if ($ctl_line =~ m/^(\S+)(\s.*)?$/) {
		$file = $1;
		my $size = -s "$ST::CFG_FEATFILES_DIR/$file.$ST::CFG_FEATFILE_EXTENSION";
		# 1 frame = 13 floating point numbers = 13*4bytes = 52 bytes (only valid for MFC files)
		$estimated_training_data += ($size / 52) if (lc($ST::CFG_FEATFILE_EXTENSION) eq 'mfc');
		if (! $size) {
		    $ret_value = -4;
		    $status = 'FAILED';
		    LogWarning ("CTL file, $ST::CFG_FEATFILES_DIR/$file.$ST::CFG_FEATFILE_EXTENSION, does not exist\n");
		}
	    } else {
		$status = 'FAILED';
		$ret_value = -5;
		LogWarning ("CTL line does not parse correctly:\n$ctl_line\n");
	    }
	}
    }

#    Log ("\t\t$status\n");
    HTML_Print ("\t\t<font color=\"$ST::CFG_OKAY_COLOR\"> $status </font>\n") if($status eq 'passed');
    HTML_Print ("\t\t<font color=\"$ST::CFG_ERROR_COLOR\"> $status </font>\n") if($status eq 'FAILED');
    
    $number_ctl_lines = $#ctl_lines + 1;

    
    # 4) Check number of lines in the transcript and in ctl - they should be the same\n";
    Log ("    Phase 4: CTL - Checking number of lines in the transcript should match lines in control file\n");
    open TRN,"$ST::CFG_TRANSCRIPTFILE" or die "Can not open Transcript file ($ST::CFG_TRANSCRIPTFILE)";
    my $number_transcript_lines = 0;
    while (<TRN>) {
	$number_transcript_lines++;
    }
    close TRN;
    
    $status = ($number_ctl_lines == $number_transcript_lines) ? 'passed' : 'FAILED';
    HTML_Print ("\t\t<font color=\"$ST::CFG_OKAY_COLOR\"> $status </font>\n") if($status eq 'passed');
    HTML_Print ("\t\t<font color=\"$ST::CFG_ERROR_COLOR\"> $status </font>\n") if($status eq 'FAILED');


    # 5) Should already have estimates on the total training time, 

    Log ("    Phase 5: CTL - Determine amount of training data, see if n_tied_states seems reasonable.\n");
    $status = 'passed';
    my $total_training_data = 0;
    for $ctl_line (@ctl_lines) {
	# Accept: filename int int possible_comment
	#($file,$start,$end) = map /(.+)\s(\d+)\s(\d+).*/,$ctl_line;
	# start and end time specify start and end frames
	if ($ctl_line =~ m/(.+)\s(\d+)\s(\d+).*/) {
	    $file = $1;
	    $start = $2;
	    $end = $3;
	    $total_training_data += ($end - $start) unless (($end - $start) < 0);
	} 
    }
    $total_training_data = $estimated_training_data if ($total_training_data == 0) ;

    if ($total_training_data) {
	my $total_training_hours = ($total_training_data / 3600)/100;
	Log("\t\tTotal Hours Training: $total_training_hours\n");
	my $estimated_n_tied_states = 1000;
	if ($total_training_hours < 10) {
	    $status = 'WARNING';
	    Log("\t\tThis is a small amount of data, no comment at this time\n");
	} else {
	    if ($total_training_hours < 100) {
		$status = 'WARNING';
		$estimated_n_tied_states = 3000 if ($ST::CFG_HMM_TYPE eq '.cont.'); # Likely bogus 
		$estimated_n_tied_states = 4000 if ($ST::CFG_HMM_TYPE eq '.semi.'); # 
		Log("\t\tRule of thumb suggests $estimated_n_tied_states, however there is no correct answer\n");
	    } else {
		$estimated_n_tied_states = 8000;
		$status = 'passed';
		Log("\t\t100+ hours of training data is goodly amount of data.\n");
		Log("\t\tRule of thumb suggests $estimated_n_tied_states for 100 hours, you can adjust accordingly.\n");
	    }
	}
    }
    HTML_Print ("\t\t<font color=\"$ST::CFG_OKAY_COLOR\"> $status </font>\n") if($status eq 'passed');
    HTML_Print ("\t\t<font color=\"$ST::CFG_ERROR_COLOR\"> $status </font>\n") if($status eq 'FAILED');
    HTML_Print ("\t\t<font color=\"$ST::CFG_WARNING_COLOR\"> $status </font>\n") if($status eq 'WARNING');
#    Log("\t\t$status\n");
    @ctl_lines = ();
}

my %transcript_phonelist_hash = ();

# Verify that all transcription words are in the dictionary, and all
# phones are covered
{
    Log("    Phase 6: TRANSCRIPT - Checking that all the words in the transcript are in the dictionary\n");
    open DICT,"$ST::CFG_DICTIONARY" or die "Can not open the dictionary ($ST::CFG_DICTIONARY)";
    my @dict = <DICT>;
    close DICT;
    Log("        Words in dictionary: $#dict\n");

    my %d;
    for (@dict) {		# Create a hash of the dict entries
	/(\S+)\s+(.*)$/;
	if ($is_case_sensitive) {
	  $d{$1} = $2;
	} else {
	  $d{$1} = uc($2);
	}
    }
    
    open DICT,"$ST::CFG_FILLERDICT" or die "Can not open filler dict ($ST::CFG_FILLERDICT)\n";
    my @fill_dict = <DICT>;
    close DICT;
    Log ("        Words in filler dictionary: $#fill_dict\n");
    
    for (@fill_dict) {		# Create a hash of the dict entries
	/(\S+)\s+(.*)$/;
	if ($is_case_sensitive) {
	  $d{$1} = $2;
	} else {
	  $d{$1} = uc($2);
	}
    }
    
    @dict = undef;			# not needed
    @fill_dict = undef;		# not needed
    
    open TRN,"$ST::CFG_TRANSCRIPTFILE" or die "Can not open the transcript file ($ST::CFG_TRANSCRIPTFILE)"; 
    
    my $status = 'passed';
    while (<TRN>) {
	my ($text) = m/(.*)\s*\(.*\)$/;
	if ($text) {
	    my @words = split /\s+/,$text;
	    for my $word (@words) {
		if (! $d{$word} && ($word =~ m/\S+/)) {
		    LogWarning ("This word: $word was in the transcript file, but is not in the dictionary ($text). Do cases match?\n");
		    $status = 'FAILED';
		    $ret_value = -5;
		} else {
		    my @phones = ($d{$word} =~ m/(\S+)/g);
		    for my $phone (@phones) {
		        $transcript_phonelist_hash{$phone} = 1;
		    }
		}
	    }
	}
    }
    close TRN;

    HTML_Print ("\t\t<font color=\"$ST::CFG_OKAY_COLOR\"> $status </font>\n") if($status eq 'passed');
    HTML_Print ("\t\t<font color=\"$ST::CFG_ERROR_COLOR\"> $status </font>\n") if($status eq 'FAILED');
    HTML_Print ("\t\t<font color=\"$ST::CFG_WARNING_COLOR\"> $status </font>\n") if($status eq 'WARNING');

}

{
    Log("    Phase 7: TRANSCRIPT - Checking that all the phones in the transcript are in the phonelist, and all phones in the phonelist appear at least once\n");
    my $status = 'passed';

    for my $phone (sort keys %phonelist_hash) {
      if (!defined $transcript_phonelist_hash{$phone}) {
	    LogWarning ("This phone ($phone) occurs in the phonelist ($ST::CFG_RAWPHONEFILE), but not in any word in the transcription ($ST::CFG_TRANSCRIPTFILE)\n");
	    $status = 'FAILED';
      }
    }


    HTML_Print ("\t\t<font color=\"$ST::CFG_OKAY_COLOR\"> $status </font>\n") if($status eq 'passed');
    HTML_Print ("\t\t<font color=\"$ST::CFG_ERROR_COLOR\"> $status </font>\n") if($status eq 'FAILED');
    HTML_Print ("\t\t<font color=\"$ST::CFG_WARNING_COLOR\"> $status </font>\n") if($status eq 'WARNING');

  }

mkdir ($ST::CFG_LOG_DIR,0755) unless -d $ST::CFG_LOG_DIR;
mkdir ("$ST::CFG_BASE_DIR/bwaccumdir",0755) unless -d "$ST::CFG_LOG_DIR/bwaccumdir";

    
exit ($ret_value);

# General idea for senone: 
#   10 hours = 3000 cont. 4000 semi. 
#  100 hours = 8000 (cont and semi) 
# Rate of increase between the two is very small.

