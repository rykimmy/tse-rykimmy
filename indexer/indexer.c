/*
indexer.c – 

Usage: ./indexer pageDirectory indexFilename

Input:
Output:

Ryan Kim
CS50, 22S
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <../common/pagedir.h>
#include <../common/index.h>

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
    
    // CHECK – do we change check_dir in pagedir.c to check if it has the "/.crawler" file?
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

    FILE *fp = fopen(indexFilename, "w");
    assertp(fp, "failed to open indexFilename\n");

    // The shit

    fclose(fp);
    free(pageDirectory);
    return status;
}

// Going through every page under pageDirectory
index_t *index_build(char* pageDirectory) {
    // Initializing variables
    int id = 1;
    int size = 900;

    char* filename = calloc(strlen(pageDirectory) + 5, sizeof(char));
    sprintf(filename, "%s/%d", pageDirectory, id);
    assertp(filename, "failed to allocate memory for filename in index_build()\n");

    index_t *index = index_new(size);

    // For each page in pageDirectory
    FILE *fp;
    while ((fp = fopen(filename, "r")) != NULL) {
        int pos = 0;
        char* word;

        // Create new webpage for each page file
        webpage_t *page = load_page(fp);

        // Go through each word and add to index
        while ((word = webpage_getNextWord(page, &pos)) != NULL) {
            // Skipping over first two lines (url and depth)
            if (strcmp(word, webpage_getURL(page)) == 0 || strcmp(word, webpage_getDepth(page)) == 0) {
                continue;
            }

            index_insert(index, word, id);
            free(word);
        }

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

// webpage_getNextWord()
// void index_page() {

// }