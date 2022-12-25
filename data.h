#ifndef extern_
#define extern_ extern
#endif

#define TEXTLEN 512

extern_ int Line;
extern_ int Putback;
extern_ FILE *Infile;
extern_ FILE *Outfile;
extern_ char *Infilename;       // Name of file we are parsing
extern_ char *Outfilename;      // Name of file we opened as Outfile
extern_ struct token Token;     // Last token scanned
extern_ struct token Peektoken; // A look-ahead token
extern_ char Text[TEXTLEN];
extern_ struct symtable *Functionid; // Symbol id of the current function
extern_ int Looplevel;               // Symbol id of the current function
extern_ int Switchlevel;             // Depth of nested switches

extern_ int O_dumpAST;  // If true, dump the AST trees
extern_ int O_dumpsym;		// If true, dump the symbol table
extern_ int O_keepasm;  // If true, keep any assembly files
extern_ int O_assemble; // If true, assemble the assembly files
extern_ int O_dolink;   // If true, link the object files
extern_ int O_verbose;  // If true, print info on compilation stages

// Symbol table lists
extern_ struct symtable *Globhead, *Globtail;     // Global variables and functions
extern_ struct symtable *Loclhead, *Locltail;     // Local variables
extern_ struct symtable *Parmhead, *Parmtail;     // Local Parameters
extern_ struct symtable *Membhead, *Membtail;     // Temp list of struct/union members
extern_ struct symtable *Structhead, *Structtail; // List of struct types
extern_ struct symtable *Unionhead, *Uniontail;   // List of struct types
extern_ struct symtable *Enumhead, *Enumtail;     // List of enum types and values
extern_ struct symtable *Typehead, *Typetail;     // List of typedefs
