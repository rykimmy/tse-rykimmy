# Implementation

This document describes the implementation of the 'crawler' module.

### Pseudocode

The pseudocode for crawler follows as given by the Design spec, which can be seen below.

1. execute from a command line as shown in the User Interface
2. parse the command line, validate parameters, initialize other modules
3. make a *webpage* for the `seedURL`, marked with depth=0
4. add that page to the *bag* of webpages to crawl
5. add that URL to the *hashtable* of URLs seen
3. while there are more webpages to crawl,
	5. extract a webpage (URL,depth) item from the *bag* of webpages to be crawled,
	4. pause for at least one second,
	6. use *fetch_page* to retrieve the HTML for the page at that URL,
	5. use *save_page* to write the webpage to the `pageDirectory` with a unique document ID, as described in the Requirements.
	6. if the webpage depth is < `maxDepth`, explore the webpage to find links:
		7. use *scan_page* to parse the webpage to extract all its embedded URLs;
		7. for each extracted URL,
			8. 'normalize' the URL (see below)
			9. if that URL is not 'internal', ignore it;
			9. try to insert that URL into the *hashtable* of URLs seen
				10. if it was already in the table, do nothing;
				11. if it was added to the table,
					12. make a new *webpage* for that URL, at depth+1
					13. add the new webpage to the *bag* of webpages to be crawled

### Definition of detailed APIs, interfaces, function prototypes and their parameters

#### pagedir

*bool check_dir(char* pageDirectory)*

Checks if given pageDirectory is valid and writable.

*void save_page(webpage_t *webpage, char* filename)*

Saves crawled pages by printing their contents to new files in pageDirectory.

#### crawler

*void scan_page(webpage_t *webpage, bag_t *toVisit, hashtable_t *visited)*

Scans a given page to find urls and processes them.

*bool fetch_page(webpage_t *webpage, int id, char* pageDirectory)*

Fetches a webpage.

*void crawler(char* seedURL, char* pageDirectory, const int maxDepth)*

Main code involving initialization of structs and crawling of webpages.

### Data structures

*webpage_t* struct that represents a single webpage.

*bag_t* struct that represents a bag, which is used to hold the webpages to crawl.

*hashtable_t* struct that represents a hashtable, which is used to hold the urls that have already been crawled.

### Error handling and recovery

* The program will exit on any of the following cases:
    * out of memory
    * invalid command-line arguments
    * unable to create or write to a file of form `pageDirectory/id`
* Failure to fetch prints an error message but continues with the program

### Testing plan

Testing is done through 'testing.sh' which runs a number of different test cases that are listed below. Each test case runs the crawler executable with a different set of command-line arguments that demonstrate various usages.

0. Test the program with various forms of incorrect command-line arguments to ensure that its command-line parsing, and validation of those parameters, works correctly.

1. Test the crawler with a seedURL that points to a non-existent server.

2. Test the crawler with a seedURL that points to a non-internal server.

3. Test the crawler with a seedURL that points to a valid server but non-existent page.

4. Crawl a simple, closed set of cross-linked web pages. Ensure that some page(s) are mentioned multiple times within a page, and multiple times across the set of pages. Ensure there is a loop (a cycle in the graph of pages). In such a little site, you know exactly what set of pages should be crawled, at what depths, and you know where your program might trip up.

5. Point the crawler at a page in that site, and explore at depths 0, 1, 2, 3, 4, 5. Verify that the files created match expectations.

6. Repeat with a different seed page in that same site. If the site is indeed a graph, with cycles, there should be several interesting starting points.

7. Point the crawler at our Wikipedia playground. Explore at depths 0, 1, 2. (It takes a long time to run at depth 2 or higher!) Verify that the files created match expectations.

8. When you are confident that your crawler runs well, test it on a part of our playground or with a greater depth - but be ready to kill it if it seems to be running amok.