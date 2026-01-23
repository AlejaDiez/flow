#include <stdio.h>
#include <stdlib.h>

#include "defs.h"
#include "data.h"
#include "decl.h"

// Operator type to AST node type
static ASTnodeType arithop(TokenType tokentype)
{
    switch (tokentype)
    {
    case T_PLUS:
        return A_ADD;
    case T_MINUS:
        return A_SUB;
    case T_STAR:
        return A_MUL;
    case T_SLASH:
        return A_DIV;
    case T_EQ:
        return A_EQ;
    case T_NEQ:
        return A_NEQ;
    case T_LT:
        return A_LT;
    case T_GT:
        return A_GT;
    case T_LE:
        return A_LE;
    case T_GE:
        return A_GE;
    case T_AND:
        return A_AND;
    case T_OR:
        return A_OR;
    default:
        fprintf(stderr, "Syntax Error: unrecognized token (%d:%d)\n", Line, Column);
        exit(1);
    }
}

// Return the precedence of an operator
static int op_precedence(TokenType tokentype)
{
    switch (tokentype)
    {
    case T_STAR:
    case T_SLASH:
        return 5;
    case T_PLUS:
    case T_MINUS:
        return 4;
    case T_EQ:
    case T_NEQ:
    case T_LT:
    case T_GT:
    case T_LE:
    case T_GE:
        return 3;
    case T_AND:
        return 2;
    case T_OR:
        return 1;
    default:
        fprintf(stderr, "Syntax Error: expected an operator but another token was found (%d:%d)\n", Line, Column);
        exit(1);
    }
}

// Parse a primary expression
static ASTnode *primary(void)
{
    ASTnode *n;
    int id;

    // Check the type of the token
    switch (CurrentToken.type)
    {
    case T_IDENT:
        id = findglob(CurrentToken.value.string);
        if (id == -1)
        {
            fprintf(stderr, "Syntax Error: undeclared variable '%s' (%d:%d)", CurrentToken.value.string, Line, Column);
            exit(1);
        }
        n = mkastleaf(A_IDENT, GlobalSymbols[id].type, (Value){id});
        scan(&CurrentToken);
        return n;
    case T_INTLIT:
        n = mkastleaf(A_INTLIT, P_INT, CurrentToken.value);
        scan(&CurrentToken);
        return n;
    case T_TRUE:
        n = mkastleaf(A_TRUE, P_BOOL, (Value){1});
        scan(&CurrentToken);
        return n;
    case T_FALSE:
        n = mkastleaf(A_FALSE, P_BOOL, (Value){0});
        scan(&CurrentToken);
        return n;
    case T_LPAREN:
        match(T_LPAREN, "(");
        n = expression();
        match(T_RPAREN, ")");
        return n;
    default:
        fprintf(stderr, "Syntax Error: expected a math expression but another token was found (%d:%d)\n", Line, Column);
        exit(1);
    }
}

// Parse a binary expression, ptp (Previous Token Precedence)
static ASTnode *binary(int ptp)
{
    ASTnode *left, *right;
    TokenType tokentype;

    // Parse the left side
    left = primary();
    // Check the token precedence if it is higher than the previous one
    tokentype = CurrentToken.type;
    while (!(tokentype == T_SEMICOLON || tokentype == T_RPAREN) && op_precedence(tokentype) > ptp)
    {
        // Read next token
        scan(&CurrentToken);
        // Parse the right side
        right = binary(op_precedence(tokentype));
        // Create a new AST node
        left = mkastbinary(arithop(tokentype), left, right, NO_VALUE);
        // Update the type
        tokentype = CurrentToken.type;
    }
    return left;
}

// Parse an expression
ASTnode *expression(void)
{
    return binary(0);
}
