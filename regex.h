#ifndef REGEX_H
#define REGEX_H


void insert_implicit_concat(const char *regex, char *output);

char *regex_to_postfix(const char *regex);

#endif