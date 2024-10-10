/********************************************************************************
 * File Name: expr.c                                                            *
 * Description: This file contains all the logic for parsing expressions.       *
 ********************************************************************************/

#include "defs.h"
#include "data.h"
#include "decl.h"

static AST *primary();
static AST *binary(int prec);

// Return the precedence of an operator
static int op_prec(TOKEN_TYPE typ)
{
    switch (typ)
    {
    case T_LPAREN:
    case T_RPAREN:
        return 5;
    case T_STAR:
    case T_SLASH:
    case T_PERCENT:
        return 4;
    case T_PLUS:
    case T_MINUS:
        return 3;
    case T_LESS:
    case T_LESS_EQUAL:
    case T_GREATER:
    case T_GREATER_EQUAL:
        return 2;
    case T_EQUAL:
    case T_NOT_EQUAL:
        return 1;
    default:
        match_error("an operator", TOKEN_STR[typ]);
        return 0;
    }
}

// Operator type to AST type
static AST_TYPE op_to_ast(TOKEN_TYPE typ)
{
    switch (typ)
    {
    case T_PLUS:
        return A_ADD;
    case T_MINUS:
        return A_SUB;
    case T_STAR:
        return A_MUL;
    case T_SLASH:
        return A_DIV;
    case T_PERCENT:
        return A_MOD;
    case T_EQUAL:
        return A_EQ;
    case T_NOT_EQUAL:
        return A_NE;
    case T_LESS:
        return A_LT;
    case T_LESS_EQUAL:
        return A_LE;
    case T_GREATER:
        return A_GT;
    case T_GREATER_EQUAL:
        return A_GE;
    default:
        match_error("an operator", TOKEN_STR[typ]);
        return -1;
    }
}

// Check if the current token is the end of an expression
static bool end_of_expr(TOKEN_TYPE typ)
{
    return typ == T_RPAREN || typ == T_SEMICOLON || typ == T_EOF;
}

// Parse a primary expression
static AST *primary()
{
    int id;
    AST *n = NULL;

    // Check the type of the token
    switch (Token.type)
    {
    case T_INTLIT:
        n = make_AST_leaf(A_INTLIT, NO_PRIMITIVE, Token.value);
        scan();
        break;
    case T_IDENT:
        id = find_glob(Token.value.string);
        if (id == -1)
            undeclared_variable_error(Token.value.string);
        n = make_AST_leaf(A_IDENT, NO_PRIMITIVE, (VALUE){id});
        scan();
        break;
    case T_LPAREN:
        match(T_LPAREN);
        n = binary(0);
        match(T_RPAREN);
        n = make_AST_unary(A_PAREN, NO_PRIMITIVE, n, NO_VALUE);
        break;
    default:
        match_error("math expression", TOKEN_STR[Token.type]);
    }
    return n;
}

// Parse a binary expression
static AST *binary(int prec)
{
    AST *lft, *rgt;
    TOKEN_TYPE typ;

    // Parse the left side
    lft = primary();
    typ = Token.type;
    // While the precedence of this token is greater than the previous
    while (!end_of_expr(typ) && op_prec(typ) > prec)
    {
        scan();
        // Parse the right side
        rgt = binary(op_prec(typ));
        // Create a new AST node
        lft = make_AST_binary(op_to_ast(typ), NO_PRIMITIVE, lft, rgt, NO_VALUE);
        // Update the type
        typ = Token.type;
    }
    return lft;
}

// Parse an expression
AST *expression()
{
    AST *expr = NULL;

    expr = binary(0);
    return expr;
}
