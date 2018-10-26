#!/bin/bash

make -C .. -j4

while true
do 
  python run_perfectlink.py .. | tee run.log
  python test_perfectlink_all_properties.py .. | tee test.log
  if [ "$(cat test.log|grep INCORRECT|wc -l)" != "0" ]
  then
    break
  fi
done
