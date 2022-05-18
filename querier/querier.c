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
struct twocts {
    counters_t* result;
    counters_t* append;
};

struct document {
    int id;
    int score;
};

struct documentlist {
    int size;
    int pos;
    struct document** docs;
};

/******************* Local Function Prototypes ******************/
// void spline(char** words, char* line);
char** split_line(char* line);
bool validate_query(char** words);
int print_query(char** words);
void counters_intersect(counters_t* ct1, counters_t* ct2);
void intersect_helper(void *arg, const int key, const int count);
void counters_union(counters_t* ct1, counters_t* ct2);
void union_helper(void* arg, const int key, const int count);
void querier_delete(char** words, int num_words);
void itemcount(void* arg, const int key, const int count);
void sort_array(struct documentlist* list, int size);
void fill_array(void* arg, const int key, const int count);
int comparefunc(const void* a, const void* b);
void print_array(struct documentlist* glist, int size, char* pageDirectory);
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
    printf("Enter Query: ");
    char* input;
    bool correct = true; // Works as an exit method to break out of while loop

    // Repetitively get queries
    while ((input = freadlinep(stdin)) != NULL) {

        //------------- Checking/Setup of Query -------------//
        // Getting array of words
        char** words;
        words = split_line(input);

        // Error checking
        if (words == NULL) {
            printf("Enter Query: ");
            free(words);
            free(input);
            continue;
        }

        // Validate query syntax (checking operators)
        if (!validate_query(words)) {
            printf("Enter Query: ");
            free(words);
            free(input);
            continue;
        }

        // Print the query
        int num_words = print_query(words);

        //------------- Querying -------------//

        counters_t *result = counters_new();
        counters_t *tmp = NULL;
        char* a = "and";
        char* o = "or";

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
                if (counters == NULL) {
                    fprintf(stderr, "invalid word: %s not found in index\n", words[count]);
                    counters_delete(tmp);
                    counters_delete(result);
                    correct = false;
                    break;
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
        if (!correct) {
            printf("Enter Query: ");
            correct = true;

            querier_delete(words, num_words);
            free(input);
            continue;
        }

        // To avoid warnings
        if (tmp != NULL) {
            counters_union(result, tmp);
            counters_delete(tmp);
        }

        counters_print(result, stdout);
        printf("\n");

        //------------- Ranking/Sorting Scores -------------//
        
        // Get size of counterset
        int num_items = 0;
        counters_iterate(result, &num_items, itemcount);

        // Create struct to hold array of documents (id, score)
        struct document** d = calloc(num_items, sizeof(struct document));
        struct documentlist list = {num_items, 0, d};
        
        // Iterate through counter and fill document array with counterset data
        counters_iterate(result, &list, fill_array);

        // Sort the list using comparator function
        qsort(d, num_items, sizeof(struct document), comparefunc);

        print_array(&list, num_items, pageDirectory);

        delete_array(&list, num_items);

        // Cleanup
        querier_delete(words, num_words);
        free(input);
        counters_delete(result);

        printf("Enter Query: ");
    }

    index_delete(index);
    free(pageDirectory);
    free(indexFilename);
    return status;
}

/******************* HELPER FUNCTIONS ******************/
/******************* delete_array() ******************/
void delete_array(struct documentlist* glist, int size) {
    struct documentlist* list = glist;

    for (int i = 0; i < size; i++) {
        if (list->docs[i] != NULL) {
            free(list->docs[i]);
        }
    }
    free(list->docs);
}
/******************* print_array() ******************/
void print_array(struct documentlist* glist, int size, char* pageDirectory) {
    struct documentlist* list = glist;
    struct document** array = list->docs;

    for (int i = 0; i < size; i++) {
        char* filename = calloc(strlen(pageDirectory) + 3, sizeof(char));
        sprintf(filename, "%s/%d", pageDirectory, array[i]->id);
        assertp(filename, "print_array() failed: invalid filename creation\n");

        FILE* fp = fopen(filename, "r");
        if (fp == NULL) {
            fprintf(stderr, "print_array() failed: cannot access files in pageDirectory\n");
            return;
            // SHOULD RETURN SMTH AND THEN PROMPT FOR A NEW QUERY
        }
        char* url = freadlinep(fp);

        printf("DocID: %d Score: %d URL: %s\n", array[i]->id, array[i]->score, url);

        fclose(fp);
        free(url);
        free(filename);
    }
    printf("\n");
}

/******************* sort_array() ******************/
void sort_array(struct documentlist* glist, int size) {
    if (size < 2) {
        return;
    }

    struct documentlist* list = glist;
    struct document** array = list->docs;
    struct document* temp;
    for (int i = 0; i < size; i++) {
        printf("%d %d\n", array[i]->id, array[i]->score);
    }

    for (int i = 1; i < size; i++) {
        int val = array[i]->score;
        int j = i - 1;

        while (j >= 0 && array[j]->score < val) {
            // array[j+1] = array[j];
            array[j+1]->id = array[j]->id;
            array[j+1]->score = array[j]->score;

            // printf("%d %d\n", array[j]->id, array[j]->score);

            j = j - 1;
        }
        // array[j+1] = array[i];
        array[j+1]->id = array[i]->id;
        array[j+1]->score = array[i]->score;
    }
}

/******************* comparefunc() ******************/
int comparefunc(const void* a, const void* b) {
    struct document* docA = *(struct document**)a;
    struct document* docB = *(struct document**)b;
    return (docB->score - docA->score);
}

/******************* fill_array() ******************/
void fill_array(void* arg, const int key, const int count) {
    struct documentlist* list = arg;

    if (list != NULL && key > 0 && count > 0) {
        struct document *pair = malloc(sizeof(struct document));
        pair->id = key;
        pair->score = count;

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

void union_helper(void* arg, const int key, const int count) {
    struct twocts* two = arg;
    counters_set(two->result, key, counters_get(two->result, key) + count);
}

/******************* split_line() ******************/
/*
Given a string containing at least one word, split_line splits the line into multiple words and puts them all into an array of words, which is returned back to the caller.

Takes:
    char* line - a string of words
Returns:
    char** words - an array of words from 'line'
Assumes:
    char* line holds at least one word
*/
char** split_line(char* line) {
    char** words = calloc((strlen(line)+1) / 2, sizeof(char*));
    char* word = strtok(line, " ");
    int count = 0;

    while (word != NULL) {
        // Error Checking -> checking each char to see if it is a valid char
        for (int i = 0; i < strlen(word); i++) {
            char ch = word[i];
            if (isalpha(ch) == 0 && isspace(ch) == 0) {
                fprintf(stderr, "invalid query: character '%c' is unacceptable\n", ch);
                return NULL;
            }
        }

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
    int count = 0;
    while (words[count] != NULL) {
        printf("%s ", words[count]);
        count++;
    }
    printf("\n");
    return count;
}

/******************* spline ******************/
/*
Given a string of multiple words, spline breaks the string into its constituent words by looking at individual characters and setting null characters accordingly. The function also does error checking to ensure that the given string does not include invalid characters (non alphabet or space).

Takes:
    a string that will be decomposed into words
Returns:
    a pointer to an array of words
Exits:
    if we find an invalid character (non alphabet or space)
*/

// void spline(char** words, char* line) {
//     /*
//     How to call in main:

//     //char** words = malloc(((strlen(input) + 1)/2) * sizeof(char*));
//     //spline(words, input);
//     */
//     bool word = false;
//     int count = 0;

//     for (int i = 0; i < strlen(line); i++) {

//         // Error Checking -> exit if not alphabet or white space
//         if (isalpha(line[i]) == 0 && isspace(line[i]) == 0) {
//             fprintf(stderr, "invalid query: character '%c' is unacceptable\n", line[i]);
//             exit(1);
//         }

//         // If not in a word...
//         if (!word) {

//             // If char is a letter (signalling beginning of word)
//             if (isalpha(line[i]) != 0) {

//                 // Set the address of the next word in words array
//                 //words[count] = malloc(sizeof(char*));
//                 words[count] = &line[i];

//                 // Set word to true to mark we are in a word
//                 word = true;
//             }
//         }

//         // If already in a word...
//         else if (word) {

//             // If char is white-space (signalling end of word)
//             if (isspace(line[i]) != 0) {

//                 // Set white-space char to null character
//                 line[i] = '\0';

//                 // Set word to false to mark we are not in a word
//                 word = false;

//                 // Increment count to move onto next word in words
//                 count++;
//             }
//         }
//     }
//     printf("%s\n", words[1]);
// }

/*
char* start;
start = line[i];

This is if we have to allocate memory for each word:

use a char pointer to hold the start of the word
when the word is done, add the null character
malloc for the word[]
strcpy the char* start pointer to the word[]
*/