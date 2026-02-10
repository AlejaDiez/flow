#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "defs.h"
#include "data.h"
#include "decl.h"

// Find a symbol in the global symbol table and return its index, or -1 if not found
int findglob(char *s)
{
    int i;

    for (i = 0; i < Globals; i++)
    {
        if (*s == *GlobalSymbols[i].name && !strcmp(s, GlobalSymbols[i].name))
        {
            return i;
        }
    }
    return -1;
}

// Add a new symbol to the global symbol table and return its index
int addglob(char *name, SType stype, PType ptype)
{
    int y;

    if ((y = findglob(name)) != -1)
    {
        return y;
    }

    if (Globals >= MAX_SYM)
    {
        fprintf(stderr, "Error: too many global symbols");
        exit(1);
    }

    y = Globals++;

    strncpy(GlobalSymbols[y].name, name, MAX_LEN);
    GlobalSymbols[y].name[MAX_LEN - 1] = '\0';
    GlobalSymbols[y].stype = stype;
    GlobalSymbols[y].ptype = ptype;

    return y;
}
