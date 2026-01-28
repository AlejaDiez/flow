#ifndef DECL_H
#define DECL_H

// Code
void genglobsym(int id);
void gencode(ASTnode *n);
/// arm64.c
void arm64_freeall_registers(void);
void arm64_free_register(int r);
int arm64_alloc_register(void);
void arm64_label(int l);
void arm64_jump(int l);
void arm64_preamble(void);
void arm64_postamble(void);
void arm64_postamble(void);
void arm64_globsym(int id);
int arm64_storglob(int r, int id);
int arm64_loadglob(int id);
int arm64_loadint(int value);
int arm64_neg(int r);
int arm64_add(int r1, int r2);
int arm64_sub(int r1, int r2);
int arm64_mul(int r1, int r2);
int arm64_div(int r1, int r2);
int arm64_mod(int r1, int r2);
int arm64_pow(int r1, int r2);
int arm64_cmp(int r1, int r2, char *op);
int arm64_and(int r1, int r2);
int arm64_or(int r1, int r2);
int arm64_not(int r);
void arm64_printint(int r);
void arm64_data_seg(void);

// Parser
ASTnode *expression(void);
ASTnode *compound_statement(void);

// Scanner
int scan(Token *t);
void match(TokenType t, char *what);

// Utils
int label(void);
int findglob(char *s);
int addglob(char *name, PType ptype);
ASTnode *mkastleaf(ASTnodeType type, PType ptype, Value value);
ASTnode *mkastunary(ASTnodeType type, ASTnode *child, Value value);
ASTnode *mkastbinary(ASTnodeType type, ASTnode *left, ASTnode *right, Value value);

#endif