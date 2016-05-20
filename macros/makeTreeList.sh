#!/bin/bash

# Make fileLists for tree production
#
# #############################################
#  Authors:
#
#    Jochen Thaeder  (jmthader@lbl.gov)
#
# #############################################

channels="kPionKaonProton kProtonK0short kLambdaPion" 

basePath=/global/project/projectdirs/starprod/rnc/jthaeder/lambdaC

# ------------------------------------------------------

pushd $basePath > /dev/null

for channel in ${channels} ; do 
    treeName=LambdaC.${channel}.picoHFtree
    inFolder=production/${treeName}
    
    baseFolder=`pwd`/${inFolder}
   
    # ------------------------------------------------------
    # -- Create out structures
    # ------------------------------------------------------
    outFolder=${basePath}/lists/${treeName}
    mkdir -p ${outFolder}
    if [ -d ${outFolder}/runs ] ; then
	rm -rf ${outFolder}/runs
    fi
    mkdir -p ${outFolder}/runs

    all=${outFolder}/${treeName}_all.list
    rm -f $all
    touch $all
    
    runList=${outFolder}/${treeName}_runList.list
    rm -f $runList
    touch $runList
    
    faultyFiles=${outFolder}/${treeName}_faulty.list
    rm -f $faultyFiles
    touch $faultyFiles

    yesterday=`date --date="1 days ago" '+%F'`
    yesterdayFile=${outFolder}/daily/${treeName}_${yesterday}.list
    
    tmpYesterdayFile=${outFolder}/${treeName}_${yesterday}.list
    touch  $tmpYesterdayFile
   
    # ------------------------------------------------------
    # -- Find / loop over files
    # ------------------------------------------------------
    for day in `ls ${baseFolder} | sort` ; do 
	for run in `ls ${baseFolder}/${day} | sort` ; do 
	    nFiles=`ls ${baseFolder}/${day}/${run} | wc -l`
	    if [ $nFiles -eq 0 ] ; then 
		continue
	    fi
	    
	    runFiles=${outFolder}/runs/${treeName}_${day}_${run}.list
	    echo ${runFiles} >> ${runList} 
	    
	    find ${baseFolder}/${day}/${run} -name "*.${treeName}.root" -type f ! -size 0 | sort > ${runFiles}
	    find ${baseFolder}/${day}/${run} -name "*.${treeName}.root" -type f -size 0 | sort >> ${faultyFiles}

	    cat ${runFiles} >> ${all}
	done
    done
    
    nEntries=`cat ${all} | wc -l `
    echo "TreeFiles ${treeName} : $nEntries"

    # ------------------------------------------------------
    # -- Create an extra list of new files : "daily"
    # ------------------------------------------------------
    if [ ! -f ${yesterdayFile} ] ; then
	
	if [ ! -d ${outFolder}/daily ] ; then
	    mkdir -p ${outFolder}/daily
	fi
	
	cat ${outFolder}/daily/${treeName}_*.list > ${outFolder}/${treeName}UpToYesterday.list
	
	while read -r line ; do 
	    grep ${line} ${outFolder}/${treeName}UpToYesterday.list > /dev/null
	    ret=$?
	    
	    if [ ${ret} -eq 0 ] ; then
		continue
	    fi
	    echo $line  >> ${tmpYesterdayFile}
	done < <(cat ${all})

	nNewFiles=`cat ${tmpYesterdayFile} | wc -l`
	if [ $nNewFiles -ne 0 ] ; then
	    cp -r ${tmpYesterdayFile} ${yesterdayFile}
	fi
    fi

    rm -f ${outFolder}/${treeName}UpToYesterday.list
done


popd > /dev/null
