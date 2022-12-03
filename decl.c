#include "defs.h"
#include "data.h"
#include "decl.h"

// Parse the declaration of a variable
void var_declaration(void)
{
    // Ensure we have an 'int' token followed by an identifier
    // and a semicolon. Text now has the identifier's name.
    // Add it as a known identifier
    match(T_INT, "int");
    ident();
    addglob(Text);
    genglobsym(Text);
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
    nameslot = addglob(Text);
    lparen();
    rparen();

    // Get the AST tree for the compound statement
    tree = compound_statement();

    // Return an A_FUNCTION node which has the function's nameslot
    // and the compound statement sub-tree
    return mkastunary(A_FUNCTION, tree, nameslot);
}