/********************************************************************************
 * File Name: src/backend/arm64.c                                               *
 *                                                                              *
 * Description: ARM64 Backend Implementation, generates assembly code for       *
 *              Apple Silicon and Linux AArch64.                                *
 * Author: Alejandro Diez Bermejo                                               *
 * Date: 2026-01-01                                                             *
 * Version: 0.0.0                                                               *
 ********************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "defs.h"
#include "data.h"
#include "decl.h"

// Register management
static char *reglist[7] = {"x9", "x10", "x11", "x12", "x13", "x14", "x15"};
static char *wreglist[7] = {"w9", "w10", "w11", "w12", "w13", "w14", "w15"};
static int freeregs[7];

/**
 * Marks all temporary registers as available.
 */
static void freeall_registers(void)
{
    for (int i = 0; i < 7; i++)
    {
        freeregs[i] = 1;
    }
}

/**
 * Free a specific temporary register.
 *
 * @param r The register to free
 */
static void free_register(int r)
{
    if (freeregs[r])
    {
        fprintf(stderr, "Error: internal compiler error (double free reg x%d)\n", r + 9);
        exit(1);
    }
    freeregs[r] = 1;
}

/**
 * Allocates a free temporary register.
 *
 * @return The register index
 */
static int alloc_register(void)
{
    for (int i = 0; i < 7; i++)
    {
        if (freeregs[i])
        {
            freeregs[i] = 0;
            return i;
        }
    }
    fprintf(stderr, "Error: out of registers in expression evaluation\n");
    exit(1);
}

// Sections
/**
 * Emits the data segment to the output assembly file.
 *
 * @param gen Pointer to a function that takes a Symbol* and generates its assembly code
 * @param sym The Symbol structure containing the first global
 */
static void data_seg(void (*gen)(Symbol *), Symbol *sym)
{
    fprintf(OutFile, "\t.data\n");
    gen(sym);
}

/**
 * Emits the text segment, entry point, and standard exit sequence.
 *
 * @param gen Pointer to a function that traverses the AST and generates instructions
 * @param node The root node of the Abstract Syntax Tree to be processed
 */
static void text_seg(int (*gen)(ASTnode *), ASTnode *node)
{
    fprintf(OutFile, "\t.text\n");
    fprintf(OutFile, "\t.global _start\n");
    fprintf(OutFile, "\t.align 2\n");
    fprintf(OutFile, "_start:\n");
    gen(node);
    fprintf(OutFile, "\tmov x0, #0\n");  // Exit code 0
    fprintf(OutFile, "\tmov x16, #1\n"); // Syscall Exit
    fprintf(OutFile, "\tsvc #0x80\n");
}

// Global variables
/**
 * Generate a global symbol.
 *
 * @param sym The symbol to generate
 */
static void globsym(Symbol *sym)
{
    // .comm name, size, alignment
    fprintf(OutFile, "\t.comm __%s, 8, 3\n", sym->name);
}

// Functions
/**
 * Generate a function label.
 *
 * @param name The function name
 */
static void genfunlabel(char *name)
{
    fprintf(OutFile, "__%s:\n", name);
}

/**
 * Function prologue, saves FP/LR and allocates stack space for locals.
 *
 * @param stackSize The function stack size
 */
static void preamble(int stackSize)
{
    // Ensure 16-byte stack alignment
    if (stackSize % 16 != 0)
    {
        stackSize += (16 - (stackSize % 16));
    }

    fprintf(OutFile, "\tstp x29, x30, [sp, -16]!\n"); // Save FP, LR
    fprintf(OutFile, "\tmov x29, sp\n");              // Set FP

    if (stackSize > 0)
    {
        fprintf(OutFile, "\tsub sp, sp, #%d\n", stackSize); // Allocate Locals
    }
}

/**
 * Function epilogue, restores stack and FP/LR, then returns.
 *
 * @param stackSize The function stack size
 */
static void postamble(int stackSize)
{
    // Ensure alignment matches preamble
    if (stackSize % 16 != 0)
    {
        stackSize += (16 - (stackSize % 16));
    }

    if (stackSize > 0)
    {
        fprintf(OutFile, "\tadd sp, sp, #%d\n", stackSize); // Free Locals
    }
    fprintf(OutFile, "\tldp x29, x30, [sp], 16\n"); // Restore FP, LR
    fprintf(OutFile, "\tret\n");
}

/**
 * Generates a Branch with Link (bl) instruction to call a function.
 *
 * @param name The name of the function to be called.
 */
static void call(char *name)
{
    fprintf(OutFile, "\tbl __%s\n", name);
}

/**
 * Handles return statement, moves result to x0.
 *
 * @param r The register where the return value is located
 */
static void ret(int r)
{
    if (r != NO_REG)
    {
        fprintf(OutFile, "\tmov x0, %s\n", reglist[r]);
    }
}

// Loading & Storing
/**
 * Allocates a register and loads a literal integer value into it.
 *
 * @param val The integer constant to load
 * @return The index of the register containing the value
 */
static int load_int(int val)
{
    int r = alloc_register();

    fprintf(OutFile, "\tmov %s, #%d\n", reglist[r], val);
    return r;
}

/**
 * Loads the value of a global variable into a new register, uses ADRP and PAGEOFF for Position Independent Code (PIC) compatibility.
 *
 * @param sym Pointer to the symbol representing the global variable
 * @return The index of the register where the value was loaded
 */
static int load_glob(Symbol *sym)
{
    int r = alloc_register();

    fprintf(OutFile, "\tadrp %s, __%s@PAGE\n", reglist[r], sym->name);
    fprintf(OutFile, "\tadd %s, %s, __%s@PAGEOFF\n", reglist[r], reglist[r], sym->name);
    fprintf(OutFile, "\tldr %s, [%s]\n", reglist[r], reglist[r]);
    return r;
}

/**
 * Stores a register's value into a global variable's memory location.
 *
 * @param r The index of the register holding the value to store
 * @param sym Pointer to the destination symbol
 * @return The index of the source register
 */
static int store_glob(int r, Symbol *sym)
{
    int addr = alloc_register();

    fprintf(OutFile, "\tadrp %s, __%s@PAGE\n", reglist[addr], sym->name);
    fprintf(OutFile, "\tadd %s, %s, __%s@PAGEOFF\n", reglist[addr], reglist[addr], sym->name);
    fprintf(OutFile, "\tstr %s, [%s]\n", reglist[r], reglist[addr]);
    free_register(addr);
    return r;
}

/**
 * Loads a local variable from the stack into a register.
 *
 * @param offset The negative offset from the Frame Pointer (x29)
 * @return The index of the register containing the local variable
 */
static int load_local(int offset)
{
    int r = alloc_register();

    fprintf(OutFile, "\tldr %s, [x29, #%d]\n", reglist[r], offset);
    return r;
}

/**
 * Stores a register's value into a local variable's stack slot.
 *
 * @param r The index of the register holding the value
 * @param offset The stack offset relative to the Frame Pointer (x29)
 * @return The index of the source register
 */
static int store_local(int r, int offset)
{
    fprintf(OutFile, "\tstr %s, [x29, #%d]\n", reglist[r], offset);
    return r;
}

/**
 * Saves incoming function arguments (x0-x7) to their assigned stack slots, this is typically used during the function prologue.
 *
 * @param idx The argument index (0-7)
 * @param offset The stack offset where the parameter should be stored
 */
static void store_param(int idx, int offset)
{
    if (idx < 8)
    {
        fprintf(OutFile, "\tstr x%d, [x29, #%d]\n", idx, offset);
    }
    else
    {
        fprintf(stderr, "Error: +8 params not supported\n");
        exit(1);
    }
}

/**
 * Prepares a function call by moving a value into an argument register.
 *
 * @param r The temporary register holding the argument value
 * @param idx The target argument register index (x0-x7)
 */
static void load_arg(int r, int idx)
{
    if (idx < 8)
    {
        fprintf(OutFile, "\tmov x%d, %s\n", idx, reglist[r]);
    }
}

/**
 * Retrieves the return value of a function call from the result register (x0).
 *
 * @param r The destination register index
 */
static void store_result(int r)
{
    fprintf(OutFile, "\tmov %s, x0\n", reglist[r]);
}

// Control flow
static int _label = 0;

/**
 * Generates a new, unique label ID.
 *
 * @return An integer representing the next available label index
 */
static int label(void)
{
    return ++_label;
}

/**
 * Emits a label definition to the output assembly file.
 *
 * @param l The label ID to define
 */
static void genlabel(int l)
{
    fprintf(OutFile, "L%d:\n", l);
}

/**
 * Generates an unconditional branch to a specific label.
 *
 * @param l The destination label ID
 */
static void jump(int l)
{
    fprintf(OutFile, "\tb L%d\n", l);
}

/**
 * Generates a conditional branch if a register is zero.
 *
 * @param r The index of the register to check
 * @param l The destination label ID if the condition is met
 */
static void jump_cond(int r, int l)
{
    fprintf(OutFile, "\tcbz %s, L%d\n", reglist[r], l);
}

// Arithmetic operations
/**
 * Performs numerical negation.
 *
 * @param r Index of the register to negate
 * @return The same register index containing the negated value
 */
static int neg(int r)
{
    fprintf(OutFile, "\tneg %s, %s\n", reglist[r], reglist[r]);
    return r;
}

/**
 * Generates an addition instruction.
 *
 * @param r1 Index of the first operand register
 * @param r2 Index of the second operand register
 * @return The index of the register containing the result
 */
static int add(int r1, int r2)
{
    fprintf(OutFile, "\tadd %s, %s, %s\n", reglist[r2], reglist[r1], reglist[r2]);
    free_register(r1);
    return r2;
}

/**
 * Generates a subtraction instruction.
 *
 * @param r1 Index of the minuend register
 * @param r2 Index of the subtrahend register
 * @return The index of the register containing the result
 */
static int sub(int r1, int r2)
{
    fprintf(OutFile, "\tsub %s, %s, %s\n", reglist[r2], reglist[r1], reglist[r2]);
    free_register(r1);
    return r2;
}

/**
 * Generates a multiplication instruction.
 *
 * @param r1 Index of the first factor register
 * @param r2 Index of the second factor register
 * @return The index of the register containing the result
 */
static int mul(int r1, int r2)
{
    fprintf(OutFile, "\tmul %s, %s, %s\n", reglist[r2], reglist[r1], reglist[r2]);
    free_register(r1);
    return r2;
}

/**
 * Generates a signed integer division.
 *
 * @param r1 Index of the dividend register
 * @param r2 Index of the divisor register
 * @return The index of the register containing the result
 */
static int sdiv(int r1, int r2)
{
    fprintf(OutFile, "\tsdiv %s, %s, %s\n", reglist[r2], reglist[r1], reglist[r2]);
    free_register(r1);
    return r2;
}

/**
 * Calculates the remainder of a division.
 *
 * @param r1 Index of the dividend register
 * @param r2 Index of the divisor register
 * @return The index of the register containing the remainder
 */
static int mod(int r1, int r2)
{
    int r_quot = alloc_register();

    // quot = r1 / r2
    fprintf(OutFile, "\tsdiv %s, %s, %s\n", reglist[r_quot], reglist[r1], reglist[r2]);
    // r2 = r1 - (quot * r2)
    fprintf(OutFile, "\tmsub %s, %s, %s, %s\n", reglist[r2], reglist[r_quot], reglist[r2], reglist[r1]);
    free_register(r1);
    free_register(r_quot);
    return r2;
}

/**
 * Implements integer exponentiation.
 *
 * @param r1 Index of the base register
 * @param r2 Index of the exponent register
 * @return The index of the register containing the power result
 */
static int pow(int r1, int r2)
{
    int Lloop = label();
    int Lend = label();
    int r_res = alloc_register();

    // Initialize result to 1
    fprintf(OutFile, "\tmov %s, #1\n", reglist[r_res]);

    genlabel(Lloop);
    // If exponent is 0, we are done
    fprintf(OutFile, "\tcbz %s, L%d\n", reglist[r2], Lend);

    // res = res * base
    fprintf(OutFile, "\tmul %s, %s, %s\n", reglist[r_res], reglist[r_res], reglist[r1]);
    // exp--
    fprintf(OutFile, "\tsub %s, %s, #1\n", reglist[r2], reglist[r2]);

    jump(Lloop);

    genlabel(Lend);
    // Copy final result to r2
    fprintf(OutFile, "\tmov %s, %s\n", reglist[r2], reglist[r_res]);

    free_register(r1);
    free_register(r_res);
    return r2;
}

// Logic operations
/**
 * Performs a logical NOT operation on a boolean value.
 *
 * @param r Index of the register containing the value to negate
 * @return The index of the register with the inverted value
 */
static int not(int r)
{
    fprintf(OutFile, "\teor %s, %s, #1\n", reglist[r], reglist[r]);
    return r;
}

/**
 * Performs a bitwise/logical AND operation between two registers.
 *
 * @param r1 Index of the first operand register
 * @param r2 Index of the second operand register
 * @return The index of the register containing the result
 */
static int and(int r1, int r2)
{
    fprintf(OutFile, "\tand %s, %s, %s\n", reglist[r2], reglist[r1], reglist[r2]);
    free_register(r1);
    return r2;
}

/**
 * Performs a bitwise/logical OR operation between two registers.
 *
 * @param r1 Index of the first operand register
 * @param r2 Index of the second operand register
 * @return The index of the register containing the result
 */
static int or(int r1, int r2)
{
    fprintf(OutFile, "\torr %s, %s, %s\n", reglist[r2], reglist[r1], reglist[r2]);
    free_register(r1);
    return r2;
}

// Comparison operations
/**
 * Performs a comparison between two registers and sets a boolean result.
 *
 * @param r1 Index of the first operand register
 * @param r2 Index of the second operand register
 * @param cond ARM64 condition code string ("eq", "ne", "lt", "gt", "le", "ge")
 * @return The index of the register containing the boolean result
 */
static int cmp(int r1, int r2, char *cond)
{
    fprintf(OutFile, "\tcmp %s, %s\n", reglist[r1], reglist[r2]);
    fprintf(OutFile, "\tcset %s, %s\n", reglist[r2], cond);
    free_register(r1);
    return r2;
}

// IO
/**
 * Prints an integer to standard output by converting it to ASCII.
 *
 * @param r Index of the register containing the integer to be printed
 */
static void print(int r)
{
    int L_is_positive = label();
    int L_convert_loop = label();
    int L_skip_minus = label();
    int L_print_now = label();

    // 1. Preparar pila (32 bytes para el buffer del string)
    fprintf(OutFile, "\tsub sp, sp, #32\n");

    // 2. Mover el número a x13 (registro temporal)
    fprintf(OutFile, "\tmov x13, %s\n", reglist[r]);

    // 3. Comprobar si es negativo
    fprintf(OutFile, "\tmov x12, #0\n");             // x12 será nuestra bandera is_negative = 0
    fprintf(OutFile, "\tcmp x13, #0\n");             // Comparamos el número con 0
    fprintf(OutFile, "\tb.ge L%d\n", L_is_positive); // Si es >= 0, saltamos la negación

    // Si es negativo:
    fprintf(OutFile, "\tneg x13, x13\n"); // Lo hacemos positivo (x13 = -x13)
    fprintf(OutFile, "\tmov x12, #1\n");  // Marcamos la bandera is_negative = 1

    genlabel(L_is_positive);

    // 4. Puntero al final del buffer (sp + 30)
    fprintf(OutFile, "\tadd x14, sp, #30\n");

    // 5. Poner salto de línea ('\n', ASCII 10) al final
    fprintf(OutFile, "\tmov w15, #10\n");
    fprintf(OutFile, "\tstrb w15, [x14]\n");
    fprintf(OutFile, "\tsub x14, x14, #1\n");

    // 6. BUCLE DE CONVERSIÓN (Sacar dígitos de derecha a izquierda)
    genlabel(L_convert_loop);

    fprintf(OutFile, "\tmov x15, #10\n");
    fprintf(OutFile, "\tudiv x16, x13, x15\n");      // x16 = x13 / 10
    fprintf(OutFile, "\tmsub x17, x16, x15, x13\n"); // x17 = x13 - (x16 * 10) [RESTO]

    fprintf(OutFile, "\tadd x17, x17, #48\n"); // Convertir el resto a ASCII sumando '0'
    fprintf(OutFile, "\tstrb w17, [x14]\n");   // Guardar el carácter en el buffer
    fprintf(OutFile, "\tsub x14, x14, #1\n");  // Mover el puntero a la izquierda

    fprintf(OutFile, "\tmov x13, x16\n"); // x13 = x16 para la siguiente iteración

    fprintf(OutFile, "\tcbnz x13, L%d\n", L_convert_loop); // Si quedan dígitos, repetir

    // 7. Añadir el signo menos '-' si la bandera x12 es 1
    fprintf(OutFile, "\tcbz x12, L%d\n", L_skip_minus); // Si x12 es 0, saltar
    fprintf(OutFile, "\tmov w15, #45\n");               // ASCII 45 es el '-'
    fprintf(OutFile, "\tstrb w15, [x14]\n");            // Guardar el '-'
    fprintf(OutFile, "\tsub x14, x14, #1\n");
    genlabel(L_skip_minus);

    // 8. IMPRIMIR (Syscall)
    genlabel(L_print_now);

    fprintf(OutFile, "\tadd x14, x14, #1\n"); // Ajustar el puntero al primer carácter

    // Calcular longitud de la cadena (Final - Inicio + 1)
    fprintf(OutFile, "\tadd x15, sp, #30\n");
    fprintf(OutFile, "\tsub x2, x15, x14\n");
    fprintf(OutFile, "\tadd x2, x2, #1\n");

    // Syscall Write (macOS/Linux AArch64)
    fprintf(OutFile, "\tmov x0, #1\n");  // Descriptor de archivo 1 (stdout)
    fprintf(OutFile, "\tmov x1, x14\n"); // Puntero al buffer
    fprintf(OutFile, "\tmov x16, #4\n"); // Número de syscall (write)
    fprintf(OutFile, "\tsvc #0x80\n");   // Llamada al sistema

    // 9. Limpiar pila (No liberamos registro porque gen.c lo hace)
    fprintf(OutFile, "\tadd sp, sp, #32\n");
}

// Interface definition
struct Backend ARM64_Backend = {
    .freeall_registers = freeall_registers,
    .free_register = free_register,
    .alloc_register = alloc_register,
    .data_seg = data_seg,
    .text_seg = text_seg,
    .globsym = globsym,
    .genfunlabel = genfunlabel,
    .preamble = preamble,
    .postamble = postamble,
    .call = call,
    .ret = ret,
    .load_int = load_int,
    .load_glob = load_glob,
    .store_glob = store_glob,
    .load_local = load_local,
    .store_local = store_local,
    .store_param = store_param,
    .load_arg = load_arg,
    .store_result = store_result,
    .label = label,
    .genlabel = genlabel,
    .jump = jump,
    .jump_cond = jump_cond,
    .neg = neg,
    .add = add,
    .sub = sub,
    .mul = mul,
    .div = sdiv,
    .mod = mod,
    .pow = pow,
    .not = not,
    .and = and,
    .or = or,
    .cmp = cmp,
    .print = print,
};
