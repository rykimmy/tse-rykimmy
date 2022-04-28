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
#include "/thayerfs/home/f004qsx/cs50/labs/tse-rykimmy/libcs50/hashtable.h"
#include "/thayerfs/home/f004qsx/cs50/labs/tse-rykimmy/libcs50/webpage.h"
#include "/thayerfs/home/f004qsx/cs50/labs/tse-rykimmy/libcs50/memory.h"

void itemdelete(void *item);
void scan_page(webpage_t *webpage, bag_t *toVisit, hashtable_t *visited);
void crawler(char* seedURL, char* pageDirectory, const int maxDepth);
void parseArgs(int numArgs, char** seedURL, char* pageDirectory, int maxDepth);

int main(const int argc, const char *argv[]) {

    char** seedURL = argv[1];
    // HAVE TO CHANGE
    char* pageDirectory = assertp(malloc(strlen(argv[2]) + 1), argv[2]);
    int maxDepth = argv[3];

    // Error checking
    parseArgs(argc, seedURL, pageDirectory, maxDepth);

    // Crawling
    crawler(seedURL, pageDirectory, maxDepth);

    return 0;
}

/**************** Helper Functions ****************/
/**************************************************/

// Parsing arguments
void parseArgs(int numArgs, char** seedURL, char* pageDirectory, int maxDepth) {
    // Checking – num arguments
    if (numArgs != 4) {
        fprintf(stderr, "invalid usage: ./crawler seedURL pageDirectory maxDepth\n");
        exit(1);
    }

    // Checking – url validity
    if (!IsInternalURL(seedURL)) {
        fprintf(stderr, "invalid seedURL: make sure URL is valid and internal to our domain\n");
        exit(1);
    }
    
    // Checking – page directory validity
    // CHECK – syntax
    char* filename = assertp(malloc(strlen(pageDirectory) + strlen("/.crawler") + 2), "file name");
    sprintf(filename, "%s/.crawler", pageDirectory);
    // char* filename = malloc(strlen(pageDirectory) + strlen("/.crawler") + 2);
    // strcpy(filename, pageDirectory);
    // strcat(filename, "/.crawler");
    if (!check_dir(filename)) {
        fprintf(stderr, "invalid arg 3: page directory does not exist or is not writable\n");
        exit(1);
    }

    // Checking – max depth validity
    if (maxDepth < 0 || maxDepth > 10) {
        fprintf(stderr, "invalid arg 4: integer must be in range [0, 10]\n");
        exit(1);
    }
}

// Scanning the page for urls
void scan_page(webpage_t *webpage, bag_t *toVisit, hashtable_t *visited) {
    int pos = 0;
    char* url;
    while ((url = webpage_getNextURL(webpage, &pos)) != NULL) {
        if (IsInternalURL(url)) {
            if (hashtable_insert(visited, url, "")) {
                webpage_t *new = webpage_new(url, webpage_getDepth(webpage) + 1, NULL);
                new = assertp(malloc(sizeof(new)), "new page");
                bag_insert(toVisit, new);
            }
        }
        free(url);
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
        //status++;
    }
    
    return;
}

// Crawler
void crawler(char* seedURL, char* pageDirectory, const int maxDepth) {
    int size = 10;
    int currDepth = 0;
    int id = 1;
    int status = 0;
    
    char* urlCopy = malloc(strlen(seedURL) + 1);
    strcpy(urlCopy, seedURL);
    assertp(malloc(strlen(seedURL) + 1), urlCopy);

    bag_t *toVisit = bag_new();
    hashtable_t *visited = hashtable_new(size);
    // assertp(sizeof(visited), visited);
    // assertp(sizeof(toVisit), toVisit);
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

    webpage_t *webpage = webpage_new(urlCopy, currDepth, NULL);
    assertp(sizeof(webpage), "webpage");

    bag_insert(toVisit, webpage);

    // The Shit
    while (toVisit != NULL) {
        webpage_t *wp = bag_extract(toVisit);
        //sleep(1);

        // Fetch + save page
        fetch_page(wp, id, pageDirectory);
        save_page(wp, id, pageDirectory);
        id++;

        if (currDepth < maxDepth) {
            scan_page(wp, toVisit, visited);
            currDepth++;
        }
        webpage_delete(wp);
    }

    hashtable_delete(visited, itemdelete);
    bag_delete(toVisit, itemdelete);
    exit(status);
}

// Deletes (frees) non-null items
void itemdelete(void *item) {
    if (item != NULL) {
        free(item);
    }
}