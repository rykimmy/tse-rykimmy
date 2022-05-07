/*
index.c

Ryan Kim
CS50, 22S
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "../libcs50/hashtable.h"
#include "../libcs50/counters.h"
#include "../common/index.h"

typedef hashtable_t index_t;

/********************* index_new() ********************/
index_t *index_new(const int num_slots) {
    return (index_t*)hashtable_new(num_slots);
}

/******************* index_insert() ********************/
bool index_insert(index_t *index, const char *key, int id) {
    // Error checking --> needed when using hashtable_find because that returns NULL if key does not exist or if arguments are invalid; thus, confirming the parameter arguments helps confirm later that when hashtable_find returns NULL, it means there is no key and not that there was an issue with arguments
    if (index == NULL || key == NULL || id < 1) {
        fprintf(stderr, "invalid parameters for index_insert()\n");
        return false;
    }

    // If key does not yet exist in hashtable, add new key:val pair
    if (hashtable_find(index, key) == NULL) {
        counters_t *new = counters_new();
        assertp(new, "failed to create new counterset in index_insert()\n");
        counters_add(new, id);
        return hashtable_insert(index, key, new);
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

/******************* index_find() ********************/
counters_t *index_find(index_t *index, const char *key) {
    if (index == NULL || key == NULL) {
        fprintf(stderr, "index_find() failed: invalid parameters\n");
        return NULL;
    }

    // If returns NULL, means the key does not exist
    return hashtable_find(index, key);
}

/******************* index_delete() ********************/
void index_delete(index_t *index) {
    hashtable_delete((hashtable_t*)index, counters_delete);
    free(index);
}

/******************** index_load() ********************/
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
    size *= (1/0.7);

    index_t *index = index_new(size);
    if (index == NULL) {
        fprintf(stderr, "failed to create new index in index_load()\n");
        return NULL;
    }

    char* line;
    char* word;
    int id;
    int wordcount;

    // MIGHT GET UNDERCOUNT IF NO \N AT END
    while ((line = freadlinep(fp)) != EOF) {

        // Convert line into multiple words
        char *ptr = strtok(line, " ");
        while(ptr != NULL) {
            ptr = strtok(NULL, " ");
        }

        word = line[0];
        counters_t *counters = counters_new();

        int count = 1;
        while (line[count] != NULL) {
            id = atoi(line[count]);
            wordcount = atoi(line[count+1]);

            // id = line[count];
            // wordcount = line[count+1];

            counters_set(counters, id, wordcount);

            count += 2;
        }
        index_insert(index, word, counters);
        free(line);
    }

    fclose(fp);
    return index;
}

/******************* index_save() ********************/
bool index_save(index_t* index, char* index_file) {
    if (index == NULL || index_file == NULL) {
        fprintf(stderr, "invalid parameters in index_save()\n");
        return false;
    }

    FILE *fp = fopen(index_file, "w");
    hashtable_iterate((hashtable_t*)index, fp, save_item);
    fclose(fp);
}

/******************* HELPER FUNCTIONS ********************/
/******************* save_item() ********************/
void save_item(void* arg, const char* key, void* item) {
    FILE *fp = arg;
    counters_t *counters = item;

    fprintf(fp, "%s", key);
    counters_iterate(counters, fp, save_counts);
    fprintf(fp, "\n");
}

/******************* save_counts() ********************/
void save_counts(void* arg, const int key, int count) {
	FILE *fp = arg;
	fprintf(fp, "%d %d", key, count);
}

/*
Lab 5 - Class Notes
- The four main index APIs should be one line
- Casting

pagedir.c
- validate
- innit - opens it up
- save page
*/