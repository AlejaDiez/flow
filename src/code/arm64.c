#include <stdio.h>
#include <stdlib.h>
#include "defs.h"
#include "data.h"
#include "decl.h"

// Register list
static char *reglist[4] = {"x0", "x1", "x2", "x3"};
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

// Load an integer literal into a register, returns the register number
int arm64_load(int value)
{
    int r = arm64_alloc_register();

    fprintf(OutFile, "\tmov %s, #%d\n", reglist[r], value);
    return r;
}

// Add two registers
int arm64_add(int r1, int r2)
{
    fprintf(OutFile, "\tadd %s, %s, %s\n", reglist[r2], reglist[r1], reglist[r2]);
    arm64_free_register(r1);
    return r2;
}

// Subtract two registers
int arm64_sub(int r1, int r2)
{
    fprintf(OutFile, "\tsub %s, %s, %s\n", reglist[r2], reglist[r1], reglist[r2]);
    arm64_free_register(r1);
    return r2;
}

// Multiply two registers
int arm64_mul(int r1, int r2)
{
    fprintf(OutFile, "\tmul %s, %s, %s\n", reglist[r2], reglist[r1], reglist[r2]);
    arm64_free_register(r1);
    return r2;
}

// Divide two registers
int arm64_div(int r1, int r2)
{
    fprintf(OutFile, "\tsdiv %s, %s, %s\n", reglist[r2], reglist[r1], reglist[r2]);
    arm64_free_register(r1);
    return r2;
}

// Print a register value using C library's printf
void arm64_printint(int r)
{
    // ARM64 macOS calling convention:
    // x0 = first argument (format string)
    // x1 = second argument (value to print)
    
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
void arm64_data_seg(void) {
    fprintf(OutFile, "\t.data\n");
    fprintf(OutFile, "_.str:\n");
    fprintf(OutFile, "\t.asciz \"Result: %%ld\\n\"\n");
}