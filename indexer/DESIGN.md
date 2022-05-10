# TSE Indexer Design Spec

### User interface

The indexer's only interface with the user is on the command-line; it must always have 2 arguments.

```
indexer pageDirectory indexFilename
```

For example:

``` bash
$ ./indexer pages index_file
```

### Inputs and outputs

Input: the only inputs are command-line parameters; see the User Interface above.

Output: We save the index to a new file with proper format

Within a file, we write

 * unique words
 * for each unique word, the page it occurs in
 * for each page it occurs in, the number of times it occurs

### Functional decomposition into modules

We anticipate the following modules or functions:

 1. *main*, which parses arguments and initializes other modules
 2. *index_build*, which loops over pages to explore, until the list is exhausted
 3. *index_page*, which reads through a page and edits/adds to index

### Dataflow through modules

 1. *main* parses parameters and passes them to the crawler.
 2. *crawler* uses a bag to track pages to explore, and hashtable to track pages seen; when it explores a page it gives the page URL to the pagefetcher, then the result to pagesaver, then to the pagescanner.
 3. *pagefetcher* fetches the contents (HTML) for a page from a URL and returns.
 4. *pagesaver* outputs a page to the appropriate file.
 4. *pagescanner* extracts URLs from a page and returns one at a time.

### Major data structures

Three helper modules provide data structures:

 1. *hashtable* of words to countersets of their occurences
 2. *counters* to hold the id's of pages and the number of times the word occurred in said page