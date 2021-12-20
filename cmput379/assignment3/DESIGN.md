The variation of the implementation of valws379 is the minimum implementation.

Calling 'make' will generate an execuatble 'valws379', and it can be executed as pipe.
e.g "valgrind --tool=lackey --trace-mem=yes ./program_name 2>&1 | ./valws379 [-s skipsize] pgsize windowsize"

skipsize is an optional parameter which determines whether the prorgam ignores first 'skipsize' amount of memory reference.
pgsize is required parameter which determines the pagesize, and the minimum value is 16.
windowsize is required parameter which determines the windowsize, and the minimum value is 10.

The data structure I used for resolving memory efficiency problem is a defined hash map.
The main idea of the hash map is that it has a changing size, which can dynamically grow or shrink accordingly to the amount of data need to be stored.
The key of each element is the page number, the hash function will hash each element into different bucket based on their page numbers.
If collision happens, new element will be added as the tail of linked list within that bucket.
If the depth of the linked list of any bucket is considerable large, then the mapping efficieny of the hashmap is relative low, and it will be resized with doubling the size.

The advantages of dynamic hash map can be seen in terms of both time complexity and space complexity.
The hash function reduce the searching time dramatically, O(n) for linear array -> O(1) (best case) for hash map
The dynamic resizing functionality can reduce the amount of space required by the program.

***FYI: 'valws379' assumes there is no string starting with 'I' or ' ' being printed to stderr, otherwise the string processing might be incorrect.