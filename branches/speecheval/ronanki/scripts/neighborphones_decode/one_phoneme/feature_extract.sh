SPHINX_fe=/home/ronanki/web/source/sphinxbase/bin/sphinx_fe

$SPHINX_fe -c split.ctl -ei wav -di split_wav -eo mfc -do split_feats -argfile feat.params -mswav yes
