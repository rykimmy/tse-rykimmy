# Implementation

This document describes the implementation of the 'querier' module.

### Pseudo Code for Logic/Algorithmic Flow

The pseudo code for the general querier can be found below.

1. Parse and validate arguments
2. Initialize modules and variables (index)
3. Take queries from stdin, until EOF
    4. Parse and validate the query (syntax notes can be found in the REQUIREMENTS spec)
    5. Use the index to find matching documents
    6. Rank the documents based on their scores
    7. Print the list of documents

The pseudo code for the process of querying can be found below.

1. Initialize two counters: a *temp* and a *result*
2. Go through each of the words in the query
    3. If the word is 'or':
        3. merge *temp* with *result* (union)
        3. free *temp*
    4. If the word is 'and':
        4. continue
    5. If the word is an actual word:
        5. use index_find to get counterset for the word
        5. update *temp* (union if *temp* is empty/NULL, else intersection)
6. At the end of the query, merge *temp* with *result* (union)

### Definition of Detailed APIs, Interfaces, Function Prototypes

#### Relating to obtaining and verifying the query

*char** split_line(char* line)*

Splits the stdin string into individual words and places them into an array of words.

*bool isempty(char* line)*

Checks if stdin string is an empty line (called in *split_line*)

*bool validate_query(char** words)*

Given a list of words, checks the placement of 'or' 'and' operators. See REQUIREMENTS spec for syntax rules.

*int print_query(char** words)*

Prints the query of words and returns the number of words as a side effect.

*bool check_operators(char** words, int num_words)*

Checks if there is an or-sequence in the query as that determines how to handle words not found in the index.

*void querier_delete(char** words, int num_words)*

Deletes the query of words.

#### Relating to the process of querying

*void counters_intersect(counters_t* ct1, counters_t* ct2)*

Intersects two counters by setting the count of shared keys to the smaller of the two counts. Uses *intersect_helper*.

*void intersect_helper(void *arg, const int key, const int count)*

Gets passed into *counters_iterate* and merges the two countersets into one.

*void counters_union(counters_t* ct1, counters_t* ct2)*

Unionizes two counters by setting the count of keys to the sum of both counts. Uses *union_helper*.

*void union_helper(void* arg, const int key, const int count)*

Gets passed into *counters_iterate* and merges the two countersets into one.

*static inline int min(const int a, const int b)*

Helper function used by *intersect_helper* to find the smaller of two values.

*bool print_array(struct documentlist* glist, int size, char* pageDirectory)*

Prints the final list of documents in their sorted order.

*void delete_array(struct documentlist* glist, int size)*

Deletes the documentlist struct and its array of documents.

#### Helper functions

*void itemcount(void* arg, const int key, const int count)*

Counts the number items in counterset. Used in *counters_iterate*.

*void fill_array(void* arg, const int key, const int count)*

Fills the array of document structs by creating a document struct and assigning its values based on the counterset node's key and count values. It then adds this to the array of documents. Used in *counters_iterate*.

*int comparefunc(const void* a, const void* b)*

Comparator function that compares two documents' scores. Used with *qsort* to rank the documents based on their score.

### Error Handling and Recovery

* The program exits on the following cases:
    * memory issues caught by *assertp*
    * invalid command-line arguments
* Other errors regarding the query and stdin will print error messages but prompt the user to enter another query.