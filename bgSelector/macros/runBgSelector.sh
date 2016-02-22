#!/bin/bash

outFile=${1:-bgSelectedAllStat.root}
root -l -b -q bgSelector/macros/loadBgSelector.C 'bgSelector/macros/runBgSelector.C++("/global/project/projectdirs/star/pwg/starhf/simkomir/LambdaC/production/picoHFLambdaCMaker.root","secondary","'$outFile'")'

