#!/bin/bash
#
# testing.sh
#
# Usage: ./testing.sh
#
# Output: querier test output from querier.c
#
# Assumptions:
#
# Ryan Kim
# CS50, 22S

# Using cs50 provided pageDirectory and indexFilename
pagedir="/thayerfs/courses/22spring/cosc050/cs50tse/tse-output/letters-depth-3"
indexfile="/thayerfs/courses/22spring/cosc050/cs50tse/tse-output/letters-index-3"

# Link to Index File -> http://cs50tse.cs.dartmouth.edu/tse-output/letters-index-3
# Link to Page Directory -> http://cs50tse.cs.dartmouth.edu/tse-output/letters-depth-3

# Invalid Test Cases
echo "Testing Error Cases"
echo -----------------------------------------

echo "Error Test Case 1: no arguments"
./querier
echo -----------------------------------------

echo "Error Test Case 2: invalid pagedir"
./querier invalid $indexfile
echo ----------------------------------------- 

echo "Error Test Case 3: invalid indexFilename"
./querier $pagedir invalid
echo -----------------------------------------

echo "Error Test Case 4: blank query"
echo "        " | ./querier $pagedir $indexfile
echo -----------------------------------------

echo "Error Test Case 5: invalid operators"
echo "and for or first" | ./querier $pagedir $indexfile
echo -----------------------------------------

echo "Error Test Case 6: invalid operators"
echo "for and or first" | ./querier $pagedir $indexfile
echo -----------------------------------------

echo "Error Test Case 7: invalid operators"
echo " for or first and   " | ./querier $pagedir $indexfile
echo -----------------------------------------

echo "Error Test Case 8: invalid query (bad chars)"
echo "    home or 9first   " | ./querier $pagedir $indexfile
echo -----------------------------------------

echo "Error Test Case 9: nonexistent word in <andsequence>"
echo "for and free" | ./querier $pagedir $indexfile
echo -----------------------------------------

echo "Testing Valid Cases"
echo -----------------------------------------

echo "Valid Test Case 1: single word"
echo "for" | ./querier $pagedir $indexfile
echo -----------------------------------------

echo "Valid Test Case 2: nonexistent word in <orsequence>"
echo "for or free" | ./querier $pagedir $indexfile
echo -----------------------------------------

echo "Valid Test Case 3: <andsequence> with valid words"
echo "home and this" | ./querier $pagedir $indexfile
echo -----------------------------------------

echo "Valid Test Case 4: sequence of valid words (with valgrind)"
echo "home this or search algorithm" | valgrind ./querier $pagedir $indexfile
echo -----------------------------------------

exit 0