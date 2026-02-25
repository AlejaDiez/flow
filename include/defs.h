/********************************************************************************
 * File Name: include/defs.h                                                    *
 *                                                                              *
 * Description: Defines macros, enums, unions, and structs  for the Flow        *
 *              language compiler.                                              *
 * Author: Alejandro Diez Bermejo                                               *
 * Date: 2026-01-01                                                             *
 * Version: 0.0.0                                                               *
 ********************************************************************************/

#ifndef DEFS_H
#define DEFS_H

// Define macros
#define NO_VALUE \
    (Value) { .symbol = NULL } // Empty value initializer
#define NO_REG -1              // No register indicator
#define NO_LABEL -1            // No label indicator
#define MAX_LEN 512            // Max text characters

// Token types
typedef enum TokenType
{
    T_EOF,

    // Arithmetic Operators
    T_PLUS,
    T_MINUS,
    T_STAR,
    T_DSTAR,
    T_SLASH,
    T_PERCENT,

    // Comparison Operators
    T_EQ,
    T_NEQ,
    T_LT,
    T_LE,
    T_GT,
    T_GE,

    // Logical Operators
    T_DAMPERSAND,
    T_DPIPE,
    T_BANG,

    // Assignment Operators
    T_ASSIGN,
    T_ASPLUS,
    T_ASMINUS,
    T_ASSTAR,
    T_ASSLASH,
    T_ASPERCENT,
    T_ASDSTAR,
    T_ASDAMPERSAND,
    T_ASDPIPE,

    // Literals and Identifiers
    T_IDENT,
    T_INTLIT,
    T_TRUE,
    T_FALSE,
    T_UNDERSCORE,

    // Types
    T_VOID,
    T_INT,
    T_BOOL,

    // Keywords
    T_CONST,
    T_VAR,
    T_FUN,
    T_RETURN,
    T_IF,
    T_ELSE,
    T_MATCH,
    T_LOOP,
    T_STOP,
    T_NEXT,
    T_PRINT,

    // Punctuation
    T_COLON,
    T_SEMICOLON,
    T_COMMA,
    T_LPAREN,
    T_RPAREN,
    T_LBRACE,
    T_RBRACE

} TokenType;

// Token types strings
static const char *const TokenTypeStr[] = {
    "eof",
    // Operators
    "+",
    "-",
    "*",
    "/",
    "%",
    "**",
    // Comparison
    "==",
    "!=",
    "<",
    "<=",
    ">",
    ">=",
    // Logicians
    "&&",
    "||",
    "!",
    // Assignment
    "=",
    "+=",
    "-=",
    "*=",
    "/=",
    "%=",
    "**=",
    "&&=",
    "||=",
    // Literals and Identifiers
    "identifier",
    "integer",
    "true",
    "false",
    "_",
    // Types
    "void",
    "int",
    "bool",
    // Keywords
    "const",
    "var",
    "fun",
    "return",
    "if",
    "else",
    "match",
    "loop",
    "stop",
    "next",
    "print",
    // Punctuation
    ":",
    ";",
    ",",
    "(",
    ")",
    "{",
    "}"};

// Primitive types
typedef enum PType
{
    P_VOID,
    P_INT,
    P_BOOL
} PType;

// Abstract Syntax Tree node types
typedef enum ASTnodeType
{
    // Arithmetic
    A_POS,
    A_NEG,
    A_ADD,
    A_SUB,
    A_MUL,
    A_DIV,
    A_MOD,
    A_POW,

    // Comparison
    A_EQ,
    A_NEQ,
    A_LT,
    A_LE,
    A_GT,
    A_GE,

    // Logical
    A_AND,
    A_OR,
    A_NOT,

    // Assignment
    A_ASSIGN,
    A_ASADD,
    A_ASSUB,
    A_ASMUL,
    A_ASDIV,
    A_ASMOD,
    A_ASPOW,
    A_ASAND,
    A_ASOR,

    // Leaves
    A_IDENT,
    A_INTLIT,
    A_TRUE,
    A_FALSE,

    // Control Flow & Structure
    A_FUNCTION,
    A_CALL,
    A_RETURN,
    A_IFELSE,
    A_MATCH,
    A_CASE,
    A_LOOP,
    A_STOP,
    A_NEXT,
    A_PRINT,
    A_GLUE
} ASTnodeType;

// Structural types
typedef enum SType
{
    S_CONSTANT,
    S_VARIABLE,
    S_FUNCTION
} SType;

// Storage classes
typedef enum SClass
{
    C_GLOBAL,
    C_LOCAL,
    C_PARAM
} SClass;

// Symbol Table entry
typedef struct Symbol
{
    char *name;    // Name of the symbol
    SType stype;   // Structural type
    PType ptype;   // Primitive type
    SClass sclass; // Storage class

    int size;   // Size in bytes
    int offset; // Stack offset or label

    int numParams;         // Number of function parameters
    struct Symbol *params; // Function parameters

    struct Symbol *next; // Pointer to the next symbol in the list
} Symbol;

// Generic value union
typedef union Value
{
    int integer;
    char *string;
    Symbol *symbol;
} Value;

// Token structure
typedef struct Token
{
    TokenType type;
    Value value;
} Token;

// Abstract Syntax Tree node
typedef struct ASTnode
{
    ASTnodeType type;      // Type of the node (A_ADD, A_IF, etc.)
    PType ptype;           // Primitive type this node results in
    struct ASTnode *left;  // Left child
    struct ASTnode *mid;   // Middle child (e.g., 'then' block)
    struct ASTnode *right; // Right child (e.g., 'else' block)
    Value value;           // Value of the node
} ASTnode;

// Control Stack entry
typedef struct Control
{
    int start_label;
    int end_label;
    struct Control *next;
} Control;

// Scope Stack entry
typedef struct Scope
{
    struct Symbol *head;  // Head of symbols in this scope
    struct Symbol *tail;  // Tail of symbols in this scope
    struct Scope *parent; // Pointer to the enclosing scope
    int level;            // 0 = Global, 1+ = Local/Block
    int pending_offset;   // Tracks stack offset allocation for this specific scope
} Scope;

// Backend interface
typedef struct Backend
{
    // Register management
    void (*freeall_registers)(void);
    void (*free_register)(int);
    int (*alloc_register)(void);
    // Sections
    void (*data_seg)(void (*gen)(Symbol *), Symbol *);
    void (*text_seg)(int (*gen)(ASTnode *), ASTnode *);
    void (*end)(void);
    // Global variables
    void (*globsym)(Symbol *);
    // Functions
    void (*genfunlabel)(char *);
    void (*preamble)(int);
    void (*postamble)(int);
    void (*call)(char *);
    void (*ret)(int);
    // Loading & Storing
    int (*load_int)(int);
    int (*load_glob)(Symbol *);
    int (*store_glob)(int, Symbol *);
    int (*load_local)(int);
    int (*store_local)(int, int);
    void (*store_param)(int, int);
    void (*load_arg)(int, int);
    void (*store_result)(int);
    // Control flow
    int (*label)(void);
    void (*genlabel)(int);
    void (*jump)(int);
    void (*jump_cond)(int, int);
    // Arithmetic operations
    int (*neg)(int);
    int (*add)(int, int);
    int (*sub)(int, int);
    int (*mul)(int, int);
    int (*div)(int, int);
    int (*mod)(int, int);
    int (*pow)(int, int);
    // Logic operations
    int (*not)(int);
    int (*and)(int, int);
    int (*or)(int, int);
    // Comparison operations
    int (*cmp)(int, int, char *);
    // Print
    void (*print)(int);
} Backend;

#endif
