#!/bin/bash

first=${1:-1}
last=${2:-9223372036854775807}
root -l -b -q simCutsMaker/macros/loadSimCutsMaker.C 'simCutsMaker/macros/runSimCutsMaker.C++('$first','$last')'
