#!/bin/sh

for i in `ls JSGF/`
do
echo $i
j=`basename $i ".jsgf"`
/home/ronanki/web/source/sphinxbase/bin/sphinx_jsgf2fsg <JSGF/$i >FSG/$j.fsg
done
