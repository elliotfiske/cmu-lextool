#!/bin/bash
java -Djava.library.path=. -cp ../../fst/fst.jar:openfst.jar edu.cmu.sphinx.fst.openfst.Import $1 $2


