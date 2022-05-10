/*
word.c - holds APIs for handling words

Ryan Kim
CS50, 22S
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/********************* normalize_word() ********************/
// See 'word.h' for details
char* normalize_word(char* word) {
    if (word == NULL) {
        fprintf(stderr, "normalize_word() failed: invalid word\n");
    }

    for (int i = 0; i < strlen(word); i++) {
        char ch = word[i];
        word[i] = tolower(ch);
    }
    return word;
}