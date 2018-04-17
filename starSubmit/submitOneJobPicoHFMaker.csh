#!/bin/csh

#   Submit fileLists for classes derived from 
#    StPicoHFMaker;
#
#  - script will create a folder ${baseFolder}/jobs/${productionId}
#    all submission related files will end up there
#
#  - in ${baseFolder} the script expects (links or the actual folders)
#      .sl73_gcc485
#      StRoot                     ( from the git repo )
#      run14AuAu200GeVPrescales   ( from the git repo )
#      starSubmit                 ( from the git repo )
#
#      picoLists                  ( from the fileList git repo )
#
#   - the rootMacro is expected in StRoot/macros
#
#   - the bad run list is expected in ${baseFolder}
#     or in ${baseFolder}/picoLists
#
# ###############################################

# -- set decay channel
#    0 - kPionKaonProton
#    1 - kProtonK0short
#    2 - kLambdaPion
#    3 - Mixed Event
set decayChannel=0

if ( $decayChannel == 0 ) then
    set tree=LambdaC.kPionKaonProton.picoHFtree
else if ( $decayChannel == 1 ) then
    set tree=LambdaC.kProtonK0short.picoHFtree
else if ( $decayChannel == 2 ) then
    set tree=LambdaC.kLambdaPion.picoHFtree
else if ( $decayChannel == 3 ) then
    set tree=LambdaC.MixedEvent.picoHFtree
endif

# ###############################################

# -- baseFolder of job
set baseFolder=/gpfs/mnt/gpfs01/star/pwg/msimko/LambdaC/run16

# --input file 
#    makerMode 0,1 : list must contain picoDst.root files
#    makerMode 2   : list must contain ${treeName}.root files

#set input=${baseFolder}/lists/test.list
# set input=${baseFolder}/lists/picoList_all_05_18.list

# set input=${baseFolder}/test.list
#set input=${baseFolder}/LambdaCtreeLists/LambdaC.kProtonK0shortNoPt.picoHFtree/test.list
# set input=${baseFolder}/lists/${tree}/${tree}_all.list
# set input=${baseFolder}/treeLists/${tree}_allTrees.list
set input=${baseFolder}/${1}
# set input=${baseFolder}/prodListAug15.list
# set input=${baseFolder}/20160817.picoDst.Miro.list
# set input=${baseFolder}/divideList/listAll0.list
#set input=/global/homes/j/jthaeder/analysis/200GeV/lambdaC/lists/LambdaC.kProtonK0short.picoHFtree/LambdaC.kProtonK0short.picoHFtree_missing.list

# -- set maker mode
#    0 - kAnalyze, 
#    1 - kWrite
#    2 - kRead
#    3 - Mixed Event
set makerMode=0

# -- set root macro
if ( $decayChannel == 3 && $makerMode == 3 ) then
  set rootMacro=runPicoMixedEventTriplets.C
else if ( $decayChannel == 3 || $makerMode == 3 ) then # one of them is 3, but the other one is not
  echo Invalid decay channel or maker mode. Exiting ....
  exit 1
else
  set rootMacro=runPicoHFLambdaCMaker.C
endif


# -- set filename for bad run list
set badRunListFileName="BadRunList_MB.list"

# ###############################################
# -- CHANGE CAREFULLY BELOW THAT LINE
# ###############################################

# -- tree name (kWrite / kRead)
set treeName=${tree}

# -- production Id (kAnalyse / kRead)
set productionId=`date +%F_%H-%M`

# -- set STAR software version
set starVersion=SL18a

# -- production base path (to find picoDsts to corresponding trees
set productionbasePath=root://xrdstar.rcf.bnl.gov:1095//home/starlib/home/starreco/reco/AuAu200_production2_2016/ReversedFullField/P16ij

# -- submission xml file 
set xmlFile=submitPicoHFMakerAsOneJob.xml

# -- set min and mx number of files
set minNFiles=100
set maxNFiles=100

# ###############################################
# -- DON'T CHANGE BELOW THAT LINE
# ###############################################

# -- job submission directory
mkdir -p ${baseFolder}/jobs/${productionId}

# -- result directory
mkdir -p ${baseFolder}/production/${productionId}

pushd ${baseFolder}/jobs/${productionId} > /dev/null

# -- prepare folder
mkdir -p report err log list csh

# -----------------------------------------------

# -- check for prerequisits and create links
set folders=".sl73_gcc485"

echo -n "Checking prerequisits folders ...  "
foreach folder ( $folders ) 
    if ( ! -d ${baseFolder}/${folder} ) then
	echo "${folder} does not exist in ${baseFolder}"
	exit
    else
	ln -sf ${baseFolder}/${folder}
    endif
end
echo "ok"

# -----------------------------------------------

# -- check for prerequisits and copy folders
set folders="StRoot starSubmit"

echo -n "Checking prerequisits folders ...  "
foreach folder ( $folders ) 
    if ( ! -d ${baseFolder}/${folder} ) then
	echo "${folder} does not exist in ${baseFolder}"
	exit
    else
	cp -rfL ${baseFolder}/${folder} .
    endif
end
echo "ok"

# -----------------------------------------------

echo -n "Checking run macro ...             "
if  ( ! -e ${baseFolder}/StRoot/macros/${rootMacro} ) then
    echo "${rootMacro} does not exist in ${baseFolder}/StRoot/macros"
    exit
endif
echo "ok"

# -----------------------------------------------

## check if macro compiles
if ( -e compileTest.log ) then
    rm compileTest.log
endif

echo -n "Testing compilation ...            "
root -l -b -q starSubmit/compileTest.C |& cat > compileTest.log 
cat compileTest.log |& grep "Compilation failed!"
if ( $status == 0 ) then
    echo "Compilation of ${rootMacro} failed"
    cat compileTest.log
    exit
else
    rm compileTest.log
endif
echo "ok"

# -----------------------------------------------

echo -n "Checking xml file  ...             "
if ( ! -e ${baseFolder}/starSubmit/${xmlFile} ) then
    echo "XML ${xmlFile} does not exist"
    exit
else
    ln -sf ${baseFolder}/starSubmit/${xmlFile} 
endif
echo "ok"

# -----------------------------------------------

echo -n "Checking bad run list  ...         "
if  ( -e ${baseFolder}/${badRunListFileName} ) then
    cp  ${baseFolder}/${badRunListFileName} picoList_badRuns.list
else if ( -e ${baseFolder}/picoLists/${badRunListFileName} ) then
    cp  ${baseFolder}/picoLists/${badRunListFileName} picoList_badRuns.list
else
    echo "${badRunListFileName} does not exist in ${baseFolder} nor ${baseFolder}/picoLists"
    exit
endif
echo "ok"

# -----------------------------------------------

echo -n "Checking input file list ...       "
if ( ! -e ${input} ) then
    echo "Filelist ${input} does not exist"
    exit
else
    cp ${input} st_sstStream_pico.list
endif

if ( ${makerMode} == 0 || ${makerMode} == 1 ) then
    head -n 2 ${input} | grep ".picoDst.root" > /dev/null
    if ( $? != 0 ) then
	echo "Filelist does not contain picoDsts!"
	exit
    endif
else if ( ${makerMode} == 2 ) then
    head -n 2 ${input} | grep ".${treeName}.root" > /dev/null
    if ( $? != 0 ) then
	echo "Filelist does not contain ${treeName} trees!"
	exit
    endif
endif
echo "ok"

# -----------------------------------------------

if ( -e LocalLibraries.zip ) then
    rm LocalLibraries.zip
endif 

if ( -d LocalLibraries.package ) then
    rm -rf LocalLibraries.package
endif 

# ###############################################
# -- submit 
# ###############################################

##### temporary hack until -u ie option becomes availible

set hackTemplate=submitPicoHFMaker_temp.xml 

if ( -e submitPicoHFMaker_temp.xml  ) then
    rm submitPicoHFMaker_temp.xml 
endif 

echo '<?xml version="1.0" encoding="utf-8" ?>'		        > $hackTemplate
echo '<\!DOCTYPE note ['                      		       >> $hackTemplate
echo '<\!ENTITY treeName "'${treeName}'">'    		       >> $hackTemplate
echo '<\!ENTITY decayChannel "'${decayChannel}'">'	       >> $hackTemplate
echo '<\!ENTITY mMode "'${makerMode}'">'		       >> $hackTemplate
echo '<\!ENTITY rootMacro "'${rootMacro}'">'  		       >> $hackTemplate
echo '<\!ENTITY prodId "'${productionId}'">'  		       >> $hackTemplate
echo '<\!ENTITY basePath "'${baseFolder}'">'  		       >> $hackTemplate
# echo '<\!ENTITY listOfFiles "'${input}'">'                     >> $hackTemplate
echo '<\!ENTITY listOfFiles "st_sstStream_pico.list">'                     >> $hackTemplate
echo '<\!ENTITY productionBasePath "'${productionbasePath}'">' >> $hackTemplate
echo '<\!ENTITY starVersion "'${starVersion}'">'               >> $hackTemplate
echo '<\!ENTITY minNFiles "'${minNFiles}'">'                   >> $hackTemplate
echo '<\!ENTITY maxNFiles "'${maxNFiles}'">'                   >> $hackTemplate
echo ']>'					       	       >> $hackTemplate

tail -n +2 ${xmlFile} >> $hackTemplate

star-submit -u ie $hackTemplate

#star-submit-template -template ${xmlFile} -entities listOfFiles=${input},basePath=${baseFolder},prodId=${productionId},mMode=${makerMode},treeName=${treeName},decayChannel=${decayChannel},productionBasePath=${productionbasePath},rootMacro=${rootMacro},starVersion=${starVersion},minNFiles=${minNFiles},maxNFiles=${maxNFiles}
popd > /dev/null
