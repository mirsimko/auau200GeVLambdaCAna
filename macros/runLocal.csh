#!/bin/tcsh

starver SL16j

root4star -l -b -q StRoot/macros/loadSharedHFLibraries.C 'StRoot/macros/runPicoHFLambdaCMaker.C++g("test_new.list","output_new_final_test",0,"BadRunList_MB.list","LambdaC.kPionKaonProton.picoHFtree","root://xrdstar.rcf.bnl.gov:1095//home/starlib/home/starreco/reco/AuAu_200_production_2016/ReversedFullField/P16ij/2016",0)'
