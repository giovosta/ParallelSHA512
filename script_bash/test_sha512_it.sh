#!/bin/bash

# bash script to test and save the execution time of sha512_it program for each number of rounds. 40 tests are executed.
# if you want to use the script adjust the folder path.
password=$1
salt=$2

rounds=(32767 65535 131071 262143 524287 1048575 2097151 4194303 8388607 16777215 33554431 67108863)

for round in "${rounds[@]}"; do
    for i in {1..40}; do
        /usr/bin/time -f "Real: %E, User: %U, Sys: %S" ../../executables/sha512_it "$password" "$salt" "$round" 2>&1 | tee -a "test_sha512_it_results/test_sha512_it_results_r$round.txt"
    done
done