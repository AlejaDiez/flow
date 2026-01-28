#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defs.h"
#include "data.h"
#include "decl.h"

// Type to primary type
static PType ptype(TokenType tokentype)
{
    switch (tokentype)
    {
    case T_INT:
        return P_INT;
    case T_BOOL:
        return P_BOOL;
    default:
        fprintf(stderr, "Syntax Error: expected a type but another token was found (%d:%d)\n", Line, Column);
        exit(1);
    }
}

// Parse a variable declaration
static ASTnode *var_declaration(void)
{
    ASTnode *n = NULL, *left, *right;
    int id;
    char name[MAX_LEN];
    PType type;

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
    type = ptype(CurrentToken.type);
    scan(&CurrentToken);

    // Add the variable to the global symbol table
    id = addglob(name, type);
    genglobsym(id);

    // Try to initialize the variable
    if (CurrentToken.type == T_ASSIGN)
    {
        scan(&CurrentToken);
        right = expression();
        left = mkastleaf(A_IDENT, type, (Value){id});
        n = mkastbinary(A_ASSIGN, left, right, NO_VALUE);
    }
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

    left = mkastleaf(A_IDENT, GlobalSymbols[id].type, (Value){id});
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
        fprintf(stderr, "Syntax Error: Expected assignment operator\n");
        exit(1);
    }
    scan(&CurrentToken);

    // Parse the expression
    right = expression();

    // Create the AST
    n = mkastbinary(type, left, right, NO_VALUE);

    // TODO: Check type
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
    ASTnode *stmt;

    switch (CurrentToken.type)
    {
    case T_IDENT:
        stmt = assignment_statement();
        break;
    case T_VAR:
        stmt = var_declaration();
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
                seq = mkastbinary(A_SEQ, seq, stmt, NO_VALUE);
            }
        }
    }
    return seq;
}
