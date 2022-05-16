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

/******************* Local Function Prototypes ******************/
// void spline(char** words, char* line);
char** split_line(char* line);
bool validate_query(char** words);
int print_query(char** words);
void counters_intersect(counters_t* ct1, counters_t* ct2);
void intersect_helper(void *arg, const int key, const int count);
void counters_union(counters_t* ct1, counters_t* ct2);
void union_helper(void* arg, const int key, const int count);

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
    
    index_t* index = index_load(indexFilename);

    //------------- Query -------------//
    printf("Enter Query:\n");
    char* input;

    // Repetitively get queries
    while ((input = freadlinep(stdin)) != NULL) {

        //------------- Checking/Setup of Query -------------//
        // Getting array of words
        char** words;
        words = split_line(input);

        // Error checking
        if (words == NULL) {
            printf("Enter Query:\n");
            free(words);
            free(input);
            continue;
        }

        // Validate query syntax (checking operators)
        if (!validate_query(words)) {
            printf("Enter Query:\n");
            free(words);
            free(input);
            continue;
        }

        // Print the query
        int num_words = print_query(words);
        printf("%d\n", num_words);

        //------------- The Shit -------------//

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
                continue;
            }

            // word:
            else {
                counters_t* counters = index_find(index, words[count]);

                // Updating counterset 'tmp'
                // CHECK – might need to include additional check to see if tmp is empty
                if (tmp == NULL) {
                    tmp = counters_new();
                    counters_union(tmp, counters);

                    // counters_print(tmp, stdout);
                    // fflush(stdout);
                }
                else {
                    counters_intersect(tmp, counters);

                    // counters_print(tmp, stdout);
                    // fflush(stdout);
                }
                counters_delete(counters);
            }
            count++;
        }

        // To avoid warnings
        if (tmp != NULL) {
            counters_union(result, tmp);
            counters_delete(tmp);
        }

        counters_print(result, stdout);
        printf("\n");

        // Cleanup
        free(words);
        free(input);
        counters_delete(result);

        printf("Enter Query:\n");
    }

    index_delete(index);
    free(pageDirectory);
    free(indexFilename);
    return status;
}

/******************* HELPER FUNCTIONS ******************/

/******************* counters_intersect() ******************/
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
    // CHECK – given a line with only one word or even a black line
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

