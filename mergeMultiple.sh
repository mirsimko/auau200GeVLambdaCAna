#!/bin/bash

outFileName=run16LambdaCoutput.root
mergedFiles=merge/picoHFLambdaCMaker.root

if [ -f "$outFileName" ]
then
  echo Output file already exists
  echo Exitting ..
  exit 1
fi

if [ ! -d "$1" ]
then
  echo The arguments must be directories
  echo Exitting ..
  exit 1
fi

numberOfDirs=0
while true
do
  if [ -d "$1" ]
  then
    echo ======================================
    echo Merging in $1
    echo ======================================
    ./merge.sh "$1"

    # save the merged directory
    directories[$numberOfDirs]="$1"/"$mergedFiles"
    let "numberOfDirs++"
  else
    if [ "$1" ]
    then
      echo The arguments must be directories
      echo Exitting ..
      exit 1
    else
      break
    fi
  fi
  shift   # moves the arguments
done

echo ======================================
echo merging ${directories[*]} into $outFileName ..
echo ======================================
hadd "$outFileName" ${directories[*]}

echo
echo Done ..
exit 0
