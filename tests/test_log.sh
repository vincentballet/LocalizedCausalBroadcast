#!/bin/bash

cd ..
make -j10

g++ -O3 -o tests/test_log tests/test_log.cpp

while true
do
  ./da_proc 1 ./membership testLOG
  cat da_proc_1.out | ./tests/test_log
  if [ "X$?" != "X0" ]
  then
    echo "Error, please see output.txt"
    exit 1
  fi
done
