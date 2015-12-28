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

from pocketsphinx.pocketsphinx import *
from sphinxbase.sphinxbase import *

#some dumb test for checking during developent

MODELDIR = "../../../model"

config = Decoder.default_config()

intval = 256
floatval = 8000.0
stringval = "~/pocketsphinx"
boolval = True

# Check values that was previously set.
s = config.get_float("-samprate")
print ("Float: ",floatval ," ", s)
config.set_float("-samprate", floatval)
s = config.get_float("-samprate")
print ("Float: ",floatval ," ", s)

s = config.get_int("-nfft")
print ("Int:",intval, " ", s)
config.set_int("-nfft", intval)
s = config.get_int("-nfft")
print ("Int:",intval, " ", s)

s = config.get_string("-rawlogdir")
print ("String:",stringval, " ", s)
config.set_string("-rawlogdir", stringval)
s = config.get_string("-rawlogdir")
print ("String:",stringval, " ", s)

s = config.get_boolean("-backtrace")
print ("Boolean:", boolval, " ", s)
config.set_boolean("-backtrace", boolval);
s = config.get_boolean("-backtrace")
print ("Boolean:", boolval, " ", s)

config.set_string_extra("-something12321", "abc")
print config.get_string("-something12321")

