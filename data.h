#ifndef extern_
#define extern_ extern
#endif

#define TEXTLEN 512

extern_ int Line;
extern_ int Putback;
extern_ FILE *Infile;
extern_ FILE *Outfile;
extern_ struct token Token;
extern_ char Text[TEXTLEN];
extern_ int Functionid; // Symbol id of the current function
extern_ int Globs;      // Position of next free global symbol slot
extern_ int Locls;      // Position of next free local symbol slot

#define NSYMBOLS 1024                   // Number of symbol table entries
extern_ struct symtable Symtable[NSYMBOLS]; // Global symbol table

extern_ int O_dumpAST;
