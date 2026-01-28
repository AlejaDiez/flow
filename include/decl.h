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

// Scanner
int scan(Token *t);
void match(TokenType t, char *what);

// Sym Table
int findglob(char *s);
int addglob(char *name, PType ptype);

#endif