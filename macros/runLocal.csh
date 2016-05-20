#!/bin/tcsh


starver SL16d

# -- Test writing mode - which works on single file:
cons && root -l -b -q StRoot/macros/loadSharedHFLibraries.C StRoot/macros/runPicoHFLambdaCMaker.C++g'("/project/projectdirs/starprod/picodsts/Run14/AuAu/200GeV/physics2/P16id/107/15107005/st_physics_15107005_raw_0000008.picoDst.root", "outputBaseName", 1)'
 




#cons && root -l -b -q StRoot/macros/loadSharedHFLibraries.C StRoot/macros/runPicoHFLambdaCMaker.C++g

#cons && root -l -b -q StRoot/macros/loadSharedHFLibraries.C StRoot/macros/runPicoHFLambdaCMaker.C++g'("/project/projectdirs/starprod/picodsts/Run14/AuAu/200GeV/physics2/P15ic/094/15094070/st_physics_15094070_raw_0000007.picoDst.root", "lambdaC", 1, "xxTree")'


#cons && root -l -b -q StRoot/macros/loadSharedHFLibraries.C StRoot/macros/runPicoHFLambdaCMaker.C++g'("/project/projectdirs/starprod/picodsts/Run14/AuAu/200GeV/physics2/P15ic/094/15094070/st_physics_15094070_raw_0000007.picoDst.root", "lambdaC", 1, "picoList_bad_MB.list", "fooTree", "/project/projectdirs/starprod/picodsts/Run14/AuAu/200GeV/physics2/P15ic", 1)'


#cons && root -l -b -q StRoot/macros/loadSharedHFLibraries.C StRoot/macros/runPicoHFLambdaCMaker.C++g'("foo.list", "lambdaC_read", 2, "picoList_bad_MB.list", "LambdaC.kProtonK0short.picoHFtree", "/project/projectdirs/starprod/picodsts/Run14/AuAu/200GeV/physics2/P15ic", 1)'

#cons && root -l -b -q StRoot/macros/loadSharedHFLibraries.C StRoot/macros/runPicoHFLambdaCMaker.C++g'("lists/tree.list", "lambdaC_read", 2, "picoList_bad_MB.list", "LambdaC.kProtonK0short.picoHFtree", "/project/projectdirs/starprod/picodsts/Run14/AuAu/200GeV/physics2/P15ic", 1)'
