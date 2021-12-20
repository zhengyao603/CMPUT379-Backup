#include <stdio.h>
#include <unistd.h> // needed for getopt
#include <libgen.h> // needed for basename

void print_usage(char *program_name) {
    printf("Usage %s [-a -h] [-b <argument>]\n", basename(program_name));
}

int main(int argc, char *argv[]) {
    int c;

    // call getopt until all options are proccessed
    // the options are 'a', 'b', and h
    // 'a' does not have any argument, but 'b' requires an argument
    // 'h' simply prints out a usage message
    while ((c = getopt(argc, argv, "ab:h")) != -1) {
        switch (c) {
            case 'a':
                // process argument
                printf("option a\n");
                break;
            case 'b':
                printf("option b with argument %s\n", optarg);
                break;
            case 'h':
                print_usage(argv[0]);
                return 1;
            case '?':
                // option not in optstring or error
                break;
        }
    }

    // there may be non-optional arguments to process
    // this loop is for demonstrative purposes and you will almost never want to do this
    for (; optind < argc; optind++) {
        printf("non-optional argument %s\n", argv[optind]);
    }

    return 0;
}