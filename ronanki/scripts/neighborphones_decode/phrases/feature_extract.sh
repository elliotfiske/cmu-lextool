SPHINX_fe=/home/ronanki/web/source/sphinxbase/bin/sphinx_fe

$SPHINX_fe -c phrase.ctl -ei wav -di wav -eo mfc -do feats -argfile feat.params -mswav yes
