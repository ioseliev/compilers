#include "utils.h"
#include "regex.h"
#include "dfa.h"

#include <stdio.h>


int main(int argc, char **argv) {
    //char *input = read_from_stdin();
    
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

    return 0;
}