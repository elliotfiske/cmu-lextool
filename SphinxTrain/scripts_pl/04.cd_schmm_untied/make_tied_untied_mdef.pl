#!/usr/bin/perl -w
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


require "../sphinx_train.cfg";

die "This module not ported....";


die "USAGE: $0 <CI mdef> <untied mdef> <output tied-untied mdef>" unless ($#ARGV == 2);

$cimdef = $ARGV[0];
$untied = $ARGV[1];
$outmdef = $ARGV[2];

set xx = tieduntied1.$$
set yy = tieduntied2.$$
set zz = tieduntied3.$$
set ww = tieduntied4.$$
set tmdef = mdef.$$
set tmdef2 = mdef2.$$

onintr cleanup

set nbase = `grep n_base $cimdef | awk '{print $1}'`
echo "$nbase ci phones"
awk -v n=$nbase 'NR>n &&$2!="-"&&$3!="-"&&$4!="-" {printf("%-4s %-4s %-4s %s   %s   %s   %s %s %s   %s\n",$1,$2,$3,$4,$5,$6,$7,$8,$9,$10)}' $untied >! $tmdef
set ntph = `wc -l $tmdef | awk '{print $1}'`
echo "$ntph triphones"

set X = `tail -1 $cimdef | awk '{print $9}'`

#1:sorts by 2nd and 4th fields
awk '{printf("%-4s %-4s %-4s %s    %s %s %s %s %s    %s\n",$1,$2,$4,$3,$5,$6,$7,$8,$9,$10)}' $tmdef | sort | awk '{printf("%-4s %-4s %-4s %s    %s %s %s %s %s    %s\n",$1,$2,$4,$3,$5,$6,$7,$8,$9,$10)}' >! $xx 

#2:ties states based on left context and 4th field
awk -v a="DUMMY" -v c="DUMMY" -v b=$X '1==1 {if($2!=a || $4!=c) {b++};$7=b; printf("%-4s %-4s %-4s %s    %s %s %s %s %s    %s\n",$1,$2,$3,$4,$5,$6,$7,$8,$9,$10); a=$2; c=$4;}' $xx >! $yy 

set X = `tail -1 $yy | awk '{print $7}'`

awk -v a="DUMMY" -v c="DUMMY" -v b=$X '1==1 {b++;$8=b; printf("%-4s %-4s %-4s %s    %s %s %s %s %s    %s\n",$1,$2,$3,$4,$5,$6,$7,$8,$9,$10);}' $yy >! $zz


set X = `tail -1 $zz | awk '{print $8}'`

#3:then sorts by 3rd and 4th fields
awk '{printf("%-4s %-4s %-4s %s    %s %s %s %s %s    %s\n",$1,$3,$4,$2,$5,$6,$7,$8,$9,$10)}' $zz | sort | awk '{printf("%-4s %-4s %-4s %s    %s %s %s %s %s    %s\n",$1,$4,$2,$3,$5,$6,$7,$8,$9,$10)}' >! $ww

#then ties states based on right context and 4th field
awk -v a="DUMMY" -v c="DUMMY" -v b=$X '1==1 {if($3!=a || $4!=c) {b++};$9=b; printf("%-4s %-4s %-4s %s    %s %s %s %s %s    %s\n",$1,$2,$3,$4,$5,$6,$7,$8,$9,$10); a=$3; c=$4;}' $ww | sort | awk '{printf("%4s %4s %4s %s    %s  %s  %s  %s  %s    %s\n",$1,$2,$3,$4,$5,$6,$7,$8,$9,$10)}' >! $tmdef2

set ntphst = `awk '{print $9}' $tmdef2 | sort -r -n | head -1`

echo "# Generated by $0 on `date`" >! $outmdef
echo "0.3" >> $outmdef
echo "$nbase n_base" >> $outmdef
echo "$ntph n_tri" >> $outmdef
echo "dummy" | awk -v t=$ntph -v c=$nbase '{printf("%d n_state_map\n",(t+c)*4)}' >> $outmdef
echo "dummy" | awk -v t=$ntphst '{printf("%d n_tied_state\n",t+1)}' >> $outmdef
echo "dummy" | awk -v c=$nbase '{printf("%d n_tied_ci_state\n",c*3)}' >> $outmdef
echo "dummy" | awk -v c=$nbase '{printf("%d n_tied_tmat\n#\n",c)}' >> $outmdef
echo "dummy" | awk '{printf("# Columns definitions\n")}' >> $outmdef
echo "#base lft  rt p attrib tmat      ... state id's ..." >> $outmdef
awk '$2=="-"&&$3=="-"&&$4=="-" {print $0}' $cimdef >> $outmdef
cat $tmdef2 >> $outmdef

cleanup:
   /bin/rm -f $xx $yy $zz $ww $tmdef $tmdef2
