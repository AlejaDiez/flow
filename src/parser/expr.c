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
        return 4;
    case T_PLUS:
    case T_MINUS:
        return 3;
    default:
        fprintf(stderr, "Syntax Error: expected an operator, but another token was found (%d:%d)\n", Line, Column);
        exit(1);
    }
}

// Parse a primary expression
static ASTnode *primary(void)
{
    ASTnode *n;

    // Check the type of the token
    switch (CurrentToken.type)
    {
    case T_INTLIT:
        n = mkastleaf(A_INTLIT, CurrentToken.value);
        scan(&CurrentToken);
        return n;
    default:
        fprintf(stderr, "Syntax Error: expected a math expression, but another token was found (%d:%d)\n", Line, Column);
        exit(1);
    }
}

// Parse a binary expression
// ptp: Previous Token Precedence
static ASTnode *binary(int ptp)
{
    ASTnode *left, *right;
    TokenType tokentype;

    // Parse the left side
    left = primary();
    // Check the token precedence if it is higher than the previous one
    tokentype = CurrentToken.type;
    if (tokentype == T_EOF)
    {
        return left;
    }
    while (op_precedence(tokentype) > ptp)
    {
        // Read next token
        scan(&CurrentToken);
        // Parse the right side
        right = binary(op_precedence(tokentype));
        // Create a new AST node
        left = mkastbinary(arithop(tokentype), left, right, NO_VALUE);
        // Update the type
        tokentype = CurrentToken.type;
        if (tokentype == T_EOF)
        {
            return left;
        }
    }
    return left;
}

// Parse an expression
ASTnode *expression(void)
{
    return binary(0);
}
