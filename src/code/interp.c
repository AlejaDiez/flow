#include <stdio.h>
#include <stdlib.h>
#include "defs.h"
#include "data.h"
#include "decl.h"

// Get AST result
int interpretAST(ASTnode *n)
{
    int leftval, rightval;

    if (n->type == A_INTLIT)
    {
        return n->value.integer;
    }

    leftval = interpretAST(n->left);
    rightval = interpretAST(n->right);

    switch (n->type)
    {
    case A_ADD:
        return leftval + rightval;
    case A_SUB:
        return leftval - rightval;
    case A_MUL:
        return leftval * rightval;
    case A_DIV:
        return leftval / rightval;
    default:
        fprintf(stderr, "Uknown AST node type\n");
        exit(1);
    }
}
