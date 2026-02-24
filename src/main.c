/********************************************************************************
 * File Name: src/main.c                                                        *
 *                                                                              *
 * Description: Main entry point for the Flow Compiler. Handles command line    *
 *              arguments, file I/O, and triggers the compilation stages.       *
 * Author: Alejandro Diez Bermejo                                               *
 * Date: 2026-01-01                                                             *
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
#include "data.h"
#include "decl.h"

#define VERSION "0.0.0-dev"

/**
 * Prints help menu and exits.
 *
 * @param prog_name Program name
 */
void usage(char *prog_name)
{
    fprintf(stderr, "Usage: %s [options] <file>\n", prog_name);
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -o <file>    Specify output file name (default: out.s)\n");
    fprintf(stderr, "  -v           Show compiler version\n");
    fprintf(stderr, "  -h           Show this help message\n");
    exit(1);
}

/**
 * Prints version info and exits.
 */
void version(void)
{
    fprintf(stdout, "Flow Compiler version %s\n", VERSION);
    fprintf(stdout, "Target: ARM64 (Apple Silicon / Linux Aarch64)\n");
    exit(0);
}

/**
 * Handles command line arguments.
 *
 * @param argc Argument count
 * @param argv Argument values
 */
void parse_args(int argc, char *argv[])
{
    // Check for flags
    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-')
        {
            if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
            {
                usage(argv[0]);
            }
            else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0)
            {
                version();
            }
            else if (strcmp(argv[i], "-o") == 0)
            {
                if (i + 1 < argc)
                {
                    OutputFilename = argv[i + 1];
                    i++;
                }
                else
                {
                    fprintf(stderr, "Error: missing filename after '-o'\n");
                    usage(argv[0]);
                }
            }
            else
            {
                fprintf(stderr, "Error: unknown option '%s'\n", argv[i]);
                usage(argv[0]);
            }
        }
        else
        {
            if (InputFilename != NULL)
            {
                fprintf(stderr, "Error: multiple input files provided, only one supported\n");
                usage(argv[0]);
            }
            InputFilename = argv[i];
        }
    }

    // Validation
    if (InputFilename == NULL)
    {
        fprintf(stderr, "Error: no input file provided\n");
        usage(argv[0]);
    }

    // Set default output if not provided
    if (OutputFilename == NULL)
    {
        OutputFilename = "out.s";
    }
}

/**
 * Entry point of the compiler.
 *
 * @param argc Argument count
 * @param argv Argument values
 * @return 0 on success, 1 on failure
 */
int main(int argc, char *argv[])
{
    // Parse command line arguments
    parse_args(argc, argv);

    // Open input file
    if ((InputFile = fopen(InputFilename, "r")) == NULL)
    {
        fprintf(stderr, "Error: cannot open input file '%s': %s\n", InputFilename, strerror(errno));
        exit(1);
    }

    // Create output file
    if ((OutFile = fopen(OutputFilename, "w")) == NULL)
    {
        fprintf(stderr, "Error: cannot create output file '%s': %s\n", OutputFilename, strerror(errno));
        fclose(InputFile); // Clean up input file
        exit(1);
    }

    // Initialize scanner
    scan(&CurrentToken);

    // Parse the program
    ASTnode *tree = compound_statement();

    // Generate Code
    if (tree != NULL)
    {
        gencode(tree);
    }

    // Cleanup and exit
    if (InputFile != NULL)
    {
        fclose(InputFile);
    }
    if (OutFile != NULL)
    {
        fclose(OutFile);
    }
    return 0;
}