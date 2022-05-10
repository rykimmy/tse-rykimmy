#!/bin/bash
#
# testing.sh
#
# Usage: ./testing.sh
#
# Output: crawler test output
#
# Assumptions:
#   - Directory for test files exists
#
# Ryan Kim
# CS50, 22S

pagedir="pages"
prog="./crawler"

# Test Case 0: invalid arguments
echo "TEST CASE 0"

# Test with invalid pageDirectory
echo "test with invalid pageDirectory:"
$prog http://cs50tse.cs.dartmouth.edu/tse/letters/index.html invalidDirectory 0

# Test with invalid maxDepth
echo "test with invalid maxDepth:"
$prog http://cs50tse.cs.dartmouth.edu/tse/letters/index.html $pagedir 20

# Test Case 1: seedURL that points to non-existent server
echo "TEST CASE 1"
echo "test with seedURL that points to non-existent server:"
$prog http://invalidurl.html $pagedir 0

# Test Case 2: seedURL that poitns to non-internal server
echo "TEST CASE 2"
echo "test with seedURL that points to non-internal server:"
$prog http://google.com $pagedir 0

# Test Case 3: seedURL that points to valid server but non-existent page
echo "TEST CASE 3"
echo "test with seedURL that points to valid server but non-existent page:"
$prog http://cs50tse.cs.dartmouth.edu/tse/letters/nonexistentpage.html $pagedir 0

# Test Case 4
echo "TEST CASE 4"

echo "test with seedURL: wiki, depth: 1"
$prog http://cs50tse.cs.dartmouth.edu/tse/wikipedia/index.html $pagedir 1

echo "test with seedURL: toscrape, depth: 1"
$prog http://cs50tse.cs.dartmouth.edu/tse/toscrape/index.html $pagedir 1

echo "test with seedURL: letters, depth: 1 (with valgrind)"
valgrind $prog http://cs50tse.cs.dartmouth.edu/tse/letters/index.html $pagedir 1

# Test Case 5
echo "TEST CASE 5"
echo "test with seedURL: letters, depth: 0"
$prog http://cs50tse.cs.dartmouth.edu/tse/letters/index.html $pagedir 0

echo "test with seedURL: letters, depth: 1"
$prog http://cs50tse.cs.dartmouth.edu/tse/letters/index.html $pagedir 1

echo "test with seedURL: letters, depth: 2"
$prog http://cs50tse.cs.dartmouth.edu/tse/letters/index.html $pagedir 2

echo "test with seedURL: letters, depth: 3"
$prog http://cs50tse.cs.dartmouth.edu/tse/letters/index.html $pagedir 3

echo "test with seedURL: letters, depth: 4"
$prog http://cs50tse.cs.dartmouth.edu/tse/letters/index.html $pagedir 4

echo "test with seedURL: letters, depth: 5"
$prog http://cs50tse.cs.dartmouth.edu/tse/letters/index.html $pagedir 5

# Test Case 6
echo "TEST CASE 6"
echo "test with seedURL: letters page at depth 1"
$prog http://old-www.cs.dartmouth.edu/~cs50/data/tse/letters/A.html $pagedir 1

echo "test with seedURL: letters page at depth 4"
$prog http://old-www.cs.dartmouth.edu/~cs50/data/tse/letters/G.html $pagedir 1

# Test Case 7
echo "TEST CASE 7"
echo "test with seedURL: wiki, depth: 0"
$prog http://cs50tse.cs.dartmouth.edu/tse/wikipedia/index.html $pagedir 0

echo "test with seedURL: wiki, depth: 1"
$prog http://cs50tse.cs.dartmouth.edu/tse/wikipedia/index.html $pagedir 1

echo "test with seedURL: wiki, depth: 2"
$prog http://cs50tse.cs.dartmouth.edu/tse/wikipedia/index.html $pagedir 2

# Test Case 8
echo "TEST CASE 8"
echo "test with seedURL: letters, depth: "
bash $prog http://cs50tse.cs.dartmouth.edu/tse/letters/index.html $pagedir 5

exit