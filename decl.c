#include "defs.h"
#include "data.h"
#include "decl.h"

// Parse the current token and
// return a primitive type enum value
int parse_type(int t)
{
    if (t == T_CHAR)
        return P_CHAR;
    if (t == T_INT)
        return P_INT;
    if (t == T_VOID)
        return P_VOID;
    fatald("Illegal type, token", t);
}

// Parse the declaration of a variable
void var_declaration(void)
{
    int id, type;

    // Get the type of the variable, then the identifier
    type = parse_type(Token.token);
    scan(&Token);
    ident();
    id = addglob(Text, type, S_VARIABLE);
    genglobsym(id);
    semi();
}

// Parse the declaration of a simplistic function
struct ASTnode *function_declaration(void)
{
    struct ASTnode *tree;
    int nameslot;

    // Find the 'void', the identifier, and the '(' ')'.
    match(T_VOID, "void");
    ident();
    nameslot = addglob(Text, P_VOID, S_FUNCTION);
    lparen();
    rparen();

    // Get the AST tree for the compound statement
    tree = compound_statement();

    // Return an A_FUNCTION node which has the function's nameslot
    // and the compound statement sub-tree
    return mkastunary(A_FUNCTION, P_VOID, tree, nameslot);
}