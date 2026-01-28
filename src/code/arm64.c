#include <stdio.h>
#include <stdlib.h>
#include "defs.h"
#include "data.h"
#include "decl.h"

// Register list
static char *reglist[4] = {"x0", "x1", "x2", "x3"};
static char *wreglist[4] = {"w0", "w1", "w2", "w3"};
// Register availability
static int freeregs[4];

// Set all registers as available
void arm64_freeall_registers(void)
{
    freeregs[0] = freeregs[1] = freeregs[2] = freeregs[3] = 1;
}

// Free a register by setting it as available
void arm64_free_register(int r)
{
    if (freeregs[r] != 0)
    {
        fprintf(stderr, "Compile Error: error trying to free register x%d\n", r);
        exit(1);
    }
    freeregs[r] = 1;
}

// Allocate a free register, return the number of the register.
int arm64_alloc_register(void)
{
    for (int i = 0; i < 4; i++)
    {
        if (freeregs[i])
        {
            freeregs[i] = 0;
            return i;
        }
    }
    fprintf(stderr, "Compile Error: out of registers\n");
    exit(1);
}

// Write the assembly preamble
void arm64_preamble(void)
{
    arm64_freeall_registers();

    fprintf(OutFile, "\t.global _main\n");
    fprintf(OutFile, "\t.align 2\n");
    fprintf(OutFile, "\t.text\n");
    fprintf(OutFile, "_main:\n");
    fprintf(OutFile, "\tstp x29, x30, [sp, -16]!\n");
    fprintf(OutFile, "\tmov x29, sp\n");
}

// Write the assembly postamble
void arm64_postamble(void)
{
    fprintf(OutFile, "\tldp x29, x30, [sp], 16\n");
    fprintf(OutFile, "\tret\n");
}

// Generate a label
void arm64_label(int l)
{
    fprintf(OutFile, "L%d:\n", l);
}

// Generate a jump to a label
void arm64_jump(int l)
{
    fprintf(OutFile, "\tb L%d\n", l);
}

// Generate the assembly code for a global symbol
void arm64_globsym(int id)
{
    int type = GlobalSymbols[id].type;

    switch (type)
    {
    case P_INT:
        fprintf(OutFile, "\t.comm %s, 8, 3\n", GlobalSymbols[id].name);
        break;
    case P_BOOL:
        fprintf(OutFile, "\t.comm %s, 1, 0\n", GlobalSymbols[id].name);
        break;
    default:
        break;
    }
}

// Store a register value in a global variable and return the register number
int arm64_storglob(int r, int id)
{
    int type = GlobalSymbols[id].type;
    int r_addr = arm64_alloc_register();

    fprintf(OutFile, "\tadrp %s, %s@PAGE\n", reglist[r_addr], GlobalSymbols[id].name);
    fprintf(OutFile, "\tadd %s, %s, %s@PAGEOFF\n", reglist[r_addr], reglist[r_addr], GlobalSymbols[id].name);

    switch (type)
    {
    case P_INT:
        fprintf(OutFile, "\tstr %s, [%s]\n", reglist[r], reglist[r_addr]);
        break;
    case P_BOOL:
        fprintf(OutFile, "\tstrb %s, [%s]\n", wreglist[r], reglist[r_addr]);
        break;
    default:
        break;
    }

    arm64_free_register(r_addr);
    return r;
}

// Load a global variable into a register and return the register number
int arm64_loadglob(int id)
{
    int type = GlobalSymbols[id].type;
    int r = arm64_alloc_register();

    fprintf(OutFile, "\tadrp %s, %s@PAGE\n", reglist[r], GlobalSymbols[id].name);
    fprintf(OutFile, "\tadd %s, %s, %s@PAGEOFF\n", reglist[r], reglist[r], GlobalSymbols[id].name);

    switch (type)
    {
    case P_INT:
        fprintf(OutFile, "\tldr %s, [%s]\n", reglist[r], reglist[r]);
        break;
    case P_BOOL:
        fprintf(OutFile, "\tldrb %s, [%s]\n", wreglist[r], reglist[r]);
        break;
    default:
        break;
    }

    return r;
}

// Load an integer literal into a register, returns the register number
int arm64_loadint(int value)
{
    int r = arm64_alloc_register();

    fprintf(OutFile, "\tmov %s, #%d\n", reglist[r], value);
    return r;
}

// Negation operation for one register
int arm64_neg(int r)
{
    fprintf(OutFile, "\tneg %s, %s\n", reglist[r], reglist[r]);
    return r;
}

// Addition operation between two registers
int arm64_add(int r1, int r2)
{
    fprintf(OutFile, "\tadd %s, %s, %s\n", reglist[r2], reglist[r1], reglist[r2]);
    arm64_free_register(r1);
    return r2;
}

// Subtraction operation between two registers
int arm64_sub(int r1, int r2)
{
    fprintf(OutFile, "\tsub %s, %s, %s\n", reglist[r2], reglist[r1], reglist[r2]);
    arm64_free_register(r1);
    return r2;
}

// Multiplication operation between two registers
int arm64_mul(int r1, int r2)
{
    fprintf(OutFile, "\tmul %s, %s, %s\n", reglist[r2], reglist[r1], reglist[r2]);
    arm64_free_register(r1);
    return r2;
}

// Division operation between two registers
int arm64_div(int r1, int r2)
{
    fprintf(OutFile, "\tsdiv %s, %s, %s\n", reglist[r2], reglist[r1], reglist[r2]);
    arm64_free_register(r1);
    return r2;
}

// Modulo operation between two registers
int arm64_mod(int r1, int r2)
{
    int r_quot = arm64_alloc_register();

    fprintf(OutFile, "\tsdiv %s, %s, %s\n", reglist[r_quot], reglist[r1], reglist[r2]);
    fprintf(OutFile, "\tmsub %s, %s, %s, %s\n", reglist[r2], reglist[r_quot], reglist[r2], reglist[r1]);
    arm64_free_register(r1);
    arm64_free_register(r_quot);
    return r2;
}

// Power operation between two registers
int arm64_pow(int r1, int r2)
{
    int Lloop = label();
    int Lend = label();
    int r_res = arm64_alloc_register();

    fprintf(OutFile, "\tmov %s, #1\n", reglist[r_res]);
    arm64_label(Lloop);
    fprintf(OutFile, "\tcbz %s, L%d\n", reglist[r2], Lend);
    fprintf(OutFile, "\tmul %s, %s, %s\n", reglist[r_res], reglist[r_res], reglist[r1]);
    fprintf(OutFile, "\tsub %s, %s, #1\n", reglist[r2], reglist[r2]);
    arm64_jump(Lloop);
    arm64_label(Lend);
    fprintf(OutFile, "\tmov %s, %s\n", reglist[r2], reglist[r_res]);
    arm64_free_register(r1);
    arm64_free_register(r_res);

    return r2;
}

// Comparison operation between two registers
// eq: ==
// ne: !=
// lt: <
// le: <=
// gt: >
// ge: >=
int arm64_cmp(int r1, int r2, char *op)
{
    fprintf(OutFile, "\tcmp %s, %s\n", reglist[r1], reglist[r2]);
    fprintf(OutFile, "\tcset %s, %s\n", reglist[r2], op);
    arm64_free_register(r1);
    return r2;
}

// AND bitwise operation between two registers
int arm64_and(int r1, int r2)
{
    fprintf(OutFile, "\tand %s, %s, %s\n", reglist[r2], reglist[r1], reglist[r2]);
    arm64_free_register(r1);
    return r2;
}

// OR bitwise operation between two registers
int arm64_or(int r1, int r2)
{
    fprintf(OutFile, "\torr %s, %s, %s\n", reglist[r2], reglist[r1], reglist[r2]);
    arm64_free_register(r1);
    return r2;
}

// NOT operation for one register
int arm64_not(int r)
{
    fprintf(OutFile, "\teor %s, %s, #1\n", reglist[r], reglist[r]);
    return r;
}

// Print a register value using C library's printf
void arm64_printint(int r)
{
    // Move the value to print into x1
    fprintf(OutFile, "\tstr	%s, [sp]\n", reglist[r]);

    // Load the address of the format string into x0
    fprintf(OutFile, "\tadrp x0, _.str@PAGE\n");
    fprintf(OutFile, "\tadd x0, x0, _.str@PAGEOFF\n");

    // Call printf
    fprintf(OutFile, "\tbl _printf\n");

    arm64_free_register(r);
}

// Define the data section
void arm64_data_seg(void)
{
    fprintf(OutFile, "\t.data\n");
    fprintf(OutFile, "_.str:\n");
    fprintf(OutFile, "\t.asciz \"%%ld\\n\"\n");
}