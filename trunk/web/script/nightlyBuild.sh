#!/bin/sh


. ${HOME}/.profile

# Check that we have all executables
if ! SVN=`command -v svn 2>&1`; then exit 1; fi
if ! SCP=`command -v scp 2>&1`; then exit 1; fi
if ! TAR=`command -v gtar 2>&1`; then
  if ! TAR=`command -v tar 2>&1`; then exit 1; fi
fi

PUSH_DEST=/usr1/httpd/html/sphinx/download/nightly/

loopUntilSuccess () {
    cmd=$@
    # start loop to download code
    count=0;

    while ! $cmd; do
        count=`expr $count + 1`
        if [ $count -gt 50 ]; then
            # not successful, and we attempted it too many times. Clean up and l
eave.
            return $count
        fi
    done
}

createNightlyBuild () {
    location=$1
    module=$2
    name=`echo $module | awk -F/ '{print $NF}'`

    TMP_DIR=${HOME}/project/SourceForge/build/nightly

    mkdir -p $TMP_DIR
    cd $TMP_DIR

    if test x$module != xsphinxbase; then
	loopUntilSuccess ${SVN} export -q https://cmusphinx.svn.sourceforge.net/svnroot/cmusphinx/trunk/sphinxbase
	touch sphinxbase/src/libsphinxbase/libsphinxbase.la
    fi

    loopUntilSuccess ${SVN} export -q https://cmusphinx.svn.sourceforge.net/svnroot/cmusphinx/trunk/$location $module
    cd $module || exit 1
# Somehow, we need to run autogen.sh twice...
    test -e autogen.sh && ./autogen.sh -q
    test -e autogen.sh && ./autogen.sh -q
    cd $TMP_DIR
    $TAR -czf /tmp/${name}.nightly.tar.gz $module || exit 1
    chmod 664 /tmp/$name.nightly.tar.gz
    ${SCP} -o "BatchMode yes" /tmp/$name.nightly.tar.gz www.speech.cs.cmu.edu:${PUSH_DEST}

    cd
    /bin/rm -rf $TMP_DIR
    /bin/rm /tmp/${name}.nightly.tar.gz
}

nightlyBuildPage () {
    WEBPAGE=$1

    cat <<-END > $WEBPAGE
<html>
<head>
<title>Nightly Builds</title>
<meta http-equiv=content-type
content=text/html;charset=ISO-8859-1>
<!-- Created by $0 on `hostname` -->
</head>
<body>
<h1>Sphinx Open Source nightly builds available at SourceForge.net</h1>
<h2>Applications</h2>
<ul>
<li><a href="dictator.tar.gz">Eval Dictator tarball</a> or <a href="dictator.jar">Eval Dictator jar</a></li>
</ul>
<h2>Common Libraries</h2>
<ul>
<li><a href="sphinxbase.nightly.tar.gz">sphinxbase</a></li>
</ul>
<h2>Decoders</h2>
<ul>
<li><a href="s3flat.nightly.tar.gz">s3flat</a></li>
<li><a href="pocketsphinx.nightly.tar.gz">pocketsphinx</a></li>
<li><a href="sphinx2.nightly.tar.gz">sphinx2</a></li>
<li><a href="sphinx3.nightly.tar.gz">sphinx3</a></li>
<li><a href="sphinx4.nightly.tar.gz">sphinx4</a></li>
</ul>
<h2>Acoustic Model Training</h2>
<ul>
<li><a href="SphinxTrain.nightly.tar.gz">SphinxTrain</a></li>
</ul>
<h2>Language Model Training</h2>
<ul>
<li><a href="SimpleLM.nightly.tar.gz">SimpleLM</a></li>
<li><a href="cmuclmtk.nightly.tar.gz">cmuclmtk</a></li>
</ul>
<h2>Tools</h2>
<ul>
<li><a href="cepview.nightly.tar.gz">cepview</a></li>
<li><a href="lm3g2dmp.nightly.tar.gz">lm3g2dmp</a></li>
</ul>
<p>Last update on `date`</p>
</body>
</html>
END

}

createNightlyBuild cmuclmtk cmuclmtk
createNightlyBuild sphinx2 sphinx2
createNightlyBuild sphinx3 sphinx3
createNightlyBuild sphinx4 sphinx4
#createNightlyBuild tools tools
#updateDictator.sh
createNightlyBuild sphinxbase sphinxbase
createNightlyBuild pocketsphinx pocketsphinx
createNightlyBuild SphinxTrain SphinxTrain
createNightlyBuild archive_s3/s3 s3flat
createNightlyBuild share/cepview cepview
createNightlyBuild share/lm3g2dmp lm3g2dmp
createNightlyBuild SimpleLM SimpleLM
nightlypage=nightlybuild.html
nightlyBuildPage $nightlypage
${SCP} -o "BatchMode yes" $nightlypage www.speech.cs.cmu.edu:${PUSH_DEST}/index.html
/bin/rm $nightlypage
