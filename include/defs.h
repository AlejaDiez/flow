#ifndef DEFS_H
#define DEFS_H

#define NO_VALUE (Value){0}
#define NO_REG -1

// Values
typedef union Value
{
    int integer;
} Value;

// Tokens
typedef enum TokenType
{
    T_EOF,
    // Arithmetic Operators
    T_PLUS,
    T_MINUS,
    T_STAR,
    T_SLASH,
    // Literals
    T_INTLIT
} TokenType;

typedef struct Token
{
    TokenType type;
    Value value;
} Token;

// Abstract Syntax Tree
typedef enum ASTnodeType
{
    // Arithmetic Operations
    A_ADD,
    A_SUB,
    A_MUL,
    A_DIV,
    // Literals
    A_INTLIT
} ASTnodeType;

typedef struct ASTnode
{
    ASTnodeType type;
    struct ASTnode *left;
    struct ASTnode *right;
    Value value;
} ASTnode;

#endif