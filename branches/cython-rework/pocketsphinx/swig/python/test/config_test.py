# -*- c-basic-offset: 4; indent-tabs-mode: nil -*- */
# ====================================================================
# Copyright (c) 2013 Carnegie Mellon University.  All rights
# reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer. 
#
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in
#    the documentation and/or other materials provided with the
#    distribution.
#
# This work was supported in part by funding from the Defense Advanced 
# Research Projects Agency and the National Science Foundation of the 
# United States of America, and the CMU Sphinx Speech Consortium.
#
# THIS SOFTWARE IS PROVIDED BY CARNEGIE MELLON UNIVERSITY ``AS IS'' AND 
# ANY EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
# THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY
# NOR ITS EMPLOYEES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# ====================================================================


from os import environ, path
from itertools import izip
from pocketsphinx import *

#some dumb test for checking during developent

MODELDIR = environ.get('MODELDIR', path.join(cvar.DATADIR, 'model'))
DATADIR = environ.get('DATADIR', path.join(cvar.DATADIR, 'examples/python'))

config = Config()
config.set_string('-hmm', path.join(MODELDIR, 'hmm/en_US/hub4wsj_sc_8k'))
config.set_string('-lm', path.join(MODELDIR, 'lm/en_US/wsj0vp.5000.DMP'))
config.set_string('-dict', path.join(DATADIR, 'defective.dic'))
config.set_boolean('-dictcase', True)
config.set_boolean('-bestpath', False)
config.set_boolean('-fwdflat', False)
config.set_string('-input_endian', 'little')
config.set_int('-samprate', 16000)

intval = 512
floatval = 8000.0
stringval = "~/pocketsphinx"
boolval = True

# Check values that was previously set.
config.set_float("-samprate", floatval)
s = config.get_float("-samprate")
print "Float: ",floatval ,"--------", s

config.set_int("-nfft", intval)
s = config.get_int("-nfft")
print "Int:",intval, "-------", s

config.set_string("-rawlogdir", stringval)
s = config.get_string("-rawlogdir")
print "String:",stringval, "--------", s

config.set_boolean("-backtrace", boolval);
s = config.get_boolean("-backtrace")
print "Boolean:", boolval, "-------", s

# Check values of the Config instance obtained from Decoder
decoder = Decoder(config)
cn = decoder.get_config()

s = cn.get_float("-samprate")
print "Float: ",floatval ,"--------", s

s = cn.get_int("-nfft")
print "Int:",intval, "-------", s

s = cn.get_string("-rawlogdir")
print "String:",stringval, "--------", s

s = cn.get_boolean("-backtrace")
print "Boolean:", boolval, "-------", s

# Check values read from file.
config = Config('config.cfg')
print config.get_int('-nfft')
print config.get_float('-samprate')
print config.get_string('-input_endian')
