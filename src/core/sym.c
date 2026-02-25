/********************************************************************************
 * File Name: src/core/sym.c                                                    *
 *                                                                              *
 * Description: Symbol Table Management using a Scope Stack, handles variable   *
 *              resolution (Shadowing), storage class assignment, and Stack     *
 *              Offsets.                                                        *
 * Author: Alejandro Diez Bermejo                                               *
 * Date: 2026-01-01                                                             *
 * Version: 0.0.0.                                                              *
 ********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defs.h"
#include "data.h"
#include "decl.h"

/**
 * Allocates and initializes a new Symbol structure.
 *
 * @param name Symbol name
 * @param stype Symbol type
 * @param ptype Primary type
 * @return The created symbol
 */
static Symbol *newsym(char *name, SType stype, PType ptype)
{
    Symbol *sym = (Symbol *)malloc(sizeof(Symbol));

    if (sym == NULL)
    {
        fprintf(stderr, "Fatal Error: out of memory\n");
        exit(1);
    }

    sym->name = strdup(name);
    sym->stype = stype;
    sym->ptype = ptype;
    sym->size = 0;
    sym->offset = 0;
    sym->numParams = 0;
    sym->params = NULL;
    sym->next = NULL;
    return sym;
}

/**
 * Allocates and initializes a new Scope structure.
 *
 * @param parent Parent scope
 * @return The created scope
 */
static Scope *newscope(Scope *parent)
{
    Scope *s = (Scope *)malloc(sizeof(Scope));

    if (s == NULL)
    {
        fprintf(stderr, "Fatal Error: out of memory\n");
        exit(1);
    }

    s->head = NULL;
    s->tail = NULL;
    s->parent = parent;

    if (parent == NULL)
    {
        s->level = 0;
    }
    else
    {
        s->level = parent->level + 1;
    }
    return s;
}

/**
 * Pushes a new scope onto the stack.
 */
void scope_enter(void)
{
    CurrentScope = newscope(CurrentScope);
}

/**
 * Pops the current scope from the stack.
 */
void scope_exit(void)
{
    if (CurrentScope->parent == NULL)
    {
        fprintf(stderr, "Compiler Error: syntax indicates exiting global scope\n");
        exit(1);
    }
    CurrentScope = CurrentScope->parent;
}

/**
 * Check if we are at the global scope.
 *
 * @return 1 if is global, 0 if is not
 */
int is_global_scope(void)
{
    return (CurrentScope->parent == NULL);
}

/**
 * Looks for a symbol ONLY in the active scope.
 *
 * @param name Symbol name
 * @return The symbol if it was found, otherwise NULL
 */
Symbol *find_in_current_scope(char *name)
{
    Symbol *sym = CurrentScope->head;

    while (sym != NULL)
    {
        if (strcmp(name, sym->name) == 0)
        {
            return sym;
        }
        sym = sym->next;
    }
    return NULL;
}

/**
 * Looks for a symbol walking UP the scope stack.
 *
 * @param name Symbol name
 * @return The symbol if it was found, otherwise NULL
 */
Symbol *findsymbol(char *name)
{
    Scope *scope = CurrentScope;

    while (scope != NULL)
    {
        Symbol *sym = scope->head;

        while (sym != NULL)
        {
            if (strcmp(name, sym->name) == 0)
            {
                return sym;
            }
            sym = sym->next;
        }
        scope = scope->parent;
    }
    return NULL;
}

/**
 * Adds a new symbol to the CURRENT scope.
 *
 * @param name Symbol name
 * @param stype Symbol type
 * @param ptype Primary type
 * @return The created symbol
 */
Symbol *addsymbol(char *name, SType stype, PType ptype)
{
    if (find_in_current_scope(name) != NULL)
    {
        fprintf(stderr, "Error: redefinition of symbol '%s' in the same scope at %d:%d\n", name, Line, Column);
        exit(1);
    }

    Symbol *sym = newsym(name, stype, ptype);

    // Constants and variables
    if (stype == S_CONSTANT || stype == S_VARIABLE)
    {
        sym->size = 8;

        if (is_global_scope())
        {
            sym->sclass = C_GLOBAL;
            sym->offset = 0;
        }
        else
        {
            sym->sclass = C_LOCAL;
            // Allocate space in the Stack Frame
            LocalOffset -= sym->size;
            sym->offset = LocalOffset;
        }
    }
    // Functions
    else if (stype == S_FUNCTION)
    {
        sym->sclass = C_GLOBAL;
        sym->size = 0; // Stack size
        sym->offset = 0;
    }

    if (CurrentScope->head == NULL)
    {
        CurrentScope->head = CurrentScope->tail = sym;
    }
    else
    {
        CurrentScope->tail->next = sym;
        CurrentScope->tail = CurrentScope->tail->next;
    }
    return sym;
}