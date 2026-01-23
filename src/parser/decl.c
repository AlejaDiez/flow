#include <stdio.h>
#include <stdlib.h>

#include "defs.h"
#include "data.h"
#include "decl.h"

// Parse a variable declaration
void var_declaration(void)
{
    int id;

    // Match the sintax
    match(T_VAR, "var");
    if (CurrentToken.type != T_IDENT)
    {
        fprintf(stderr, "Syntax Error: expected an identifier but found other token (%d:%d)\n", Line, Column);
        exit(1);
    }

    // Add the variable to the global symbol table
    id = addglob(CurrentToken.value.string);
    scan(&CurrentToken);

    // Match the sintax
    match(T_COLON, ":");
    match(T_INT, "int");

    // Generate the assembly code
    genglobsym(id);
}
