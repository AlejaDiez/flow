#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defs.h"
#include "data.h"
#include "decl.h"

// Parse a variable declaration
void var_declaration(void)
{
    int id;
    char name[MAX_LEN];
    PType type;

    // Match the sintax
    match(T_VAR, "var");
    if (CurrentToken.type != T_IDENT)
    {
        fprintf(stderr, "Syntax Error: expected an identifier but found other token (%d:%d)\n", Line, Column);
        exit(1);
    }
    // Copy the name of the variable
    strncpy(name, CurrentToken.value.string, MAX_LEN);
    scan(&CurrentToken);
    // Match the sintax
    match(T_COLON, ":");

    // Parse type
    if (CurrentToken.type == T_INT)
    {
        type = P_INT;
    }
    else if (CurrentToken.type == T_BOOL)
    {
        type = P_BOOL;
    }
    else
    {
        fprintf(stderr, "Syntax Error: unknown type (%d:%d)", Line, Column);
        exit(1);
    }
    scan(&CurrentToken);

    // Add the variable to the global symbol table
    id = addglob(name, type);
    genglobsym(id);
}
