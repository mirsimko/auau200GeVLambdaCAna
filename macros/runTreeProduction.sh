#!/bin/bash

# Run tree production - as cron job
#
# #############################################
#  Authors:
#
#    Jochen Thaeder  (jmthader@lbl.gov)
#
# #############################################

baseFolder=/global/project/projectdirs/starprod/rnc/jthaeder/lambdaC

# ------------------------------------------------------

yesterday=`date --date="1 days ago" '+%F'`
listName=picoList_${yesterday}.list

inList=${baseFolder}/picoLists/daily/${listName}

myList=${baseFolder}/lists/${listName}
myAllList=${baseFolder}/lists/picoList_all_${yesterday}.list

# -- Check if daily list of yesterday is there 
#    -> Do nothing if not there yet
if [ ! -f $inList ] ; then
    exit
fi

# -- Check if my list of yesterday is there 
#    -> Do nothing if already there
if [[ -f $myList || -f $myAllList ]] ; then
    exit
fi

# -- Copy List and submit jobs ...
cp $inList $myList

pushd $baseFolder > /dev/null

#    0 - kPionKaonProton
./submitPicoHFMakerTreeProduction.csh 0 ${listName}

#    1 - kProtonK0short
./submitPicoHFMakerTreeProduction.csh 1 ${listName}

#    2 - kLambdaPion
./submitPicoHFMakerTreeProduction.csh 2 ${listName}

popd > /dev/null