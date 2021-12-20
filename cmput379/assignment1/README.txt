1)
mem_1 use malloc() to allocating a array with size 100000000 and initializing it.
Difference are noted below:

first invokes the get_mem_layout() function:
0x00000000-0x08047fff NO
0x08048000-0x08049fff RO
0x0804a000-0x0804afff RW
0x0804b000-0x080a8fff NO
0x080a9000-0x080c9fff RW
0x080ca000-0xf7d44fff NO----difference
0xf7d45000-0xf7d45fff RW----difference
0xf7d46000-0xf7ef5fff RO
0xf7ef6000-0xf7ef6fff NO
0xf7ef7000-0xf7ef8fff RO
0xf7ef9000-0xf7efcfff RW
0xf7efd000-0xf7f52fff NO
0xf7f53000-0xf7f53fff RW
0xf7f54000-0xf7f54fff RO
0xf7f55000-0xf7f56fff NO

second invokes the get_mem_layout() function:
0x00000000-0x08047fff NO
0x08048000-0x08049fff RO
0x0804a000-0x0804afff RW
0x0804b000-0x080a8fff NO
0x080a9000-0x080c9fff RW
0x080ca000-0xf1de5fff NO----difference
0xf1de6000-0xf7d45fff RW----difference
0xf7d46000-0xf7ef5fff RO
0xf7ef6000-0xf7ef6fff NO
0xf7ef7000-0xf7ef8fff RO
0xf7ef9000-0xf7efcfff RW
0xf7efd000-0xf7f52fff NO
0xf7f53000-0xf7f53fff RW
0xf7f54000-0xf7f54fff RO
0xf7f55000-0xf7f56fff NO

It can be seen that there are more memory space becomde RW, because we allocate memory for a massive array, it is reasonable that those memory become RW.


----------------------------
2)
mem_2 uses mmap() to create a 1000000 byte anonymous map, and set these pages in the region RO accessibility.
Difference are noted below:

first invokes the get_mem_layout() function:
0x00000000-0x08047fff NO
0x08048000-0x08049fff RO
0x0804a000-0x0804afff RW
0x0804b000-0x088f9fff NO
0x088fa000-0x0891afff RW
0x0891b000-0xf7cdefff NO----difference
0xf7cdf000-0xf7cdffff RW----difference
0xf7ce0000-0xf7e8ffff RO
0xf7e90000-0xf7e90fff NO
0xf7e91000-0xf7e92fff RO
0xf7e93000-0xf7e96fff RW
0xf7e97000-0xf7eecfff NO
0xf7eed000-0xf7eedfff RW
0xf7eee000-0xf7eeefff RO
0xf7eef000-0xf7ef0fff NO

second invokes the get_mem_layout() function:
0x00000000-0x08047fff NO
0x08048000-0x08049fff RO
0x0804a000-0x0804afff RW
0x0804b000-0x088f9fff NO
0x088fa000-0x0891afff RW
0x0891b000-0xf7be9fff NO----difference
0xf7bea000-0xf7cdefff RO----difference
0xf7cdf000-0xf7cdffff RW
0xf7ce0000-0xf7e8ffff RO
0xf7e90000-0xf7e90fff NO
0xf7e91000-0xf7e92fff RO
0xf7e93000-0xf7e96fff RW
0xf7e97000-0xf7eecfff NO
0xf7eed000-0xf7eedfff RW
0xf7eee000-0xf7eeefff RO

It can be seen there comes up a new memory region wiyh RO accessibility, and that is because we just create a 1000000 byte anonymous map with RO accessibility.


--------------------------
3)
mem_3 defines then calls a recursive function with considerable depth of recursion.
Difference are noted below:
FYI: This output is from local vm environment but not lab machine, the memory layout within lab machine seems like not changing.
     The reason of the difference, I guess, it is due to the difference of stack settings.

first invokes the get_mem_layout() function:
0x00000000-0x08047fff NO
0x08048000-0x08049fff RO
0x0804a000-0x0804afff RW
0x0804b000-0x08906fff NO
0x08907000-0x08927fff RW
0x08928000-0xb759ffff NO
0xb75a0000-0xb75a0fff RW
0xb75a1000-0xb7750fff RO
0xb7751000-0xb7751fff NO
0xb7752000-0xb7753fff RO
0xb7754000-0xb7757fff RW
0xb7758000-0xb7776fff NO
0xb7777000-0xb7777fff RW
0xb7778000-0xb7778fff RO
0xb7779000-0xb7779fff NO
0xb777a000-0xb779efff RO
0xb779f000-0xb779ffff RW
0xb77a0000-0xbf8d0fff NO----difference
0xbf8d1000-0xbf8f1fff RW----difference
0xbf8f2000-0xffffffff NO

second invokes the get_mem_layout() function:
0x00000000-0x08047fff NO
0x08048000-0x08049fff RO
0x0804a000-0x0804afff RW
0x0804b000-0x08906fff NO
0x08907000-0x08927fff RW
0x08928000-0xb759ffff NO
0xb75a0000-0xb75a0fff RW
0xb75a1000-0xb7750fff RO
0xb7751000-0xb7751fff NO
0xb7752000-0xb7753fff RO
0xb7754000-0xb7757fff RW
0xb7758000-0xb7776fff NO
0xb7777000-0xb7777fff RW
0xb7778000-0xb7778fff RO
0xb7779000-0xb7779fff NO
0xb777a000-0xb779efff RO
0xb779f000-0xb779ffff RW
0xb77a0000-0xbf5e2fff NO----difference
0xbf5e3000-0xbf8f1fff RW----difference
0xbf8f2000-0xffffffff NO

In the stack memory region, there is an obvious difference between these two memory layouts, which is there is more RW memory after calling the recursion.
The reason of that is simply because recursion takes considerably amount of memory from stack.