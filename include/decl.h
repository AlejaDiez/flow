#ifndef DECL_H
#define DECL_H

// Code
void gencode(ASTnode *n);
/// arm64.c
void arm64_freeall_registers(void);
void arm64_free_register(int r);
int arm64_alloc_register(void);
void arm64_preamble(void);
void arm64_postamble(void);
int arm64_load(int value);
int arm64_add(int r1, int r2);
int arm64_sub(int r1, int r2);
int arm64_mul(int r1, int r2);
int arm64_div(int r1, int r2);
void arm64_printint(int r);
void arm64_data_seg(void);

// Parser
ASTnode *expression(void);

// Scanner
int scan(Token *t);

// Utils
ASTnode *mkastnode(ASTnodeType type, ASTnode *left, ASTnode *right, Value value);
ASTnode *mkastleaf(ASTnodeType type, Value value);
ASTnode *mkastunary(ASTnodeType type, ASTnode *child, Value value);
ASTnode *mkastbinary(ASTnodeType type, ASTnode *left, ASTnode *right, Value value);

#endif