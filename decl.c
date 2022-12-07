#include "defs.h"
#include "data.h"
#include "decl.h"

// Parse the current token and
// return a primitive type enum value.
// Also scan in the next token
int parse_type()
{
    int type;
    switch (Token.token)
    {
    case T_VOID:
        type = P_VOID;
        break;
    case T_CHAR:
        type = P_CHAR;
        break;
    case T_INT:
        type = P_INT;
        break;
    case T_LONG:
        type = P_LONG;
        break;
    default:
        fatald("Illegal type, token", Token.token);
    }

    // Scan in one or more further '*' tokens
    // and determine the correct pointer type
    while (1)
    {
        scan(&Token);
        if (Token.token != T_STAR)
            break;
        type = pointer_to(type);
    }

    return (type);
}

// Parse one or more global declarations, either
// variables or functions
void global_declarations(void)
{
    struct ASTnode *tree;
    int type;

    while (1)
    {
        // We have to read past the type and identifier
        // to see either a '(' for a function declaration
        // or a ',' or ';' for a variable declaration.
        // Text is filled in by the ident() call.
        type = parse_type();
        ident();
        if (Token.token == T_LPAREN)
        {
            // Parse the function declaration and
            // generate the assembly code for it
            tree = function_declaration(type);
            if (O_dumpAST)
            {
                dumpAST(tree, NOLABEL, 0);
                fprintf(stdout, "\n\n");
            }
            genAST(tree, NOREG, 0);
        }
        else
        {
            // Parse the global variable declaration
            var_declaration(type);
        }

        if (Token.token == T_EOF)
            break;
    }
}

// variable_declaration: type identifier ';'  ;
//
// Parse the declaration of a variable.
// The identifier has been scanned & we have the type
void var_declaration(int type)
{
    int id;

    // Text now has the identifier's name.
    // Add it as a known identifier
    // and generate its space in assembly
    id = addglob(Text, type, S_VARIABLE, 0);
    genglobsym(id);
    // Get the trailing semicolon
    semi();
}

// Parse the declaration of a simplistic function
struct ASTnode *function_declaration(int type)
{
    struct ASTnode *tree, *finalstmt;
    int nameslot, endlabel;

    // Get a label-id for the end label, and the function
    // to the symbol table, and set the Functionid global
    // to the function's symbol-id
    endlabel = genlabel();
    nameslot = addglob(Text, type, S_FUNCTION, endlabel);
    Functionid = nameslot;

    // Scan in the parentheses
    lparen();
    rparen();

    // Get the AST tree for the compound statement
    tree = compound_statement();

    // If the function type isn't P_VOID, check that
    // the last AST operation in the compound statement
    // was a return statement
    if (type != P_VOID)
    {
        finalstmt = (tree->op == A_GLUE) ? tree->right : tree;
        if (finalstmt == NULL || finalstmt->op != A_RETURN)
            fatal("No return for function with non-void type");
    }

    // Return an A_FUNCTION node which has the function's nameslot
    // and the compound statement sub-tree
    return mkastunary(A_FUNCTION, P_VOID, tree, nameslot);
}