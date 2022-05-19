/*
querier.c – 

Usage: ./querier pageDirectory indexFilename

Input: nothing other than command-line arguments
Output: error messages

Ryan Kim
CS50, 22S
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "../common/pagedir.h"
#include "../common/index.h"
#include "../common/word.h"
#include "../libcs50/file.h"
#include "../libcs50/memory.h"
#include "../libcs50/counters.h"

/******************* Local Data Types ******************/
/*
Holds two counters, which will be merged together into one
*/
struct twocts {
    counters_t* result;
    counters_t* append;
};

/*
Represents one element in the counter set (id:score)
*/
struct document {
    int id;
    int score;
};

/*
Represents a collection of documents
*/
struct documentlist {
    int pos;
    struct document** docs;
};

/******************* Local Function Prototypes ******************/
char** split_line(char* line);
bool isempty(char* line);
bool validate_query(char** words);
int print_query(char** words);
bool check_operators(char** words, int num_words);
void counters_intersect(counters_t* ct1, counters_t* ct2);
void intersect_helper(void *arg, const int key, const int count);
void counters_union(counters_t* ct1, counters_t* ct2);
void union_helper(void* arg, const int key, const int count);
void querier_delete(char** words, int num_words);
void itemcount(void* arg, const int key, const int count);
void fill_array(void* arg, const int key, const int count);
int comparefunc(const void* a, const void* b);
bool print_array(struct documentlist* glist, int size, char* pageDirectory);
void delete_array(struct documentlist* glist, int size);

static inline int min(const int a, const int b) {
    return (a < b ? a : b);
}

/******************* main ******************/
int main(const int argc, char *argv[]) {
    //------------- Error Checking and Setup -------------//
    int status = 0;

    // Checking number of arguments
    if (argc != 3) {
        fprintf(stderr, "Invalid number of arguments\nUsage: ./querier pageDirectory indexFilename\n");
        status++;
        return status;
    }

    // Checking pageDirectory
    char* pageDirectory = calloc(strlen(argv[1]) + 1, sizeof(char));
    strcpy(pageDirectory, argv[1]);
    assertp(pageDirectory, "invalid pageDirectory\n");

    if (!is_crawlerdir(pageDirectory)) {
        free(pageDirectory);
        status++;
        return status;
    }

    // Checking indexFilename
    char* indexFilename = calloc(strlen(argv[2]) + 1, sizeof(char));
    strcpy(indexFilename, argv[2]);
    
    FILE *fp;
    if ((fp = fopen(indexFilename, "r")) == NULL) {
        fprintf(stderr, "invalid arg 3: indexFilename does not exist or is not readable\n");
        status++;
        return status;
    }
    fclose(fp);
    
    index_t* index = index_load(indexFilename);

    //------------- Query -------------//

    printf("Enter Query: \n");
    char* input;            // Each query line
    bool correct = true;    // Works as an exit method to break out of nested while loop

    // Repetitively get queries until user quits using EOF (ctrl D)
    while ((input = freadlinep(stdin)) != NULL) {
    
        //------------- Checking/Setup of Query -------------//

        // Getting array of words
        char** words;
        words = split_line(input);

        // Error checking
        if (words == NULL) {
            printf("Enter Query: \n");
            free(words);
            free(input);
            continue;
        }

        // Validate query syntax (checking operators)
        if (!validate_query(words)) {
            printf("Enter Query: \n");
            free(words);
            free(input);
            continue;
        }

        // Print the query (gets the number of words as a side effect)
        int num_words = print_query(words);

        //------------- Querying -------------//

        counters_t *result = counters_new();
        counters_t *tmp = NULL;
        char* a = "and";
        char* o = "or";
        bool or_sequence = false;

        if (check_operators(words, num_words)) {
            or_sequence = true;
        }

        int count = 0;
        while (count < num_words) {
            // 'or':
            if (strcmp(words[count], o) == 0) {
                counters_union(result, tmp);
                counters_delete(tmp);
                tmp = NULL;
            }

            // 'and':
            else if (strcmp(words[count], a) == 0) {
                count++;
                continue;
            }

            // word:
            else {
                counters_t* counters = index_find(index, words[count]);

                // If word does not exist in index and it is an <andsequence>
                if (counters == NULL && or_sequence == false) {
                    fprintf(stderr, "No matches found\n");

                    counters_delete(tmp);
                    counters_delete(result);

                    // Breaks out of the inner while loop and catches a conditional to start a new query
                    correct = false;
                    break;
                }
                // Else if the word does not exist in index but it is an <orsequence>
                else if (counters == NULL && or_sequence == true) {
                    counters = counters_new();
                }

                // Updating counterset 'tmp'
                if (tmp == NULL) {
                    tmp = counters_new();
                    counters_union(tmp, counters);
                }
                else {
                    counters_intersect(tmp, counters);
                }
            }
            count++;
        }

        // If word was not found in index, this conditional will reset the query process
        if (!correct) {
            printf("Enter Query: \n");

            querier_delete(words, num_words);
            free(input);

            correct = true;
            continue;
        }

        // Merge the counters into one
        if (tmp != NULL) {
            counters_union(result, tmp);
            counters_delete(tmp);
        }

        //------------- Ranking/Sorting Scores -------------//
        
        // Get size of counterset
        int num_items = 0;
        counters_iterate(result, &num_items, itemcount);

        // Create struct documentlist to hold array of documents (id, score)
        struct document** d = calloc(num_items, sizeof(struct document));
        struct documentlist list = {0, d};
        
        // Iterate through counter and fill document array with counterset data using fill_array helper
        counters_iterate(result, &list, fill_array);

        // Sort the list using comparefunc helper
        qsort(d, num_items, sizeof(struct document), comparefunc);

        // Print the sorted list of documents with its ID, Score, and URL
        if (!print_array(&list, num_items, pageDirectory)) {
            delete_array(&list, num_items);
            querier_delete(words, num_words);
            free(input);
            counters_delete(result);
            printf("Enter Query: \n");
            continue;
        }

        // Delete array of documents and all of its contents
        delete_array(&list, num_items);

        // Cleanup
        querier_delete(words, num_words);
        free(input);
        counters_delete(result);

        // Prompts the next query
        printf("Enter Query: \n");
    }

    // Cleanup
    index_delete(index);
    free(pageDirectory);
    free(indexFilename);
    return status;
}

/******************* HELPER FUNCTIONS ******************/

/******************* check_operators ******************/
/*
Goes through the list of words from the query and checks if there are any 'or's. If so, returns true; else, returns false.

Takes:
    char** words - the list of words from the query
    int num_words - the number of words in the list
Returns:
    true, if there are any 'or's
    false, if there are none
*/
bool check_operators(char** words, int num_words) {
    char* or = "or";
    for (int i = 0; i < num_words; i++) {
        if (strcmp(words[i], or) == 0) {
            return true;
        }
    }
    return false;
}

/******************* delete_array ******************/
/*
Delete (free) all of the documents within given struct documentlist.

Takes:
    struct documentlist* glist - an array of documents
    int size - the size of the array
Returns:
    nothing
*/
void delete_array(struct documentlist* glist, int size) {
    // Error checking
    if (glist == NULL || size < 1) {
        fprintf(stderr, "delete_array failed: invalid arguments\n");
        return;
    }

    struct documentlist* list = glist;

    for (int i = 0; i < size; i++) {
        if (list->docs[i] != NULL) {
            free(list->docs[i]);
        }
    }
    free(list->docs);
}
/******************* print_array() ******************/
/*
Prints an array of documents, providing the ID, Score, and URL of each document.

Takes:
    struct documentlist* glist - the struct holding the array of documents
    int size - the size of the array
    char* pageDirectory - the directory holding all of the pages crawled by crawler
Returns:
    true if printed successfully
    false if parameters are invalid or any errors (invalid filename/pageDirectory)
Assumes:
    pageDirectory documents are numbered and formatted properly (1, 2, 3...; URL is first line)
*/
bool print_array(struct documentlist* glist, int size, char* pageDirectory) {
    // Error checking
    if (glist == NULL || size < 1 || pageDirectory == NULL) {
        fprintf(stderr, "print_array failed: invalid arguments\n");
        return false;
    }

    // Initialize values
    struct documentlist* list = glist;
    struct document** array = list->docs;

    printf("Matches %d document(s) (ranked):\n", size);
    printf("--------------------------------------------------------------------------\n");

    // For each document in the array
    for (int i = 0; i < size; i++) {

        // Create filename to read into (in order to obtain the url of the document)
        char* filename = calloc(strlen(pageDirectory) + 3, sizeof(char));
        sprintf(filename, "%s/%d", pageDirectory, array[i]->id);
        assertp(filename, "print_array() failed: invalid filename creation\n");

        // Open file and read line to get url
        FILE* fp = fopen(filename, "r");
        if (fp == NULL) {
            fprintf(stderr, "print_array() failed: cannot access files in pageDirectory\n");
            return false;
        }
        char* url = freadlinep(fp);

        // Print each document's info
        printf("DocID: %d Score: %d URL: %s\n", array[i]->id, array[i]->score, url);

        // Cleanup
        fclose(fp);
        free(url);
        free(filename);
    }
    printf("--------------------------------------------------------------------------\n");
    return true;
}

/******************* comparefunc() ******************/
/*
Helper function to qsort() that takes two void* arguments (two struct documents) and compares their scores. This function is not called by the user but rather used by qsort().

Takes:
    const void* a - one struct document
    const void* b - another struct document to be compared with the first
Returns:
    the score of docB minus score of docA
*/
int comparefunc(const void* a, const void* b) {
    struct document* docA = *(struct document**)a;
    struct document* docB = *(struct document**)b;
    return (docB->score - docA->score);
}

/******************* fill_array() ******************/
/*
A helper function that is passed into counters_iterate to collect each counterset element's key=count data and store it within the struct documentlist's array of documents. It first makes a struct document out of the key=count values of the counterset and then adds that to the array of documents.

Takes:
    void* arg - this is the struct documentlist
    const int key - the key of the counterset, which acts as the docID
    const int count - the count of the counterset, which acts as the score of the document
Returns:
    nothing
*/
void fill_array(void* arg, const int key, const int count) {
    struct documentlist* list = arg;

    if (list != NULL && key > 0 && count > 0) {
        // Create a document struct and set its id and score to the key and count of the counterset node
        struct document *pair = malloc(sizeof(struct document));
        pair->id = key;
        pair->score = count;

        // Add the document to the array of documents in the documentlist
        list->docs[list->pos] = pair;
        list->pos++;
    }
}

/******************* itemcount() ******************/
/*
Helper function that gets passed into counters_iterate to count the number of items in the counterset.

Takes:
    void* arg - the counter variable which holds the count of items
    int key - the key of a given node
    int count - the corresponding value associated with the key (number of times key has appeared)
Returns:
    nothing
*/
void itemcount(void* arg, const int key, const int count) {
    int *nitems = arg;

    if (nitems != NULL && key >= 0 && count > 0) {
        (*nitems)++;
    }
}

/******************* querier_delete() ******************/
/*
Frees each word in an array of words as well as the array itself.

Takes:
    char** words - an array of words
    int num_words - the number of words in the array
Returns:
    nothing
Assumes:
    parameters given are valid
*/
void querier_delete(char** words, int num_words) {
    if (words == NULL || num_words < 1) {
        fprintf(stderr, "querier_delete() failed: invalid parameters\n");
    }
    else {
        for (int i = 0; i < num_words; i++) {
            free(words[i]);
        }
        free(words);
    }
}

/******************* counters_intersect() ******************/
/*
Given two counters, goes through both to intersect them; looks for keys that are in both counters and sets the score (count) of that key as the minimum count of both counters. At the end, there should be one main counterset that represents the intersection of the original two.

Takes:
    two counters
Returns:
    nothing
Uses:
    a helper function that is passed into counters_iterate and applied to each key:count node in the counter set
*/
void counters_intersect(counters_t* ct1, counters_t* ct2) {
    assertp(ct1, "counters_intersect() failed: invalid ct1\n");
    assertp(ct2, "counters_intersect() failed: invalid ct2\n");

    struct twocts args = {ct1, ct2};
    counters_iterate(ct1, &args, intersect_helper);
}

/******************* intersect_helper ******************/
/*
Helper function that is passed into counters_iterate when intersecting counters. This function sets the count of a key to the smaller of the counts in two countersets that are being merged.

Takes:
    void* arg - this is a struct twocts, which holds the two countersets being merged
    const int key - the key of the counterset node, which represents the docID
    const int count - the count of the counterset node, which represents the score of the document
Returns:
    nothing
*/
void intersect_helper(void* arg, const int key, const int count) {
    struct twocts* two = arg;
    counters_set(two->result, key, min(count, counters_get(two->append, key)));
}

/******************* counters_union() ******************/
/*
Given two counters, goes through both to unionize them; adds the counts of keys to combine the counters into one.

Takes:
    two counters
Returns:
    nothing
Uses:
    a helper function that is passed into counters_iterate and applied to each key:count node in the counter set
*/
void counters_union(counters_t* ct1, counters_t* ct2) {
    assertp(ct1, "counters_union() failed: invalid ct1\n");
    assertp(ct2, "counters_union() failed: invalid ct2\n");

    struct twocts args = {ct1, ct2};
    counters_iterate(ct2, &args, union_helper);
}

/******************* union_helper ******************/
/*
Helper function that is passed into counters_iterate when unionizing counters. This function sets the count of a key to the sum of the counts in two countersets that are being merged.

Takes:
    void* arg - this is a struct twocts, which holds the two countersets being merged
    const int key - the key of the counterset node, which represents the docID
    const int count - the count of the counterset node, which represents the score of the document
Returns:
    nothing
*/
void union_helper(void* arg, const int key, const int count) {
    struct twocts* two = arg;
    counters_set(two->result, key, counters_get(two->result, key) + count);
}

/******************* split_line() ******************/
/*
Given a string, split_line splits the line into multiple words and puts them all into an array of words, which is returned back to the caller. If the string is an empty line or just has spaces, returns NULL back to its caller, which prompts for another query.

Takes:
    char* line - a string of words
Returns:
    char** words - an array of words from 'line'
    NULL - if line is empty, NULL, or if there are invalid chars
*/
char** split_line(char* line) {
    // Error checking
    if (line == NULL || isempty(line)) {
        return NULL;
    }

    // Initializing values
    char** words = calloc((strlen(line)+1) / 2, sizeof(char*));
    char* word = strtok(line, " ");
    int count = 0;

    // Going through the rest of the words and using strtok to split
    while (word != NULL) {

        // Error Checking -> checking each char to see if it is a valid char (alphabet or space)
        for (int i = 0; i < strlen(word); i++) {
            char ch = word[i];
            if (isalpha(ch) == 0 && isspace(ch) == 0) {
                fprintf(stderr, "invalid query: character '%c' is unacceptable\n", ch);
                return NULL;
            }
        }

        // Changes everything to lower case
        word = normalize_word(word);

        // Add word to the array of words
        words[count] = calloc(strlen(word) + 1, sizeof(char));
        strcpy(words[count], word);

        // Move onto next word
        word = strtok(NULL, " ");
        count++;
    }
    return words;
}

/******************* isempty ******************/
/*
Checks if an entered query is just space

Takes:
    char* line - the user's input
Returns:
    false, if any characters in the line are not a space
    true, if there are only spaces
*/
bool isempty(char* line) {
    for (int i = 0; i < strlen(line); i++) {

        // If any char is not a space, return false;
        if (isspace(line[i]) == 0) {
            return false;
        }
    }
    return true;
}

/******************* validate_query ******************/
/*
Validates the query by going through words in given array of words and looking at the operators. Operators may not be at the front or back of the array, nor can they appear back to back.

Takes:
    char** words - array of words
Returns:
    true, if operators are all in valid positions within the array
    false, if 'and' or 'or' operators appear at the beginning of the array, end of the array, or if they appear back to back
*/
bool validate_query(char** words) {
    // Initializing values
    char* a = "and";
    char* o = "or";
    int count = 0;
    bool operator = false;

    // Checking first word
    if (strcmp(words[count], a) == 0 || strcmp(words[count], o) == 0) {
        fprintf(stderr, "invalid query: cannot begin with an operator\n");
        return false;
    }

    // Checking the rest of the words
    while (words[count] != NULL) {

        // If previous word was an operator
        if (operator) {
            if (strcmp(words[count], a) == 0 || strcmp(words[count], o) == 0) {
                fprintf(stderr, "invalid query: cannot have back to back operators\n");
                return false;
            }
            operator = false;
        }

        // If previous word was NOT an operator
        else {
            if (strcmp(words[count], a) == 0 || strcmp(words[count], o) == 0) {
                operator = true;
            }
        }
        count++;
    }

    // Checking the last word
    if (strcmp(words[count-1], a) == 0 || strcmp(words[count-1], o) == 0) {
        fprintf(stderr, "invalid query: cannot end with an operator\n");
        return false;
    }

    return true;
}

/******************* print_query ******************/
/*
Given an array of words, print_query prints all the words in a single line and returns the number of words in the array.

Takes:
    char** words - array of words
Returns:
    count - number of words in array
*/
int print_query(char** words) {
    printf("Query: ");

    int count = 0;
    while (words[count] != NULL) {
        printf("%s ", words[count]);
        count++;
    }
    printf("\n");

    return count;
}