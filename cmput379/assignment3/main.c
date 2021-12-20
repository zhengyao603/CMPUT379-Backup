#include "hashmap.h"


static unsigned long long count = 0; // count of lines read from stdin
static unsigned long long skipsize;
static unsigned long long pagesize;
static unsigned long long windowsize;
static struct HashMap *hashmap; // defined data structure


int process_input(char *input){
	count += 1;

    // if working set size should be computed
	if (count == windowsize){
		unsigned long long size = workingset_size(hashmap);
		printf("%llu\n", size);
		hashmap = hashmap_init(windowsize / 10);

		count = 0;
	}


    // separate the string
	char *temp1 = strtok(input, " ");
	temp1 = strtok(NULL, " ");
	char *temp2 = strtok(temp1, ",");
	char *temp3 = strtok(NULL, ",");

    // get address and count of bytes read from the address
    char *endptr;
	unsigned long long address = strtoull(temp2, &endptr, 16);
	unsigned long long byte_read = atoll(temp3);

    // store new information into hashmap
	unsigned long long key1 = address / pagesize;
	unsigned long long if_resize = hashmap_put(hashmap, key1, 1);
	if (if_resize > hashmap -> resize_factor){
		hashmap = hashmap_resize(hashmap);
	}

	// if the memory reference accesses two pages
	unsigned long long key2 = (address + byte_read) / pagesize;
	if (key1 != key2){
		if_resize = hashmap_put(hashmap, key2, 1);
		if (if_resize > hashmap -> resize_factor){
			hashmap = hashmap_resize(hashmap);
		}
	}

	return 0;
}


// defined function for reading input with 'stream' fashion
int read_input(){
	char buffer[256];
	int index = 0;
	char c;
	int skipcount = 0;

    // keep reading from stdin
	for (;;){
		c = getchar();

		if (c < 0){
			hashmap_destroy(hashmap);
			break;
		}

		buffer[index] = c;

        // if '\n' is read
		if (c == '\n'){
			buffer[index] = '\0';
			index = 0;

            // process input if it is useful
			if (buffer[0] == 'I' || buffer[0] == ' '){
				skipcount += 1;
				if (skipcount > skipsize){
					process_input(buffer);
				}
			}

			continue;
		}

		index++;
	}

	return 0;
}


// defined main function
int main(int argc, char *argv[]){
	// if no skipsize defined
	if (argc == 3){
		skipsize = 0;
		pagesize = atoll(argv[1]);
		windowsize = atoll(argv[2]);
	}

    // if skipsize defined
	else if (argc == 5){
		if (argv[1][0] == '-' && argv[1][1] == 's'){
			skipsize = atoll(argv[2]);
			pagesize = atoll(argv[3]);
			windowsize = atoll(argv[4]);
		} else {
			printf("Invalid Input Argument.\n");
		}
	}

	else {
		printf("Invalid Input Argument.\n");
		exit(0);
	}

	if (windowsize < 10){
		printf("Windowsize need to be at least 10.\n");
		exit(0);
	}

	if (pagesize < 16){
		printf("Pageszie need to be at least 16\n");
		exit(0);
	}

	hashmap = hashmap_init(windowsize / 6);
	read_input();
	
	return 0;
}