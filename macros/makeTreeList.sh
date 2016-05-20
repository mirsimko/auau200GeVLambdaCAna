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

# ------------------------------------------------------

for channel in ${channels} ; do 
    treeName=LambdaC.${channel}.picoHFtree
    inFolder=production/${treeName}
    
    baseFolder=`pwd`/${inFolder}

    # ------------------------------------------------------

    outFolder=lists/${treeName}
    if [ -d ${outFolder} ] ; then 
	rm -rf ${outFolder}
    fi
    mkdir -p ${outFolder}/runs
    
    all=${outFolder}/${treeName}_all.list
    touch $all
    
    runList=${outFolder}/${treeName}_runList.list
    touch $runList
    
    faultyFiles=${outFolder}/${treeName}_faulty.list
    touch $faultyFiles
    
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
	    runEntry=${outFolder}/runs/${treeName}_${day}_${run}.list
	    echo ${runEntry} >> ${runList} 
	    
	    find ${baseFolder}/${day}/${run} -name "*.${treeName}.root" -type f ! -size 0 | sort > ${runFiles}
	    find ${baseFolder}/${day}/${run} -name "*.${treeName}.root" -type f -size 0 | sort >> ${faultyFiles}
	    
	    cat ${runFiles} >> ${all}
	done
    done
    
    nEntries=`cat ${all} | wc -l `
    
    echo "TreeFiles ${treeName} : $nEntries"
done

