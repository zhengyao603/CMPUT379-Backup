#include "hashmap.h"


// defined hash function
unsigned long long hashmap_hash(unsigned long long key, unsigned long long size){
	return key % size;
}


// defined function for calculating size of working set
unsigned long long workingset_size(struct HashMap *hashmap){
    // if there is no such hashmap
	if (hashmap == NULL){
		return 0;
	}

    // if there is no hashtable within the hashmap
	if (hashmap -> hashtable == NULL){
		return 0;
	}

	unsigned long long result = 0;

    // for each hashentry
	for (unsigned long long i = 0; i < hashmap -> size; i++){
		struct HashEntry *current = hashmap -> hashtable[i];
		struct HashEntry *next = NULL;

		// increment result and free the linked list
		while (current){
			result += 1;
			next = current -> next;
			free(current);
			current = next;
		}
	}

	// free pointers
	free(hashmap -> hashtable);
	free(hashmap);

	return result;
}


// defined function for initializing hashmap
struct HashMap *hashmap_init(unsigned long long size){
	// if size too small
	if (size == 0){
		size = 5;
	}

	// allocate memory for hashmap
	struct HashMap *hashmap = malloc(sizeof(struct HashMap));
	if (hashmap == NULL){
		printf("fail to initialize hashmap.\n");
		return NULL;
	}

	// allocate memory for hashtable
	hashmap -> hashtable = malloc(sizeof(struct HashEntry *) * size);
	if (hashmap -> hashtable == NULL){
		free(hashmap);
		printf("fail to initialize hashtable.\n");
		return NULL;
	}

    // initialize memory
	memset(hashmap -> hashtable, 0, sizeof(struct HashEntry *) * size);

    // initialize attributes
	hashmap -> size = size;
	hashmap -> resize_factor = size / 10;
	return hashmap;
}


// defined function for putting new element into hashmap, return the depth of the added key-value pair
unsigned long long hashmap_put(struct HashMap *hashmap, unsigned long long key, unsigned long long value){
	// map key into entry
	unsigned long long pos = hashmap_hash(key, hashmap -> size);
	struct HashEntry *hashentry = hashmap -> hashtable[pos];
	struct HashEntry *prev = NULL;

	unsigned long long depth = 0;

    // if the entry already been occupied
	while (hashentry){
		if (hashentry -> key == key){
			hashentry -> value += 1;
			break;
		}
		prev = hashentry;
		hashentry = hashentry -> next;
		depth += 1;
	}

	// if no data is stored
	if (hashentry == NULL){
		struct HashEntry *new_entry = malloc(sizeof(struct HashEntry));
		if (new_entry == NULL){
			printf("fail to allocate new hashentry.\n");
			return -1;
		}

        // initialize new entry
		new_entry -> key = key;
		new_entry -> value = value;
		new_entry -> next = NULL;

        // if it is the first node of linked list
		if (prev == NULL){
			hashmap -> hashtable[pos] = new_entry;
		} else {
			prev -> next = new_entry;
		}
	}

	return depth;
}


struct HashMap *hashmap_resize(struct HashMap *hashmap){
	// if the hashmap dose not exist
	if (hashmap == NULL){
		return NULL;
	}

    // if the hashtable does not exist
	if (hashmap -> hashtable == NULL){
		return NULL;
	}

	struct HashMap *new_hashmap = hashmap_init(hashmap -> size * 2);

	// for each entry of hash table
	for (unsigned long long i = 0; i < hashmap -> size; i++){
		struct HashEntry *current = hashmap -> hashtable[i];
		struct HashEntry *next = NULL;

        // put all the key value pairs into new hashmap
		while (current){
			next = current -> next;
			hashmap_put(new_hashmap, current -> key, current -> value);
			free(current);
			current = next;
		}
	}

	// free pointers
	free(hashmap -> hashtable);
	free(hashmap);

	return new_hashmap;
}


// defined function for deleting hashmap
void hashmap_destroy(struct HashMap *hashmap){
	// if the hashmap dose not exist
	if (hashmap == NULL){
		return;
	}

    // if the hashtable does not exist
	if (hashmap -> hashtable == NULL){
		return;
	}

    // for each entry of hash table
	for (unsigned long long i = 0; i < hashmap -> size; i++){
		struct HashEntry *current = hashmap -> hashtable[i];
		struct HashEntry *next = NULL;

        // free the linked list
		while (current){
			next = current -> next;
			free(current);
			current = next;
		}
	}

    // free pointers
	free(hashmap -> hashtable);
	free(hashmap);
}


// defined function for printing current hashmap
void hashmap_print(struct HashMap *hashmap){
	// if the hashmap dose not exist
	if (hashmap == NULL){
		printf("NULL\n");
		return;
	}

    // if the hashtable does not exist
	if (hashmap -> hashtable == NULL){
		printf("NULL\n");
		return;
	}

    // for each entry of hash table
	for (unsigned long long i = 0; i < hashmap -> size; i++){
		struct HashEntry *current = hashmap -> hashtable[i];
		struct HashEntry *next = NULL;

        // free the linked list
		while (current){
			next = current -> next;
			printf("Node %llu: key: %llu, value:%llu\n", i, current -> key, current -> value);
			current = next;
		}
	}
}