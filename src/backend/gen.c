/********************************************************************************
 * File Name: src/backend/gen.c                                                 *
 *                                                                              *
 * Description: Generic Code Generator, walks the AST and calls                 *
 *              backend-specific functions.                                     *
 * Author: Alejandro Diez Bermejo                                               *
 * Date: 2026-01-01                                                             *
 * Version: 0.0.0                                                               *
 ********************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "defs.h"
#include "data.h"
#include "decl.h"

/**
 * Pushes a new control context onto the stack.
 *
 * @param start Label index for 'next'
 * @param end Label index for 'stop'
 */
static void push_flow(int start, int end)
{
    Control *c = (Control *)malloc(sizeof(Control));

    if (!c)
    {
        fprintf(stderr, "Fatal Error: out of memory\n");
        exit(1);
    }
    c->start_label = start;
    c->end_label = end;
    c->next = CurrentControl;
    CurrentControl = c;
}

/**
 * Pops the top context from the stack.
 */
static void pop_flow(void)
{
    if (CurrentControl == NULL)
    {
        fprintf(stderr, "Fatal Error: control stack underflow\n");
        exit(1);
    }
    Control *tmp = CurrentControl;

    CurrentControl = CurrentControl->next;
    free(tmp);
}

/**
 * Returns the label to jump to for 'stop'.
 *
 * @return The break label
 */
static int get_break_label(void)
{
    if (CurrentControl == NULL)
    {
        return NO_LABEL;
    }
    return CurrentControl->end_label;
}

/**
 * Returns the label to jump to for 'next'.
 *
 * @return The continue label
 */
static int get_continue_label(void)
{
    Control *c = CurrentControl;

    while (c != NULL)
    {
        if (c->start_label != NO_LABEL)
        {
            return c->start_label;
        }
        c = c->next;
    }
    return NO_LABEL;
}

/**
 * Load a variable.
 *
 * @param sym Symbol to load
 * @return The register where the symbol has been loaded
 */
static int load_var(Symbol *sym)
{
    if (sym->sclass == C_GLOBAL)
    {
        return CG->load_glob(sym);
    }
    else
    {
        return CG->load_local(sym->offset);
    }
}

/**
 * Store a variable.
 *
 * @param reg The register that contains the value
 * @param sym Symbol to store
 * @return The register
 */
static int store_var(int reg, Symbol *sym)
{
    if (sym->sclass == C_GLOBAL)
    {
        return CG->store_glob(reg, sym);
    }
    else
    {
        return CG->store_local(reg, sym->offset);
    }
}

/**
 * Code generation for Globals.
 *
 * @param s First global symbol
 */
static void genGlobs(Symbol *s)
{
    while (s != NULL)
    {
        if (s->stype == S_CONSTANT || s->stype == S_VARIABLE)
        {
            CG->globsym(s);
        }
        s = s->next;
    }
}

/**
 * Code generation for Abstract Syntax Tree.
 *
 * @param n Current AST node
 * @return Register number containing the result of the operation
 */
static int genAST(ASTnode *n)
{
    if (n == NULL)
    {
        return NO_REG;
    }

    switch (n->type)
    {
    // Literals
    case A_INTLIT:
        return CG->load_int(n->value.integer);
    case A_TRUE:
        return CG->load_int(1);
    case A_FALSE:
        return CG->load_int(0);
    // Identifiers
    case A_IDENT:
        return load_var(n->value.symbol);
    // Unary operations
    case A_POS:
        return genAST(n->left);
    case A_NEG:
        return CG->neg(genAST(n->left));
    case A_NOT:
        return CG->not(genAST(n->left));
    // Binary operations
    case A_ADD:
        return CG->add(genAST(n->left), genAST(n->right));
    case A_SUB:
        return CG->sub(genAST(n->left), genAST(n->right));
    case A_MUL:
        return CG->mul(genAST(n->left), genAST(n->right));
    case A_DIV:
        return CG->div(genAST(n->left), genAST(n->right));
    case A_MOD:
        return CG->mod(genAST(n->left), genAST(n->right));
    case A_POW:
        return CG->pow(genAST(n->left), genAST(n->right));
    case A_AND:
        return CG->and(genAST(n->left), genAST(n->right));
    case A_OR:
        return CG->or(genAST(n->left), genAST(n->right));
    // Comparisons
    case A_EQ:
        return CG->cmp(genAST(n->left), genAST(n->right), "eq");
    case A_NEQ:
        return CG->cmp(genAST(n->left), genAST(n->right), "ne");
    case A_LT:
        return CG->cmp(genAST(n->left), genAST(n->right), "lt");
    case A_GT:
        return CG->cmp(genAST(n->left), genAST(n->right), "gt");
    case A_LE:
        return CG->cmp(genAST(n->left), genAST(n->right), "le");
    case A_GE:
        return CG->cmp(genAST(n->left), genAST(n->right), "ge");
    // Assignment
    case A_ASSIGN:
        return store_var(genAST(n->right), n->left->value.symbol);
    case A_ASADD:
        return store_var(CG->add(load_var(n->left->value.symbol), genAST(n->right)), n->left->value.symbol);
    case A_ASSUB:
        return store_var(CG->sub(load_var(n->left->value.symbol), genAST(n->right)), n->left->value.symbol);
    case A_ASMUL:
        return store_var(CG->mul(load_var(n->left->value.symbol), genAST(n->right)), n->left->value.symbol);
    case A_ASDIV:
        return store_var(CG->div(load_var(n->left->value.symbol), genAST(n->right)), n->left->value.symbol);
    case A_ASMOD:
        return store_var(CG->mod(load_var(n->left->value.symbol), genAST(n->right)), n->left->value.symbol);
    case A_ASPOW:
        return store_var(CG->pow(load_var(n->left->value.symbol), genAST(n->right)), n->left->value.symbol);
    case A_ASAND:
        return store_var(CG->and(load_var(n->left->value.symbol), genAST(n->right)), n->left->value.symbol);
    case A_ASOR:
        return store_var(CG->or(load_var(n->left->value.symbol), genAST(n->right)), n->left->value.symbol);
    // Control flow
    case A_IFELSE:
    {
        int Lfalse = CG->label();
        int Lend = (n->right) ? CG->label() : Lfalse;

        int cmpReg = genAST(n->left);

        CG->jump_cond(cmpReg, Lfalse); // Jump if false
        CG->free_register(cmpReg);

        genAST(n->mid); // True Body
        CG->freeall_registers();

        if (n->right)
        {
            CG->jump(Lend);
            CG->genlabel(Lfalse);
            genAST(n->right); // False Body
            CG->freeall_registers();
        }
        CG->genlabel(Lend);
        return NO_REG;
    }
    case A_MATCH:
    {
        ASTnode *c;
        int Lend = CG->label();

        // Push match context to stack
        push_flow(NO_LABEL, Lend);

        // Iterate cases
        c = n->right;
        while (c != NULL)
        {
            int Lnext = CG->label();

            // Value check
            if (c->left)
            {
                int exprReg = genAST(n->left);
                int valReg = genAST(c->left);
                int cmpReg = CG->cmp(exprReg, valReg, "eq");

                CG->jump_cond(cmpReg, Lnext);
                CG->free_register(cmpReg);
            }

            genAST(c->mid); // Body
            CG->freeall_registers();

            CG->genlabel(Lnext);
            c = c->right;
        }
        CG->genlabel(Lend);

        // Pop match context
        pop_flow();
        return NO_REG;
    }
    case A_LOOP:
    {
        int Lstart, Lcontinue, Lend;

        Lstart = CG->label();
        Lcontinue = CG->label();
        Lend = CG->label();

        // Push loop context
        push_flow(Lcontinue, Lend);

        CG->genlabel(Lstart);

        // Condition
        int condReg = genAST(n->left);
        CG->jump_cond(condReg, Lend);
        CG->free_register(condReg);

        // Body
        genAST(n->mid);
        CG->freeall_registers();

        CG->genlabel(Lcontinue);

        // Update
        if (n->right)
        {
            genAST(n->right);
            CG->freeall_registers();
        }

        CG->jump(Lstart);
        CG->genlabel(Lend);

        // Pop loop context
        pop_flow();
        return NO_REG;
    }
    case A_STOP:
    {
        int lbl = get_break_label();

        if (lbl == NO_LABEL)
        {
            fprintf(stderr, "Error: 'stop' outside loop/match\n");
            exit(1);
        }
        CG->jump(lbl);
        return NO_REG;
    }
    case A_NEXT:
    {
        int lbl = get_continue_label();

        if (lbl == NO_LABEL)
        {
            fprintf(stderr, "Error: 'next' outside loop\n");
            exit(1);
        }
        CG->jump(lbl);
        return NO_REG;
    }
    // Functions
    case A_FUNCTION:
    {
        int Lskip = CG->label();

        CG->jump(Lskip);
        CG->genfunlabel(n->value.symbol->name);
        CG->preamble(n->value.symbol->size);

        // Load params
        Symbol *param = n->value.symbol->params;
        int idx = 0;

        // Ensure we only loop up to the number of parameters to avoid grabbing local scope variables
        while (param != NULL && idx < n->value.symbol->numParams)
        {
            CG->store_param(idx, param->offset);
            param = param->next;
            idx++;
        }

        // Body
        genAST(n->left);

        CG->postamble(n->value.symbol->size);
        CG->genlabel(Lskip);
        return NO_REG;
    }
    case A_RETURN:
    {
        int reg = (n->left != NULL) ? genAST(n->left) : NO_REG;

        CG->ret(reg);
        if (reg != NO_REG)
        {
            CG->free_register(reg);
        }
        CG->postamble(n->value.symbol->size);
        return NO_REG;
    }
    case A_CALL:
    {
        ASTnode *arg = n->left;
        int idx = 0;

        while (arg)
        {
            int val = genAST(arg->left);

            CG->load_arg(val, idx++);
            CG->free_register(val);
            arg = arg->right;
        }

        int r = CG->alloc_register();

        CG->call(n->value.symbol->name);
        CG->store_result(r);
        return r;
    }
    case A_PRINT:
    {
        int reg = genAST(n->left);

        CG->print(reg);
        CG->free_register(reg);
        return NO_REG;
    }
    case A_GLUE:
        genAST(n->left);
        CG->freeall_registers();
        genAST(n->right);
        CG->freeall_registers();
        return NO_REG;
    default:
        fprintf(stderr, "Fatal Error: unknown AST Node %d\n", n->type);
        exit(1);
    }
    return NO_REG;
}

/**
 * Driver function.
 */
void gencode(ASTnode *n)
{
    Symbol *glob = CurrentScope->head;
    ASTnode *tree = n;

    CG->data_seg(genGlobs, glob);
    CG->freeall_registers();
    CG->text_seg(genAST, tree);
}
