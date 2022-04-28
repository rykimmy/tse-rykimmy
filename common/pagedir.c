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

//thayerfs/home/f004qsx/cs50/labs

bool check_dir(char* filename) {
    
    FILE *fp = fopen(filename, "w");

    if (fopen(fp, "w") == NULL) {
        fclose(fp);
        free(filename);
        return false;
    }
    else {
        fclose(fp);
        free(filename);
        return true;
    }
    // CHECK - DO I HAVE TO FREE THE MEMORY HERE?
}

void save_page(webpage_t *webpage, int id, const char* pageDirectory) {
    char* filename = assertp(malloc(strlen(pageDirectory) + 2), "file name");
    sprintf(filename, "%s/%d", pageDirectory, id);
    FILE *fp = fopen(filename, "w");
    
    assertp(fp, "cannot open file for writing\n");
    fprintf(fp, "%s\n%d\n%s", webpage_getURL(webpage), webpage_getDepth(webpage), webpage_getHTML(webpage));

    // CHECK – format; if assertp exits, do we still need to close the file?
    // if (fp == NULL) {
    //     fprintf(stderr, "cannot open file for writing\n");
    //     fclose(fp);
    //     exit(1);
    // }
    // else {
    //     fprintf(fp, "%s\n%d\n%s", webpage_getURL(webpage), webpage_getDepth(webpage), webpage_getHTML(webpage));
    // }
    free()
	fclose(fp);
}