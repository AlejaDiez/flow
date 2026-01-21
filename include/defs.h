#ifndef DEFS_H
#define DEFS_H

#define NO_VALUE (Value){0}

// Values
typedef union
{
    int integer;
} Value;

// Tokens
typedef enum
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

typedef struct
{
    TokenType type;
    Value value;
} Token;

#endif