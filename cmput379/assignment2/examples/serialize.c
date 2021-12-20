#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#define FLOAT_PARAM_OFFSET  sizeof(int)
#define STRING1_OFFSET      (FLOAT_PARAM_OFFSET + sizeof(float))

// In this file there is an example struct that is serialised and de-serialised (example struct)
// Three functions for serialising and de-serialising it
//  serialise: takes a single struct and returns a buffer of bytes that contain the serialisation
//  deserialise: takes a buffer and length containing exactly one struct and de-serialises it returning a struct
//  deserialise_next: takes a buffer that may contain a serialised struct and returns the next de-serialised struct if was able to find one
//
// remember that all pointers must have the content being pointed to serialised, as the pointer does not hold the actual information

// the example struct to demonstrate a method of serialisation and de-serialisation
typedef struct {
    int int_param;
    float float_param;
    char *string1;
    char *string2;
} example_struct;

// serialises in the following order: int_param, float_param, string1, string2
// both strings are null terminated
// the passed bufffer len pointer is used to return the length of the buffer
uint8_t *serialise(example_struct *example, int *buffer_len) {
    int str1_len = strlen(example->string1);
    int str2_len = strlen(example->string2);

    *buffer_len = sizeof(int) + sizeof(float) + str1_len + 1 + str2_len + 1;

    uint8_t *buffer = calloc(sizeof(uint8_t), *buffer_len);
    if (buffer == NULL) {
        return NULL;
    }

    // sets the int and float paramters
    *(int *)(buffer) = example->int_param;
    *(float *)(buffer + FLOAT_PARAM_OFFSET) = example->float_param;

    // copy over first string
    memcpy(buffer + STRING1_OFFSET, example->string1, str1_len);

    // copy over second string, add 1 to account for null terminator
    memcpy(buffer + STRING1_OFFSET + str1_len + 1, example->string2, str2_len);

    return buffer;
}

// de-serialises a given buffer containing a single serialised struct
// this will not work if an incomplete struct is in the buffer or there are multiple structs in the buffer
// it will not check for errors and assumes exactly one struct is in the buffer
example_struct *deserialise(uint8_t *buffer, int len) {
    example_struct *example = malloc(sizeof(example_struct));

    example->int_param = *(int *)(buffer);
    example->float_param = *(float *)(buffer + FLOAT_PARAM_OFFSET);

    // copy out string1
    // get the length first and then copy with strncpy
    int str1_len = strnlen(buffer + STRING1_OFFSET, len - STRING1_OFFSET - 1);
    if (str1_len >= len - STRING1_OFFSET - 1) {
        free(example);
        return NULL;
    }

    example->string1 = malloc(str1_len + 1);
    if (example->string1 == NULL) {
        free(example);
        return NULL;
    }

    strncpy(example->string1, buffer + STRING1_OFFSET, str1_len);
    example->string1[str1_len] = '\0';

    // copy out string2
    // assume string2 is the remaining bytes (the 1 is needed because of the null terminator)
    int str2_len = len - (STRING1_OFFSET + str1_len + 1);

    example->string2 = malloc(str2_len + 1);
    if (example->string2 == NULL) {
        free(example->string1);
        free(example);
        return NULL;
    }

    strncpy(example->string2, buffer + STRING1_OFFSET + str1_len + 1, str2_len);
    example->string2[str2_len] = '\0';

    return example;
}

// this function will attempt to de-serialise the next struct in the buffer
// it take a buffer, the length of it and a pointer that will be used to return the end of the struct in the buffer
// if a full struct cannot be found the, end will be unmodified and NULL returned
// if a struct is found but an error occured, end will be set to the end of that struct and NULL will be returned
example_struct *deserialise_next(uint8_t *buffer, int len, int *end) {
    // find two null terminators after the first fixed size elements
    int null_terms = 0;
    int i = STRING1_OFFSET;

    for (; i <= len; i++) {
        if (buffer[i] == '\0') {
            null_terms++;
            if (null_terms == 2) {
                break;
            }
        }
    }

    // if two null terminators were not found a full struct does not exist
    if (null_terms != 2) {
        return NULL;
    }

    // assign the end of the struct
    *end = i;

    // deserialise the struct that was found
    return deserialise(buffer, *end);
}

// the following are helper functions

// free's the struct and elements within
void free_example_struct(example_struct *example) {
    free(example->string1);
    free(example->string2);
    free(example);
}

// prints out the example struct
void print_example_struct(example_struct *example) {
    printf("int_param %d\n", example->int_param);
    printf("float_param %f\n", example->float_param);
    printf("string1 \"%s\"\n", example->string1);
    printf("string2 \"%s\"\n", example->string2);
}

// prints out a byte array for analysis
void print_bytes(uint8_t *buffer, int len) {
    printf("printing bytes\n");

    for (int i = 0; i < len; i++) {
        printf("%02X ", buffer[i]);
        if (i % 8 == 7) {
            printf("\n");
        }
    }
    printf("\n");
}

// the following are test functions that show how the functions can be used

// serialises and de-serialises a single struct
void serialise_deserialise_example() {
    // starting struct to test out
    example_struct original = {
        1,
        5.0,
        "string 1",
        "second string"
    };

    printf("original struct\n");
    print_example_struct(&original);
    printf("\n");

    // serialise the original struct into a byte array
    int buffer_len;
    uint8_t *buffer = serialise(&original, &buffer_len);

    printf("serialised bytes\n");
    print_bytes(buffer, buffer_len);
    printf("\n");

    // de-serialise the byte array
    example_struct *copy = deserialise(buffer, buffer_len);

    printf("de-serialise copy\n");
    print_example_struct(copy);
    printf("\n");

    // clean up
    free(buffer);
    free_example_struct(copy);
}

// de-serialises multiple structs from a single buffer using de-serialise next
void deserialise_multiple_from_buffer() {
    example_struct original1 = {
        1,
        5.0,
        "string 1",
        "second string"
    };

    example_struct original2 = {
        4,
        7.0,
        "second struct",
        "string 2"
    };

    // create buffer with both, this is a hack for the example
    int buffer_len1;
    int buffer_len2;

    uint8_t *buffer_original1 = serialise(&original1, &buffer_len1);
    uint8_t *buffer_original2 = serialise(&original2, &buffer_len2);

    int buffer_len = buffer_len1 + buffer_len2;
    uint8_t *buffer = malloc(buffer_len);

    memcpy(buffer, buffer_original1, buffer_len1);
    memcpy(buffer + buffer_len1, buffer_original2, buffer_len2);

    free(buffer_original1);
    free(buffer_original2);

    // buffer construction complete

    print_bytes(buffer, buffer_len);

    // attempt to deserialise with only part of the buffer
    if (deserialise_next(buffer, 15, NULL) == NULL) {
        // this should print
        printf("failed to de-serialises, with incomplete struct\n\n");
    }

    // de-serialise the first struct
    int end = 0;
    example_struct *copy1 = deserialise_next(buffer, buffer_len, &end);

    printf("struct extracted, end = %d\n", end);
    print_example_struct(copy1);
    printf("\n");

    // increment end so it points to the start of the next struct
    end++;

    // deserialise next, accounting for first struct
    example_struct *copy2 = deserialise_next(buffer + end, buffer_len - end, &end);

    printf("second struct extracted\n");
    print_example_struct(copy2);

    // clean up
    free(buffer);
    free_example_struct(copy1);
    free_example_struct(copy2);
}

int main() {
    printf("Testing single serialisation and de-serialisation\n");
    serialise_deserialise_example();

    // printf("Testing de-serialisation of multiple structs on the same buffer\n");
    // deserialise_multiple_from_buffer();

    return 0;
}