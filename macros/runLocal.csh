#!/bin/csh

# starver SL16j
starver SL18a

root -l -b -q StRoot/macros/loadSharedHFLibraries.C 'StRoot/macros/runPicoHFLambdaCMaker.C++g("st_sstStream_pico.list","test_cuts_decayLength",0,"BadRunList_MB.list","LambdaC_kPionKaonProton_picoHFtree_st_sst_stream","root://xrdstar.rcf.bnl.gov:1095//home/starlib/home/starreco/reco/AuAu_200_production2_2016/ReversedFullField/P16ij/2016",0)'
