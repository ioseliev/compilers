#include "utils.h"
#include "regex.h"
#include "dfa.h"

#include <stdio.h>
#include <stdlib.h>


int main(int argc, char **argv) {
    /* const char *source = NULL;

    if (argc == 1) {
        printf("Usage: %s filename\n", argv[0]);
        return 1;
    }

    if ((source = read_from_file(argv[1])) == NULL) {
        fprintf(stderr, "File `%s` not found.\n", argv[1]);
        return 1;
    } */
    
    {
        char regex[100];
        scanf("%s", regex);

        char* postfix = regex_to_postfix(regex);
        printf("Expressao em postfix: %s\n", postfix);

        DFA_t dfa = regex_to_dfa(postfix);
        printf("DFA relacionado:\n");
        debug(&dfa);

        destroy(&dfa);
    }

    //free((void *) source);

    return 0;
}