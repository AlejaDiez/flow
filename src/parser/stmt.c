/********************************************************************************
 * File Name: stmt.c                                                            *
 * Description: This file contains all the logic for parsing statements.        *
 ********************************************************************************/

#include "defs.h"
#include "data.h"
#include "decl.h"

// Add a statement to a sequence and return the new sequence
static AST *add_seq(AST *seq, AST *stmt)
{
    // Check if we have a statement
    if (!stmt)
    {
        return seq;
    }
    // Check if we have a sequence
    if (!seq)
    {
        return stmt;
    }
    return make_AST_binary(A_SEQ, NO_PRIMITIVE, seq, stmt, NO_VALUE);
}

// Parse a variable assignment
static AST *assignment_statement()
{
    AST *n, *lft, *rgt;
    VALUE ident;
    int id;

    // Match the sintax
    ident = match(T_IDENT);
    // Find the identifier in the global symbol table
    id = find_glob(ident.string);
    if (id == -1)
        undeclared_variable_error(ident.string);
    rgt = make_AST_leaf(A_IDENT, NO_PRIMITIVE, (VALUE){id});
    // Match the sintax
    match(T_ASSIGN);
    // Parse the expression
    lft = expression();
    // Create the AST
    n = make_AST_binary(A_ASSIGN, NO_PRIMITIVE, lft, rgt, NO_VALUE);
    return n;
}

// Parse an if else statement
static AST *if_else_statement()
{
    AST *n, *cond, *true_stmt, *false_stmt = NULL;

    // Match syntax
    match(T_IF);
    match(T_LPAREN);
    // Parse the condition
    cond = expression();
    if (cond->type < A_EQ || cond->type > A_GE)
        match_error("comparison or logical expression", "other expression");
    // Match syntax
    match(T_RPAREN);
    // Parse the true statement
    true_stmt = statement(true);
    // Check if we have an else statement
    if (Token.type == T_ELSE)
    {
        // Match syntax
        match(T_ELSE);
        // Parse the false statement
        false_stmt = statement(true);
    }
    // Create the AST
    n = make_AST_node(A_IFELSE, NO_PRIMITIVE, cond, true_stmt, false_stmt, NO_VALUE);
    return n;
}

// Parse a loop statement
static AST *loop_statement()
{
    AST *n, *init = NULL, *cond, *upd = NULL, *stmt;

    // Match syntax
    match(T_LOOP);
    match(T_LPAREN);
    // Try to parse the initialization
    init = statement(false);
    if (init->type >= A_EQ && init->type <= A_GE)
    {
        // We don't have a initialization statement
        cond = init;
        init = NULL;
    }
    // Check the condition
    if (init)
    {
        match(T_SEMICOLON);
        cond = expression();
        if (cond->type < A_EQ || cond->type > A_GE)
            match_error("comparison or logical expression", "other expression");
    }
    // Check if there is a update statement
    if (Token.type == T_SEMICOLON)
    {
        match(T_SEMICOLON);
        upd = statement(false);
    }
    // Match syntax
    match(T_RPAREN);
    // Parse the statement
    stmt = statement(true);
    // Create the AST
    n = stmt;
    if (upd)
        n = make_AST_binary(A_SEQ, NO_PRIMITIVE, n, upd, NO_VALUE);
    n = make_AST_binary(A_LOOP, NO_PRIMITIVE, cond, n, NO_VALUE);
    if (init)
        n = make_AST_binary(A_SEQ, NO_PRIMITIVE, init, n, NO_VALUE);
    return n;
}

// Parse a print statement
static AST *print_statement()
{
    AST *expr;

    // Match syntax
    match(T_PRINT);
    match(T_LPAREN);
    // Parse an expression
    expr = expression();
    // Match syntax
    match(T_RPAREN);
    return make_AST_unary(A_PRINT, NO_PRIMITIVE, expr, NO_VALUE);
}

// Parse a statement
AST *statement(bool req_semi)
{
    AST *stmt = NULL;

    switch (Token.type)
    {
    case T_LPAREN:
    case T_INTLIT:
        stmt = expression();
        break;
    case T_IDENT:
        if (look_ahead().type == T_ASSIGN)
        {
            stmt = assignment_statement();
        }
        else
        {
            stmt = expression();
        }
        break;
    case T_VAR:
        var_declaration();
        break;
    case T_FUN:
        fun_declaration();
        return NULL;
    case T_IF:
        return if_else_statement();
    case T_LOOP:
        return loop_statement();
    case T_PRINT:
        stmt = print_statement();
        break;
    case T_LBRACE:
        return block_statement();
    default:
        unrecognized_token_error();
    }
    if (req_semi)
    {
        match(T_SEMICOLON);
    }
    return stmt;
}

// Parse a block and return an AST
AST *block_statement()
{
    AST *seq = NULL, *stmt = NULL;

    // Match the sintax
    match(T_LBRACE);
    // Parse the statements
    while (Token.type != T_RBRACE)
    {
        stmt = statement(true);
        seq = add_seq(seq, stmt);
    }
    // Match the sintax
    match(T_RBRACE);
    return seq;
}

// Parse multiple statements
AST *sequence()
{
    AST *seq = NULL, *stmt = NULL;

    // Parse the statements
    while (Token.type != T_EOF)
    {
        stmt = statement(true);
        seq = add_seq(seq, stmt);
    }
    // Match the sintax
    match(T_EOF);
    return seq;
}
