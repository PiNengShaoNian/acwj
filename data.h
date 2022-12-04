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

#define NSYMBOLS 1024                   // Number of symbol table entries
extern_ struct symtable Gsym[NSYMBOLS]; // Global symbol table