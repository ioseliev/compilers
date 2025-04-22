#include "utils.h"

#include <stdio.h>


int main(int argc, char **argv) {
    char *input = read_from_stdin();

    printf("%s\n", input);

    return 0;
}