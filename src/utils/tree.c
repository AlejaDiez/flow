#include <stdio.h>
#include <stdlib.h>

#include "defs.h"
#include "data.h"
#include "decl.h"

// Make a new AST node
ASTnode *mkastnode(ASTnodeType type, ASTnode *left, ASTnode *right, Value value)
{
    ASTnode *n;

    // Malloc a new AST node
    n = (ASTnode *)malloc(sizeof(ASTnode));
    if (n == NULL)
    {
        fprintf(stderr, "Error: out of memory\n");
        exit(1);
    }
    // Copy all the field values and return it
    n->type = type;
    n->left = left;
    n->right = right;
    n->value = value;
    return n;
}

// Make a new AST leaf node
ASTnode *mkastleaf(ASTnodeType type, Value value)
{
    return mkastnode(type, NULL, NULL, value);
}

// Make a new AST unary node
ASTnode *mkastunary(ASTnodeType type, ASTnode *child, Value value)
{
    return mkastnode(type, child, NULL, value);
}

// Make a new AST unary node
ASTnode *mkastbinary(ASTnodeType type, ASTnode *left, ASTnode *right, Value value)
{
    return mkastnode(type, left, right, value);
}
