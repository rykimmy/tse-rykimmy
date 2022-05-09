/*
index.h - holds APIs for the index type

An index is a hashtable, as defined in hashtable.h

Ryan Kim
CS50, 22S
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "../libcs50/hashtable.h"
#include "../libcs50/counters.h"
#include "../libcs50/file.h"
#include "../libcs50/memory.h"
#include "../libcs50/webpage.h"

typedef hashtable_t index_t;

/******************* FUNCTIONS ********************/

/******************* index_new() ********************/
/*
Creates a new index

Takes:
    number of slots (size) of the index (hashtable)
Returns:
    a new index that the caller is responsible for freeing with 'index_delete'
Assumptions/Expectations:
    num_slots should be greater than 1
*/
index_t *index_new(const int num_slots);

/******************* index_insert() ********************/
/*
Inserts into an index; if the key already exists, increments the counterset item associated with the key and id

Takes:
    index; key; id
Returns:
    true, if successfully inserted into index
    false otherwise
*/
bool index_insert(index_t *index, const char* key, int id);

/******************* index_find() ********************/
/*
Looks through the given index and returns the counterset associated with the given key, if it exists

Takes:
    index; key
Returns:
    counterset of the associated key
    NULL, if invalid parameters, or if key does not exist in the index
*/
counters_t *index_find(index_t *index, const char *key);

/******************* index_delete() ********************/
/*
Deletes an index and its content

Takes:
    index to be deleted
Returns:
    nothing
*/
void index_delete(index_t *index);

/******************* index_load() ********************/
/*
Reads an index_file which is in the correct index format and creates an index

Takes:
    index_file - a name to a file that contains index data in the correct format
Returns:
    an index struct that contains all the data in the index_file
Assumptions/Expectations:
    size is the number of lines in the file (number of unique words)
*/
index_t *index_load(char* index_file);

/******************* index_save() ********************/
/*
Given an index and a file, write the index data into the file following the correct format; the index_file does not need to exist but needs to be writable; it will be overwritten upon call

Takes:
    index to collect data
    index_file to output index data
Returns:
    true, if successfully wrote into index_file
    false otherwise
Assumptions/Expectations:
    Uses helpers to print into file
*/
bool index_save(index_t *index, char* index_file);