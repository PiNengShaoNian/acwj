#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

struct token
{
    int token;
    int intvalue;
};

enum
{
    T_EOF,
    T_PLUS,
    T_MINUS,
    T_STAR,
    T_SLASH,
    T_INTLIT,
    T_SEMI,
    T_EQUALS,
    T_IDENT,
    // Keywords
    T_PRINT,
    T_INT
};

// AST node types
enum
{
    A_ADD,
    A_SUBTRACT,
    A_MULTIPLY,
    A_DIVIDE,
    A_INTLIT,
    A_IDENT,
    A_LVIDENT,
    A_ASSIGN
};

// Abstract Syntax Tree structure
struct ASTnode
{
    int op;
    struct ASTnode *left;
    struct ASTnode *right;
    union
    {
        int intvalue;
        int id;
    } v;
};

// Symbol table structure
struct symtable
{
    char *name; // Name of a symbol
};
