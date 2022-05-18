/*
index.c - holds APIs for the index type

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

void save_counts(void* arg, const int key, const int count);
void save_item(void* arg, const char* key, void* item);
static void word_delete(void* item);

/********************* FUNCTIONS ********************/

/********************* index_new() ********************/
// see 'index.h' for details
index_t *index_new(const int num_slots) {
    return (index_t*)hashtable_new(num_slots);
}

/******************* index_find() ********************/
// see 'index.h' for details
counters_t *index_find(index_t *index, const char *key) {
    // Error checking --> included because then if hashtable_find returns NULL, we know it's because there's no key in the hashtable
    if (index == NULL || key == NULL) {
        fprintf(stderr, "index_find() failed: invalid parameters\n");
        return NULL;
    }

    // If returns NULL, means the key does not exist
    return (counters_t*)hashtable_find((hashtable_t*)index, key);
}

/******************* index_insert() ********************/
// see 'index.h' for details
bool index_insert(index_t *index, counters_t *counters, const char *key) {
    // Error checking --> needed when using hashtable_find because that returns NULL if key does not exist or if arguments are invalid; thus, confirming the parameter arguments helps confirm later that when hashtable_find returns NULL, it means there is no key and not that there was an issue with arguments
    if (index == NULL || key == NULL || counters == NULL) {
        fprintf(stderr, "invalid parameters for index_insert()\n");
        return false;
    }

    return hashtable_insert((hashtable_t*)index, key, counters);
}

/******************* index_delete() ********************/
// see 'index.h' for details
void index_delete(index_t *index) {
    hashtable_delete((hashtable_t*)index, word_delete);
}
static void
word_delete(void* item) {
    counters_t* counterset = item;

    if (counterset != NULL){
        counters_delete(item);
    }
}
/******************** index_load() ********************/
// see 'index.h' for details
index_t *index_load(char* index_file) {
    // Checking args
    if (index_file == NULL) {
        fprintf(stderr, "index_load() failed: invalid index_file\n");
        return NULL;
    }

    // Open given file
    FILE *fp = fopen(index_file, "r");
    if (fp == NULL) {
        fprintf(stderr, "index_load() failed: file pointer null\n");
        fclose(fp);
        return NULL;
    }

    // Create new index
    index_t *index = index_new(lines_in_file(fp));
    if (index == NULL) {
        fprintf(stderr, "failed to create new index in index_load()\n");
        return NULL;
    }

    char* word;
    int id;
    int wordcount;

    // Read the file, collect info for indexing, insert into index
    while ((word = freadwordp(fp)) != NULL) {

        counters_t *counters = counters_new();

        // Could also use fscanf()
        while (fscanf(fp, "%d %d", &id, &wordcount) == 2) {
            counters_set(counters, id, wordcount);
        }

        index_insert(index, counters, word);
        free(word);
    }

    // Cleanup
    fclose(fp);
    return index;
}

/******************* index_save() ********************/
// see 'index.h' for details
bool index_save(index_t* index, char* index_file) {
    if (index == NULL || index_file == NULL) {
        return false;
    }

    // Use helpers to iterate through counterset and print items correctly
    FILE *fp = fopen(index_file, "w");
    hashtable_iterate((hashtable_t*)index, fp, save_item);
    fclose(fp);
    return true;
}

/******************* HELPER FUNCTIONS ********************/

// Print the id:count; works with counters_iterate
/******************* save_counts() ********************/
void save_counts(void* arg, const int key, const int count) {
	FILE *fp = arg;
	fprintf(fp, "%d %d ", key, count);
}

// Prints the words and calls counters_iterate to print the id:count
/******************* save_item() ********************/
void save_item(void* arg, const char* key, void* item) {
    FILE *fp = arg;
    counters_t *counters = item;

    fprintf(fp, "%s ", key);
    counters_iterate(counters, fp, save_counts);
    fprintf(fp, "\n");
}