/********************************************************************************
 * File Name: decl.h                                                            *
 * Description: This file contains all the prototypes for the functions used.   *
 ********************************************************************************/

// code/gen.c
void gen_load_lib();
void gen_global(const char *str);
void gen_function(const char *str, AST *n);
int gen_ast(AST *n, AST *prt, int lbl);

// code/x86_64.c
void x86_64_free_regs();
void x86_64_free_reg(int reg);
int x86_64_alloc_reg();
void x86_64_load_lib();
void x86_64_label(int lbl);
void x86_64_jump(int lbl);
void x86_64_global(const char *str);
int x86_64_store_global(int reg, const char *str);
int x86_64_load_global(const char *str);
void x86_64_function_prologue(const char *str);
void x86_64_function_epilogue();
int x86_64_load_int(int val);
int x86_64_add(int reg_1, int reg_2);
int x86_64_sub(int reg_1, int reg_2);
int x86_64_mul(int reg_1, int reg_2);
int x86_64_div(int reg_1, int reg_2);
int x86_64_mod(int reg_1, int reg_2);
int x86_64_cmp(int reg_1, int reg_2, const char *cmp);
int x86_64_cmp_jump(int reg_1, int reg_2, const char *jmp, int lbl);
void x86_64_print(int reg);

// parser/decl.c
void var_declaration();
void fun_declaration();

// parser/expr.c
AST *expression();

// parser/stmt.c
AST *statement(bool req_semi);
AST *block_statement();
AST *sequence();

// scanner/scan.c
void scan();
VALUE match(TOKEN_TYPE typ);
TOKEN look_ahead();

// utils/err.c
void fatal(const char *msg, int cod);
void syntax_error(const char *msg);
void unrecognized_char_error(char chr);
void unrecognized_token_error();
void match_error(const char *exptd, const char *fnd);
void undeclared_variable_error(const char *ident);
void compile_error(const char *msg);

// utils/sym.c
int find_glob(const char *str);
int add_glob(SYMBOL_TYPE typ, const char *str, PRIMITIVE_TYPE prim);

// utils/tree.c
AST *make_AST_node(AST_TYPE typ, PRIMITIVE_TYPE prim, AST *lft, AST *mid, AST *rgt, VALUE val);
AST *make_AST_binary(AST_TYPE typ, PRIMITIVE_TYPE prim, AST *lft, AST *rgt, VALUE val);
AST *make_AST_unary(AST_TYPE typ, PRIMITIVE_TYPE prim, AST *chld, VALUE val);
AST *make_AST_leaf(AST_TYPE typ, PRIMITIVE_TYPE prim, VALUE val);
void free_AST(AST *n);
void free_AST_chld(AST *n);
