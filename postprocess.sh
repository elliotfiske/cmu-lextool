#!/bin/bash

java -cp lib/sphinx4.jar:lib/commons-lang-2.0.jar:dist/postprocessing.jar edu.cmu.sphinx.post.dynamic.PostProcessing "$1" "$2" "$3" "$4"
