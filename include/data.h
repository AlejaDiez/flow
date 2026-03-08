/********************************************************************************
 * File Name: include/data.h                                                    *
 *                                                                              *
 * Description: Declarations of global variables used across the compiler.      *
 *              Uses the extern_ hack to allocate storage in main only.         *
 * Author: Alejandro Diez Bermejo                                               *
 * Date: 2026-01-01                                                             *
 * Version: 0.0.0                                                               *
 ********************************************************************************/

#ifndef DATA_H
#define DATA_H

#include <stdio.h>

#include "defs.h"

#ifndef extern_
#define extern_ extern
#endif

// Scanner state
extern_ int Putback;        // Character put back into stream
extern_ int Line;           // Current line number
extern_ int Column;         // Current column number
extern_ int Length;         // Length of current token
extern_ Token CurrentToken; // The token currently being analyzed

// Parser state
extern_ Symbol *CurrentFunction; // The function that is currently being analyzed
extern_ Control *CurrentControl; // The control that is currently being analyzed

// Scope manager
extern_ Scope *CurrentScope; // Points to the currently active scope
extern_ int LocalOffset;     // Stack position

// Code generation
extern_ struct Backend *CG; // Pointer to the backend implementation

// File handles
extern_ FILE *InputFile;      // Pointer to the input file
extern_ char *InputFilename;  // Name of the input file
extern_ FILE *OutFile;        // Pointer to the output file
extern_ char *OutputFilename; // Name of the output file

#endif
