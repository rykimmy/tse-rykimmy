# TSE Querier Design Spec

A Querier is a standalone program that reads the index file produced by the TSE Indexer, and page files produced by the TSE Querier, and answers search queries submitted via stdin.

This Design Spec contains the following sections:

* User Interface
* Inputs and Outputs
* Functional Decomposition Into Modules
* Pseudo Code for Logic/Algorithmic Flow
* Dataflow Through Modules
* Major Data Structures
* Testing Plan

### User Interface

The querier interacts with the user both when being called in the command-line as well as through stdin. In the command-line, the user is expected to include two additional arguments: the *pageDirectory* which holds the pages that were crawled by crawler, and the *indexFilename* which holds the index file produced by indexer. When the user runs the querier program with the appropriate arguments, they will be prompted to enter a query, thus allowing the user to type into stdin their search queries.

### Inputs and Outputs

Input: As mentioned above, the querier takes input through both the command-line calling of querier as well as through stdin after being prompted to enter a query.

Output: The querier ouputs numerous things. First, it outputs text prompting the user to enter a query. Once the user has done so, the querier will output the query that was entered. On success, it will then output the number of documents that match the query as well as each document's ID, score, and URL, in order of the highest scored documents. Additionally, the querier will output any error messages that occur during the running of the program.

### Functional Decomposition Into Modules

The main modules and/or functions used are listed below:

1. *main*, which parses arguments, initializes other modules, and calls on other functions.
2. *split_line*, which splits a string of words into individual words that are then put into an array.
3. *validate_query*, which looks through the array of words and verifies the placement of operators; a description of operator placement can be found in the REQUIREMENTS spec.
4. *counters_intersect*, which intersects two countersets with the help of *intersect_helper*.
5. *counters_union*, which unionizes two countersets with the help of *union_helper*.
6. *querier_delete*, which deletes the array of words.
7. *fill_array*, which fills an array of documents (struct) with the data from each counterset node.
8. *print_array*, which outputs matching documents (ID, score, url), listed in order of score.
9. *delete_array*, which deletes the array of documents.

Below are additional helper functions:

1. *comparefunc*, a comparator which compares two document structs and gets passed into *qsort*.
2. *itemcount*, which counts the number of nodes in a counterset and is passed into *counters_iterate*.
3. *intersect_helper*, which sets the count of a key as the minimum of two counts from two countersets.
4. *union_helper*, which sets the count of a key as the sum of two counts from two countersets.
5. *check_operators*, which looks through the array of words and sees whether there is an or-sequence; this information is used to decide how to handle words in the query that are not in index (more information about sequences can be found in the REQUIREMENTS spec).
6. *print_query*, which prints the query the user entered and returns the number of words in the query.
7. *isempty*, which checks if the user's stdin is an empty line.
8. *is_crawlerdir*, from pagedir.c in common, which verifies  the given pageDirectory.

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

### Major Data Structures

Querier uses three structs:

1. *twocts*, which holds two counters that are to be merged
2. *document*, which holds an ID and score and represents a single document
3. *documentlist*, which holds an array of documents and a *position* variable to hold the number of slots filled.

In addition, querier uses the following:

1. *index* of words, the docIDs of the pages that hold the word, and the counts of times they appear.
2. *counters* to hold the docIDs and scores of words

### Testing Plan

Testing is done in 'testing.sh', which pipelines echo commands into the querier. Additionally, 'testing.sh' utilizes the given TSE output instead of my own *crawler* and *indexer* output. The pageDirectory references letters-depth-3 and the indexFilename provides the index-letters-3.

'testing.sh' runs through a series of test cases, beginning with error test cases involving invalid command-line arguments and invalid query input. It then runs through a series of valid test cases, outputting matching documents in order of their score.

As mentioned in querier's README.md, the 'testing.sh' output is stored in 'testing.out', but the format of this file may seem a little strange as the querier program is intended for a user to submit stdin and thus prompting calls appear in testing output.