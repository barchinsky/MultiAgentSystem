#!/bin/bash

echo You try to start $1 instance of Ant client.

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

echo "Clearing last logs..."
rm -v $DIR/*
echo "Clearing finished."

for (( i = 1 ; i <= $1 ; i++ ))
do
    python Ant.py >> $DIR$LOG$i 2<&1 &
    pid=$!
    PIDS[${#PIDS[*]}]=$pid
    #myarr[${#myarr[*]}]=”$newitem”
    echo $pid status $?
done

read command
echo comand: $command

# kill all running clients
if [ $command ]; then
    echo ${PIDS[*]}
    echo are going to be killed...
    kill ${PIDS[*]}
    echo All processes have been successfully killed or they were finished before this action.
fi
