#!/bin/bash

# bash script to test and save the execution time of the various versions of psha512 for each number of leaves. 40 tests are executed.
# if you want to use the script adjust the folder path.
password=$1
salt=$2

cores=(2 4 8)

leaves=(8192 65536 262144 1048576 2097152 4194304)

log_and_print() {
    echo "$1" | tee -a "$2"
}

for leaf in "${leaves[@]}"; do
    for core in "${cores[@]}"; do
        for i in {1..40}; do
            # Testing psha512_v1
            logfile="results/test_psha512_${core}cores/test_psha512_v1_${core}cores.txt"
            if [ $i -eq 1 ]; then 
                log_and_print "Testing with $leaf leaves..." $logfile
            fi
            /usr/bin/time -f "Real: %E, User: %U, Sys: %S" ./executables/psha512_v1 "$password" "$salt" "$core" "$leaf" 2>&1 | tee -a "$logfile"
            if [ $i -eq 40 ]; then 
                log_and_print "----------------------------------------" $logfile
            fi

            # Testing psha512_v2
            logfile="results/test_psha512_${core}cores/test_psha512_v2_${core}cores.txt"
            if [ $i -eq 1 ]; then 
                log_and_print "Testing with $leaf leaves..." $logfile
            fi
            /usr/bin/time -f "Real: %E, User: %U, Sys: %S" ./executables/psha512_v2 "$password" "$salt" "$core" "$leaf" 2>&1 | tee -a "$logfile"
            if [ $i -eq 40 ]; then 
                log_and_print "----------------------------------------" $logfile
            fi

            # Testing psha512_v3
            logfile="results/test_psha512_${core}cores/test_psha512_v3_${core}cores.txt"
            if [ $i -eq 1 ]; then 
                log_and_print "Testing with $leaf leaves..." $logfile
            fi
            /usr/bin/time -f "Real: %E, User: %U, Sys: %S" ./executables/psha512_v3 "$password" "$salt" "$core" "$leaf" 2>&1 | tee -a "$logfile"
            if [ $i -eq 40 ]; then 
                log_and_print "----------------------------------------" $logfile
            fi
        done
    done
done