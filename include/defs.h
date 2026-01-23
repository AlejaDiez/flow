#ifndef DEFS_H
#define DEFS_H

#define MAX_LEN 512
#define MAX_SYM 256
#define NO_VALUE (Value){0}
#define NO_REG -1

// Values
typedef union Value
{
    int integer;
    char *string;
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
    // Assignment Operators
    T_ASSIGN,
    // Literals
    T_IDENT,
    T_INTLIT,
    // Types
    T_INT,
    // Keywords
    T_VAR,
    T_PRINT,
    // Punctuation
    T_COLON,
    T_SEMICOLON,
    // Gruping
    T_LPAREN,
    T_RPAREN,
    T_LBRACE,
    T_RBRACE
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
    // Assignment Operations
    A_ASSIGN,
    // Literals
    A_IDENT,
    A_INTLIT,
    // Statements
    A_PRINT,
    // Grouping
    A_SEQ
} ASTnodeType;

typedef struct ASTnode
{
    ASTnodeType type;
    struct ASTnode *left;
    struct ASTnode *right;
    Value value;
} ASTnode;

// Symbol
typedef struct SymTable
{
    char name[MAX_LEN];
} SymTable;

#endif