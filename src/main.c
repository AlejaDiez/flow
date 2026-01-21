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

void init()
{
    Putback = 0;
    Line = 1;
    Column = 1;
    Length = 0;
}

int main(int argc, char *argv[])
{
    ASTnode *tree;
    int result;

    if (argc != 2)
    {
        fprintf(stderr, "Error: not input file\n");
        exit(1);
    }

    init();

    if ((InputFile = fopen(argv[1], "r")) == NULL)
    {
        fprintf(stderr, "Error: cannot open input file %s\n", argv[1]);
        exit(1);
    }

    // Load first token
    scan(&CurrentToken);

    tree = expression();
    result = interpretAST(tree);

    printf("Result: %d\n", result);

    return 0;
}