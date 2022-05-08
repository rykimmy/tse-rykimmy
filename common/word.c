/*
word.c - holds APIs for handling words

Ryan Kim
CS50, 22S
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

/********************* normalize_word() ********************/
// See 'word.h' for details
char* normalize_word(char* word) {
    if (word != NULL) {
        return tolower(word);
    }
    return NULL;
}