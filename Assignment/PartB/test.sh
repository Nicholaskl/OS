#! /bin/bash

# This file checks that program works as expected, file length should be 700 for the 50 line example

make

valgrind -s ./lift_sim_B 1 0 #simulates with 1 sized buffer and 0 time

wc -l sim_output #prints line lenth

rm sim_output