/********************************************************************************
 * File Name: src/parser/stmt.c                                                 *
 *                                                                              *
 * Description: Handles control flow statements and blocks.                     *
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
 * Requires a semicolon.
 *
 * @param func The function that requires a semicolon
 * @return The AST node
 */
static ASTnode *semicolon(ASTnode *(*func)(void))
{
    ASTnode *n = func();

    match(T_SEMICOLON);
    return n;
}

/**
 * Parses if-else statement.
 *
 * @return The AST node
 */
static ASTnode *if_statement(void)
{
    ASTnode *cond, *true_body, *false_body = NULL;

    match(T_IF);
    match(T_LPAREN);

    // Parse condition
    cond = expression();

    match(T_RPAREN);

    // Parse true block
    true_body = single_statement();

    // Parse optional else block
    if (CurrentToken.type == T_ELSE)
    {
        scan(&CurrentToken);
        false_body = single_statement();
    }

    return mkastternary(A_IFELSE, cond, true_body, false_body, NO_VALUE);
}

/**
 * Parse a match statement.
 *
 * @return The AST node
 */
static ASTnode *match_statement(void)
{
    ASTnode *cond, *cases = NULL, *cases_tail = NULL, *def = NULL;

    match(T_MATCH);
    match(T_LPAREN);

    // Parse condition
    cond = expression();

    match(T_RPAREN);
    match(T_LBRACE);

    // Parse case blocks
    while (CurrentToken.type != T_RBRACE)
    {
        ASTnode *val = NULL, *body;

        // Default case
        if (CurrentToken.type == T_UNDERSCORE)
        {
            scan(&CurrentToken);
        }
        // Case
        else
        {
            val = expression();
        }

        // Match the syntax
        match(T_COLON);

        // Get the body
        body = single_statement();

        // Make new case
        if (val == NULL)
        {
            if (def != NULL)
            {
                fprintf(stderr, "Syntax Error: multiple defaults in match at %d:%d\n", Line, Column);
                exit(0);
            }
            def = mkastternary(A_CASE, val, body, NULL, NO_VALUE);
        }
        else
        {
            if (cases == NULL)
            {
                cases = cases_tail = mkastternary(A_CASE, val, body, NULL, NO_VALUE);
            }
            else
            {
                cases_tail->right = mkastternary(A_CASE, val, body, NULL, NO_VALUE);
                cases_tail = cases_tail->right;
            }
        }
    }
    scan(&CurrentToken);

    // Add default case
    if (def != NULL)
    {
        if (cases == NULL)
        {
            cases = def;
        }
        else
        {
            cases_tail->right = def;
        }
    }

    return mkastbinary(A_MATCH, cond, cases, NO_VALUE);
}

/**
 * Parses loop statement.
 *
 * @return The AST node
 */
static ASTnode *loop_statement(void)
{
    ASTnode *cond, *body, *init = NULL, *update = NULL;

    match(T_LOOP);

    // Infinite loop
    if (CurrentToken.type != T_LPAREN)
    {
        // Parse condition
        cond = mkastleaf(A_TRUE, P_BOOL, (Value){1});

        // Parse body
        body = single_statement();

        return mkastternary(A_LOOP, cond, body, NULL, NO_VALUE);
    }

    match(T_LPAREN);

    // Enter a new scope
    scope_enter();

    // For loop without init
    if (CurrentToken.type == T_SEMICOLON)
    {
        scan(&CurrentToken);
    }
    // For loop with var declaration
    else if (CurrentToken.type == T_VAR)
    {
        // Parse var declaration
        init = semicolon(var_declaration);
    }
    else if (CurrentToken.type == T_IDENT)
    {
        int next = peek();

        // For loop with initialization
        if (next >= T_ASSIGN && next <= T_ASDPIPE)
        {
            // Parse initialization
            init = semicolon(expression);
        }
        // While loop
        else
        {
            // Parse condition
            cond = expression();

            match(T_RPAREN);

            // Parse body
            body = single_statement();

            // Exit of scope
            scope_exit();

            return mkastternary(A_LOOP, cond, body, NULL, NO_VALUE);
        }
    }
    // While loop
    else
    {
        // Parse condition
        cond = expression();

        match(T_RPAREN);

        // Parse body
        body = single_statement();

        // Exit of scope
        scope_exit();

        return mkastternary(A_LOOP, cond, body, NULL, NO_VALUE);
    }

    // Foor loop
    if (CurrentToken.type != T_SEMICOLON)
    {
        // Parse condition
        cond = semicolon(expression);
    }
    else
    {
        cond = mkastleaf(A_TRUE, P_BOOL, (Value){1});
        match(T_SEMICOLON);
    }

    if (CurrentToken.type != T_RPAREN)
    {
        // Parse update
        update = expression();
    }

    match(T_RPAREN);

    // Parse body
    body = single_statement();

    // Exit of scope
    scope_exit();

    return init != NULL
               ? mkastbinary(A_GLUE, init, mkastternary(A_LOOP, cond, body, update, NO_VALUE), NO_VALUE)
               : mkastternary(A_LOOP, cond, body, update, NO_VALUE);
}

/**
 * Parse a block statement.
 *
 * @return The AST node
 */
static ASTnode *block_statement(void)
{
    ASTnode *seq = NULL;

    match(T_LBRACE);

    // Enter a new scope
    scope_enter();

    // Parse statements
    while (CurrentToken.type != T_RBRACE)
    {
        ASTnode *stmt = single_statement();

        if (stmt != NULL)
        {
            if (seq == NULL)
            {
                seq = stmt;
            }
            else
            {
                seq = mkastbinary(A_GLUE, seq, stmt, NO_VALUE);
            }
        }
    }
    scan(&CurrentToken);

    // Exit of scope
    scope_exit();

    return seq;
}

// TODO: replace with std library
static ASTnode *print_statement(void)
{
    ASTnode *expr;

    match(T_PRINT);
    match(T_LPAREN);

    // Parse an expression
    expr = expression();

    match(T_RPAREN);
    return mkastunary(A_PRINT, expr, NO_VALUE);
}

/**
 * Parses a single statement.
 *
 * @return The AST node
 */
ASTnode *single_statement(void)
{
    switch (CurrentToken.type)
    {
    // Declarations
    case T_CONST:
        return semicolon(const_declaration);
    case T_VAR:
        return semicolon(var_declaration);
    case T_FUN:
        return function_declaration();
    // Control Flow
    case T_IF:
        return if_statement();
    case T_MATCH:
        return match_statement();
    case T_LOOP:
        return loop_statement();
    case T_RETURN:
    {
        scan(&CurrentToken);

        ASTnode *expr = CurrentToken.type != T_SEMICOLON ? expression() : NULL;

        match(T_SEMICOLON);
        return mkastunary(A_RETURN, expr, (Value){.symbol = CurrentFunction});
    }
    case T_STOP:
        scan(&CurrentToken);
        match(T_SEMICOLON);
        return mkastleaf(A_STOP, P_VOID, NO_VALUE);
    case T_NEXT:
        scan(&CurrentToken);
        match(T_SEMICOLON);
        return mkastleaf(A_NEXT, P_VOID, NO_VALUE);
    // Block
    case T_PRINT:
        return semicolon(print_statement);
    case T_LBRACE:
        return block_statement();
    // Expressions
    default:
        return semicolon(expression);
    }
}

/**
 * Parses statements.
 *
 * @return The AST node
 */
ASTnode *compound_statement(void)
{
    ASTnode *seq = NULL;

    // Enter a new scope
    scope_enter();

    // Parse statements
    while (CurrentToken.type != T_EOF)
    {
        ASTnode *stmt = single_statement();

        if (stmt != NULL)
        {
            if (seq == NULL)
            {
                seq = stmt;
            }
            else
            {
                seq = mkastbinary(A_GLUE, seq, stmt, NO_VALUE);
            }
        }
    }
    scan(&CurrentToken);

    // Exit of scope
    if (!is_global_scope())
    {
        scope_exit();
    }

    return seq;
}
