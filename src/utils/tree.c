#include <stdio.h>
#include <stdlib.h>

#include "defs.h"
#include "data.h"
#include "decl.h"

// Make a new AST node
static ASTnode *mkastnode(ASTnodeType type, PType ptype, ASTnode *left, ASTnode *right, Value value)
{
    ASTnode *n;

    // Malloc a new AST node
    n = (ASTnode *)malloc(sizeof(ASTnode));
    if (n == NULL)
    {
        fprintf(stderr, "Error: out of memory");
        exit(1);
    }
    // Copy all the field values and return it
    n->type = type;
    n->ptype = ptype;
    n->left = left;
    n->right = right;
    n->value = value;
    return n;
}

// Make a new AST leaf node
ASTnode *mkastleaf(ASTnodeType type, PType ptype, Value value)
{
    return mkastnode(type, ptype, NULL, NULL, value);
}

// Make a new AST unary node
ASTnode *mkastunary(ASTnodeType type, ASTnode *child, Value value)
{
    switch (type)
    {
    default:
        return mkastnode(type, NO_PRIM, child, NULL, value);
    }
}

// Make a new AST binary node
ASTnode *mkastbinary(ASTnodeType type, ASTnode *left, ASTnode *right, Value value)
{
    switch (type)
    {
    case A_ADD:
    case A_SUB:
    case A_MUL:
    case A_DIV:
        // Check left type
        if (left->ptype != P_INT)
        {
            fprintf(stderr, "Type Error: left operand of math operation is not a number");
            exit(1);
        }
        // Check right type
        if (right->ptype != P_INT)
        {
            fprintf(stderr, "Type Error: right operand of math operation is not a number");
            exit(1);
        }
        return mkastnode(type, P_INT, left, right, value);
    case A_EQ:
    case A_NEQ:
    case A_LT:
    case A_GT:
    case A_LE:
    case A_GE:
        // Check left and right types
        if (left->ptype != right->ptype)
        {
            fprintf(stderr, "Type Error: cannot compare different types");
            exit(1);
        }
        return mkastnode(type, P_BOOL, left, right, value);
    case A_AND:
    case A_OR:
        // Check left and right types
        if (left->ptype != P_BOOL || right->ptype != P_BOOL)
        {
            fprintf(stderr, "Type Error: && and || require boolean operands");
            exit(1);
        }
        return mkastnode(type, P_BOOL, left, right, value);
    case A_ASSIGN:
        // Verify that the variable type is the same as the value
        if (left->ptype != right->ptype)
        {
            fprintf(stderr, "Type Error: incompatible assignment");
            exit(1);
        }
        return mkastnode(type, NO_PRIM, left, right, value);
    default:
        return mkastnode(type, NO_PRIM, left, right, value);
    }
}
