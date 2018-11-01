#!/bin/bash
#
# Checks the output of processes.
#

echo -e "\nTESTING PROCESS OUTPUT.\n"

while [ -n "$1" ]; do
    filename="da_proc_${1}.out"
    echo "Checking file ${filename}."
    ./check_fifo.py $filename
    if [ $? -ne 0 ]; then
        echo "Error in file ${filename}."
        echo -e "\nTEST FAILED.\n"
        exit 1
    fi
    shift
done

echo -e "\nTEST SUCCEEDED.\n"
