#!/bin/bash
#
# Tests the performance of the Uniform Reliable Broadcast application.
#
# usage: ./test_performance evaluation_time
#
# evaluation_time: Specifies the number of seconds the application
#                  should run.
#

evaluation_time=$1
init_time=2

echo "127.0.0.1 11001
127.0.0.1 11002
127.0.0.1 11003
127.0.0.1 11004
127.0.0.1 11005" >> membership

#start 5 processes
for i in `seq 1 5`
do
    ./da_proc $i membership &
    da_proc_id[$i]=$!
done

#leave some time for process initialization
sleep $init_time

#start broadcasting
echo "Evaluating application for ${evaluation_time} seconds."
for i in `seq 1 5`
do
    if [ -n "${da_proc_id[$i]}" ]; then
	kill -USR1 "${da_proc_id[$i]}"
    fi
done

#let the processes do the work for some time
sleep $evaluation_time

#stop all processes
for i in `seq 1 5`
do
    if [ -n "${da_proc_id[$i]}" ]; then
	kill -TERM "${da_proc_id[$i]}"
    fi
done

#wait until all processes stop
for i in `seq 1 5`
do
	wait "${da_proc_id[$i]}"
done

#count delivered messages in the logs
#... (not implemented here)

echo "Performance test done."
