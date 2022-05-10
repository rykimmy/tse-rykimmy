/*
pagedir.c – contains functions to be used by the crawler and indexer module

Ryan Kim
CS50, 22S
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "../libcs50/webpage.h"
#include "../libcs50/hashtable.h"
#include "../libcs50/memory.h"
#include "../libcs50/file.h"

/*********************** check_dir *************************/
// see 'pagedir.h' for details
bool check_dir(char* pageDirectory) {

    // Create filename in format: 'pageDirectory/.crawler'
    char* filename = calloc(strlen(pageDirectory) + strlen("/.crawler") + 1, sizeof(char));
    sprintf(filename, "%s/.crawler", pageDirectory);
    assertp(filename, "parseArgs(): invalid filename creation\n");

    // Open new file and react accordingly based on success
    FILE *fp;
    if ((fp = fopen(filename, "w")) == NULL) {
        fprintf(stderr, "check_dir(): cannot open file for writing\n");
        free(filename);
        return false;
    }

    fclose(fp);
    free(filename);
    return true;
}

/*********************** save_page *************************/
// see 'pagedir.h' for details
void save_page(webpage_t *webpage, char* filename) {
    FILE *fp = fopen(filename, "w");

    // If fp is null, exit
    if (fp == NULL) {
        fprintf(stderr, "save_page(): cannot open file for writing\n");
        fclose(fp);
        exit(1);
    }
    
    // Else, write to the new file
    printf("Saved to: %s\n", filename);
    fprintf(fp, "%s\n%d\n%s", webpage_getURL(webpage), webpage_getDepth(webpage), webpage_getHTML(webpage));
	fclose(fp);
}

/*********************** init_dir *************************/
// see 'pagedir.h' for details
bool is_crawlerdir(const char* pageDirectory) {
    // Create filename in format: 'pageDirectory/.crawler'
    char* filename = calloc(strlen(pageDirectory) + strlen("/.crawler") + 1, sizeof(char));
    sprintf(filename, "%s/.crawler", pageDirectory);
    assertp(filename, "is_crawlerdir(): invalid filename creation\n");

    // Open new file and react accordingly based on success
    FILE *fp;
    if ((fp = fopen(filename, "r")) == NULL) {
        fprintf(stderr, "is_crawlerdir(): invalid page directory\n");
        free(filename);
        return false;
    }

    fclose(fp);
    free(filename);
    return true;
}

/********************* load_page() ********************/
// see 'pagedir.h' for details
webpage_t* load_page(FILE *fp) {
    if (fp == NULL) {
        fprintf(stderr, "load_page() failed: null file pointer\n");
        return NULL;
    }

    // Collect necessary values to create webpage
    char* url = freadlinep(fp);
    char* depth = freadlinep(fp);
    char* html = freadfilep(fp);

    int d;
    sscanf(depth, "%d", &d);
    
    webpage_t *new = webpage_new(url, d, html);
    assertp(new, "load_page() failed: could not create new webpage\n");
    free(depth);
    return new;
}