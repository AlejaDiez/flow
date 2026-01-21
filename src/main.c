/********************************************************************************
 * File Name: main.c                                                            *
 *                                                                              *
 * Description: This file contains the main entry point for the Flow Compiler,  *
 *    a new programming language that is designed to be simple and easy to use. *
 * Author: Alejandro Diez Bermejo                                               *
 * Date: 2026-01-17                                                             *
 * Version: 0.0.0-dev                                                           *
 *                                                                              *
 * Compilation:                                                                 *
 *    make clean                                                                *
 *    make                                                                      *
 *                                                                              *
 * Execution:                                                                   *
 *    ./bin/flow [args...] <file>                                               *
 ********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "defs.h"
#define extern_
#include "data.h"
#undef extern_
#include "decl.h"

// Init global variables
void init()
{
    Putback = 0;
    Line = 1;
    Column = 1;
    Length = 0;
}

// Main scan loop
void scanfile()
{
    Token T;

    // Mientras scan() devuelva 1 (no sea fin de archivo)
    while (scan(&T))
    {
        printf("Token encontrado: %d", T.type);
        if (T.type == T_INTLIT)
        {
            printf(", valor: %d", T.value.integer);
        }
        printf("\n");
    }
}

int main(int argc, char *argv[])
{
    // Check for input file
    if (argc != 2)
    {
        fprintf(stderr, "Error: not input file\n");
        exit(1);
    }
    // Initialize global variables
    init();
    // Open input file
    if ((InputFile = fopen(argv[1], "r")) == NULL)
    {
        fprintf(stderr, "Error: cannot open input file\n");
        exit(1);
    }
    // Scan the input file
    scanfile();
    exit(0);
}