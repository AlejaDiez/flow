#ifndef DECL_H
#define DECL_H

// Code
void genglobsym(int id);
void gencode(ASTnode *n);

// Parser
ASTnode *expression(void);
ASTnode *compound_statement(void);
ASTnode *mkastleaf(ASTnodeType type, PType ptype, Value value);
ASTnode *mkastunary(ASTnodeType type, ASTnode *child, Value value);
ASTnode *mkastbinary(ASTnodeType type, ASTnode *left, ASTnode *right, Value value);
ASTnode *mkastternary(ASTnodeType type, ASTnode *left, ASTnode *mid, ASTnode *right, Value value);

// Scanner
int scan(Token *t);
void match(TokenType t, char *what);
int peek(void);

// Sym Table
int findglob(char *s);
int addglob(char *name, SType stype, PType ptype, int marker);

#endif