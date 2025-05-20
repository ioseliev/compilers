#ifndef REGEX_H
#define REGEX_H
#include "dfa.h"


void insert_implicit_concat(const char *regex, char *output);

char *regex_to_postfix(const char *regex);

DFA_t character_class(const char *regex);

DFA_t regex_to_dfa(const char *regex);

#endif