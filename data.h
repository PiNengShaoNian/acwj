#ifndef extern_
#define extern_ extern
#endif

#define TEXTLEN 512

extern_ int Line;
extern_ int Putback;
extern_ FILE *Infile;
extern_ FILE *Outfile;
extern_ char *Outfilename; // Name of file we opened as Outfile
extern_ struct token Token;
extern_ char Text[TEXTLEN];
extern_ struct symtable *Functionid; // Symbol id of the current function

extern_ int O_dumpAST;  // If true, dump the AST trees
extern_ int O_keepasm;  // If true, keep any assembly files
extern_ int O_assemble; // If true, assemble the assembly files
extern_ int O_dolink;   // If true, link the object files
extern_ int O_verbose;  // If true, print info on compilation stages

// Symbol table lists
struct symtable *Globhead, *Globtail;     // Global variables and functions
struct symtable *Loclhead, *Locltail;     // Local variables
struct symtable *Parmhead, *Parmtail;     // Local Parameters
struct symtable *Membhead, *Membtail;     // Temp list of struct/union members
struct symtable *Structhead, *Structtail; // List of struct types