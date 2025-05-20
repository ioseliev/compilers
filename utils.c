#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>


char *read_from_file(const char *filename) {
    const size_t BUFFER_SIZE_STRIDE = 1024;

    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        return NULL;
    }

    size_t buffer_size = BUFFER_SIZE_STRIDE;
    size_t n_read = 0;
    char *buffer = malloc(buffer_size * sizeof(char));

    while (!feof(fp) && fgets(buffer + n_read, buffer_size - n_read, fp) != NULL) {        
        if (n_read == 0) {
            n_read = strlen(buffer);
        } else {
            n_read += 1 + strlen(buffer + n_read + 1);
        }

        if (n_read == (buffer_size - 1)) {
            buffer_size += BUFFER_SIZE_STRIDE;
            buffer = realloc(buffer, buffer_size);
        }
    }

    buffer[n_read] = '\0';

    fclose(fp);

    return buffer;
}