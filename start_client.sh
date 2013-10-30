#!/bin/bash

echo You try to start $1 Ant client

if [ $# -eq 0 ]
  then
    echo "No arguments supplied. Please enter the number of clients you want to start as second arg."
    exit 0
fi

if [ $1 -eq 0 ]; then
    echo "Client number have to be more than 0."
    exit 0
fi

DIR='logs/'
LOG='mylog'
for (( i = 1 ; i <= $1 ; i++ ))
do
    python Ant.py >> $DIR$LOG$i 2<&1 &
    pid=$!
    PIDS[${#PIDS[*]}]=$pid
    #myarr[${#myarr[*]}]=”$newitem”
    echo $pid status $?
done

echo ${PIDS[*]}
