/*
index.h

Ryan Kim
CS50, 22S
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "../libcs50/hashtable.h"
#include "../libcs50/counters.h"

typedef hashtable_t index_t;

index_t *index_new(const int num_slots);

bool index_insert(index_t *index, const char* key, int id);

counters_t *index_find(index_t *index, const char* key);

index_t *index_load(char* index_file);

void index_save(void* arg, const char* key, void* item);