#include "dfa.h"
#include "bitset.h"

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <memory.h>
#include <string.h>


/* Constructor & destructor */

DFA_t DFA(uint8_t n_states, uint16_t n_transitions) {
    bool *fs = calloc(n_states + 1, sizeof(bool)); // + 1 for stat
    Transition_t *t = calloc(n_transitions, sizeof(Transition_t));
    DFA_t ret = {n_states, fs, n_transitions, t};
    return ret;
}

void destroy(DFA_t *dfa) {
    dfa->n_states = dfa->n_transitions = 0;
    free(dfa->final_states);
    free(dfa->transitions);
    dfa->final_states = NULL;
    dfa->transitions = NULL;
}

/* Simulation */

bool accepts(DFA_t *dfa, const char *input) {
    bool *finals = dfa->final_states;
    uint16_t n_trans = dfa->n_transitions;
    Transition_t *trans = dfa->transitions;

    uint8_t state = 1;
    char c = '\0';
    while ((c = *(input++)) != '\0' && state != 0) {
        for (size_t i = 0; i < n_trans; ++i) {
            if (trans[i].from == state && trans[i].on == c) {
                printf("%hhd -> %c -> %hhd\n", trans[i].from, c, trans[i].to);
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

/* Thompson constructions */

DFA_t singleton(char c) {
    DFA_t ret = DFA(2, 1);
    SET_FINAL(&ret, 2, true);
    SET_TRANSITION(&ret, 0, 1, 2, c);
    return ret;
}

DFA_t concat(DFA_t *s, DFA_t *t) {
    DFA_t ret = DFA(s->n_states + t->n_states - 1, s->n_transitions + t->n_transitions);
    uint8_t state_shift = 0;
    uint16_t trans_shift = 0;

    memcpy(ret.transitions + trans_shift, s->transitions, s->n_transitions * sizeof(Transition_t));

    state_shift += s->n_states;
    --state_shift;
    trans_shift += s->n_transitions;
    
    memcpy(ret.transitions + trans_shift, t->transitions, t->n_transitions * sizeof(Transition_t));

    for (uint16_t i = trans_shift; i < (trans_shift + t->n_transitions); ++i) {
        ret.transitions[i].from += state_shift;
        ret.transitions[i].to += state_shift;
    }

    state_shift += t->n_states;

    SET_FINAL(&ret, state_shift, true);

    destroy(s);
    destroy(t);

    return ret;
}

DFA_t join(DFA_t *s, DFA_t *t) {
    DFA_t ret = DFA(s->n_states + t->n_states + 2, s->n_transitions + t->n_transitions + 4);
    uint8_t state_shift = 1;
    uint16_t trans_shift = 2;

    SET_TRANSITION(&ret, 0, 1, 1 + state_shift, EPSILON);

    memcpy(ret.transitions + trans_shift, s->transitions, s->n_transitions * sizeof(Transition_t));

    for (uint16_t i = trans_shift; i < (trans_shift + s->n_transitions); ++i) {
        ret.transitions[i].from += state_shift;
        ret.transitions[i].to += state_shift;
    }

    state_shift += s->n_states;
    trans_shift += s->n_transitions;

    SET_TRANSITION(&ret, 1, 1, 1 + state_shift, EPSILON);

    memcpy(ret.transitions + trans_shift, t->transitions, t->n_transitions * sizeof(Transition_t));

    for (uint16_t i = trans_shift; i < (trans_shift + s->n_transitions); ++i) {
        ret.transitions[i].from += state_shift;
        ret.transitions[i].to += state_shift;
    }

    state_shift += t->n_states;
    trans_shift += t->n_transitions;

    SET_TRANSITION(&ret, trans_shift, 1 + state_shift - t->n_states - 1, 1 + state_shift, EPSILON);
    SET_TRANSITION(&ret, trans_shift + 1, 1 + state_shift - 1, 1 + state_shift, EPSILON);

    SET_FINAL(&ret, 1 + state_shift, true);

    destroy(s);
    destroy(t);

    return ret;
}

DFA_t kleene(DFA_t *s) {
    DFA_t ret = DFA(s->n_states + 2, s->n_transitions + 4);
    uint8_t state_shift = 1;
    uint16_t trans_shift = 2;

    SET_TRANSITION(&ret, 0, 1, 1 + state_shift, EPSILON);

    memcpy(ret.transitions + trans_shift, s->transitions, s->n_transitions * sizeof(Transition_t));

    for (uint16_t i = trans_shift; i < (trans_shift + s->n_transitions); ++i) {
        ret.transitions[i].from += state_shift;
        ret.transitions[i].to += state_shift;
    }

    state_shift += s->n_states;
    trans_shift += s->n_transitions;

    SET_TRANSITION(&ret, trans_shift, 1 + state_shift - 1, 1 + state_shift - s->n_states, EPSILON);
    SET_TRANSITION(&ret, trans_shift + 1, 1 + state_shift - 1, 1 + state_shift, EPSILON);

    SET_TRANSITION(&ret, 1, 1, 1 + state_shift, EPSILON);

    SET_FINAL(&ret, 1 + state_shift, true);

    destroy(s);

    return ret;
}

/* Conversion - converts psudo-DFA into a strict DFA */

static inline bitset_t closure(DFA_t *dfa, bitset_t *states, char symbol) {
    bitset_t closure_ = *states;
    bool changed = true;

    while (changed) {
        changed = false;
        for (uint8_t i = 0; i < dfa->n_states; ++i) {
            if (!get(&closure_, i)) {
                continue;
            }
            
            for (uint16_t j = i; j < dfa->n_transitions; ++j) {
                if (dfa->transitions[i].from == i && dfa->transitions[i].on == symbol && !get(&closure_, dfa->transitions[i].to - 1)) {
                    set(states, dfa->transitions[i].to - 1);
                    changed = true;
                }
            }
        }
    }

    return closure_;
}

DFA_t strictify(DFA_t *dfa) {
    DFA_t ret = DFA(dfa->n_states, dfa->n_transitions);

    return ret;
}