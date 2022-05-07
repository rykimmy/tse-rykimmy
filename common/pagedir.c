/*
pagedir.c – contains functions to be used by the crawler module

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

/*********************** check_dir *************************/
// see pagedir.h for details
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
// see pagedir.h for details
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
// see pagedir.h for details
bool is_crawlerdir(const char* pageDirectory) {
    // Create filename in format: 'pageDirectory/.crawler'
    char* filename = calloc(strlen(pageDirectory) + strlen("/.crawler") + 1, sizeof(char));
    sprintf(filename, "%s/.crawler", pageDirectory);
    assertp(filename, "init_dir(): invalid filename creation\n");

    // Open new file and react accordingly based on success
    FILE *fp;
    if ((fp = fopen(filename, "w")) == NULL) {
        fprintf(stderr, "init_dir(): invalid page directory\n");
        fclose(fp);
        free(filename);
        return false;
    }

    fclose(fp);
    free(filename);
    return true;
}
// it reads page from file into a webpage structure, 
// and returns the webpage structure
webpage_t* load_page(FILE *fp) {
    if (fp == NULL) {
        fprintf(stderr, "load_page() failed: null file pointer\n");
        return NULL;
    }
    // CHECK – syntax; is this correct?
    webpage_t *new = webpage_new(freadlinep(fp), freadlinep(fp), freadfilep(fp));
    assertp(new, "load_page() failed: could not create new webpage\n");
    return new;
}