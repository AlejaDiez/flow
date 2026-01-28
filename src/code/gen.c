#include <stdio.h>
#include <stdlib.h>

#include "defs.h"
#include "data.h"
#include "decl.h"

// Generic AST walker to generate assembly code, returns the register number containing the result of the sub-tree
static int cgAST(ASTnode *n)
{
    int id, leftreg, rightreg;

    // Empty tree
    if (n == NULL)
    {
        return NO_REG;
    }

    switch (n->type)
    {
    // SEQUENCES
    case A_SEQ:
        leftreg = cgAST(n->left);
        if (leftreg != NO_REG)
        {
            CG->free_register(leftreg);
        }
        return cgAST(n->right);
    // LITERALS
    case A_INTLIT:
        return CG->loadint(n->value.integer);
    case A_IDENT:
        return CG->loadglob(n->value.integer);
    case A_TRUE:
        return CG->loadint(1);
    case A_FALSE:
        return CG->loadint(0);
    // ASSIGNMENTS
    case A_ASSIGN:
    case A_ASADD:
    case A_ASSUB:
    case A_ASMUL:
    case A_ASDIV:
    case A_ASMOD:
    case A_ASPOW:
    case A_ASAND:
    case A_ASOR:
        id = n->left->value.integer;
        // R-Value
        rightreg = cgAST(n->right);
        if (n->type == A_ASSIGN)
        {
            return CG->storglob(rightreg, id);
        }
        // L-Value
        leftreg = CG->loadglob(id);
        switch (n->type)
        {
        case A_ASADD:
            rightreg = CG->add(leftreg, rightreg);
            break;
        case A_ASSUB:
            rightreg = CG->sub(leftreg, rightreg);
            break;
        case A_ASMUL:
            rightreg = CG->mul(leftreg, rightreg);
            break;
        case A_ASDIV:
            rightreg = CG->div(leftreg, rightreg);
            break;
        case A_ASMOD:
            rightreg = CG->mod(leftreg, rightreg);
            break;
        case A_ASPOW:
            rightreg = CG->pow(leftreg, rightreg);
            break;
        case A_ASAND:
            rightreg = CG->and(leftreg, rightreg);
            break;
        case A_ASOR:
            rightreg = CG->or(leftreg, rightreg);
            break;
        default:
            rightreg = NO_REG;
            break;
        }
        return CG->storglob(rightreg, id);
    // BINARY OPERATIONS
    case A_ADD:
    case A_SUB:
    case A_MUL:
    case A_DIV:
    case A_MOD:
    case A_POW:
    case A_EQ:
    case A_NEQ:
    case A_LT:
    case A_GT:
    case A_LE:
    case A_GE:
    case A_AND:
    case A_OR:
        leftreg = cgAST(n->left);
        rightreg = cgAST(n->right);
        switch (n->type)
        {
            // ARITHMETIC
        case A_ADD:
            return CG->add(leftreg, rightreg);
        case A_SUB:
            return CG->sub(leftreg, rightreg);
        case A_MUL:
            return CG->mul(leftreg, rightreg);
        case A_DIV:
            return CG->div(leftreg, rightreg);
        case A_MOD:
            return CG->mod(leftreg, rightreg);
        case A_POW:
            return CG->pow(leftreg, rightreg);
        // LOGICAL
        case A_EQ:
            return CG->cmp(leftreg, rightreg, "eq");
        case A_NEQ:
            return CG->cmp(leftreg, rightreg, "ne");
        case A_LT:
            return CG->cmp(leftreg, rightreg, "lt");
        case A_GT:
            return CG->cmp(leftreg, rightreg, "gt");
        case A_LE:
            return CG->cmp(leftreg, rightreg, "le");
        case A_GE:
            return CG->cmp(leftreg, rightreg, "ge");
        case A_AND:
            return CG->and(leftreg, rightreg);
        case A_OR:
            return CG->or(leftreg, rightreg);
        default:
            return NO_REG;
        }
    // UNARY OPERATIONS
    case A_POS:
    case A_NEG:
    case A_NOT:
        leftreg = cgAST(n->left);
        switch (n->type)
        {
        case A_POS:
            return leftreg;
        case A_NEG:
            return CG->neg(leftreg);
        case A_NOT:
            return CG->not(leftreg);
        default:
            return NO_REG;
        }
    // OTHERS
    case A_PRINT:
        leftreg = cgAST(n->left);
        CG->printint(leftreg);
        return NO_REG;
    default:
        fprintf(stderr, "Fatal Error: Unknown AST Node type %d\n", n->type);
        exit(1);
    }
}

// Generate the assembly code for a global symbol
void genglobsym(int id)
{
    CG->globsym(id);
}

// Generate the assembly code for the entire program
void gencode(ASTnode *n)
{
    int reg;

    CG->preamble();
    reg = cgAST(n);
    if (reg != NO_REG)
    {
        CG->free_register(reg);
    }
    CG->postamble();
    CG->data_seg();
}
