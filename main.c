#include "utils.h"
#include "dfa.h"

#include <stdio.h>


int main(int argc, char **argv) {
    char *input = read_from_stdin();

    // DFA that recognizes a(a|b)*
    {
        DFA_t dfa = DFA(2, 3);
        dfa.final_states[2] = true;
    
        dfa.transitions[0].from = 1;
        dfa.transitions[0].to = 2;
        dfa.transitions[0].on = 'a';

        dfa.transitions[1].from = 2;
        dfa.transitions[1].to = 2;
        dfa.transitions[1].on = 'a';

        dfa.transitions[2].from = 2;
        dfa.transitions[2].to = 2;
        dfa.transitions[2].on = 'b';

        printf("%s %s\n", (accepts(&dfa, input) ? "ACCEPTS" : "REJECTS"), input);
    }

    return 0;
}