#!/bin/bash

crash=-1

run() {
    first=$(python run_with_crashes.py .. 2>&1)
    second=$(python test_localized_causal_all_properties.py .. 2>&1)  
    if [ "$second" == "CORRECT" ]; then
        echo "Correct !"
    else
        crash=1
        echo "Incorrect."
        echo "$first"
        echo "$second"
    fi
    
}

run #1st execution

while ((crash < 0)); do
   run
done