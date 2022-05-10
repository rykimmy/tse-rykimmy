#!/bin/bash
#
# testing.sh
#
# Usage: ./testing.sh
#
# Output: indexer test output from indexer and indextest
#
# Assumptions:
#
# Ryan Kim
# CS50, 22S

pagedir="pages"
crawler_out=thayerfs/courses/22spring/cosc050/cs50tse/tse-output

# Invalid Test Cases

echo "Testing Error Cases: Command Line"

# Indexer

echo "indexer: invalid number of arguments"
./indexer
echo

echo "indexer: invalid pagedir"
./indexer nonexistentdir index_file
echo

# Indextest

echo "indextest: invalid number of arguments"
./indextest
echo

# Valid Test Cases (using my crawler output)

# Setting up crawler
cd ../crawler
rm -rf pages
cd ../indexer
mkdir pages
cd ../crawler
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../indexer/pages 3
cd ../indexer

# Testing indexer

echo "Testing Valid Cases (using my crawler output)"

# Indexing letters-depth-1, output to 'test1'
echo "indexer: indexing letters-depth-1 into 'test1'"
./indexer $pagedir test1
echo

# Indexing letters-depth-2, output to 'test2'
echo "indexer: indexing letters-depth-2 into 'test2'"
./indexer $pagedir test2
echo

# Indexing letters-depth-3, output to 'test3'
echo "indexer: indexing letters-depth-3 into 'test3'"
./indexer $pagedir test3
echo

# Testing indextest

# Indextest: test1 --> new_test1
echo "indextest: moving 'test1' index file into 'new_test1' index file"
./indextest test1 new_test1
echo

# Indextest: test2 --> new_test2
echo "indextest: moving 'test2' index file into 'new_test2' index file"
./indextest test2 new_test2
echo

# Indextest: test3 --> new_test3
echo "indextest: moving 'test3' index file into 'new_test3' index file"
./indextest test3 new_test3
echo

# Comparing tests

# Comparing test case 1: test1 and new_test1
echo "Comparing Test Files: test1 and new_test1"
diff --brief <(sort test1) <(sort new_test1) >/dev/null
comp_value=$?
if [ $comp_value -eq 1 ]
then
    echo "test1: they're different"
else
    echo "test1: they're the same"
fi
echo

# Comparing test case 2: test2 and new_test2
echo "Comparing Test Files: test2 and new_test2"
diff --brief <(sort test2) <(sort new_test2) >/dev/null
comp_value=$?
if [ $comp_value -eq 1 ]
then
    echo "test2: they're different"
else
    echo "test2: they're the same"
fi
echo

# Comparing test case 3: test3 and new_test3
echo "Comparing Test Files: test3 and new_test3"
diff --brief <(sort test3) <(sort new_test3) >/dev/null
comp_value=$?
if [ $comp_value -eq 1 ]
then
    echo "test1: they're different"
else
    echo "test1: they're the same"
fi

exit 0