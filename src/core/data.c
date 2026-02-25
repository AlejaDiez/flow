/********************************************************************************
 * File Name: src/core/data.c                                                   *
 *                                                                              *
 * Description: Instantiation and static initialization of global variables.    *
 * Author: Alejandro Diez Bermejo                                               *
 * Date: 2026-01-01                                                             *
 * Version: 0.0.0                                                               *
 ********************************************************************************/

#include "defs.h"
#define extern_
#include "data.h"
#undef extern_

// Scanner state
int Putback = 0;
int Line = 1;
int Column = 1;
int Length = 0;

// Parser state
Symbol *CurrentFunction = NULL;
Control *CurrentControl = NULL;

// Scope manager
Scope *CurrentScope = NULL;
int LocalOffset = 0;

// Code generation
extern struct Backend ARM64_Backend;
struct Backend *CG = &ARM64_Backend;

// File handles
char *InputFilename = NULL;
char *OutputFilename = NULL;
