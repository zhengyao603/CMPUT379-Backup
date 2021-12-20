#include "memlayout.h"
#define N 20

// defined function for printing output
void print(struct memregion *regions, int size){
    for (int i = 0; i < size; i++){
        printf("0x%08x-0x%08x ",(unsigned int) regions[i].from, (unsigned int) regions[i].to);
        if (regions[i].mode == 0){
            printf("RW\n");
        } else if(regions[i].mode == 1){
            printf("RO\n");
        } else {
            printf("NO\n");
        }
    }
    printf("\n");
}

// defined resursion function
int recursion(int x){
    if (x == 1){
        return 1;
    }
    return (x + recursion(x - 1));
}

int main(){
    // allocate memory for regions
    struct memregion *regions = (struct memregion *) malloc(sizeof(struct memregion) * N);
    if (regions == NULL){
        printf("Failed to allocate memory for regions\n");
        exit(1);
    }
    // first call to get_mem_layout function
    int size = get_mem_layout(regions, N);
    printf("first invokes the get_mem_layout() function:\n");
    print(regions, size);

    // call the recursion program
    int val = recursion(100000);

    // second call to get_mem_layout function
    printf("second invokes the get_mem_layout() function:\n");
    size = get_mem_layout(regions, N);
    print(regions, size);

    // free allocated memory and return
    free(regions);
    return 0;
}