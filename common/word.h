/*
word.h - holds APIs for 'word' module

Ryan Kim
CS50, 22S
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

/********************* normalize_word() ********************/
/*
Turns a word into lower case

Takes:
    a word
Returns:
    the normalized word (turned into lower case)
*/
char* normalize_word(char* word);