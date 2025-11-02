#!/bin/bash


declare -a arr=( "iCFc" "iUMo" "iUMc" )
# "iCFo" "pCFo" "pCFc" "pUMo" "pUMc" "odd" "proper"
# Proper has been checked from 3:13, it is correct
# Others have been checked from 3:11
# iCFo, iCFc, iUMo, iUMc to 14

## loop through above array
for i in "${arr[@]}"
do
  echo "$i:"
  ./checkNaiveOutputs.sh 12:14 -c "$i" | sed 's/^/  /'
done
