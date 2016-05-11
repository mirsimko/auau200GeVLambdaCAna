#!/bin/bash

hackTemplate=runjob_temp.xml
xmlFile=runjob.xml


nEntries=900215583

for i in $( seq 1 50000000 $nEntries); do
  let  j=$i+50000000-1
  echo
  echo Submitting for $i to $j
  echo

  # producing hack Template
  echo '<?xml version="1.0" encoding="utf-8" ?>' > $hackTemplate
  echo '<!DOCTYPE note ['		        >> $hackTemplate
  echo '<!ENTITY first "'$i'">'			>> $hackTemplate
  echo '<!ENTITY last "'$j'">'		   	>> $hackTemplate
  echo '<!ENTITY baseDir "'$PWD'">'		   	>> $hackTemplate
  echo ']>'                                     >> $hackTemplate
    
  tail -n +2 $xmlFile >> $hackTemplate

  star-submit -u ie $hackTemplate
done
