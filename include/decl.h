/********************************************************************************
 * File Name: include/decl.h                                                    *
 *                                                                              *
 * Description: Function prototypes.                                            *
 * Author: Alejandro Diez Bermejo                                               *
 * Date: 2026-01-01                                                             *
 * Version: 0.0.0                                                               *
 ********************************************************************************/

#ifndef DECL_H
#define DECL_H

#include "defs.h"

// Lexer
int scan(Token *t);
void match(TokenType t);
TokenType peek(void);

// Parser
ASTnode *var_declaration(void);
ASTnode *const_declaration(void);
ASTnode *function_declaration(void);
ASTnode *expression(void);
ASTnode *single_statement(void);
ASTnode *compound_statement(void);
ASTnode *mkastleaf(ASTnodeType type, PType ptype, Value value);
ASTnode *mkastunary(ASTnodeType type, ASTnode *child, Value value);
ASTnode *mkastbinary(ASTnodeType type, ASTnode *left, ASTnode *right, Value value);
ASTnode *mkastternary(ASTnodeType type, ASTnode *left, ASTnode *mid, ASTnode *right, Value value);

// Symbol Table
void scope_enter(void);
void scope_exit(void);
int is_global_scope(void);
Symbol *find_in_current_scope(char *name);
Symbol *findsymbol(char *name);
Symbol *addsymbol(char *name, SType stype, PType ptype);

// Code generation
void gencode(ASTnode *n);

#endif
