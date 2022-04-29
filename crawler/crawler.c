/*
crawler.c – using a given seed URL, page directory, and max depth, crawls through pages to find other pages and documents the content

Input: command-line arguments
Output: outputs pages into new files; also outputs the progress of the crawler as well as any error messages

Usage: ./crawler seedURL pageDirectory maxDepth

Assumptions:
    pageDirectory already exists and is not created through crawler
    pageDirectory does not contain any existing files named with integers

Ryan Kim
CS50, 22S
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "../libcs50/bag.h"
#include "../libcs50/set.h"
#include "../libcs50/hashtable.h"
#include "../libcs50/webpage.h"
#include "../libcs50/memory.h"
#include "../common/pagedir.h"

void scan_page(webpage_t *webpage, bag_t *toVisit, hashtable_t *visited);
void crawler(char* seedURL, char* pageDirectory, const int maxDepth);
bool fetch_page(webpage_t *webpage, int id, char* pageDirectory);

int main(const int argc, char *argv[]) {
    // Checking – num arguments
    if (argc != 4) {
        fprintf(stderr, "invalid usage: ./crawler seedURL pageDirectory maxDepth\n");
        exit(1);
    }

    // Checking – seedURL
    char* seedURL = argv[1];
    if (!IsInternalURL(seedURL)) {
        fprintf(stderr, "invalid seedURL: make sure URL is valid and internal to our domain\n");
        exit(1);
    }

    // Checking – pageDirectory
    char* pageDirectory = calloc(strlen(argv[2]) + 1, sizeof(char));
    strcpy(pageDirectory, argv[2]);
    assertp(pageDirectory, "invalid pageDirectory\n");

    if (!check_dir(pageDirectory)) {
        fprintf(stderr, "invalid arg 3: page directory does not exist or is not writable\n");
        free(pageDirectory);
        exit(1);
    }

    // Checking – maxDepth
    int maxDepth;
    if (sscanf(argv[3], "%d", &maxDepth) != 1) {
        fprintf(stderr, "invalid arg 4: must be integer within range [0, 10]\n");
        exit(1);
    }
    if (maxDepth < 0 || maxDepth > 10) {
        fprintf(stderr, "invalid arg 4: integer must be in range [0, 10]\n");
        free(pageDirectory);
        exit(1);
    }

    // Crawling
    crawler(seedURL, pageDirectory, maxDepth);

    free(pageDirectory);
    return 0;
}

/**************** Helper Functions ****************/
/**************************************************/

// Scanning the page for urls
void scan_page(webpage_t *webpage, bag_t *toVisit, hashtable_t *visited) {
    int pos = 0;
    char* url;
    while ((url = webpage_getNextURL(webpage, &pos)) != NULL) {
        printf("Found: %s\n", url);

        // If not internal and valid
        if (!IsInternalURL(url)) {
            free(url);
            url = NULL;
        }
        // If internal and valid
        else {
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

// Fetches a webpage; returns true if successfully fetched, otherwise false
bool fetch_page(webpage_t *webpage, int id, char* pageDirectory) {
    if (webpage_fetch(webpage)) {
        printf("Fetched: %s\n", webpage_getURL(webpage));
        return true;
    }
    else {
        fprintf(stderr, "failed to fetch %s\n", webpage_getURL(webpage));
        webpage_delete(webpage);
        webpage = NULL;
        return false;
    }
}

// Crawler - crawls through pages to explore all urls
void crawler(char* seedURL, char* pageDirectory, const int maxDepth) {
    int size = 100;
    int id = 1;
    int status = 0;
    
    // Making copy of seedURL
    char* urlCopy = calloc(strlen(seedURL) + 1, sizeof(char));
    strcpy(urlCopy, seedURL);
    assertp(urlCopy, "failed to initialize url copy\n");

    // Initializing structs: bag (pages to visit), hashtable (visited pages), webpage (of seed)
    bag_t *toVisit = bag_new();
    hashtable_t *visited = hashtable_new(size);
    webpage_t *webpage = webpage_new(urlCopy, 0, NULL);

    // Error handling - making sure structs created successfully
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

    // Insert first page accordingly
    bag_insert(toVisit, webpage);
    hashtable_insert(visited, urlCopy, "");

    // Continuous crawling starting with seed
    webpage_t *wp;
    while ((wp = bag_extract(toVisit)) != NULL) {

        // Fetch page -> if page is not fetchable, move on to next url
        if (!fetch_page(wp, id, pageDirectory)) {
            continue;
        }

        // Create 'filename' of new page
        char* filename = calloc(strlen(pageDirectory) + 4, sizeof(char));
        sprintf(filename, "%s/%d", pageDirectory, id);
        assertp(filename, "failed to initialize filename of new url file\n");

        // Save page and update id
        save_page(wp, filename);
        id++;

        // Free 'filename' because don't need anymore
        free(filename);
        filename = NULL;

        // Scan for other pages, if not yet at max depth
        // use webpage_getDepth
        if (webpage_getDepth(wp) < maxDepth) {
            scan_page(wp, toVisit, visited);
        }
        webpage_delete(wp);
        wp = NULL;
    }

    bag_delete(toVisit, webpage_delete);
    hashtable_delete(visited, NULL);
}