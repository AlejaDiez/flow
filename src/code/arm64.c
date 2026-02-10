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
    fprintf(OutFile, "\t.text\n");
    fprintf(OutFile, "\t.global _start\n");
    fprintf(OutFile, "\t.align 2\n");
    fprintf(OutFile, "_start:\n");
    fprintf(OutFile, "\tstp x29, x30, [sp, -16]!\n");
    fprintf(OutFile, "\tmov x29, sp\n");
}

// Write the function epilogue
static void arm64_postamble(void)
{
    fprintf(OutFile, "\tmov x0, #0\n");
    fprintf(OutFile, "\tmov x16, #1\n");
    fprintf(OutFile, "\tsvc #0x80\n");
}

// Define the data section
static void arm64_data_seg(void)
{
    fprintf(OutFile, "\t.data\n");
}

// Generate a global symbol definition
static void arm64_globsym(int id)
{
    PType ptype = GlobalSymbols[id].ptype;

    switch (ptype)
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

// Print a function preamble to the output file
static void arm64_genfunlabel(int id)
{
    char *name = GlobalSymbols[id].name;

    fprintf(OutFile, "_%s:\n", name);
    fprintf(OutFile, "\tstp x29, x30, [sp, -16]!\n");
    fprintf(OutFile, "\tmov x29, sp\n");
}

// Print a function postamble to the output file
static void arm64_genfunend(void)
{
    fprintf(OutFile, "\tldp x29, x30, [sp], 16\n");
    fprintf(OutFile, "\tret\n");
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

// Call a function
static int arm64_call(int id)
{
    int r = arm64_alloc_register();

    fprintf(OutFile, "\tbl _%s\n", GlobalSymbols[id].name);
    fprintf(OutFile, "\tmov %s, x0\n", reglist[r]);
    return r;
}

// Return a value
static void arm64_return(int r)
{
    fprintf(OutFile, "\tmov x0, %s\n", reglist[r]);
    arm64_free_register(r);
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
    PType ptype = GlobalSymbols[id].ptype;
    int r = arm64_alloc_register();

    // Load address of the variable
    fprintf(OutFile, "\tadrp %s, %s@PAGE\n", reglist[r], GlobalSymbols[id].name);
    fprintf(OutFile, "\tadd %s, %s, %s@PAGEOFF\n", reglist[r], reglist[r], GlobalSymbols[id].name);

    // Load value based on type
    switch (ptype)
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
    PType ptype = GlobalSymbols[id].ptype;
    int r_addr = arm64_alloc_register();

    // Load address of the variable
    fprintf(OutFile, "\tadrp %s, %s@PAGE\n", reglist[r_addr], GlobalSymbols[id].name);
    fprintf(OutFile, "\tadd %s, %s, %s@PAGEOFF\n", reglist[r_addr], reglist[r_addr], GlobalSymbols[id].name);

    // Store value based on type
    switch (ptype)
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
// Print integer
static void arm64_printint(int r)
{
    int L_convert_loop = arm64_label();
    int L_print_now = arm64_label();

    // 1. Preparar pila (32 bytes)
    fprintf(OutFile, "\tsub sp, sp, #32\n");

    // 2. Mover número a x9 (Registro temporal)
    fprintf(OutFile, "\tmov x9, %s\n", reglist[r]);

    // 3. Puntero al final del buffer (sp + 30)
    fprintf(OutFile, "\tadd x10, sp, #30\n");

    // 4. Poner salto de línea ('\n')
    fprintf(OutFile, "\tmov w11, #10\n");
    fprintf(OutFile, "\tstrb w11, [x10]\n");
    fprintf(OutFile, "\tsub x10, x10, #1\n"); // Retroceder puntero

    // 5. BUCLE DE CONVERSIÓN (Tipo Do-While)
    arm64_genlabel(L_convert_loop);

    // A. División y Módulo
    fprintf(OutFile, "\tmov x11, #10\n");
    fprintf(OutFile, "\tudiv x12, x9, x11\n");      // x12 = Cociente (x9 / 10)
    fprintf(OutFile, "\tmsub x13, x12, x11, x9\n"); // x13 = Resto (x9 - x12*10)

    // B. Convertir a ASCII y guardar
    fprintf(OutFile, "\tadd x13, x13, #48\n"); // 0 -> '0'
    fprintf(OutFile, "\tstrb w13, [x10]\n");   // Guardar en la pila
    fprintf(OutFile, "\tsub x10, x10, #1\n");  // Mover puntero atrás

    // C. Actualizar el número y comprobar
    fprintf(OutFile, "\tmov x9, x12\n"); // x9 = Cociente

    // D. Condición del Do-While: ¿Es 0 el cociente?
    // cbnz: Compare and Branch if Not Zero
    // Si x9 NO es 0, volvemos arriba. Si es 0, seguimos bajando.
    fprintf(OutFile, "\tcbnz x9, L%d\n", L_convert_loop);

    // 6. IMPRIMIR (Syscall)
    arm64_genlabel(L_print_now);

    // Ajustar puntero al inicio real (porque el bucle restó 1 de más al final)
    fprintf(OutFile, "\tadd x10, x10, #1\n");

    // Calcular longitud: (sp + 31) - inicio
    fprintf(OutFile, "\tadd x11, sp, #30\n"); // Dirección del final (\n)
    fprintf(OutFile, "\tsub x2, x11, x10\n"); // x2 = Diferencia
    fprintf(OutFile, "\tadd x2, x2, #1\n");   // +1 para incluir el \n

    // Syscall Write
    fprintf(OutFile, "\tmov x0, #1\n");  // stdout
    fprintf(OutFile, "\tmov x1, x10\n"); // buffer address
    fprintf(OutFile, "\tmov x16, #4\n"); // write (macOS)
    fprintf(OutFile, "\tsvc #0x80\n");

    // 7. Limpiar pila y liberar registro AST
    fprintf(OutFile, "\tadd sp, sp, #32\n");
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
    .genfunlabel = arm64_genfunlabel,
    .genfunend = arm64_genfunend,
    .jump = arm64_jump,
    .jump_cond = arm64_jump_cond,
    .call = arm64_call,
    .ret = arm64_return,
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