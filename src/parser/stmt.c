#include <stdio.h>
#include <stdlib.h>

#include "defs.h"
#include "data.h"
#include "decl.h"

// Parse a variable assignment
static ASTnode *assignment_statement(void)
{
    ASTnode *n, *left, *right;
    int id;

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
    left = mkastleaf(A_IDENT, (Value){id});
    scan(&CurrentToken);

    // Match the sintax
    match(T_ASSIGN, "=");

    // Parse the expression
    right = expression();

    // Create the AST
    n = mkastbinary(A_ASSIGN, left, right, NO_VALUE);
    return n;
}

// Parse a print statement
static ASTnode *print_statement(void)
{
    ASTnode *expr;

    // Match syntax
    match(T_PRINT, "print");
    match(T_LPAREN, "(");

    // Parse an expression
    expr = expression();

    // Match syntax
    match(T_RPAREN, ")");

    return mkastunary(A_PRINT, expr, NO_VALUE);
}

// Parse a statement
static ASTnode *statement(void)
{
    ASTnode *stmt = NULL;

    switch (CurrentToken.type)
    {
    case T_IDENT:
        stmt = assignment_statement();
        break;
    case T_VAR:
        var_declaration();
        break;
    case T_PRINT:
        stmt = print_statement();
        break;
    default:
        fprintf(stderr, "Syntax Error: unexpected token (%d:%d)\n", Line, Column);
        exit(1);
    }

    match(T_SEMICOLON, ";");
    return stmt;
}

// Parse multiple statements
ASTnode *compound_statement(void)
{
    ASTnode *seq = NULL;
    ASTnode *stmt;

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
                seq = mkastnode(A_SEQ, seq, stmt, NO_VALUE);
            }
        }
    }
    return seq;
}
