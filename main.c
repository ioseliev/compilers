#include "utils.h"
#include "regex.h"
#include "dfa.h"
#include "token.h"

#include <stdio.h>
#include <stdlib.h>


static const struct { TokenType token; const char *regex; } EXPRESSIONS[] = {
    /* Keywords */
    { KW_NUM,  "num" },
    { KW_TEXT, "text" },
    { KW_SHOW, "show" },
 
    /* Literals */
    { NUM_LITERAL, "[0-9][0-9]*" },
    { TEXT_LITERAL, "\"[^\"]*\"" },

    /* Identifiers */
    /* NOTE: It's not specified whether identifiers can contain reserved sym- 
     * bols like numbers or operators and whether can begin with them. As a
     * result, a conservative interpretation was taken.
     */
    { ID, "[a-zA-Z][a-zA-Z!@#$%&?/_|]{0,29}" },

    /* Operators */
    { OP_ADD, "+" },
    { OP_SUB, "-" },
    { OP_MUL, "[*]" },
    { OP_DIV, "/" },
    { OP_LT, "<" },
    { OP_GT, ">" },
    { OP_ASSIGN, "=" },

    /* Punctuation */
    { SEMICOLON, ";" }
};

int main(int argc, char **argv) {
    /* const char *source = NULL;

    if (argc == 1) {
        printf("Usage: %s filename\n", argv[0]);
        return 1;
    }

    if ((source = read_from_file(argv[1])) == NULL) {
        fprintf(stderr, "File `%s` not found.\n", argv[1]);
        return 1;
    } */
    
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

    //free((void *) source);

    return 0;
}