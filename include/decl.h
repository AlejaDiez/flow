#ifndef DECL_H
#define DECL_H

// Code
int interpretAST(ASTnode *n);

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