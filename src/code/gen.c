#include <stdio.h>
#include <stdlib.h>

#include "defs.h"
#include "data.h"
#include "decl.h"

// Wrapper function for set all registers as available
static void cg_freeall_registers(void)
{
    return arm64_freeall_registers();
}

// Wrapper function for free a register by setting it as available
static void cg_free_register(int r)
{
    return arm64_free_register(r);
}

// Wrapper function for allocate a free register, return the number of the register.
static int cg_alloc_register(void)
{
    return arm64_alloc_register();
}

// Wrapper function for write the assembly preamble
static void cg_preamble(void)
{
    return arm64_preamble();
}

// Wrapper function for write the assembly postamble
static void cg_postamble(void)
{
    return arm64_postamble();
}

// Wrapper function for generate a label
static void cg_label(int l)
{
    return arm64_label(l);
}

// Wrapper function for generate a jump to a label
static void cg_jump(int l)
{
    return arm64_jump(l);
}

// Wrapper function for store a register value in a global variable and return the register number
static int cg_storglob(int r, int id)
{
    return arm64_storglob(r, id);
}

// Wrapper function for load a global variable into a register and return the register number
static int cg_loadglob(int id)
{
    return arm64_loadglob(id);
}

// Wrapper function for load an integer literal into a register, returns the register number
static int cg_loadint(int value)
{
    return arm64_loadint(value);
}

// Wrapper for addition operation between two registers
static int cg_add(int r1, int r2)
{
    return arm64_add(r1, r2);
}

// Wrapper for subtraction operation between two registers
static int cg_sub(int r1, int r2)
{
    return arm64_sub(r1, r2);
}

// Wrapper for multiplication operation between two registers
static int cg_mul(int r1, int r2)
{
    return arm64_mul(r1, r2);
}

// Wrapper for division operation between two registers
static int cg_div(int r1, int r2)
{
    return arm64_div(r1, r2);
}

// Wrapper for floor division operation between two registers
static int cg_fdiv(int r1, int r2)
{
    return arm64_fdiv(r1, r2);
}

// Wrapper for modulo operation between two registers
static int cg_mod(int r1, int r2)
{
    return arm64_mod(r1, r2);
}

// Wrapper for power operation between two registers
static int cg_pow(int r1, int r2)
{
    return arm64_pow(r1, r2);
}

// Wrapper for comparison operation between two registers
static int cg_cmp(int r1, int r2, char *op)
{
    return arm64_cmp(r1, r2, op);
}

// Wrapper for AND bitwise operation between two registers
static int cg_and(int r1, int r2)
{
    return arm64_and(r1, r2);
}

// Wrapper for OR bitwise operation between two registers
static int cg_or(int r1, int r2)
{
    return arm64_or(r1, r2);
}

// Wrapper function for print a register value using C library's printf
static void cg_printint(int r)
{
    return arm64_printint(r);
}

// Wrapper function for define the data section
static void cg_data_seg(void)
{
    return arm64_data_seg();
}

// Generic AST walker to generate assembly code, returns the register number containing the result of the sub-tree
static int genAST(ASTnode *n)
{
    int leftreg, rightreg;

    // Handle NULL AST nodes
    if (n == NULL)
    {
        return NO_REG;
    }

    // Handle special cases
    switch (n->type)
    {
    case A_SEQ:
        // Generate the left AST node
        leftreg = genAST(n->left);
        if (leftreg != NO_REG)
        {
            arm64_free_register(leftreg);
        }
        // Generate the right AST node
        rightreg = genAST(n->right);
        return rightreg;
    case A_ASSIGN:
        // R-Value
        rightreg = genAST(n->right);
        return cg_storglob(rightreg, n->left->value.integer);
    default:
        break;
    }

    // Generate the left AST node
    if (n->left)
    {
        leftreg = genAST(n->left);
    }
    // Generate the right AST node
    if (n->right)
    {
        rightreg = genAST(n->right);
    }

    // Handle the different AST node types
    switch (n->type)
    {
    case A_ADD:
        return cg_add(leftreg, rightreg);
    case A_SUB:
        return cg_sub(leftreg, rightreg);
    case A_MUL:
        return cg_mul(leftreg, rightreg);
    case A_DIV:
        return cg_div(leftreg, rightreg);
    case A_FDIV:
        return cg_fdiv(leftreg, rightreg);
    case A_MOD:
        return cg_mod(leftreg, rightreg);
    case A_POW:
        return cg_pow(leftreg, rightreg);
    case A_EQ:
        return cg_cmp(leftreg, rightreg, "eq");
    case A_NEQ:
        return cg_cmp(leftreg, rightreg, "ne");
    case A_LT:
        return cg_cmp(leftreg, rightreg, "lt");
    case A_LE:
        return cg_cmp(leftreg, rightreg, "le");
    case A_GT:
        return cg_cmp(leftreg, rightreg, "gt");
    case A_GE:
        return cg_cmp(leftreg, rightreg, "ge");
    case A_AND:
        return cg_and(leftreg, rightreg);
    case A_OR:
        return cg_or(leftreg, rightreg);
    case A_IDENT:
        return cg_loadglob(n->value.integer);
    case A_INTLIT:
        return cg_loadint(n->value.integer);
    case A_TRUE:
        return cg_loadint(1);
    case A_FALSE:
        return cg_loadint(0);
    case A_PRINT:
        cg_printint(leftreg);
        return NO_REG;
    default:
        fprintf(stderr, "Compile Error: unknown AST node type\n");
        exit(1);
    }
}

// Wrapper function for generate the assembly code for a global symbol
void genglobsym(int id)
{
    return arm64_globsym(id);
}

// Generate the assembly code for the entire program
void gencode(ASTnode *n)
{
    int reg;

    cg_preamble();
    reg = genAST(n);
    if (reg != NO_REG)
    {
        arm64_free_register(reg);
    }
    cg_postamble();
    cg_data_seg();
}