#!/bin/bash

if [ -f Q1_Results ]
then
    rm -rf Q1_Results
fi

echo -e "Example 7.3 Test Cases:\n" >> Q1_Results
./Example_7.3.exe -S 100 -N 100000000 -T 1 >> Q1_Results
./Example_7.3.exe -S 100 -N 100000000 -T 2 >> Q1_Results
./Example_7.3.exe -S 100 -N 100000000 -T 4 >> Q1_Results 
./Example_7.3.exe -S 100 -N 100000000 -T 8 >> Q1_Results
./Example_7.3.exe -S 100 -N 100000000 -T 16 >> Q1_Results
./Example_7.3.exe -S 100 -N 100000000 -T 32 >> Q1_Results

echo -e "\nExample 7.7 Test Cases:\n" >> Q1_Results  
./Example_7.7.exe -S 100 -N 100000000 -T 1 >> Q1_Results 
./Example_7.7.exe -S 100 -N 100000000 -T 2 >> Q1_Results 
./Example_7.7.exe -S 100 -N 100000000 -T 4 >> Q1_Results  
./Example_7.7.exe -S 100 -N 100000000 -T 8 >> Q1_Results  
./Example_7.7.exe -S 100 -N 100000000 -T 16 >> Q1_Results  
./Example_7.7.exe -S 100 -N 100000000 -T 32 >> Q1_Results  
