#include "defs.h"
#include "data.h"

struct ASTnode *mkastnode(int op, struct ASTnode *left, struct ASTnode *right, int intvalue)
{
    struct ASTnode *n;

    // Malloc a new ASTnode
    n = (struct ASTnode *)malloc(sizeof(struct ASTnode));
    if (n == NULL)
    {
        fprintf(stderr, "Unable to malloc in mkastnode()\n");
        exit(1);
    }

    // Copy in the filed values and return it
    n->op = op;
    n->left = left;
    n->right = right;
    n->v.intvalue = intvalue;
    return n;
}

// Make an AST leaf node
struct ASTnode *mkastleaf(int op, int intvalue)
{
    return mkastnode(op, NULL, NULL, intvalue);
}

// Make a unary AST node: only one child
struct ASTnode *mkastunary(int op, struct ASTnode *left, int intvalue)
{
    return mkastnode(op, left, NULL, intvalue);
}
