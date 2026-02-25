/********************************************************************************
 * File Name: src/parser/decl.c                                                 *
 *                                                                              *
 * Description: Handles variable, constant, and function declarations. Updates  *
 *              the Symbol Table and creates AST nodes.                         *
 * Author: Alejandro Diez Bermejo                                               *
 * Date: 2026-01-01                                                             *
 * Version: 0.0.0                                                               *
 ********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defs.h"
#include "data.h"
#include "decl.h"

/**
 * Parse a type token.
 *
 * @return The primary type enum value
 */
static PType parse_type(void)
{
    PType type;
    switch (CurrentToken.type)
    {
    case T_INT:
        type = P_INT;
        break;
    case T_BOOL:
        type = P_BOOL;
        break;
    case T_VOID:
        type = P_VOID;
        break;
    default:
        fprintf(stderr, "Syntax Error: expected type at %d:%d\n", Line, Column);
        exit(1);
    }
    scan(&CurrentToken);
    return type;
}

/**
 * Parses a variable declaration.
 *
 * @return The AST node or null if it is just a declaration
 */
ASTnode *var_declaration(void)
{
    ASTnode *left, *right;
    Symbol *sym;
    char name[MAX_LEN];
    PType type;

    match(T_VAR);

    // Get identifier
    if (CurrentToken.type != T_IDENT)
    {
        fprintf(stderr, "Syntax Error: expected identifier after 'var' at %d:%d\n", Line, Column);
        exit(1);
    }
    strncpy(name, CurrentToken.value.string, MAX_LEN);
    scan(&CurrentToken);

    // Get type
    match(T_COLON);
    type = parse_type();

    // Create a new symbol in current scope
    sym = addsymbol(name, S_VARIABLE, type);
    if (sym == NULL)
    {
        fprintf(stderr, "Error: variable '%s' already declared in this scope at %d:%d\n", name, Line, Column);
        exit(1);
    }

    // Check for initilization
    if (CurrentToken.type == T_ASSIGN)
    {
        scan(&CurrentToken);

        right = expression();
        left = mkastleaf(A_IDENT, type, (Value){.symbol = sym});

        return mkastbinary(A_ASSIGN, left, right, NO_VALUE);
    }
    return NULL;
}

/**
 * Parses a constant declaration.
 *
 * @return The AST node
 */
ASTnode *const_declaration(void)
{
    ASTnode *left, *right;
    Symbol *sym;
    char name[MAX_LEN];
    PType type;

    match(T_CONST);

    // Get identifier
    if (CurrentToken.type != T_IDENT)
    {
        fprintf(stderr, "Syntax Error: expected identifier after 'const' at %d:%d\n", Line, Column);
        exit(1);
    }
    strncpy(name, CurrentToken.value.string, MAX_LEN);
    scan(&CurrentToken);

    // Get type
    match(T_COLON);
    type = parse_type();

    // Create a new symbol in current scope
    sym = addsymbol(name, S_CONSTANT, type);
    if (sym == NULL)
    {
        fprintf(stderr, "Error: constant '%s' already declared at %d:%d\n", name, Line, Column);
        exit(1);
    }

    // Parse initilization
    match(T_ASSIGN);

    right = expression();
    left = mkastleaf(A_IDENT, type, (Value){.symbol = sym});

    return mkastbinary(A_ASSIGN, left, right, NO_VALUE);
}

/**
 * Parses a function definition.
 *
 * @return The AST node
 */
ASTnode *function_declaration(void)
{
    ASTnode *body = NULL;
    Symbol *sym, *prevFunc;
    int prevOffset;
    char name[MAX_LEN];
    PType ptype;

    match(T_FUN);

    // Get identifier
    if (CurrentToken.type != T_IDENT)
    {
        fprintf(stderr, "Syntax Error: expected function name at %d:%d\n", Line, Column);
        exit(1);
    }
    strncpy(name, CurrentToken.value.string, MAX_LEN);
    scan(&CurrentToken);

    // Create a new symbol in current scope
    sym = addsymbol(name, S_FUNCTION, P_VOID);
    if (sym == NULL)
    {
        fprintf(stderr, "Error: redefinition of '%s' at %d:%d\n", name, Line, Column);
        exit(1);
    }

    // Enter Scope
    prevFunc = CurrentFunction;
    CurrentFunction = sym;
    prevOffset = LocalOffset;
    LocalOffset = 0;
    scope_enter();

    // Get parameters
    Symbol *lastParam = NULL;
    int paramCount = 0;

    match(T_LPAREN);
    while (CurrentToken.type != T_RPAREN)
    {
        char name[MAX_LEN];
        PType ptype;

        // Get identifier
        if (CurrentToken.type != T_IDENT)
        {
            fprintf(stderr, "Syntax Error: expected param name at %d:%d\n", Line, Column);
            exit(1);
        }
        strncpy(name, CurrentToken.value.string, MAX_LEN);
        scan(&CurrentToken);

        // Get type
        match(T_COLON);
        ptype = parse_type();

        // Create a new symbol in current scope
        Symbol *paramSym = addsymbol(name, S_VARIABLE, ptype);

        paramSym->sclass = C_PARAM;

        // Link param with function
        if (sym->params == NULL)
        {
            sym->params = paramSym;
        }
        else
        {
            lastParam->next = paramSym;
        }
        lastParam = paramSym;
        paramCount++;

        if (CurrentToken.type == T_COMMA)
        {
            scan(&CurrentToken);
        }
        else
        {
            break;
        }
    }
    match(T_RPAREN);
    sym->numParams = paramCount;

    // Get type
    if (CurrentToken.type == T_COLON)
    {
        match(T_COLON);
        ptype = parse_type();
    }
    else
    {
        ptype = P_VOID;
    }
    sym->ptype = ptype;

    // Body
    match(T_LBRACE);
    while (CurrentToken.type != T_RBRACE)
    {
        ASTnode *stmt = single_statement();

        if (stmt != NULL)
        {
            if (body == NULL)
            {
                body = stmt;
            }
            else
            {
                body = mkastbinary(A_GLUE, body, stmt, NO_VALUE);
            }
        }
    }
    scan(&CurrentToken);

    // Calculate stack frame
    int frameSize = -LocalOffset;

    if (frameSize % 16 != 0)
    {
        frameSize += (16 - (frameSize % 16));
    }
    sym->size = frameSize;

    // Exit scope
    scope_exit();
    LocalOffset = prevOffset;
    CurrentFunction = prevFunc;

    return mkastunary(A_FUNCTION, body, (Value){.symbol = sym});
}