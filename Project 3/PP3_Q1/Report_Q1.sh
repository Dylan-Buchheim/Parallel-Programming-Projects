#!/bin/bash

if [ -f Q1_Results ]
then
    rm -rf Q1_Results
fi

echo -e "Example 7.14 Test Cases:" >> Q1_Results
echo -e "Testing with 1 thread..." >> Q1_Results 
export OMP_NUM_THREADS=1
./Example_7.14.exe >> Q1_Results
echo -e "Testing with 2 threads..." >> Q1_Results 
export OMP_NUM_THREADS=2
./Example_7.14.exe >> Q1_Results
echo -e "Testing with 4 threads..." >> Q1_Results 
export OMP_NUM_THREADS=4
./Example_7.14.exe >> Q1_Results
echo -e "Testing with 8 threads..." >> Q1_Results 
export OMP_NUM_THREADS=8
./Example_7.14.exe >> Q1_Results
echo -e "Testing with 16 threads..." >> Q1_Results 
export OMP_NUM_THREADS=16
./Example_7.14.exe >> Q1_Results
echo -e "Testing with 32 threads..." >> Q1_Results 
export OMP_NUM_THREADS=32
./Example_7.14.exe >> Q1_Results

