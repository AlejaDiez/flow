/********************************************************************************
 * File Name: tree.c                                                            *
 * Description: This file contains all the functions for the AST tree.          *
 ********************************************************************************/

#include "defs.h"
#include "data.h"
#include "decl.h"

// Make a new AST node
AST *make_AST_node(AST_TYPE typ, PRIMITIVE_TYPE prim, AST *lft, AST *mid, AST *rgt, VALUE val)
{
    AST *n;

    // Malloc a new AST node
    n = (AST *)malloc(sizeof(AST));
    if (n == NULL)
        fatal("out of memory", EX_SOFTWARE);
    // Copy all the field values and return it
    n->type = typ;
    n->primitive = prim;
    n->left = lft;
    n->middle = mid;
    n->right = rgt;
    n->value = val;
    return n;
}

// Make a new AST binary node
AST *make_AST_binary(AST_TYPE typ, PRIMITIVE_TYPE prim, AST *lft, AST *rgt, VALUE val)
{
    return make_AST_node(typ, prim, lft, NULL, rgt, val);
}

// Make a new AST unary node
AST *make_AST_unary(AST_TYPE typ, PRIMITIVE_TYPE prim, AST *chld, VALUE val)
{
    return make_AST_node(typ, prim, chld, NULL, NULL, val);
}

// Make a new AST leaf node
AST *make_AST_leaf(AST_TYPE typ, PRIMITIVE_TYPE prim, VALUE val)
{
    return make_AST_node(typ, prim, NULL, NULL, NULL, val);
}

// Clean up the AST nodes
void free_AST(AST *n)
{
    if (n)
    {
        free_AST(n->left);
        free_AST(n->middle);
        free_AST(n->right);
        free(n);
    }
}

// Clean up the AST nodes children
void free_AST_chld(AST *n)
{
    if (n)
    {
        free(n->left);
        n->left = NULL;
        free(n->middle);
        n->middle = NULL;
        free(n->right);
        n->right = NULL;
    }
}
