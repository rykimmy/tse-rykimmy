/*
crawler.c – 

Input: 
Output: 

Usage: ./crawler seedURL pageDirectory maxDepth

Assumptions:

Ryan Kim
CS50, 22S
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
//#include "thayerfs/home/f004qsx/cs50/labs/tse-rykimmy/libcs50/libcs50-given.a"
//#include <../libcs50/libcs50-given.a>
#include "/thayerfs/home/f004qsx/cs50/labs/tse-rykimmy/libcs50/bag.h"
#include "/thayerfs/home/f004qsx/cs50/labs/tse-rykimmy/libcs50/set.h"
#include "/thayerfs/home/f004qsx/cs50/labs/tse-rykimmy/libcs50/hashtable.h"
#include "/thayerfs/home/f004qsx/cs50/labs/tse-rykimmy/libcs50/webpage.h"
#include "/thayerfs/home/f004qsx/cs50/labs/tse-rykimmy/libcs50/memory.h"
#include "../common/pagedir.h"

// edit makefile to not have to include everything
/*
Under clfags, add -I[path]

*/

static void itemdelete(void *item);
void scan_page(webpage_t *webpage, bag_t *toVisit, hashtable_t *visited);
void crawler(char* seedURL, char* pageDirectory, const int maxDepth);
void parseArgs(int numArgs, char* seedURL, char* pageDirectory, int maxDepth);

int main(const int argc, char *argv[]) {
    // seedURL
    char* seedURL = argv[1];

    // pageDirectory
    //char* pageDirectory = malloc(strlen(argv[2]) + 1);

    char* pageDirectory = calloc(strlen(argv[2]) + 1, sizeof(char));
    strcpy(pageDirectory, argv[2]);
    assertp(pageDirectory, "invalid pageDirectory\n");

    // maxDepth
    int maxDepth;
    if (sscanf(argv[3], "%d", &maxDepth) != 1) {
        fprintf(stderr, "invalid arg 4: must be integer within range [0, 10]\n");
        exit(1);
    }

    // Error checking
    parseArgs(argc, seedURL, pageDirectory, maxDepth);

    // Crawling
    crawler(seedURL, pageDirectory, maxDepth);

    free(pageDirectory);
    return 0;
}

/**************** Helper Functions ****************/
/**************************************************/

// Parsing arguments
void parseArgs(int numArgs, char* seedURL, char* pageDirectory, int maxDepth) {
    // Checking – num arguments
    if (numArgs != 4) {
        fprintf(stderr, "invalid usage: ./crawler seedURL pageDirectory maxDepth\n");
        // CHECK – do I have to do this? if so apply to all
        free(pageDirectory);
        exit(1);
    }

    // Checking – url validity
    if (!IsInternalURL(seedURL)) {
        fprintf(stderr, "invalid seedURL: make sure URL is valid and internal to our domain\n");
        free(pageDirectory);
        exit(1);
    }
    
    // Checking – page directory validity
    //char* filename = malloc(strlen(pageDirectory) + strlen("/.crawler") + 1);
    char* filename = calloc(strlen(pageDirectory) + strlen("/.crawler") + 1, sizeof(char));
    sprintf(filename, "%s/.crawler", pageDirectory);
    assertp(filename, "parseArgs(): invalid filename creation\n");
    // strcpy(filename, pageDirectory);
    // strcat(filename, "/.crawler");
    
    if (!check_dir(filename)) {
        fprintf(stderr, "invalid arg 3: page directory does not exist or is not writable\n");
        free(filename);
        free(pageDirectory);
        exit(1);
    }

    // Checking – max depth validity
    if (maxDepth < 0 || maxDepth > 10) {
        fprintf(stderr, "invalid arg 4: integer must be in range [0, 10]\n");
        free(filename);
        free(pageDirectory);
        exit(1);
    }
}

// Scanning the page for urls
void scan_page(webpage_t *webpage, bag_t *toVisit, hashtable_t *visited) {
    int pos = 0;
    char* url;
    while ((url = webpage_getNextURL(webpage, &pos)) != NULL) {
        printf("Found: %s\n", url);
        if (!IsInternalURL(url)) {
            free(url);
            url = NULL;
        }
        else {
            // printf("%s", url);
            // printf("%s\n", webpage_getURL(webpage));
            if (hashtable_insert(visited, url, "")) {
                webpage_t *new = webpage_new(url, webpage_getDepth(webpage) + 1, NULL);
                assertp(new, "failed to initialize new webpage within scan_page()\n");
                bag_insert(toVisit, new);
            }
            else {
                free(url);
                url = NULL;
            }
        }
    }
}

// Fetches a webpage
void fetch_page(webpage_t *webpage, int id, char* pageDirectory) {
    if (webpage_fetch(webpage)) {
        return;
    }
    else {
        fprintf(stderr, "failed to fetch %s\n", webpage_getURL(webpage));
        webpage_delete(webpage);
        webpage = NULL;
        return;
    }
}

// Crawler
void crawler(char* seedURL, char* pageDirectory, const int maxDepth) {
    int size = 100;
    int currDepth = 0;
    int id = 1;
    int status = 0;
    
    char* urlCopy = calloc(strlen(seedURL) + 1, sizeof(char));
    strcpy(urlCopy, seedURL);
    assertp(urlCopy, "failed to initialize url copy\n");

    bag_t *toVisit = bag_new();
    hashtable_t *visited = hashtable_new(size);
    webpage_t *webpage = webpage_new(urlCopy, currDepth, NULL);

    if (toVisit == NULL) {
        fprintf(stderr, "bag_new() failed\n");
        status++;
        exit(status);
    }
    if (visited == NULL) {
        fprintf(stderr, "hashtable_new() failed\n");
        status++;
        exit(status);
    }
    if (webpage == NULL) {
        fprintf(stderr, "failed to initialize new webpage (first)\n");
        status++;
        exit(status);
    }

    bag_insert(toVisit, webpage);
    hashtable_insert(visited, urlCopy, "");

    // The Shit
    webpage_t *wp;
    while ((wp = bag_extract(toVisit)) != NULL) {
        // Fetch page
        fetch_page(wp, id, pageDirectory);
        printf("Fetched: %s\n", webpage_getURL(wp));
        fflush(stdout);

        // Create 'filename' of new page
        char* filename = calloc(strlen(pageDirectory) + 3, sizeof(char));
        sprintf(filename, "%s/%d", pageDirectory, id);
        assertp(filename, "failed to initialize filename of new url file\n");

        // Save page and update id
        save_page(wp, filename);
        id++;

        printf("Saved to: %s\n", filename);
        fflush(stdout);

        // Free 'filename' because don't need anymore
        free(filename);
        filename = NULL;

        if (currDepth < maxDepth) {
            scan_page(wp, toVisit, visited);
            currDepth++;
        }
        webpage_delete(wp);
        wp = NULL;
    }

    bag_delete(toVisit, itemdelete);
    hashtable_delete(visited, itemdelete);
    /*
    What gets used by hashtable
    - urlCopy
    - url in scan_page() --> gets freed by the function or is used by hashtable
    */
    //printf("everything deleted\n");
    fflush(stdout);
}

//Deletes (frees) non-null items
static void itemdelete(void *item) {
    if (item != NULL) {
        free(item);
        item = NULL;
    }
}

/*
To Do
- Seems to work but hashtable_delete() throws an error
- Some memory leaks

- Testing: unit testing, integration testing
- testing.sh
- Makefile to clean created pages
*/