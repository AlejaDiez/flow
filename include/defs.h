#ifndef DEFS_H
#define DEFS_H

#define MAX_LEN 512
#define MAX_SYM 256
#define NO_VALUE (Value){0}
#define NO_REG -1
#define NO_PRIM -1

// Values
typedef union Value
{
    int integer;
    char *string;
} Value;

// Types
typedef enum PType
{
    P_VOID,
    P_INT,
    P_BOOL
} PType;

// Tokens
typedef enum TokenType
{
    T_EOF,
    // Arithmetic Operators
    T_PLUS,
    T_MINUS,
    T_STAR,
    T_SLASH,
    // Comparison Operators
    T_EQ,
    T_NEQ,
    T_LT,
    T_LE,
    T_GT,
    T_GE,
    // Logical Operators
    T_AND,
    T_OR,
    T_NOT,
    // Assignment Operators
    T_ASSIGN,
    // Literals
    T_IDENT,
    T_INTLIT,
    T_TRUE,
    T_FALSE,
    // Types
    T_INT,
    T_BOOL,
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
    // Comparison Operations
    A_EQ,
    A_NEQ,
    A_LT,
    A_LE,
    A_GT,
    A_GE,
    // Logical Operations
    A_AND,
    A_OR,
    A_NOT,
    // Assignment Operations
    A_ASSIGN,
    // Literals
    A_IDENT,
    A_INTLIT,
    A_TRUE,
    A_FALSE,
    // Statements
    A_PRINT,
    // Grouping
    A_SEQ
} ASTnodeType;

typedef struct ASTnode
{
    ASTnodeType type;
    PType ptype;
    struct ASTnode *left;
    struct ASTnode *right;
    Value value;
} ASTnode;

// Symbol
typedef struct SymTable
{
    char name[MAX_LEN];
    PType type;
} SymTable;

#endif