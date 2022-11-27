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
    T_PRINT
};

// AST node types
enum
{
    A_ADD,
    A_SUBTRACT,
    A_MULTIPLY,
    A_DIVIDE,
    A_INTLIT
};

// Abstract Syntax Tree structure
struct ASTnode
{
    int op;
    struct ASTnode *left;
    struct ASTnode *right;
    int intvalue;
};