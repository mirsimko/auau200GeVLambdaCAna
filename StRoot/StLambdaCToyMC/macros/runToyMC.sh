#!/bin/bash

outFile=${1:-'lambdaC.root'}
DecayMode=${2:-3}
# nEntries=${3:-7.46445e+08}
nEntries=${3:-300}
# kKstarProton=0, kLambda1520Pion=1, kDeltaPPkaon=2, kPionKaonProton=3, kLambdaPion=4, kKshortProton=5

echo outFile = $outFile, DecayMode = $DecayMode, nEntries = $nEntries
echo
echo Running 'toyMcEffLambdaC.C++g('$nEntries',"'$outFile'",'$DecayMode')'
echo
echo
root -l -b -q 'StRoot/StLambdaCToyMC/macros/toyMcEffLambdaC.C++('$nEntries',"'$outFile'",'$DecayMode')'
