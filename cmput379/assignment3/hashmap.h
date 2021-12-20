#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct HashEntry{
	unsigned long long key; // page number(page index)
	unsigned long long value; // count of times the page is referenced
	struct HashEntry *next;
};

struct HashMap{
	unsigned long long size; // current capacity of hashtable
	unsigned long long resize_factor; // determine if the hashtable need to be rehashing
    struct HashEntry **hashtable; // array of pointers which points to first element of each linked list
};


unsigned long long hashmap_hash(unsigned long long key, unsigned long long size);
unsigned long long workingset_size(struct HashMap *hashmap);
struct HashMap *hashmap_init(unsigned long long size);
unsigned long long hashmap_put(struct HashMap *hashmap, unsigned long long key, unsigned long long value);
struct HashMap *hashmap_resize(struct HashMap *hashmap);
void hashmap_destroy(struct HashMap *hashmap);
void hashmap_print(struct HashMap *hashmap);