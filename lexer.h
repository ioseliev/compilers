#ifndef LEXER_H
#define LEXER_H


typedef enum {
    /* Keywords */
    KW_NUM,  // num
    KW_TEXT, // text
    KW_SHOW, // show
    
    /* Literals */
    NUM_LITERAL,  // [0-9][0-9]*
    TEXT_LITERAL, // \"[^\"]*\"

    /* Identifiers */
    /* NOTE: It's not specified whether identifiers can contain reserved sym- 
     * bols like numbers or operators and whether can begin with them. As a
     * result, a conservative interpretation was taken.
     */
    ID, // [a-zA-Z][a-zA-Z!@#$%&?/_|]{0,29}

    /* Operators */
    OP_ADD,    // \+
    OP_SUB,    // -
    OP_MUL,    // \*
    OP_DIV,    // /
    OP_LT,     // <
    OP_GT,     // >
    OP_ASSIGN, // =

    /* Punctuation */
    SEMICOLON, // ;
} TokenType;

#endif