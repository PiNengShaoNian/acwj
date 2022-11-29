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
    T_EQ,
    T_NE,
    T_LT,
    T_GT,
    T_LE,
    T_GE,
    T_INTLIT,
    T_SEMI,
    T_ASSIGN,
    T_IDENT,
    // Keywords
    T_PRINT,
    T_INT
};

// AST node types
enum
{
    A_ADD = 1,
    A_SUBTRACT,
    A_MULTIPLY,
    A_DIVIDE,
    A_EQ,
    A_NE,
    A_LT,
    A_GT,
    A_LE,
    A_GE,
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
