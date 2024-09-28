/********************************************************************************
 * File Name: main.c                                                            *
 *                                                                              *
 * Description: This file contains the main entry point for the Flow Compiler,  *
 *    a new programming language that is designed to be simple and easy to use. *
 * Author: Alejandro Diez Bermejo                                               *
 * Date: 2024-09-28                                                             *
 * Version: 0.0.0-dev                                                           *
 *                                                                              *
 * Compilation:                                                                 *
 *    make clean                                                                *
 *    make                                                                      *
 *                                                                              *
 * Execution:                                                                   *
 *    ./bin/flow [args...] <file>                                               *
 ********************************************************************************/

#include "defs.h"
#define extern_
#include "data.h"
#undef extern_
#include "decl.h"

int main(int argc, char *argv[])
{
    char *ext;

    if (argc < 2) 
        fatal("no input files", EX_USAGE);
    Input = fopen(argv[1], "r");
    if (Input == NULL)
        fatal("cannot open input file", EX_NOINPUT);
    Name = strdup(argv[1]);
    ext = strrchr(Name, '.');
    if (ext == NULL || strcmp(ext, ".flow") != 0)
        fatal("input file must have a '.flow' extension", EX_USAGE);
    printf("Flow Compiler v%s\n", VERSION);
    return 0;
}