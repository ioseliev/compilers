#include "dfa.h"

#include <stdlib.h>
#include <stddef.h>


DFA_t DFA(uint8_t n_states, uint16_t n_transitions) {
    bool *fs = calloc(n_states + 1, sizeof(bool)); // + 1 for stat
    Transition_t *t = calloc(n_transitions, sizeof(Transition_t));
    DFA_t ret = {n_states, fs, n_transitions, t};
    return ret;
}

bool accepts(DFA_t *dfa, const char *input) {
    bool *finals = dfa->final_states;
    uint16_t n_trans = dfa->n_transitions;
    Transition_t *trans = dfa->transitions;

    uint8_t state = 1;
    char c = '\0';
    while ((c = *(input++)) != '\0' && state != 0) {
        for (size_t i = 0; i < n_trans; ++i) {
            if (trans[i].from == state && trans[i].on == c) {
                state = trans[i].to;
                break;
            }
            // Dead state
            if (i == (n_trans - 1)) {
                state = 0;
                break;
            }
        }
    }
    return finals[state];
}