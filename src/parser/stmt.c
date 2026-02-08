#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defs.h"
#include "data.h"
#include "decl.h"

static ASTnode *statement(void);
static ASTnode *block_statement(void);

// Required semicolon
static ASTnode *semicolon(ASTnode *(*func)(void))
{
    ASTnode *n = func();

    // Match the syntax
    match(T_SEMICOLON, ";");
    return n;
}

// Parse a variable declaration
static ASTnode *var_declaration(void)
{
    ASTnode *n = NULL, *left, *right;
    int id;
    char name[MAX_LEN];
    PType ptype;

    // Match the sintax
    match(T_VAR, "var");
    if (CurrentToken.type != T_IDENT)
    {
        fprintf(stderr, "Syntax Error: expected an identifier but found other token (%d:%d)\n", Line, Column);
        exit(1);
    }
    // Copy the name of the variable
    strncpy(name, CurrentToken.value.string, MAX_LEN);
    scan(&CurrentToken);

    // Match the sintax
    match(T_COLON, ":");

    // Parse type
    switch (CurrentToken.type)
    {
    case T_INT:
        ptype = P_INT;
        break;
    case T_BOOL:
        ptype = P_BOOL;
        break;
    default:
        fprintf(stderr, "Syntax Error: expected a type but another token was found (%d:%d)\n", Line, Column);
        exit(1);
    }
    scan(&CurrentToken);

    // Add the variable to the global symbol table
    id = addglob(name, S_VARIABLE, ptype, NO_MARKER);
    genglobsym(id);

    // Try to initialize the variable
    if (CurrentToken.type == T_ASSIGN)
    {
        scan(&CurrentToken);
        right = expression();
        left = mkastleaf(A_IDENT, ptype, (Value){id});
        n = mkastbinary(A_ASSIGN, left, right, NO_VALUE);
    }
    return n;
}

// Parse a variable declaration
static ASTnode *function_declaration(void)
{
    ASTnode *n, *child;
    int id, marker = NO_MARKER;
    char name[MAX_LEN];
    PType ptype = P_VOID;

    // Match the sintax
    match(T_FUN, "fun");
    if (CurrentToken.type != T_IDENT)
    {
        fprintf(stderr, "Syntax Error: expected an identifier but found other token (%d:%d)\n", Line, Column);
        exit(1);
    }
    // Copy the name of the variable
    strncpy(name, CurrentToken.value.string, MAX_LEN);
    scan(&CurrentToken);

    // Match the sintax
    match(T_LPAREN, "(");
    match(T_RPAREN, ")");

    // Add the variable to the global symbol table
    id = addglob(name, S_FUNCTION, ptype, marker);

    // Parse statement
    child = statement();

    // Create the AST
    n = mkastunary(A_FUNCTION, child, (Value){id});
    return n;
}

// Parse a variable assignment
static ASTnode *assignment_statement(void)
{
    int id;
    ASTnode *n, *left, *right;
    ASTnodeType type;

    // L-Value
    if (CurrentToken.type != T_IDENT)
    {
        fprintf(stderr, "Syntax Error: expected an identifier but found other token (%d:%d)\n", Line, Column);
        exit(1);
    }

    // Find the identifier in the global symbol table
    id = findglob(CurrentToken.value.string);
    if (id == -1)
    {
        fprintf(stderr, "Syntax Error: undeclared variable '%s' (%d:%d)\n", CurrentToken.value.string, Line, Column);
        exit(1);
    }

    left = mkastleaf(A_IDENT, GlobalSymbols[id].ptype, (Value){id});
    scan(&CurrentToken);

    // Match the sintax
    switch (CurrentToken.type)
    {
    case T_ASSIGN:
        type = A_ASSIGN;
        break;
    case T_ASPLUS:
        type = A_ASADD;
        break;
    case T_ASMINUS:
        type = A_ASSUB;
        break;
    case T_ASSTAR:
        type = A_ASMUL;
        break;
    case T_ASSLASH:
        type = A_ASDIV;
        break;
    case T_ASPERCENT:
        type = A_ASMOD;
        break;
    case T_ASDSTAR:
        type = A_ASPOW;
        break;
    case T_ASDAMPERSAND:
        type = A_ASAND;
        break;
    case T_ASDPIPE:
        type = A_ASOR;
        break;
    default:
        fprintf(stderr, "Syntax Error: Expected assignment operator (%d:%d)\n", Line, Column);
        exit(1);
    }
    scan(&CurrentToken);

    // Parse the expression
    right = expression();

    // Create the AST
    n = mkastbinary(type, left, right, NO_VALUE);
    return n;
}

// Parse a call
static ASTnode *call_statement(void)
{
    int id;
    ASTnode *n;

    // Match the syntax
    if (CurrentToken.type != T_IDENT)
    {
        fprintf(stderr, "Syntax Error: expected an identifier but found other token (%d:%d)\n", Line, Column);
        exit(1);
    }

    // Find the identifier in the global symbol table
    id = findglob(CurrentToken.value.string);
    if (id == -1)
    {
        fprintf(stderr, "Syntax Error: undeclared variable '%s' (%d:%d)\n", CurrentToken.value.string, Line, Column);
        exit(1);
    }

    // Create the AST
    n = mkastleaf(A_CALL, GlobalSymbols[id].ptype, (Value){id});
    scan(&CurrentToken);

    // Match the sintax
    match(T_LPAREN, "(");
    match(T_RPAREN, ")");
    return n;
}

// Parse an if-else statement
static ASTnode *ifelse_statement(void)
{
    ASTnode *n, *left, *mid, *right = NULL;

    // Match the syntax
    match(T_IF, "if");
    match(T_LPAREN, "(");

    // Parse the condition
    left = expression();
    if (left->ptype != P_BOOL)
    {
        fprintf(stderr, "Type Error: if condition must be boolean (%d:%d)", Line, Column);
        exit(1);
    }

    // Match the syntax
    match(T_RPAREN, ")");

    // Get the true statement
    mid = statement();

    // Check if there is an else statement
    if (CurrentToken.type == T_ELSE)
    {
        scan(&CurrentToken);
        right = statement();
    }

    // Create the AST
    n = mkastternary(A_IFELSE, left, mid, right, NO_VALUE);
    return n;
}

// Parse a match statement
static ASTnode *match_statement(void)
{
    ASTnode *n, *d = NULL, *left, *right = NULL;

    // Match the syntax
    match(T_MATCH, "match");
    match(T_LPAREN, "(");

    // Parse the condition
    left = expression();

    // Match the syntax
    match(T_RPAREN, ")");
    match(T_LBRACE, "{");

    // Get all the match cases
    while (CurrentToken.type != T_RBRACE)
    {
        ASTnode *c, *val, *body;

        // Get default case
        if (CurrentToken.type == T_UNDERSCORE)
        {
            scan(&CurrentToken);
            val = NULL;
        }
        // Get case
        else
        {
            val = expression();
        }

        // Match the syntax
        match(T_COLON, ":");

        // Get the body
        body = statement();

        // Make new case
        c = mkastternary(A_CASE, val, body, NULL, NO_VALUE);

        if (val == NULL)
        {
            if (d != NULL)
            {
                fprintf(stderr, "Syntax Error: multiple defaults in match\n");
            }
            d = c;
        }
        else
        {
            // Link case with other
            if (right == NULL)
            {
                right = c;
            }
            else
            {
                n->right = c;
            }
            n = c;
        }
    }

    // Match the syntax
    match(T_RBRACE, "}");

    // Add default case
    if (d != NULL)
    {
        if (right == NULL)
        {
            right = d;
        }
        else
        {
            n->right = d;
        }
    }

    // Create the AST
    n = mkastbinary(A_MATCH, left, right, NO_VALUE);
    return n;
}

// Parse a loop statement
static ASTnode *loop_statement(void)
{
    ASTnode *n, *left, *right;
    ASTnode *init = NULL, *upd = NULL;

    // Match the syntax
    match(T_LOOP, "loop");

    // Infinite loop
    if (CurrentToken.type != T_LPAREN)
    {
        left = mkastleaf(A_TRUE, P_BOOL, (Value){1});

        // Parse statement
        right = statement();

        // Create the AST
        n = mkastternary(A_LOOP, left, right, NULL, NO_VALUE);
        return n;
    }

    // Match the syntax
    match(T_LPAREN, "(");

    if (CurrentToken.type == T_SEMICOLON) // For loop without init
    {
        scan(&CurrentToken);
    }
    else if (CurrentToken.type == T_VAR) // For loop with var declaration
    {
        // Parse var declaration
        init = semicolon(var_declaration);
    }
    else if (CurrentToken.type == T_IDENT)
    {
        int next = peek();

        if (next == T_ASSIGN || next == T_ASPLUS || next == T_ASMINUS || next == T_ASSTAR || next == T_ASSLASH || next == T_ASPERCENT || next == T_ASDSTAR || next == T_ASDAMPERSAND || next == T_ASDPIPE) // For loop with initiation
        {
            // Parse var assignment
            init = semicolon(assignment_statement);
        }
        else // While loop
        {
            // Read an expression
            left = expression();
            // Match the syntax
            match(T_RPAREN, ")");
            // Parse statement
            right = statement();
            // Create the AST
            n = mkastternary(A_LOOP, left, right, NULL, NO_VALUE);
            return n;
        }
    }
    else // While loop
    {
        // Read an expression
        left = expression();
        // Match the syntax
        match(T_RPAREN, ")");
        // Parse statement
        right = statement();
        // Create the AST
        n = mkastternary(A_LOOP, left, right, NULL, NO_VALUE);
        return n;
    }

    // Parse condition
    if (CurrentToken.type != T_SEMICOLON)
    {
        left = semicolon(expression);
        if (left->ptype != P_BOOL)
        {
            fprintf(stderr, "Type Error: loop condition must be boolean\n");
            exit(1);
        }
    }
    else
    {
        left = mkastleaf(A_TRUE, P_BOOL, (Value){1});
        match(T_SEMICOLON, ";");
    }

    // Parse update
    if (CurrentToken.type != T_RPAREN)
    {
        upd = assignment_statement();
    }
    match(T_RPAREN, ")");

    // Parse statement
    right = statement();

    // Create the AST
    n = mkastternary(A_LOOP, left, right, upd, NO_VALUE);
    if (init != NULL)
    {
        n = mkastbinary(A_SEQ, init, n, NO_VALUE);
    }
    return n;
}

// Parse a stop statement
static ASTnode *stop_statement(void)
{
    // Match the syntax
    match(T_STOP, "stop");
    return mkastleaf(A_STOP, NO_PRIM, NO_VALUE);
}

// Parse a jump statement
static ASTnode *jump_statement(void)
{
    // Match the syntax
    match(T_NEXT, "next");
    return mkastleaf(A_NEXT, NO_PRIM, NO_VALUE);
}

// TODO: replace with std library
static ASTnode *print_statement(void)
{
    ASTnode *expr;

    // Match syntax
    match(T_PRINT, "print");
    match(T_LPAREN, "(");

    // Parse an expression
    expr = expression();

    // Match the sintax
    match(T_RPAREN, ")");
    return mkastunary(A_PRINT, expr, NO_VALUE);
}

// Parse a statement
static ASTnode *statement(void)
{
    switch (CurrentToken.type)
    {
    case T_IDENT:
    {
        int id = findglob(CurrentToken.value.string);

        if (id == -1)
        {
            fprintf(stderr, "Syntax Error: undeclared identifier '%s' (%d:%d)", CurrentToken.value.string, Line, Column);
            exit(1);
        }
        switch (GlobalSymbols[id].stype)
        {
        case S_VARIABLE:
            return semicolon(assignment_statement);

        case S_FUNCTION:
            return semicolon(call_statement);
        default:
            fprintf(stderr, "Syntax Error: unexpected token (%d:%d)\n", Line, Column);
            exit(1);
        }
    }
    case T_VAR:
        return semicolon(var_declaration);
    case T_FUN:
        return function_declaration();
    case T_IF:
        return ifelse_statement();
    case T_MATCH:
        return match_statement();
    case T_LOOP:
        return loop_statement();
    case T_STOP:
        return semicolon(stop_statement);
    case T_NEXT:
        return semicolon(jump_statement);
    case T_PRINT:
        return semicolon(print_statement);
    case T_LBRACE:
        return block_statement();
    default:
        fprintf(stderr, "Syntax Error: unexpected token (%d:%d)\n", Line, Column);
        exit(1);
    }
}

// Parse a block statement
static ASTnode *block_statement(void)
{
    ASTnode *stmt, *seq = NULL;

    // Match syntax
    match(T_LBRACE, "{");

    // Parse the statements
    while (CurrentToken.type != T_RBRACE && CurrentToken.type != T_EOF)
    {
        stmt = statement();

        if (stmt)
        {
            if (seq == NULL)
            {
                seq = stmt;
            }
            else
            {
                seq = mkastbinary(A_SEQ, seq, stmt, NO_VALUE);
            }
        }
    }

    // Match syntax
    match(T_RBRACE, "}");
    return seq;
}

// Parse multiple statements
ASTnode *compound_statement(void)
{
    ASTnode *stmt, *seq = NULL;

    // Parse the statements
    while (CurrentToken.type != T_EOF)
    {
        stmt = statement();

        if (stmt)
        {
            if (seq == NULL)
            {
                seq = stmt;
            }
            else
            {
                seq = mkastbinary(A_SEQ, seq, stmt, NO_VALUE);
            }
        }
    }
    return seq;
}
