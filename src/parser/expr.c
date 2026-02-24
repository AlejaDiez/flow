/********************************************************************************
 * File Name: src/parser/expr.c                                                 *
 *                                                                              *
 * Description: Parses mathematical and logical expressions, building the AST.  *
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
 * Returns the precedence level of an operator token.
 *
 * @param tokentype Token type
 * @return The operator precedence
 */
static int op_precedence(TokenType tokentype)
{
    switch (tokentype)
    {
    // Power
    case T_DSTAR:
        return 9;
    // Multiplicative
    case T_STAR:
    case T_SLASH:
    case T_PERCENT:
        return 8;
    // Additive
    case T_PLUS:
    case T_MINUS:
        return 7;
    // Relational
    case T_GT:
    case T_GE:
    case T_LT:
    case T_LE:
        return 6;
    // Equality
    case T_EQ:
    case T_NEQ:
        return 5;
    // Logic AND
    case T_DAMPERSAND:
        return 4;
    // Logic OR
    case T_DPIPE:
        return 3;
    // Assignment
    case T_ASSIGN:
    case T_ASPLUS:
    case T_ASMINUS:
    case T_ASSTAR:
    case T_ASSLASH:
    case T_ASPERCENT:
    case T_ASDSTAR:
    case T_ASDAMPERSAND:
    case T_ASDPIPE:
        return 2;
    // End of expression
    default:
        return 0;
    }
}

/**
 * Maps a token type to an AST node type.
 *
 * @param tokentype Token type
 * @return The AST binary node type
 */
static ASTnodeType binary_ast_op(TokenType tokentype)
{
    switch (tokentype)
    {
    case T_PLUS:
        return A_ADD;
    case T_MINUS:
        return A_SUB;
    case T_STAR:
        return A_MUL;
    case T_DSTAR:
        return A_POW;
    case T_SLASH:
        return A_DIV;
    case T_PERCENT:
        return A_MOD;
    case T_EQ:
        return A_EQ;
    case T_NEQ:
        return A_NEQ;
    case T_LT:
        return A_LT;
    case T_LE:
        return A_LE;
    case T_GT:
        return A_GT;
    case T_GE:
        return A_GE;
    case T_DAMPERSAND:
        return A_AND;
    case T_DPIPE:
        return A_OR;
    case T_ASSIGN:
        return A_ASSIGN;
    case T_ASPLUS:
        return A_ASADD;
    case T_ASMINUS:
        return A_ASSUB;
    case T_ASSTAR:
        return A_ASMUL;
    case T_ASSLASH:
        return A_ASDIV;
    case T_ASPERCENT:
        return A_ASMOD;
    case T_ASDSTAR:
        return A_ASPOW;
    case T_ASDAMPERSAND:
        return A_ASAND;
    case T_ASDPIPE:
        return A_ASOR;
    default:
        fprintf(stderr, "Syntax Error: unknown binary operator token '%s' at %d:%d\n", TokenTypeStr[tokentype], Line, Column);
        exit(1);
    }
}

/**
 * Maps a unary token type to an AST node type.
 *
 * @param tokentype Token type
 * @return The AST unary node type
 */
static ASTnodeType unary_ast_op(TokenType tokentype)
{
    switch (tokentype)
    {
    case T_PLUS:
        return A_POS;
    case T_MINUS:
        return A_NEG;
    case T_BANG:
        return A_NOT;
    default:
        fprintf(stderr, "Syntax Error: unknown unary operator token '%s' at %d:%d\n", TokenTypeStr[tokentype], Line, Column);
        exit(1);
    }
}

/**
 * Parse a primary expression (literals, identifiers, parens).
 *
 * @return The AST node
 */
static ASTnode *primary(void)
{
    ASTnode *expr;
    Symbol *sym;
    TokenType tokentype;

    switch (CurrentToken.type)
    {
    case T_INTLIT:
        expr = mkastleaf(A_INTLIT, P_INT, CurrentToken.value);
        scan(&CurrentToken);

        return expr;
    case T_TRUE:
        expr = mkastleaf(A_TRUE, P_BOOL, (Value){1});
        scan(&CurrentToken);

        return expr;
    case T_FALSE:
        expr = mkastleaf(A_FALSE, P_BOOL, (Value){0});
        scan(&CurrentToken);

        return expr;
    case T_IDENT:
        // Look up the identifier in the Symbol Table (Scope Stack)
        sym = findsymbol(CurrentToken.value.string);
        if (sym == NULL)
        {
            fprintf(stderr, "Error: undeclared identifier '%s' at %d:%d\n", CurrentToken.value.string, Line, Column);
            exit(1);
        }

        // Check what kind of symbol it is
        switch (sym->stype)
        {
        case S_CONSTANT:
        case S_VARIABLE:
            scan(&CurrentToken);
            return mkastleaf(A_IDENT, sym->ptype, (Value){.symbol = sym});
        case S_FUNCTION:
        {
            scan(&CurrentToken);
            match(T_LPAREN);

            // Parse arguments
            int argCount = 0;
            ASTnode *args = NULL, *argsTail;

            while (CurrentToken.type != T_RPAREN)
            {
                ASTnode *arg = expression();

                if (args == NULL)
                {
                    args = argsTail = mkastbinary(A_GLUE, arg, NULL, NO_VALUE);
                }
                else
                {
                    argsTail->right = mkastbinary(A_GLUE, arg, NULL, NO_VALUE);
                    argsTail = argsTail->right;
                }
                argCount++;

                if (CurrentToken.type == T_COMMA)
                {
                    scan(&CurrentToken);
                }
                else
                {
                    break;
                }
            }
            if (argCount != sym->numParams)
            {
                fprintf(stderr, "Error: Function '%s' expects %d arguments but got %d on line %d\n",
                        sym->name, sym->numParams, argCount, Line);
                exit(1);
            }

            match(T_RPAREN);

            return mkastunary(A_CALL, args, (Value){.symbol = sym});
        }
        }
    case T_LPAREN:
        scan(&CurrentToken);
        expr = expression();
        match(T_RPAREN);

        return expr;
    case T_PLUS:
    case T_MINUS:
    case T_BANG:
        tokentype = CurrentToken.type;
        scan(&CurrentToken);
        expr = primary();

        return mkastunary(unary_ast_op(tokentype), expr, NO_VALUE);
    default:
        fprintf(stderr, "Syntax Error: expected primary expression but found %s at %d:%d\n", TokenTypeStr[CurrentToken.type], Line, Column);
        exit(1);
    }
}

/**
 * Operator precedence parsing.
 *
 * @param ptp Previous token precedence
 * @return The AST node
 */
static ASTnode *binary(int ptp)
{
    ASTnode *left, *right;
    TokenType tokentype;

    // Get the left side
    left = primary();

    // While the next operator has higher precedence than our current context
    tokentype = CurrentToken.type;
    while (op_precedence(tokentype) > ptp)
    {
        scan(&CurrentToken);

        // Right-associativity
        if (tokentype >= T_ASSIGN && tokentype <= T_ASDPIPE)
        {
            right = binary(op_precedence(tokentype) - 1);
        }
        // Left-associative
        else
        {
            right = binary(op_precedence(tokentype));
        }

        if (tokentype >= T_ASSIGN && tokentype <= T_ASDPIPE)
        {
            if (left->type != A_IDENT)
            {
                fprintf(stderr, "Syntax Error: assignment to non-identifier at %d:%d\n", Line, Column);
                exit(1);
            }
            if (left->value.symbol->stype == S_CONSTANT)
            {
                fprintf(stderr, "Type Error: cannot reassign constant '%s' at %d:%d\n", left->value.symbol->name, Line, Column);
                exit(1);
            }
        }
        left = mkastbinary(binary_ast_op(tokentype), left, right, NO_VALUE);

        tokentype = CurrentToken.type;
    }

    return left;
}

/**
 * Parse an expressions.
 *
 * @return The AST node
 */
ASTnode *expression(void)
{
    return binary(0);
}
