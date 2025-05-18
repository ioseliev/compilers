#ifndef DFA_H
#define DFA_H
#include <stdint.h>
#include <stdbool.h>


/* NOTE: As a result of NFAs being generalizations of DFA, we merge them toge-
 * ther here for code brevity.
 */

// EPSILON shan't occur in a string except at EOF, so it's fine assigning it \0
#define EPSILON '\0'

typedef struct {
    uint8_t from;
    uint8_t to;
    char on;
} Transition_t;

typedef struct {
    uint8_t n_states;
    bool *final_states;
    uint16_t n_transitions;
    Transition_t *transitions;
} DFA_t;

DFA_t DFA(uint8_t n_states, uint16_t n_transitions);

bool accepts(DFA_t *dfa, const char *input);

#endif