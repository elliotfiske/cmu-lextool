#!/usr/bin/env python

from pocketsphinx import Config, Decoder

#some dumb test for checking during developent

intval = 512
floatval = 8000.0
stringval = "~/pocketsphinx"
boolval = True

c = Config()

print "----Smoke testing config----"
c.setFloat("-samprate", floatval)
s = c.getFloat("-samprate")
print "Float: ",floatval ,"--------", s

c.setInt("-nfft", intval)
s = c.getInt("-nfft")
print "Int:",intval, "-------", s

c.setString("-rawlogdir", stringval)
s = c.getString("-rawlogdir")
print "String:",stringval, "--------", s

c.setBoolean("-backtrace", boolval);
s = c.getBoolean("-backtrace")
print "Boolean:", boolval, "-------", s

print "----Smoke testing config reread----"
decoder = Decoder(c)
cn = decoder.getConfig()
#print "config:", cn

s = cn.getFloat("-samprate")
print "Float: ",floatval ,"--------", s

s = cn.getInt("-nfft")
print "Int:",intval, "-------", s

s = cn.getString("-rawlogdir")
print "String:",stringval, "--------", s

s = cn.getBoolean("-backtrace")
print "Boolean:", boolval, "-------", s
