/*

*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
//#include "/thayerfs/home/f004qsx/cs50/labs/tse-rykimmy/libcs50/libcs50-given.a"
#include "/thayerfs/home/f004qsx/cs50/labs/tse-rykimmy/libcs50/webpage.h"
#include "/thayerfs/home/f004qsx/cs50/labs/tse-rykimmy/libcs50/hashtable.h"
#include "/thayerfs/home/f004qsx/cs50/labs/tse-rykimmy/libcs50/memory.h"

bool check_dir(char* filename) {
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        fprintf(stderr, "check_dir(): cannot open file for writing\n");
        free(filename);
        fclose(fp);
        return false;
    }
    fclose(fp);
    free(filename);
    return true;
}

void save_page(webpage_t *webpage, char* filename) {
    FILE *fp = fopen(filename, "w");

    if (fp == NULL) {
        fprintf(stderr, "save_page(): cannot open file for writing\n");
        fclose(fp);
        exit(1);
    }
    
    fprintf(fp, "%s\n%d\n%s", webpage_getURL(webpage), webpage_getDepth(webpage), webpage_getHTML(webpage));
	fclose(fp);
}