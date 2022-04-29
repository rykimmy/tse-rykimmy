/* 
 * pagedir - functions related to pages (initialize, scan)
 * 
 * A 'page' refers to a webpage_t struct; the pagedir module offers functionality regarding pages.
 *
 * Ryan Kim
 * CS50, 22S
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
//#include "/thayerfs/home/f004qsx/cs50/labs/tse-rykimmy/libcs50/libcs50-given.a"
#include "/thayerfs/home/f004qsx/cs50/labs/tse-rykimmy/libcs50/webpage.h"
#include "/thayerfs/home/f004qsx/cs50/labs/tse-rykimmy/libcs50/hashtable.h"
#include "/thayerfs/home/f004qsx/cs50/labs/tse-rykimmy/libcs50/memory.h"

/**************** check_dir **************************/
/* If pointer fp is NULL, print error message to stderr and die,
 * otherwise, open new file "pageDirectory/.crawler"
 * We assume:
 *   filename is not null
 * We return:
 *   true if new file with given filename is opened
 *   false if filename is invalid or file was not opened properly
 * We exit if fp=NULL or filename=NULL, after printing message to stderr.
 */
bool check_dir(char* filename);

/**************** save_page **************************/
/* If pointer fp is NULL, print error message to stderr and die,
 * otherwise, open new file "pageDirectory/id" and print the url, depth, and html on separate lines
 * We assume:
 *   filename is not null
 * We return:
 *   nothing if success
 * We exit if fp=NULL or filename=NULL, after printing message to stderr.
 */
void save_page(webpage_t *webpage, char* filename);