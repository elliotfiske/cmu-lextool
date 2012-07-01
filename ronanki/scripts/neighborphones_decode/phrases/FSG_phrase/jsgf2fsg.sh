#!/bin/sh

SPHINXBASE_jsgf2fsg=/home/ronanki/web/source/sphinxbase/bin/sphinx_jsgf2fsg # set your own path

for i in `ls JSGF/`
do
echo $i
j=`basename $i ".jsgf"`
$SPHINXBASE_jsgf2fsg <JSGF/$i >FSG/$j.fsg
done
