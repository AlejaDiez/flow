/********************************************************************************
 * File Name: sym.c                                                             *
 * Description: This file contains all the functions for the  symbol table.     *
 ********************************************************************************/

#include "defs.h"
#include "data.h"
#include "decl.h"

// Add a new symbol to the global symbol table and return its index
static int new_glob()
{
    int i;

    i = Globals++;
    if (i >= MAX_SYM)
        fatal("too many global symbols", EX_SOFTWARE);
    return i;
}

// Find a symbol in the global symbol table and return its index, or -1 if not found
int find_glob(const char *str)
{
    for (int i = 0; i < Globals; i++)
    {
        if (*str == *GlobalSymbols[i].name && !strcmp(str, GlobalSymbols[i].name))
        {
            return i;
        }
    }
    return -1;
}

// Add a new symbol to the global symbol table and return its index
int add_glob(SYMBOL_TYPE typ, const char *str, PRIMITIVE_TYPE prim)
{
    int i;

    i = find_glob(str);
    if (i != -1)
    {
        return i;
    }
    i = new_glob();
    GlobalSymbols[i].type = typ;
    GlobalSymbols[i].name = strdup(str);
    GlobalSymbols[i].primitive = prim;
    return i;
}
