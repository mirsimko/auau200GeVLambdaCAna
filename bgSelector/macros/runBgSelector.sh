#!/bin/bash

outFile=${1:-bgSelected2ndIter.root}
root -l -b -q bgSelector/macros/loadBgSelector.C 'bgSelector/macros/runBgSelector.C++("bgSelect1stIterProtons.root","secondarySelected","'$outFile'")'

