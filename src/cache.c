#include "cache.h"
#include "trace.h"

int write_xactions = 0;
int read_xactions = 0;

/*
   Print help message to user
 */
void printHelp(const char * prog) {
	printf("%s [-h] | [-s <size>] [-w <ways>] [-l <line>] [-t <trace>]\n", prog);
	printf("options:\n");
	printf("-h: print out help text\n");
	printf("-s <cache size>: set the total size of the cache in KB\n");
	printf("-w <ways>: set the number of ways in each set\n");
	printf("-l <line size>: set the size of each cache line in bytes\n");
	printf("-t <trace>: use <trace> as the input file for memory traces\n");
	/* EXTRA CREDIT
	   printf("-lru: use LRU replacement policy instead of FIFO\n");
	 */
}
/*
   Main function. Feed to options to set the cache

Options:
-h : print out help message
-s : set L1 cache Size (KB)
-w : set L1 cache ways
-l : set L1 cache line size
 */
int main(int argc, char* argv[])
{
	int i;
	uint32_t size = 32; //total size of L1$ (KB)
	uint32_t ways = 1; //# of ways in L1. Default to direct-mapped
	uint32_t line = 32; //line size (B)

	// hit and miss counts
	int totalHits = 0;
	int totalMisses = 0;

	char * filename;
	char * outputFile;



	//strings to compare
	const char helpString[] = "-h";
	const char sizeString[] = "-s";
	const char waysString[] = "-w";
	const char lineString[] = "-l";
	const char traceString[] = "-t";
	const char lruString[] = "-lru";

	if (argc == 1) {
		// No arguments passed, show help
		printHelp(argv[0]);
		return 1;
	}

	//parse command line
	for(i = 1; i < argc; i++)
	{
		//check for help
		if(!strcmp(helpString, argv[i]))
		{
			//print out help text and terminate
			printHelp(argv[0]);
			return 1; //return 1 for help termination
		}
		//check for size
		else if(!strcmp(sizeString, argv[i]))
		{
			//take next string and convert to int
			i++; //increment i so that it skips data string in the next loop iteration
			//check next string's first char. If not digit, fail
			if(isdigit(argv[i][0]))
			{
				size = atoi(argv[i]);
			}
			else
			{
				printf("Incorrect formatting of size value\n");
				return -1; //input failure
			}
		}
		//check for ways
		else if(!strcmp(waysString, argv[i]))
		{
			//take next string and convert to int
			i++; //increment i so that it skips data string in the next loop iteration
			//check next string's first char. If not digit, fail
			if(isdigit(argv[i][0]))
			{
				ways = atoi(argv[i]);
			}
			else
			{
				printf("Incorrect formatting of ways value\n");
				return -1; //input failure
			}
		}
		//check for line size
		else if(!strcmp(lineString, argv[i]))
		{
			//take next string and convert to int
			i++; //increment i so that it skips data string in the next loop iteration
			//check next string's first char. If not digit, fail
			if(isdigit(argv[i][0]))
			{
				line = atoi(argv[i]);
			}
			else
			{
				printf("Incorrect formatting of line size value\n");
				return -1; //input failure
			}
		}
		else if (!strcmp(traceString, argv[i])) {
			filename = argv[++i];
		}
		else if (!strcmp(lruString, argv[i])) {
			// Extra Credit: Implement Me!
			printf("Unrecognized argument. Exiting.\n");
			return -1;
		}
		//unrecognized input
		else{
			printf("Unrecognized argument. Exiting.\n");
			return -1;
		}
	}

	// Do the calculation for tag bits, index bits, offset bits and sets
	int offsetBit = logtwo(line);
	int sets = (1024*size)/(line*ways);
	int indexBit = logtwo(sets);
	int tagBit = 32-(indexBit+offsetBit);
	int tagBit1 = 32 - offsetBit;
	// printf("offset: %d\n", offsetBit);
	// printf("sets: %d\n", sets);
	// printf("indexBit: %d\n", indexBit);
	// printf("tagBit: %d\n", tagBit);



	/* TODO: Probably should intitalize the cache */
	struct cache* cache = newCache(ways, sets);
	struct cache* cache1 = newCache(ways * sets, 1); // Run a fully associative cache
	printf("Ways: %u; Sets: %u; Line Size: %uB\n", ways, sets, line);
	printf("Tag: %d bits; Index: %d bits; Offset: %d bits\n", tagBit, indexBit, offsetBit);
	/* TODO: Now we read the trace file line by line */
	FILE *fp;
	FILE *fp1;
	char type;
	uint32_t address;

	// open trace file for reading
	fp = fopen(filename, "r");
	outputFile=concat(filename, ".simulated");

	fp1 = fopen(outputFile, "w+");
	int alpha;
	//while (fscanf(f, "   %s   ", &etc) != EOF)
	while (fscanf(fp, "%s %x", &type, &address) != EOF) {
		// while(!feof(fp)){
		// while((alpha = fgetc(fp)) != EOF){
		// read 1 line of trace file
		// fscanf(fp, "%c %x", &type, &address);

		int a = 32 - offsetBit;
		uint32_t offset = (address << a) >> a;
		int c = offsetBit + indexBit;
		int b = 32 - c;
		uint32_t index = (address << b) >> (32 - indexBit);
		uint32_t tag = address >> c;
		uint32_t tag1 = address >> offsetBit;

		/*
0: hit
1: compulsory miss
2: capacity miss
3: conflict miss
		 */
		int missType = -1;

		if(indexBit == 0){
			missType = fetch(cache, type, 0, tag, ways, tag1, 1);
			if(missType == 3) missType = 2;
		}else{
			missType = fetch(cache, type, index, tag, ways, tag1, 0);
			int missType1 = fetch(cache1, type, 0, tag1, ways * sets, tag1, 1);
			if(missType1 == 3){
				if(missType == 3) missType = 2;
				//missType = 2;
			}
		}

		// never see the memory address before, compulsory
		// then check fully associative one
		// if miss in the fully associative one is capacity miss, even if hit in regular but miss in fully, still capacity
		// otherwise if a miss in regular, then conflict miss

		/* TODO: Now we output the file */

		if(missType == 0){	//hit
			fprintf(fp1, "%s %s%08x %s", &type,"0x", address,"hit\n");
			totalHits ++;
		}else if(missType == 1){	//compulsory miss
			fprintf(fp1, "%s %s%08x %s", &type,"0x", address,"compulsory\n");
			totalMisses ++;
		}else if(missType == 2){ //capacity miss
			fprintf(fp1, "%s %s%08x %s", &type,"0x",address,"capacity\n");
			totalMisses ++;
		}else if(missType == 3){ //conflict miss
			fprintf(fp1, "%s %s%08x %s", &type,"0x",address,"conflict\n");
			totalMisses ++;
		}else{
			printf("Error\n");
		}





	}

	fclose(fp);
	fclose(fp1);

	/* TODO: Now we simulate the cache */

	/* Print results */
	printf("Miss Rate: %8lf%%\n", ((double) totalMisses) / ((double) totalMisses + (double) totalHits) * 100.0);
	printf("Read Transactions: %d\n", totalMisses);	// all misses
	printf("Write Transactions: %d\n", write_xactions);	// when overwriting and checking dirty bit is one -> write_xactions+1;

	// printf("Total Hits: %d\n" , totalHits);

	/* TODO: Cleanup */
	freeEverything(cache, sets);

	}


	int logtwo(int number){
		int counter = 0;
		while(number >>= 1){
			counter++;
		}
		return counter;
	}

	struct cache* newCache(int ways, int sets){
		struct cache* myCache = malloc(sizeof(struct cache));
		myCache -> valid = malloc(sets*sizeof(int*));
		myCache -> dirty = malloc(sets*sizeof(int*));
		myCache -> tags = malloc(sets*sizeof(int*));
		myCache -> data = malloc(sets*sizeof(int*));
		myCache -> counter = malloc(sets*sizeof(int));
		myCache -> accessedBefore = malloc(1000*sizeof(int));
		myCache -> arrLength = 1000;
		myCache -> accessedLength = 0;

		for(int i = 0; i < myCache -> arrLength; i++){
			myCache -> accessedBefore[i] = -1;
		}
		for(int i = 0; i < sets; i++){
			myCache -> valid[i]=malloc(ways*sizeof(int));
			myCache -> dirty[i]=malloc(ways*sizeof(int));
			myCache -> tags[i]=malloc(ways*sizeof(int));
			myCache -> data[i]=malloc(ways*sizeof(int));
		}

		for(int i = 0; i < sets; i++){
			for(int j = 0; j < ways; j++){
				myCache -> valid[i][j]=0;
				myCache -> dirty[i][j]=0;
				myCache -> tags[i][j]=0;
				myCache -> data[i][j]=0;
			}
		}
		return myCache;
	}


	char* concat(const char *a, const char *b){
		char* result = malloc(strlen(a)+strlen(b)+1);
		strcpy(result, a);
		strcat(result, b);
		return result;
	}

	/*
	   return code:
0: hit
1: compulsory miss
2: capacity miss
3: conflict miss
	 */


	int accessedBefore(struct cache* c, uint32_t tag){	//it should also consider the index along with tag
		for(int i = 0; i <= c -> accessedLength; i ++){
			if (c -> accessedBefore[i] == tag) {
				return 1;
			}
		}
		if(c -> accessedLength > 0.7 * c -> arrLength){
			uint32_t * temp = malloc(c->arrLength * 2 * sizeof(int));
			for(int i = 0; i < c -> accessedLength; i++){
				temp [i] = c -> accessedBefore [i];
			}
			for(int i = c -> accessedLength; i < c -> accessedLength * 2; i++){
				temp [i] = -1;
			}
			free(c -> accessedBefore);
			c -> accessedBefore = temp;
			c -> arrLength *= 2;
		}
		c -> accessedBefore[c -> accessedLength] = tag;
		c -> accessedLength ++;
		return 0;
	}

	int fetch(struct cache* c, char type, uint32_t index, uint32_t tag, int ways, uint32_t address, int assoc){
		if(type=='s'){
			int j = -1;
			int full =1;
			for(int i = 0; i < ways; i++){
				if(c -> valid[index][i] == 1){
					j = i;
					if(c -> tags[index][i] == tag){	//hit
						c -> dirty[index][i] = 1;
						return 0;
					}
				}else{
					full = 0;

				}
			}
			int count = (c -> counter[index]) % ways;
			if(!full){
				count = (j+1) % ways;
			}
			if(!accessedBefore(c, address)) {	//compulsory
				if(c -> dirty[index][count] == 1 && !assoc) write_xactions++;
				c -> tags[index][count] = tag;
				c -> valid[index][count] = 1;
				c -> dirty[index][count] = 1;	//both 1 here in store
				//c -> counter[count] = 0;
				c -> counter[index] = (count + 1) % ways;
				return 1;
			}

			if(c -> dirty[index][count] == 1 && !assoc) write_xactions++;
			c -> valid[index][count] = 1;
			c -> tags[index][count] = tag;
			c -> dirty[index][count] = 1; //both 1 here in store
			c -> counter[index] = (count + 1) % ways;
			return 3;
		} else{
			int j = -1;
			int full = 1;
			for(int i = 0; i < ways; i++){
				if(c -> valid[index][i] == 1){
					j = i;
					if(c -> tags[index][i] == tag){
						return 0;
					}
				}else{
					full = 0;
				}
			}
			int count = (c -> counter[index]) % ways;
			if(!full){
				count = (j+1)% ways;
			}
			if(!accessedBefore(c, address)) { //compulsory
				if(c -> dirty[index][count] == 1 && !assoc) write_xactions++;
				c -> tags[index][count] = tag;
				c -> valid[index][count] = 1;
				c -> dirty[index][count] = 0;	//both 0 here
				//c -> counter[count] = 0;
				c -> counter[index] = (count + 1) % ways;
				return 1;
			}
			if(c -> dirty[index][count] == 1 && !assoc) write_xactions++;;
			c -> valid[index][count] = 1;
			c -> tags[index][count] = tag;
			c -> dirty[index][count] = 0; //
			c -> counter[index] = (count + 1) % ways;
			return 3;
		}
	}

	void freeEverything(struct cache* c, int sets){
		for(int i = 0; i < sets; i++){
			free(c->valid[i]);
			free(c->tags[i]);
			free(c->dirty[i]);
		}
		free(c->valid);
		free(c->tags);
		free(c->dirty);
		free(c->accessedBefore);
		free(c->counter);
		free(c);
	}
