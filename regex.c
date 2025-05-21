#include "regex.h"
#include "bitset.h"
#include "dfa.h"

#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
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
        case '{':
            return 3;
        case '[':
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

            if (current == '[') {
                char *closing = strchr(regex + i + 1, ']');
                if (closing != NULL) {
                    for (int j = i + 1; j < (i + (closing - (regex + i)) + 1); ++j) {
                        output[outpos++] = regex[j];
                    }
                    i += closing - (regex + i);
                }
            }
            
            bool quantifier_next = false;

            if (regex[i + 1] == '{') {
                unsigned int m, n, p, q;
                q = sscanf(regex + i + 1, "{%u,%u}%n", &m, &n, &p);
                if (p > 0 && q == 2) {
                    quantifier_next = true;
                    for (int j = i + 1; j < (i + p); ++j) {
                        output[outpos++] = regex[j];
                    }
                    i += p - 1;
                }
            }
            
            next = regex[i + 1];

            // casos que precisa inserir a concat
            if ((current != '|') && (current != '(') && (next != '|') && (next != ')') && (next != '*') && !quantifier_next && i != len - 1) {
                output[outpos++] = '.';
            }
            
            quantifier_next = false;
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
            case '(': {
                stack[stack_pos++] = c;
                break;
            }
            case ')': {
                while (stack_pos > 0 && stack[stack_pos - 1] != '(') {
                    postfix[postfix_pos++] = stack[--stack_pos];
                }
                if (stack_pos > 0) {
                    stack_pos--;
                }
                break;
            }
            case '[': {
                char *closing = strchr(modified_regex + i, ']');
                if (closing != NULL) {
                    for (int j = i; j < (i + (closing - (modified_regex + i)) + 1); ++j) {
                        postfix[postfix_pos++] = modified_regex[j];
                    }
                    i += closing - (modified_regex + i);
                } else {
                    postfix[postfix_pos++] = c;
                }
                break;
            }
            case '|':
            case '.':
            case '*': {
                while (stack_pos > 0 && precedence(stack[stack_pos - 1]) >= precedence(c)) {
                    if (stack[stack_pos - 1] == '{') {
                        postfix[postfix_pos++] = stack[--stack_pos];
                        while (stack_pos > 0 && stack[stack_pos - 1] != '}') {
                            postfix[postfix_pos++] = stack[--stack_pos];
                        }
                        if (stack_pos > 0) {
                            postfix[postfix_pos++] = stack[--stack_pos];
                        }
                    } else {
                        postfix[postfix_pos++] = stack[--stack_pos];
                    }
                }
                stack[stack_pos++] = c;
                break;
            }
            case '{': {
                unsigned int m, n, p, q;
                q = sscanf(modified_regex + i, "{%u,%u}%n", &m, &n, &p);
                if (p > 0 && q == 2) {
                    while (stack_pos > 0 && precedence(stack[stack_pos - 1]) >= precedence(c)) {
                        postfix[postfix_pos++] = stack[--stack_pos];
                    }
                    char buf[16];
                    stack[stack_pos++] = '}';
                    sprintf(buf, "%u%n", n, &q);
                    for (int j = q - 1; j >= 0; --j) {
                        stack[stack_pos++] = buf[j];
                    }
                    stack[stack_pos++] = ',';
                    sprintf(buf, "%u%n", m, &q);
                    for (int j = q - 1; j >= 0; --j) {
                        stack[stack_pos++] = buf[j];
                    }
                    stack[stack_pos++] = '{';

                    i += p - 1;
                } else {
                    postfix[postfix_pos++] = c;
                }
                break;
            }
            default: {
                postfix[postfix_pos++] = c; // adiciona o caractere ao resultado
                break;
            }
        }
    }

    while (stack_pos > 0) { // esvazia o stack
        postfix[postfix_pos++] = stack[--stack_pos];
    }
    postfix[postfix_pos] = '\0'; // finaliza a string
    return postfix;
}

DFA_t character_class(const char *regex) {
    bitset_t chars = bitset();
    bool negative = false;
    size_t len = strlen(regex);
    size_t i = 0;
    ++i;

    if (regex[i] == '^') {
        negative = true;
        ++i;
    }
    while (i < len && regex[i] != ']') {
        if (regex[i + 1] == '-' && (i + 2) < len && isalnum(regex[i]) && isalnum(regex[i + 2])) {
            for (uint8_t j = regex[i]; j <= regex[i + 2]; ++j) {
                set(&chars, j);
            }
            i += 3;
        } else {
            set(&chars, regex[i]);
            ++i;
        }
    }

    if (negative) {
        for (uint8_t i = 4; i < 32; ++i) {
            chars.bits[i] ^= 0xFF;
        }
    }

    uint8_t ones = 0;
    for (uint8_t i = 4; i < 32; ++i) {
        uint8_t byte = chars.bits[i];
        while (byte > 0) {
            if ((byte & 1) == 1) {
                ++ones;
            }
            byte >>= 1;
        }
    }

    DFA_t ret = DFA(2, ones);
    uint16_t t = 0;

    for (uint16_t i = 32; i < 256; ++i) {
        if (get(&chars, i) == true) {
            SET_TRANSITION(&ret, t, 1, 2, i);
            ++t;
        }
    }

    SET_FINAL(&ret, 2, true);

    return ret;
}

DFA_t quantifier(DFA_t *s, uint8_t m, uint8_t n) {
    DFA_t ret;

    if (m > 0) {
        ret = copy(s);
    }
    for (uint8_t i = 1; i < m; ++i) {
        ret = concat(&ret, s, false);
    }

    DFA_t optional_s = singleton(EPSILON);
    optional_s = join(&optional_s, s, true);

    if (m == 0 && n > 0) {
        ret = copy(&optional_s);
    }
    for (uint8_t i = m + (m == 0 ? 1 : 0); i < n; ++i) {
        ret = concat(&ret, &optional_s, false);
    }

    destroy(&optional_s);

    return ret;
}

DFA_t regex_to_dfa(const char *regex) {
    DFA_t stack[8];
    int stackpos = 0;

    for (int i = 0; regex[i] != '\0'; ++i) {
        char c = regex[i];

        switch (c) {
            case '[': {
                DFA_t item;
                char *closing = strchr(regex + i + 1, ']');
                if (closing != NULL) {
                    item = character_class(regex + i);
                    i += closing - (regex + i);
                } else {
                    item = singleton(c);
                }
                stack[stackpos++] = item;
                if (stackpos > 7) {
                    stackpos = 0;
                }
                break;
            }
            case '|': {
                stack[stackpos - 2] = join(&stack[stackpos - 2], &stack[stackpos - 1], true);
                --stackpos;
                break;
            }
            case '.': {
                stack[stackpos - 2] = concat(&stack[stackpos - 2], &stack[stackpos - 1], true);
                --stackpos;
                break;
            }
            case '*': {
                stack[stackpos - 1] = kleene(&stack[stackpos - 1]);
                break;
            }
            case '{': {
                unsigned int m, n, p, q;
                q = sscanf(regex + i, "{%u,%u}%n", &m, &n, &q);
                if (p > 0 && q == 2) {
                    stack[stackpos - 1] = quantifier(&stack[stackpos - 1], m, n);
                    i += p - 1;
                } else {
                    stack[stackpos++] = singleton(c);
                    if (stackpos > 7) {
                        stackpos = 0;
                    }
                }
                break;
            }
            default: {
                stack[stackpos++] = singleton(c);
                if (stackpos > 7) {
                    stackpos = 0;
                }
            }
        }
    }

    return stack[stackpos - 1];
}