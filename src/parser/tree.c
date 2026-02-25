/********************************************************************************
 * File Name: src/parser/tree.c                                                 *
 *                                                                              *
 * Description: Abstract Syntax Tree (AST) construction utilities, creates      *
 *              nodes for the AST, handling type checking and symbol            *
 *              references.                                                     *
 * Author: Alejandro Diez Bermejo                                               *
 * Date: 2026-01-01                                                             *
 * Version: 0.0.0                                                               *
 ********************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "defs.h"
#include "data.h"
#include "decl.h"

/**
 * Allocates a new AST node.
 *
 * @param type AST node type
 * @param ptype Primary type
 * @param left Left node
 * @param mid Mid node
 * @param right Right node
 * @param value Node value
 */
static ASTnode *mkastnode(ASTnodeType type, PType ptype, ASTnode *left, ASTnode *mid, ASTnode *right, Value value)
{
    ASTnode *n = (ASTnode *)malloc(sizeof(ASTnode));

    if (n == NULL)
    {
        fprintf(stderr, "Fatal Error: out of memory\n");
        exit(1);
    }

    n->type = type;
    n->ptype = ptype;
    n->left = left;
    n->mid = mid;
    n->right = right;
    n->value = value;
    return n;
}

/**
 * Creates a leaf node (Literal or Identifier).
 *
 * @param type Node type
 * @param ptype Primitive type
 * @param value Literal value
 */
ASTnode *mkastleaf(ASTnodeType type, PType ptype, Value value)
{
    return mkastnode(type, ptype, NULL, NULL, NULL, value);
}

/**
 * Creates a unary node.
 *
 * @param type Node type
 * @param child Child expression
 * @param value Value
 */
ASTnode *mkastunary(ASTnodeType type, ASTnode *child, Value value)
{
    switch (type)
    {
    // Unary
    case A_POS:
    case A_NEG:
        if (child->ptype != P_INT)
        {
            fprintf(stderr, "Type Error: unary +/- requires integer operand at %d:%d\n", Line, Column);
            exit(1);
        }
        return mkastnode(type, P_INT, child, NULL, NULL, value);
    case A_NOT:
        if (child->ptype != P_BOOL)
        {
            fprintf(stderr, "Type Error: unary ! requires boolean operand at %d:%d\n", Line, Column);
            exit(1);
        }
        return mkastnode(type, P_BOOL, child, NULL, NULL, value);
    // Functions
    case A_FUNCTION:
        return mkastnode(type, child->ptype, child, NULL, NULL, value);
    case A_CALL:
        return mkastnode(type, value.symbol->ptype, child, NULL, NULL, value);
    case A_RETURN:
        if (CurrentFunction == NULL)
        {
            fprintf(stderr, "Syntax Error: return statement outside of function at %d:%d\n", Line, Column);
            exit(1);
        }
        if (child != NULL && child->ptype != CurrentFunction->ptype)
        {
            fprintf(stderr, "Type Error: return type mismatch at %d:%d\n", Line, Column);
            exit(1);
        }
        else if (child == NULL && CurrentFunction->ptype != P_VOID)
        {
            fprintf(stderr, "Type Error: missing return value at %d:%d\n", Line, Column);
            exit(1);
        }
        return mkastnode(type, CurrentFunction->ptype, child, NULL, NULL, value);
    case A_PRINT:
        return mkastnode(type, P_VOID, child, NULL, NULL, value);
    default:
        fprintf(stderr, "Internal Error: unknown unary node type\n");
        exit(1);
    }
}

/**
 * Creates a binary node.
 *
 * @param type Node type
 * @param left Left node
 * @param right Right node
 * @param value Value
 */
ASTnode *mkastbinary(ASTnodeType type, ASTnode *left, ASTnode *right, Value value)
{
    switch (type)
    {
    // Arithmetic
    case A_ADD:
    case A_SUB:
    case A_MUL:
    case A_DIV:
    case A_MOD:
    case A_POW:
        if (left->ptype != P_INT || right->ptype != P_INT)
        {
            fprintf(stderr, "Type Error: arithmetic op requires integer operands at %d:%d\n", Line, Column);
            exit(1);
        }
        return mkastnode(type, P_INT, left, NULL, right, value);
    // Comparison
    case A_EQ:
    case A_NEQ:
        if (left->ptype != right->ptype)
        {
            fprintf(stderr, "Type Error: comparison requires operands of same type at %d:%d\n", Line, Column);
            exit(1);
        }
        return mkastnode(type, P_BOOL, left, NULL, right, value);
    case A_LT:
    case A_LE:
    case A_GT:
    case A_GE:
        if (left->ptype != P_INT || right->ptype != P_INT)
        {
            fprintf(stderr, "Type Error: order comparison requires integer operands at %d:%d\n", Line, Column);
            exit(1);
        }
        return mkastnode(type, P_BOOL, left, NULL, right, value);
    // Logical
    case A_AND:
    case A_OR:
        if (left->ptype != P_BOOL || right->ptype != P_BOOL)
        {
            fprintf(stderr, "Type Error: logical op requires boolean operands at %d:%d\n", Line, Column);
            exit(1);
        }
        return mkastnode(type, P_BOOL, left, NULL, right, value);
    // Assignment
    case A_ASSIGN:
    case A_ASADD:
    case A_ASSUB:
    case A_ASMUL:
    case A_ASDIV:
    case A_ASMOD:
    case A_ASPOW:
    case A_ASAND:
    case A_ASOR:
        if (left->ptype != right->ptype)
        {
            fprintf(stderr, "Type Error: assignment type mismatch at %d:%d\n", Line, Column);
            exit(1);
        }
        return mkastnode(type, left->ptype, left, NULL, right, value);
    // Match
    case A_MATCH:
        return mkastnode(type, P_VOID, left, NULL, right, value);
    // Structural
    case A_GLUE:
        return mkastnode(type, P_VOID, left, NULL, right, value);
    default:
        fprintf(stderr, "Internal Error: unknown binary node type\n");
        exit(1);
    }
}

/**
 * Creates a ternary node.
 *
 * @param type Node type
 * @param left Left node
 * @param mid Mid node
 * @param right Right node
 * @param value Value
 */
ASTnode *mkastternary(ASTnodeType type, ASTnode *left, ASTnode *mid, ASTnode *right, Value value)
{
    switch (type)
    {
    // If-Else
    case A_IFELSE:
        if (left->ptype != P_BOOL)
        {
            fprintf(stderr, "Type Error: if condition must be boolean at %d:%d\n", Line, Column);
            exit(1);
        }
        return mkastnode(type, P_VOID, left, mid, right, value);
    // Loop
    case A_LOOP:
        if (left->ptype != P_BOOL)
        {
            fprintf(stderr, "Type Error: loop condition must be boolean at %d:%d\n", Line, Column);
            exit(1);
        }
        return mkastnode(type, P_VOID, left, mid, right, value);
    // Match
    case A_CASE:
        return mkastnode(type, P_VOID, left, mid, right, value);
    default:
        fprintf(stderr, "Internal Error: unknown ternary node type\n");
        exit(1);
    }
}
