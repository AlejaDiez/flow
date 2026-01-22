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

// Wrapper function for load an integer literal into a register, returns the register number
static int cg_load(int value)
{
    return arm64_load(value);
}

// Wrapper function for add two registers
static int cg_add(int r1, int r2)
{
    return arm64_add(r1, r2);
}

// Wrapper function for subtract two registers
static int cg_sub(int r1, int r2)
{
    return arm64_sub(r1, r2);
}

// Wrapper function for multiply two registers
static int cg_mul(int r1, int r2)
{
    return arm64_mul(r1, r2);
}

// Wrapper function for divide two registers
static int cg_div(int r1, int r2)
{
    return arm64_div(r1, r2);
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
// Generic AST walker to generate assembly code.
// Returns the register number containing the result of the sub-tree.
static int genAST(ASTnode *n)
{
    int leftreg, rightreg;

    // Handle NULL AST nodes
    if (n == NULL)
    {
        return NO_REG;
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
    case A_INTLIT:
        return cg_load(n->value.integer);
    default:
        fprintf(stderr, "Compile Error: unknown AST node type\n");
        exit(1);
    }
}

// Generate the assembly code for the entire program
void gencode(ASTnode *n)
{
    int reg;

    cg_preamble();
    reg = genAST(n);
    cg_printint(reg);
    cg_postamble();
    cg_data_seg();
}