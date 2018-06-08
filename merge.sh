#!/bin/bash

# root files  merging script originally written by Jochen Thaeder

# baseDir=2015-08-28_16-27

baseDir=$1

# ----------------------------------------------------

mergeFolder () {

    inSplitList=inlist.${mergeLevel}.list

    let mergeLevel=mergeLevel+1
    outSplitListFolder=splitLists.$mergeLevel

    # -- split
    mkdir -p $outSplitListFolder
    split -l 10 -d -a 3 $inSplitList ${outSplitListFolder}/inlist.sublist.${mergeLevel}.   #5 files
    
    for ii in `find  ${outSplitListFolder} -name "inlist.sublist.${mergeLevel}.*"` ; do 
	sub=`echo $ii | cut -d '.' -f 5`
	newFile=merged_picoHFLambdaCMaker_${mergeLevel}_${sub}.root
	echo "merge  $ii -> $newFile"
	hadd $newFile `cat $ii`  
    done

    nFiles=`ls merged_picoHFLambdaCMaker_${mergeLevel}_*.root | wc -l`
    if [ $nFiles -gt 1 ] ; then
	ls merged_picoHFLambdaCMaker_${mergeLevel}_*.root > inlist.${mergeLevel}.list
	mergeFolder
    else
	mv merged_picoHFLambdaCMaker_${mergeLevel}_000.root picoHFLambdaCMaker.root
	mkdir -p attic
	mv merged_picoHFLambdaCMaker*.root attic
	mv splitLists* attic
	mv *.list attic
    fi
}

# ----------------------------------------------------

mkdir -p ${baseDir}/merge

inList=${baseDir}/merge/inlist.0.list
    
if [ -f ${inList} ] ; then
    rm ${inList}
fi

touch ${inList}

current=`pwd`
find ${current}/${baseDir} -name "*.root" | sort
for ii in `find ${current}/${baseDir} -name "*.root" | sort` ; do
    size=`stat -c %s $ii`
    if [ $size -gt 30000 ] ; then
	echo "$ii" >> ${inList}
    fi
done

pushd $baseDir/merge > /dev/null
mergeLevel=0
mergeFolder
popd > /dev/null
