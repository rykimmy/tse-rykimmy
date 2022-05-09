/*
indexer.c – goes through all page files in pageDirectory and builds an index that is outputted to given indexFilename

Usage: ./indexer pageDirectory indexFilename

Input: nothing other than command-line arguments
Output: error messages

Ryan Kim
CS50, 22S
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../common/pagedir.h"
#include "../common/index.h"
#include "../common/word.h"

index_t *index_build(char* pageDirectory);
void index_page(webpage_t *page, index_t *index, int id);

int main(const int argc, char *argv[]) {
    int status = 0;

    // Checking number of arguments
    if (argc != 3) {
        fprintf(stderr, "invalid number of arguments\n");
    }

    // Checking pageDirectory
    char* pageDirectory = calloc(strlen(argv[1]) + 1, sizeof(char));
    strcpy(pageDirectory, argv[1]);
    assertp(pageDirectory, "invalid pageDirectory\n");
    
    if (!is_crawlerdir(pageDirectory)) {
        fprintf(stderr, "invalid arg 2: cannot find page directory\n");
        free(pageDirectory);
        status++;
        return status;
    }

    // Checking indexFilename
    char* indexFilename = calloc(strlen(argv[2]) + 1, sizeof(char));
    strcpy(indexFilename, argv[2]);
    assertp(indexFilename, "invalid indexFilename\n");

    // Building index
    index_t *index = index_build(pageDirectory);

    // Save index
    if (!index_save(index, indexFilename)) {
        fprintf(stderr, "index_save() failed\n");
        status++;
        return status;
    }

    // Clean up
    index_delete(index);
    free(indexFilename);
    free(pageDirectory);
    return status;
}

/*************** HELPER FUNCTIONS **************/

/**************** index_build() *****************/
// Goes through every page in pageDirectory and creates new index with all pages' content
index_t *index_build(char* pageDirectory) {
    // Initializing variables
    int id = 1;
    // CHECK - SIZE
    int size = 900;

    char* filename = calloc(strlen(pageDirectory) + 5, sizeof(char));
    sprintf(filename, "%s/%d", pageDirectory, id);
    assertp(filename, "failed to allocate memory for filename in index_build()\n");

    index_t *index = index_new(size);

    // For each page in pageDirectory
    FILE *fp;
    while ((fp = fopen(filename, "r")) != NULL) {

        // Create new webpage for each page file
        webpage_t *page = load_page(fp);

        // Go through each word in the page and add to index accordingly
        index_page(page, index, id);

        // Revise filename to open next 'id' file
        id++;
        sprintf(filename, "%s/%d", pageDirectory, id);

        // Clean up
        webpage_delete(page);
        fclose(fp);
    }
    free(filename);
    return index;
}

/**************** index_page() *****************/
// Goes through page and adds page data to index
void index_page(webpage_t *page, index_t *index, int id) {
    char* word;
    int pos = 0;

    while ((word = webpage_getNextWord(page, &pos)) != NULL) {
        if (normalize_word(word) == NULL) {
            fprintf(stderr, "normalize_word() failed\n");
        }
        if (strcmp(word, webpage_getURL(page)) == 0 || strcmp(word, webpage_getDepth(page)) == 0 || strlen(word) < 3) {
            continue;
        }
        index_insert(index, word, id);
        free(word);
    }
}