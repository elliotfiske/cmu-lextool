#!/bin/bash
java -cp dist/fst.jar:lib/guava-12.0.jar edu.cmu.sphinx.fst.openfst.Import $1 $2
