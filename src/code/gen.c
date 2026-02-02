#include <stdio.h>
#include <stdlib.h>

#include "defs.h"
#include "data.h"
#include "decl.h"

#define MAX_NESTED_CONTROLS 64

// Control context
static struct
{
    ASTnodeType type;
    int label_continue;
    int label_end;
} control_stack[MAX_NESTED_CONTROLS];
static int control_top = -1;

// Control stack management, add a new control
static void push_control(int type, int cont, int end)
{
    if (control_top >= MAX_NESTED_CONTROLS - 1)
    {
        fprintf(stderr, "Compiler Error: controls nested too deeply\n");
        exit(1);
    }
    control_top++;
    control_stack[control_top].type = type;
    control_stack[control_top].label_continue = cont;
    control_stack[control_top].label_end = end;
}

// Control stack management, remove a control
static void pop_control(void)
{
    if (control_top < 0)
    {
        fprintf(stderr, "Compiler Error: control stack underflow\n");
        exit(1);
    }
    control_top--;
}

// Generic AST walker to generate assembly code, returns the register number containing the result of the sub-tree
static int cgAST(ASTnode *n)
{
    int id, leftreg, midreg, rightreg;

    // Empty tree
    if (n == NULL)
    {
        return NO_REG;
    }

    switch (n->type)
    {
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
    // LITERALS
    case A_INTLIT:
        return CG->loadint(n->value.integer);
    case A_IDENT:
        return CG->loadglob(n->value.integer);
    case A_TRUE:
        return CG->loadint(1);
    case A_FALSE:
        return CG->loadint(0);
    // STATEMENTS
    case A_IFELSE:
    {
        int Lfalse, Lend;

        // Get labels
        Lfalse = CG->label();
        if (n->right)
        {
            Lend = CG->label();
        }

        // Check condition
        leftreg = cgAST(n->left);
        CG->jump_cond(leftreg, Lfalse); // If false, jump to false
        CG->free_register(leftreg);

        // True statement
        midreg = cgAST(n->mid);
        if (midreg != NO_REG)
        {
            CG->free_register(midreg);
        }

        // False statement
        if (n->right)
        {
            CG->jump(Lend);       // Jump to the end to skip the else statement
            CG->genlabel(Lfalse); // False label

            rightreg = cgAST(n->right);
            if (rightreg != NO_REG)
            {
                CG->free_register(rightreg);
            }
            CG->genlabel(Lend); // End label
        }
        else
        {
            CG->genlabel(Lfalse); // False label
        }
        return NO_REG;
    }
    case A_MATCH:
    {
        int Lend, Lnext_check;
        ASTnode *c;

        // Get labels
        Lend = CG->label();

        // Add loop to the stack
        push_control(A_MATCH, 0, Lend);

        // Get all cases
        c = n->right;
        while (c != NULL)
        {
            // Create next check label
            Lnext_check = CG->label();

            if (c->left != NULL)
            {
                rightreg = cgAST(n->left);
                leftreg = cgAST(c->left);

                // Check condition
                leftreg = CG->cmp(rightreg, leftreg, "eq");
                CG->jump_cond(leftreg, Lnext_check);
                CG->free_register(leftreg);
            }

            // Case stetement
            midreg = cgAST(c->mid);
            if (midreg != NO_REG)
            {
                CG->free_register(midreg);
            }

            // Generate next label
            CG->genlabel(Lnext_check);
            
            c = c->right;
        }
        // Generate end label
        CG->genlabel(Lend);

        // Remove loop from the stack
        pop_control();
        return NO_REG;
    }
    case A_LOOP:
    {
        int Lstart, Lcontinue, Lend;

        // Get labels
        Lstart = CG->label();
        Lcontinue = CG->label();
        Lend = CG->label();

        // Add loop to the stack
        push_control(A_LOOP, Lcontinue, Lend);

        // Start label
        CG->genlabel(Lstart);

        // Check condition
        leftreg = cgAST(n->left);
        CG->jump_cond(leftreg, Lend);
        CG->free_register(leftreg);

        // Statement
        midreg = cgAST(n->mid);
        if (midreg != NO_REG)
        {
            CG->free_register(midreg);
        }

        // Continue label
        CG->genlabel(Lcontinue);

        // Check update
        if (n->right)
        {
            rightreg = cgAST(n->right);
            if (rightreg != NO_REG)
            {
                CG->free_register(rightreg);
            }
        }

        // Jump to start
        CG->jump(Lstart);

        // End label
        CG->genlabel(Lend);

        // Remove loop from the stack
        pop_control();
        return NO_REG;
    }
    case A_STOP:
        if (control_stack < 0)
        {
            fprintf(stderr, "Error: stop used outside of a control structure\n");
            exit(1);
        }
        CG->jump(control_stack[control_top].label_end);
        return NO_REG;
    case A_NEXT:
        if (control_stack < 0)
        {
            fprintf(stderr, "Error: next used outside of a control structure\n");
            exit(1);
        }
        CG->jump(control_stack[control_top].label_continue);
        return NO_REG;
    case A_PRINT:
        leftreg = cgAST(n->left);
        CG->printint(leftreg);
        return NO_REG;
    // GROUPING
    case A_SEQ:
        leftreg = cgAST(n->left);
        if (leftreg != NO_REG)
        {
            CG->free_register(leftreg);
        }
        return cgAST(n->right);
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
