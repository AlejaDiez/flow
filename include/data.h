#ifndef DATA_H
#define DATA_H

#ifndef extern_
#define extern_ extern
#endif

extern_ int Putback;        // Character that was put back
extern_ int Line;           // Current line number
extern_ int Column;         // Column at which token starts
extern_ int Length;         // Length of current token
extern_ Token CurrentToken; // Current token

extern_ SymTable GlobalSymbols[MAX_SYM]; // Global symbol table
extern_ int Globals;                     // Number of global symbols

extern_ struct Backend *CG; // Current code generator architecture

extern_ FILE *InputFile; // Current input file
extern_ FILE *OutFile;   // Current output file

#endif