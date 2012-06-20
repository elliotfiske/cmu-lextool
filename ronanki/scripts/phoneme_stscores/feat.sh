SPHINXBASE_FE=./sphinxbase/bin/sphinx_fe # path to sphinx_fe in sphinxbase

# phone.ctl - input list of wave file without extension
# -ei - input extension (wav/raw)
# -di - input directory for wave files
# -eo - output extension (default:mfc)
# -do - output directory for extracted features
# -argfile feat.params - contains feature extraction parameters

$SPHINXBASE_FE -c phone.ctl -ei wav -di wav -eo mfc -do feats -argfile feat.params -mswav yes

