#include "memlayout.h"

// global vairables
jmp_buf env;
unsigned int MAX = (0xffffffff);

// defined signal handler
void handler(int sig){
    siglongjmp(env, 1);
}


// defined function for updating info of each region from regions
void update(struct memregion *regions, unsigned int *index, unsigned char mode,
            unsigned int addr, unsigned int size){
    // if addr is 0x00
    if (addr == 0){
        regions[*index].from = (void *)(addr);
        regions[*index].mode = mode;
    }

    // if index exceeds size
    if ((*index) >= size){
        return;
    }

    // if next new region is found
    if (regions[*index].mode != mode){
        regions[*index].to = (void *)(addr - 1);
        (*index)++;
        // if index exceeds size
        if ((*index) >= size){
            return;
        }
        regions[*index].from = (void *)(addr);
        regions[*index].mode = mode;
    }
}


// defiend function for scanning the whole memory space and separate
// it into regions based on accessibility
int get_mem_layout(struct memregion *regions, unsigned int size){
    // sigaction sa for handling SIGSEGV
    struct sigaction sa;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = handler;
    sigaction(SIGSEGV, &sa, NULL);
    
    // sigaction sb for handling SIGBUS
    struct sigaction sb;
    sb.sa_flags = 0;
    sigemptyset(&sb.sa_mask);
    sb.sa_handler = handler;
    sigaction(SIGBUS, &sb, NULL);
    
    // variable declarations
    unsigned int index = 0;  // index of regions
    char *ptr;
    char value;
    int sig = 0;

    // N = # of PAGE
    int N = MAX/PAGE_SIZE + (MAX % PAGE_SIZE == 0? 0 : 1);

    unsigned int addr;
    for (unsigned int i = 0; i < N; i++){
        addr = PAGE_SIZE * i;     // addr = current address

        sig = sigsetjmp(env, 1);  // jump back point
        ptr = (char *)(addr);
        if (sig != 0){
            // if address is not readable
            update(regions, &index, MEM_NO, addr, size);
            continue;
        } else {
            // try to read from addr
            value = *ptr;
        }

        sig = sigsetjmp(env, 1);  // jump back point
        if (sig != 0){
            // if the address is not writable
            update(regions, &index, MEM_RO, addr, size);
            continue;
        } else {
            // try to write into addr
            *ptr = value;
        }
        // if the address is readable and writable
        update(regions, &index, MEM_RW, addr, size);
    }

    if (index < size){
        //update the value of regions[index].to
        regions[index].to = (void *)(MAX);
    } else {
        index--;
    }
    return index + 1;
}
