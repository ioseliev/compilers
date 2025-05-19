#include "regex.h"

#include <stdlib.h>
#include <string.h>


//inicio do codigo do parser de regex

static inline int precedence(char operator) {
    switch (operator) {
        case '|':
            return 1;
        case '.':       // simboliza a concatenacao implicita na regex
            return 2;
        case '*':
            return 3;
        default:
            return 0;
    }
}

void insert_implicit_concat(const char *regex, char *output) {
    int outpos = 0; // posição atual no output
    int len = strlen(regex); // length da regex

    for (int i = 0; i < len; i++) {
        output[outpos++] = regex[i];

        // Se o caractere atual não é o último
        if (i < len - 1) {
            char current = regex[i];
            char next = regex[i + 1];

            // casos que precisa inserir a concat
            if ((current != '|') && (current != '(') && (next != '|') && (next != ')') && (next != '*')) {
                output[outpos++] = '.';
            }
        }
    }
    output[outpos] = '\0'; // finaliza a string
}

char *regex_to_postfix(const char *regex) {
    char modified_regex[200] = {0}; // se for usar regex muito grande, aumentar o tamanho
    insert_implicit_concat(regex, modified_regex); 

    char *postfix =  malloc(200 * sizeof(char));
    char stack[200];
    int postfix_pos = 0;
    int stack_pos = 0;

    for (int i = 0; modified_regex[i] != '\0'; i++) {
        char c = modified_regex[i];

        switch (c) { // adiciona operadores ao stack e operandos ao resultado
            case '(': 
                stack[stack_pos++] = c;
                break;

            case ')': 
                while (stack_pos > 0 && stack[stack_pos - 1] != '(') {
                    postfix[postfix_pos++] = stack[--stack_pos];
                }
                if (stack_pos > 0) {
                    stack_pos--;
                }
                break;

            case '|':
            case '.':
            case '*':
                while (stack_pos > 0 && precedence(stack[stack_pos - 1]) >= precedence(c)) {
                    postfix[postfix_pos++] = stack[--stack_pos];
                }
                stack[stack_pos++] = c;
                break;

            default:
                postfix[postfix_pos++] = c; // adiciona o caractere ao resultado
                break;
        }
    }

    while (stack_pos > 0) { // esvazia o stack
        postfix[postfix_pos++] = stack[--stack_pos];
    }
    postfix[postfix_pos] = '\0'; // finaliza a string
    return postfix;
}