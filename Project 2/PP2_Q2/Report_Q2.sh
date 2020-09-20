#!/bin/bash

if [ -f Q2_Results ]
then
    rm -rf Q2_Results
fi

echo -e "Test Cases N = 500:\n" >> Q2_Results
./Gauss-Elim.exe -N 500 -T 1 >> Q2_Results
./Gauss-Elim.exe -N 500 -T 2 >> Q2_Results
./Gauss-Elim.exe -N 500 -T 4 >> Q2_Results
./Gauss-Elim.exe -N 500 -T 8 >> Q2_Results
./Gauss-Elim.exe -N 500 -T 16 >> Q2_Results
./Gauss-Elim.exe -N 500 -T 32 >> Q2_Results

echo -e "Test Cases N = 1000:\n" >> Q2_Results 
./Gauss-Elim.exe -N 1000 -T 1 >> Q2_Results
./Gauss-Elim.exe -N 1000 -T 2 >> Q2_Results
./Gauss-Elim.exe -N 1000 -T 4 >> Q2_Results
./Gauss-Elim.exe -N 1000 -T 8 >> Q2_Results
./Gauss-Elim.exe -N 1000 -T 16 >> Q2_Results
./Gauss-Elim.exe -N 1000 -T 32 >> Q2_Results

echo -e "Test Cases N = 2000:\n" >> Q2_Results 
./Gauss-Elim.exe -N 2000 -T 1 >> Q2_Results
./Gauss-Elim.exe -N 2000 -T 2 >> Q2_Results
./Gauss-Elim.exe -N 2000 -T 4 >> Q2_Results
./Gauss-Elim.exe -N 2000 -T 8 >> Q2_Results
./Gauss-Elim.exe -N 2000 -T 16 >> Q2_Results
./Gauss-Elim.exe -N 2000 -T 32 >> Q2_Results
