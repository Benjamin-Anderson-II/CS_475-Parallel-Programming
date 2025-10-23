#!/bin/bash
for s in 1024 4096 32768 131072 524288 1048576 2097152 4194304 8388608; do
    g++ proj04.cpp -DARRAYSIZE=$s -o proj04 -lm -fopenmp
    ./proj03
done
