/*
indextest.c - tests the index_load and index_save functions by reading an 'old' index file, creating an index, and writing the index into a new file

Usage: ./indextest oldindexFile newindexFile

Input: none other than command-line arguments
Output: prints the progress of index and indexer functions as well as any error messages

CS50, 22S
*/

#include <stdio.h>
#include <stdlib.h>
#include "../common/index.h"

int main(const int argc, char *argv[]) {
    int status = 0;

    if (argc != 3) {
        fprintf(stderr, "invalid number of arguments\nusage: ./indextest oldindexFile newindexFile\n");
        status++;
        return status;
    }

    char* oldindex = calloc(strlen(argv[1]) + 1, sizeof(char));
    char* newindex = calloc(strlen(argv[2]) + 1, sizeof(char));
    strcpy(oldindex, argv[1]);
    strcpy(newindex, argv[2]);
    assertp(oldindex, "failed to create oldindex\n");
    assertp(newindex, "failed to create newindex\n");

    index_t *index = index_load(oldindex);
    if (!index_save(index, newindex)) {
        fprintf(stderr, "failed to save index\n");
        status++;
        return status;
    }
    printf("Done\n");

    index_delete(index);
    free(oldindex);
    free(newindex);
    
    return status;
}