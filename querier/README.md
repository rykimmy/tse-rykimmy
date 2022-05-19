# CS50 Spring 2022, Lab 6: Querier
## Ryan Kim

### Assumptions

* Given index files will follow the correct format
* Given pageDirectory will follow the correct format (documents are named properly and each page is formatted properly); see REQUIREMENTS.md for more details
* Repeating the same words in the query will not handle the query differently and instead continues with the same functionality, with regards to scoring
* The user, when done with querying, will end the program with ctrl D
* Testing can be done without *fuzzquery* and with pipelined commands, as stated in the lab instructions

### Compiling

If executing 'querier' caller should:

1. `make clean; make` in the tse-xxxx directory
2. `cd querier`
3. `./querier pageDirectory indexFilename`

If running 'testing.sh' which runs test cases of indexer, caller should:

1. `make clean; make` in the tse-xxxx directory
2. `cd querier`
3. `make test`
4. output can be found in the testing.out file

### Functionality

I impemented the full-spec version of functionality in which my querier also prints the document set in decreasing order by score as well as takes into account precedence of operators.

### Note About Testing

I decided not to use *fuzzquery.c* and instead pipelined words into my querier within *testing.sh*. As my querier is made for a user to repetitively input queries, the output in testing.out includes prompts for the user to 'Enter Query: '. Such occurences make the testing output a little more difficult to read, but I opted to prioritize the main functionality/interface over the testing one.

More detailed information about testing can be found in the DESIGN spec.