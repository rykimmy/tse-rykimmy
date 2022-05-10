# Implementation

This document describes the implementation of the 'indexer' module.

### Pseudocode

The pseudocode for indexer follows:
1. execute from a command line as shown in the User Interface
2. parse the command line, validate parameters, initialize other modules
3. Build the index using given pageDirectory
3. For each page file in pageDirectory
    4. Create a new webpage struct and insert proper content
    5. Read through the page and add to the index accordingly
6. Save the completed index to the given filename
7. Clean up

### Definition of detailed APIs, interfaces, function prototypes and their parameters

#### indexer

*index_t *index_build(char* pageDirectory)*

Builds the index by reading through each page and adding accordingly.

*void index_page(webpage_t *page, index_t *index, int id)*

Reads through a given page and adds to the index accordingly.

#### pagedir

*webpage_t* load_page(FILE *fp)*

Given a file pointer, creates a new webpage struct

*bool is_crawlerdir(const char* pageDirectory)*

Checks if a directory is created by crawler

### Data structures

*index_t* works as a hashtable.

### Testing plan

Testing is done through 'testing.sh' which runs a number of different test cases that are described below.

1. Testing invalid cases (command line arguments)
2. Test indexer using self-crawled pages
3. Test indextest by transferring test files to new test files
4. Compare these files