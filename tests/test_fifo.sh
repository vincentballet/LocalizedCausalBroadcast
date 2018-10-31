#!/bin/bash

make -C .. -j4

sudo tc qdisc change dev lo root netem delay 50ms 200ms loss 10% 25% reorder 25% 50%

while true
do 
  python run_with_crashes.py ..
  python test_fifo_all_properties.py .. | tee test.log
  if [ "$(cat test.log|grep INCORRECT|wc -l)" != "0" ]
  then
    break
  fi
done
