/********************************************************************************
 * File Name: decl.c                                                            *
 * Description: This file contains all the logic for parsing declarations.      *
 ********************************************************************************/

#include "defs.h"
#include "data.h"
#include "decl.h"

// Parse a variable declaration
void var_declaration()
{
    VALUE ident;

    // Match the sintax
    match(T_VAR);
    // Add the variable to the global symbol table
    ident = match(T_IDENT);
    add_glob(S_VARIABLE, ident.string, NO_PRIMITIVE);
    // Match the sintax
    match(T_COLON);
    match(T_INT);
    // Generate the assembly code
    gen_global(ident.string);
    // Clean up
    free(ident.string);
}

// Parse a function declaration
void fun_declaration()
{
    VALUE ident;
    AST *stmt = NULL;

    // Match the sintax
    match(T_FUN);
    // Add the function to the global symbol table
    ident = match(T_IDENT);
    add_glob(S_FUNCTION, ident.string, NO_PRIMITIVE);
    // Match the sintax
    match(T_LPAREN);
    match(T_RPAREN);
    match(T_COLON);
    match(T_VOID);
    // Parse the function body
    stmt = block_statement();
    // Generate the assembly code
    gen_function(ident.string, stmt);
    // Clean up
    free(ident.string);
    free_AST(stmt);
}
