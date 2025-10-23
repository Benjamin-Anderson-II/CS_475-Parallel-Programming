#!/bin/bash

# compile things
g++ main.cpp -o proj1 -lm -fopenmp -DNUMT=1 -DSIZE=16384
g++ main.cpp -o proj4 -lm -fopenmp -DNUMT=4 -DSIZE=16384

# Make the single thread variable
./proj1 > out
cat out
one=$(cat out | sed 's/^[^=]*=[ ]*//;s/\ .*$//')
# make the 4 thread variable
./proj4 > out
cat out
four=$(cat out | sed 's/^[^=]*=[ ]*//;s/\ .*$//')

echo

# build a python script for dividing floats (there's prob a better way, but whatever)
echo "print(f'Speedup           = {$four/$one}')" > calc.py
echo "print(f'Parallel Fraction = {(4/3)*(1-(1/($four/$one)))}')" >> calc.py

# run the python script
python3 calc.py

# clean up
rm proj*
rm out
rm calc.py
