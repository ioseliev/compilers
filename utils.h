#ifndef UTILS_H
#define UTILS_H


char *read_from_stdin();

int precedence(char option);

void insert_implicit_concat(char *regex, char *output);

char* regex_to_postfix(char *regex);

#endif