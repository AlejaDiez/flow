/********************************************************************************
 * File Name: defs.h                                                            *
 * Description: This file contains all the definitions used.                    *
 ********************************************************************************/

#include <stdio.h>
#include <sysexits.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define MAX_LEN 256
#define MAX_SYM 1024
#define NO_PRIMITIVE -1
#define NO_VALUE \
    (VALUE) { .pointer = NULL }
#define NO_REG -1
#define NO_LABEL -1

// Values
typedef union VALUE
{
    int integer;
    char *string;
    void *pointer;
} VALUE;

// Token
typedef enum TOKEN_TYPE
{
    T_EOF,
    // Arithmetic Operators
    T_PLUS,
    T_MINUS,
    T_STAR,
    T_SLASH,
    T_PERCENT,
    // Relational Operators
    T_EQUAL,
    T_NOT_EQUAL,
    T_LESS,
    T_LESS_EQUAL,
    T_GREATER,
    T_GREATER_EQUAL,
    // Assignment Operators
    T_ASSIGN,
    // Literals and Identifiers
    T_INTLIT,
    T_IDENT,
    // Types
    T_VOID,
    T_BOOL,
    T_INT,
    // Keywords
    T_VAR,
    T_FUN,
    T_IF,
    T_ELSE,
    T_LOOP,
    T_PRINT,
    // Punctuation
    T_COLON,
    T_SEMICOLON,
    // Gruping
    T_LPAREN,
    T_RPAREN,
    T_LBRACE,
    T_RBRACE
} TOKEN_TYPE;

static const char *const TOKEN_STR[] = {
    "eof",
    // Arithmetic Operators
    "+",
    "-",
    "*",
    "/",
    "%",
    // Relational Operators
    "==",
    "!=",
    "<",
    "<=",
    ">",
    ">=",
    // Assignment Operators
    "=",
    // Literals and Identifiers
    "integer",
    "identifier",
    // Types
    "void",
    "bool",
    "int",
    // Keywords
    "var",
    "fun",
    "if",
    "else",
    "loop",
    "print",
    // Punctuation
    ":",
    ";",
    // Gruping
    "(",
    ")",
    "{",
    "}"};

typedef struct TOKEN
{
    TOKEN_TYPE type;
    VALUE value;
} TOKEN;

// Primitive Types
typedef enum PRIMITIVE_TYPE
{
    P_VOID,
    P_BOOL,
    P_INT
} PRIMITIVE_TYPE;

// Abstract Syntax Tree
typedef enum AST_TYPE
{
    // Literals and Identifiers
    A_INTLIT,
    A_IDENT,
    // Arithmetic Operations
    A_ADD,
    A_SUB,
    A_MUL,
    A_DIV,
    A_MOD,
    // Relational Operations
    A_EQ,
    A_NE,
    A_LT,
    A_LE,
    A_GT,
    A_GE,
    // Assignment Operations
    A_ASSIGN,
    // Statements
    A_IFELSE,
    A_LOOP,
    A_PRINT,
    // Grouping
    A_SEQ,
    A_PAREN
} AST_TYPE;

typedef struct AST
{
    AST_TYPE type;
    PRIMITIVE_TYPE primitive;
    struct AST *left;
    struct AST *middle;
    struct AST *right;
    VALUE value;
} AST;

// Symbol
typedef enum SYMBOL_TYPE
{
    S_VARIABLE,
    S_FUNCTION
} SYMBOL_TYPE;

typedef struct SYMBOL
{
    SYMBOL_TYPE type;
    char *name;
    PRIMITIVE_TYPE primitive;
} SYMBOL;
