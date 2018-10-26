#!/bin/bash

make -C .. -j4

while true
do 
  python run_with_crashes.py .. | tee run.log
  python test_fifo_all_properties.py .. | tee test.log
  if [ "$(cat test.log|grep INCORRECT|wc -l)" != "0" ]
  then
    break
  fi
done
