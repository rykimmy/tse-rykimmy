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

/******************* HELPER FUNCTIONS ********************/

/******************* save_counts() ********************/
void save_counts(void* arg, const int key, int count) {
	FILE *fp = arg;
	fprintf(fp, "%d %d", key, count);
}

/******************* save_item() ********************/
void save_item(void* arg, const char* key, void* item) {
    FILE *fp = arg;
    counters_t *counters = item;

    fprintf(fp, "%s", key);
    counters_iterate(counters, fp, save_counts);
    fprintf(fp, "\n");
}


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
bool index_insert(index_t *index, const char *key, int id) {
    // Error checking --> needed when using hashtable_find because that returns NULL if key does not exist or if arguments are invalid; thus, confirming the parameter arguments helps confirm later that when hashtable_find returns NULL, it means there is no key and not that there was an issue with arguments
    if (index == NULL || key == NULL || id < 1) {
        fprintf(stderr, "invalid parameters for index_insert()\n");
        return false;
    }

    // If key does not yet exist in hashtable, add new key with new counterset
    if (hashtable_find((hashtable_t*)index, key) == NULL) {
        counters_t *new = counters_new();
        assertp(new, "failed to create new counterset in index_insert()\n");
        counters_add(new, id);
        return hashtable_insert((hashtable_t*)index, key, new);
    }
    // If key already exists in hashtable, increment the counter set
    else {
        if (counters_add(index_find(index, key), id) == 0) {
            fprintf(stderr, "failed to increment counterset in index_insert()\n");
            return false;
        }
        return true;
    }
}

/******************* index_delete() ********************/
// see 'index.h' for details
void index_delete(index_t *index) {
    hashtable_delete((hashtable_t*)index, counters_delete);
}

/******************** index_load() ********************/
// see 'index.h' for details
index_t *index_load(char* index_file) {
    if (index_file == NULL) {
        fprintf(stderr, "index_load() failed: invalid index_file\n");
        return NULL;
    }

    FILE *fp = fopen(index_file, "w");
    if (fp == NULL) {
        fprintf(stderr, "index_load() failed: file pointer null\n");
        fclose(fp);
        return NULL;
    }

    int size = lines_in_file(fp);

    index_t *index = index_new(size);
    if (index == NULL) {
        fprintf(stderr, "failed to create new index in index_load()\n");
        return NULL;
    }

    char* word;
    int id;
    int wordcount;

    // MIGHT GET UNDERCOUNT IF NO \N AT END
    while ((word = freadwordp(fp)) != NULL) {

        counters_t *counters = counters_new();

        // Could also use fscanf()
        while (fscanf(fp, "%d %d", &id, &wordcount) == 2) {
            counters_set(counters, id, wordcount);
        }

        index_insert(index, word, counters);
        // CHECK - is this right if word is stored in index?
        free(word);
    }

    fclose(fp);
    return index;
}

/******************* index_save() ********************/
// see 'index.h' for details
bool index_save(index_t* index, char* index_file) {
    if (index == NULL || index_file == NULL) {
        return false;
    }

    FILE *fp = fopen(index_file, "w");
    hashtable_iterate((hashtable_t*)index, fp, save_item);
    fclose(fp);
    return true;
}

/*
To Do:
- Check size shit in index.c and indexer.c
- Makefiles
- Compile and run --> debug
- indextest
*/