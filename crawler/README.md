# CS50 Spring 2022, Lab 4
## Ryan Kim

### Assumptions

* User provides a pageDirectory that already exists within the 'crawler' directory or a path to the directory.

* The `pageDirectory` does not contain any files whose name is an integer (i.e., `1`, `2`, ...).

### Compiling

If executing 'crawler' caller should:

1. `make clean; make` in the tse-xxxx directory
2. `cd crawler`
3. run crawler (see crawler.c for usage)

If running 'testing.sh' which runs test cases of crawler, caller should:

1. `make clean; make` in the tse-xxxx directory
2. `cd crawler`
3. `make test`