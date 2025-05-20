#ifndef TOKEN_H
#define TOKEN_H


typedef enum {
    KW_NUM,
    KW_TEXT,
    KW_SHOW,
    NUM_LITERAL,
    TEXT_LITERAL,
    ID,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_LT,
    OP_GT,
    OP_ASSIGN,
    SEMICOLON,
} TokenType;

#endif