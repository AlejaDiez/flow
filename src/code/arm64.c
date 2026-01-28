#include <stdio.h>
#include <stdlib.h>

#include "defs.h"
#include "data.h"
#include "decl.h"

// Register names: 64-bit (x0-x3) and 32-bit (w0-w3)
static char *reglist[4] = {"x0", "x1", "x2", "x3"};
static char *wreglist[4] = {"w0", "w1", "w2", "w3"};

// Register availability flags (1 = free, 0 = used)
static int freeregs[4];

// Global counter for generating unique label IDs (L1, L2...)
static int label_count = 0;

// REGISTER MANAGEMENT
// Set all registers as available
static void arm64_freeall_registers(void)
{
    freeregs[0] = freeregs[1] = freeregs[2] = freeregs[3] = 1;
}

// Free a specific register
static void arm64_free_register(int r)
{
    if (freeregs[r] != 0)
    {
        fprintf(stderr, "Compile Error: trying to free register x%d which is already free\n", r);
        exit(1);
    }
    freeregs[r] = 1;
}

// Allocate a free register, returns index
static int arm64_alloc_register(void)
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

// ASSEMBLY STRUCTURE
// Write the assembly preamble
static void arm64_preamble(void)
{
    arm64_freeall_registers();
    fprintf(OutFile, "\t.global _main\n");
    fprintf(OutFile, "\t.align 2\n");
    fprintf(OutFile, "\t.text\n");
    fprintf(OutFile, "_main:\n");
    // Save Frame Pointer (x29) and Link Register (x30) to stack
    fprintf(OutFile, "\tstp x29, x30, [sp, -16]!\n");
    fprintf(OutFile, "\tmov x29, sp\n");
}

// Write the function epilogue
static void arm64_postamble(void)
{
    fprintf(OutFile, "\tmov w0, #0\n");
    fprintf(OutFile, "\tldp x29, x30, [sp], 16\n");
    fprintf(OutFile, "\tret\n");
}

// Define the data section
static void arm64_data_seg(void)
{
    fprintf(OutFile, "\t.data\n");
    // Format string for printf
    fprintf(OutFile, "_.str:\n");
    fprintf(OutFile, "\t.asciz \"%%ld\\n\"\n");
}

// Generate a global symbol definition
static void arm64_globsym(int id)
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
    }
}

// CONTROL FLOW
// Generate a new unique label ID, return the label id
static int arm64_label(void)
{
    return ++label_count;
}

// Print a label to the output file
static void arm64_genlabel(int l)
{
    fprintf(OutFile, "L%d:\n", l);
}

// Unconditional jump
static void arm64_jump(int l)
{
    fprintf(OutFile, "\tb L%d\n", l);
}

// Jump if the register is Zero
static void arm64_jump_cond(int r, int l)
{
    fprintf(OutFile, "\tcbz %s, L%d\n", reglist[r], l);
}

// MEMORY ACCESS
// Load an integer literal into a register
static int arm64_loadint(int v)
{
    int r = arm64_alloc_register();

    fprintf(OutFile, "\tmov %s, #%d\n", reglist[r], v);
    return r;
}

// Load a global variable into a register
static int arm64_loadglob(int id)
{
    int type = GlobalSymbols[id].type;
    int r = arm64_alloc_register();

    // Load address of the variable
    fprintf(OutFile, "\tadrp %s, %s@PAGE\n", reglist[r], GlobalSymbols[id].name);
    fprintf(OutFile, "\tadd %s, %s, %s@PAGEOFF\n", reglist[r], reglist[r], GlobalSymbols[id].name);

    // Load value based on type
    switch (type)
    {
    case P_INT:
        fprintf(OutFile, "\tldr %s, [%s]\n", reglist[r], reglist[r]);
        break;
    case P_BOOL:
        fprintf(OutFile, "\tldrb %s, [%s]\n", wreglist[r], reglist[r]); // Load byte
        break;
    }
    return r;
}

// Store a register value into a global variable
static int arm64_storglob(int r, int id)
{
    int type = GlobalSymbols[id].type;
    int r_addr = arm64_alloc_register();

    // Load address of the variable
    fprintf(OutFile, "\tadrp %s, %s@PAGE\n", reglist[r_addr], GlobalSymbols[id].name);
    fprintf(OutFile, "\tadd %s, %s, %s@PAGEOFF\n", reglist[r_addr], reglist[r_addr], GlobalSymbols[id].name);

    // Store value based on type
    switch (type)
    {
    case P_INT:
        fprintf(OutFile, "\tstr %s, [%s]\n", reglist[r], reglist[r_addr]);
        break;
    case P_BOOL:
        fprintf(OutFile, "\tstrb %s, [%s]\n", wreglist[r], reglist[r_addr]); // Store byte
        break;
    }

    arm64_free_register(r_addr);
    return r;
}

// ARITHMETIC OPERATIONS
// Addition operation
static int arm64_add(int r1, int r2)
{
    fprintf(OutFile, "\tadd %s, %s, %s\n", reglist[r2], reglist[r1], reglist[r2]);
    arm64_free_register(r1);
    return r2;
}

// Subtraction operation
static int arm64_sub(int r1, int r2)
{
    fprintf(OutFile, "\tsub %s, %s, %s\n", reglist[r2], reglist[r1], reglist[r2]);
    arm64_free_register(r1);
    return r2;
}

// Negation operation
static int arm64_neg(int r)
{
    fprintf(OutFile, "\tneg %s, %s\n", reglist[r], reglist[r]);
    return r;
}

// Multiplication operation
static int arm64_mul(int r1, int r2)
{
    fprintf(OutFile, "\tmul %s, %s, %s\n", reglist[r2], reglist[r1], reglist[r2]);
    arm64_free_register(r1);
    return r2;
}

// Division operation
static int arm64_div(int r1, int r2)
{
    fprintf(OutFile, "\tsdiv %s, %s, %s\n", reglist[r2], reglist[r1], reglist[r2]);
    arm64_free_register(r1);
    return r2;
}

// Modulo operation
static int arm64_mod(int r1, int r2)
{
    int r_quot = arm64_alloc_register();

    // x % y = x - (x / y) * y
    fprintf(OutFile, "\tsdiv %s, %s, %s\n", reglist[r_quot], reglist[r1], reglist[r2]);
    fprintf(OutFile, "\tmsub %s, %s, %s, %s\n", reglist[r2], reglist[r_quot], reglist[r2], reglist[r1]);

    arm64_free_register(r1);
    arm64_free_register(r_quot);
    return r2;
}

// Power operation
static int arm64_pow(int r1, int r2)
{
    int Lloop = arm64_label();
    int Lend = arm64_label();
    int r_res = arm64_alloc_register();

    // r1: base, r2: exponent, r_res: result (init 1)
    fprintf(OutFile, "\tmov %s, #1\n", reglist[r_res]);

    arm64_genlabel(Lloop);
    fprintf(OutFile, "\tcbz %s, L%d\n", reglist[r2], Lend); // If exp is 0, jump to end

    fprintf(OutFile, "\tmul %s, %s, %s\n", reglist[r_res], reglist[r_res], reglist[r1]); // res = res * base
    fprintf(OutFile, "\tsub %s, %s, #1\n", reglist[r2], reglist[r2]);                    // exp--

    arm64_jump(Lloop);

    arm64_genlabel(Lend);
    fprintf(OutFile, "\tmov %s, %s\n", reglist[r2], reglist[r_res]); // Move result to r2

    arm64_free_register(r1);
    arm64_free_register(r_res);
    return r2;
}

// LOGICAL OPERATIONS
// Not operation
static int arm64_not(int r)
{
    fprintf(OutFile, "\teor %s, %s, #1\n", reglist[r], reglist[r]);
    return r;
}

// Comparison operation
static int arm64_cmp(int r1, int r2, char *op)
{
    fprintf(OutFile, "\tcmp %s, %s\n", reglist[r1], reglist[r2]);
    fprintf(OutFile, "\tcset %s, %s\n", reglist[r2], op);
    arm64_free_register(r1);
    return r2;
}

// And operation
static int arm64_and(int r1, int r2)
{
    fprintf(OutFile, "\tand %s, %s, %s\n", reglist[r2], reglist[r1], reglist[r2]);
    arm64_free_register(r1);
    return r2;
}

// Or operation
static int arm64_or(int r1, int r2)
{
    fprintf(OutFile, "\torr %s, %s, %s\n", reglist[r2], reglist[r1], reglist[r2]);
    arm64_free_register(r1);
    return r2;
}

// I/O
// Print integer using C library printf
static void arm64_printint(int r)
{
    // Move value to print to stack
    fprintf(OutFile, "\tstr %s, [sp]\n", reglist[r]);

    // Load format string address into x0
    fprintf(OutFile, "\tadrp x0, _.str@PAGE\n");
    fprintf(OutFile, "\tadd x0, x0, _.str@PAGEOFF\n");

    // Call printf
    fprintf(OutFile, "\tbl _printf\n");

    arm64_free_register(r);
}

// ARM64 BACKEND
struct Backend ARM64_Backend = {
    .freeall_registers = arm64_freeall_registers,
    .free_register = arm64_free_register,
    .alloc_register = arm64_alloc_register,
    .preamble = arm64_preamble,
    .postamble = arm64_postamble,
    .data_seg = arm64_data_seg,
    .globsym = arm64_globsym,
    .label = arm64_label,
    .genlabel = arm64_genlabel,
    .jump = arm64_jump,
    .jump_cond = arm64_jump_cond,
    .loadint = arm64_loadint,
    .loadglob = arm64_loadglob,
    .storglob = arm64_storglob,
    .add = arm64_add,
    .sub = arm64_sub,
    .neg = arm64_neg,
    .mul = arm64_mul,
    .div = arm64_div,
    .mod = arm64_mod,
    .pow = arm64_pow,
    .not = arm64_not,
    .cmp = arm64_cmp,
    .and = arm64_and,
    .or = arm64_or,
    .printint = arm64_printint,
};