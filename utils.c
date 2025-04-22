#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>


char *read_from_stdin() {
    const size_t BUFFER_SIZE_STRIDE = 1024;

    size_t buffer_size = BUFFER_SIZE_STRIDE;
    size_t n_read = 0;
    char *buffer = malloc(buffer_size * sizeof(char));

    while (!feof(stdin) && fgets(buffer + n_read, buffer_size - n_read, stdin) != NULL) {        
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

    return buffer;
}