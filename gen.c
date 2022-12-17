#include "defs.h"
#include "data.h"
#include "decl.h"

static int genIFAST(struct ASTnode *n, int looptoplabel, int loopendlabel);
static int genWHILE(struct ASTnode *n);
static int gen_funccall(struct ASTnode *n);

// Given an AST, the register (if any) that holds
// the previous rvalue, and the AST op of the parent,
// generate assembly code recursively.
// Return the register id with tree's final value
int genAST(struct ASTnode *n, int iflabel, int looptoplabel,
           int loopendlabel, int parentASTop)
{
    int leftreg, rightreg;

    // We now have specific AST node handing at the top
    switch (n->op)
    {
    case A_IF:
        return genIFAST(n, looptoplabel, loopendlabel);
    case A_WHILE:
        return genWHILE(n);
    case A_FUNCCALL:
        return (gen_funccall(n));
    case A_GLUE:
        // Do each child statement, and free the
        // registers after each child
        genAST(n->left, iflabel, looptoplabel, loopendlabel, n->op);
        genfreeregs();
        genAST(n->right, iflabel, looptoplabel, loopendlabel, n->op);
        genfreeregs();
        return NOREG;
    case A_FUNCTION:
        // Generate the function's preamble before the code
        cgfuncpreamble(n->sym);
        genAST(n->left, NOLABEL, NOLABEL, NOLABEL, n->op);
        cgfuncpostamble(n->sym);
        return NOREG;
    }

    // General AST node handing below

    // Get the left and right sub-tree values
    if (n->left)
        leftreg = genAST(n->left, NOLABEL, NOLABEL, NOLABEL, n->op);
    if (n->right)
        rightreg = genAST(n->right, NOLABEL, NOLABEL, NOLABEL, n->op);

    switch (n->op)
    {
    case A_ADD:
        return cgadd(leftreg, rightreg);
    case A_SUBTRACT:
        return cgsub(leftreg, rightreg);
    case A_MULTIPLY:
        return cgmul(leftreg, rightreg);
    case A_DIVIDE:
        return cgdiv(leftreg, rightreg);
    case A_AND:
        return (cgand(leftreg, rightreg));
    case A_OR:
        return (cgor(leftreg, rightreg));
    case A_XOR:
        return (cgxor(leftreg, rightreg));
    case A_LSHIFT:
        return (cgshl(leftreg, rightreg));
    case A_RSHIFT:
        return (cgshr(leftreg, rightreg));
    case A_EQ:
    case A_NE:
    case A_LT:
    case A_GT:
    case A_LE:
    case A_GE:
        // If the parse AST node is an A_IF, generate a compare
        // followed by a jump. Otherwise, compare registers and
        // set one to 1 or 0 based on the comparison.
        if (parentASTop == A_IF || parentASTop == A_WHILE)
            return cgcompare_and_jump(n->op, leftreg, rightreg, iflabel);
        else
            return cgcompare_and_set(n->op, leftreg, rightreg);
    case A_STRLIT:
        return (cgloadglobstr(n->intvalue));
    case A_INTLIT:
        return (cgloadint(n->intvalue, n->type));
    case A_IDENT:
        // Load our value if we are an rvalue
        // or we are being dereferenced
        if (n->rvalue || parentASTop == A_DEREF)
        {
            if (n->sym->class == C_GLOBAL)
            {
                return (cgloadglob(n->sym, n->op));
            }
            else
            {
                return (cgloadlocal(n->sym, n->op));
            }
        }
        else
            return (NOREG);
    case A_ASSIGN:
        // Are we assigning to an identifier or through a pointer?
        switch (n->right->op)
        {
        case A_IDENT:
            if (n->right->sym->class == C_GLOBAL)
                return (cgstorglob(leftreg, n->right->sym));
            else
                return (cgstorlocal(leftreg, n->right->sym));
        case A_DEREF:
            return (cgstorderef(leftreg, rightreg, n->right->type));
        default:
            fatald("Can't A_ASSIGN in genAST(), op", n->op);
        }
    case A_DEREF:
        // If we are an rvalue, dereference to get the value we point at
        // otherwise leave it for A_ASSIGN to store through the pointer
        if (n->rvalue)
            return (cgderef(leftreg, n->left->type));
        else
            return (leftreg);
    case A_WIDEN:
        // Widen the child's type to the parent's type
        return cgwiden(leftreg, n->left->type, n->type);
    case A_RETURN:
        cgreturn(leftreg, Functionid);
        return NOREG;
    case A_ADDR:
        return (cgaddress(n->sym));
    case A_SCALE:
        // Small optimisation: use shift if the
        // scale value is a known power of two
        switch (n->size)
        {
        case 2:
            return (cgshlconst(leftreg, 1));
        case 4:
            return (cgshlconst(leftreg, 2));
        case 8:
            return (cgshlconst(leftreg, 3));
        default:
            // Load a register with the size and
            // multiply the leftreg by this size
            rightreg = cgloadint(n->size, P_INT);
            return (cgmul(leftreg, rightreg));
        }
    case A_POSTINC:
    case A_POSTDEC:
        // Load the variable's value into a register,
        // and post increment/decrement it
        if (n->sym->class == C_GLOBAL)
            return (cgloadglob(n->sym, n->op));
        else
            return (cgloadlocal(n->sym, n->op));
    case A_PREINC:
    case A_PREDEC:
        // Load and increment the variable's value into a register
        // and pre increment/decrement it
        if (n->left->sym->class == C_GLOBAL)
            return (cgloadglob(n->left->sym, n->op));
        else
            return (cgloadlocal(n->left->sym, n->op));
    case A_NEGATE:
        return (cgnegate(leftreg));
    case A_INVERT:
        return (cginvert(leftreg));
    case A_LOGNOT:
        return (cglognot(leftreg));
    case A_TOBOOL:
        // If the parent AST node is an A_IF or A_WHILE, generate
        // a compare followed by a jump. Otherwise, set the register
        // to 0 or 1 based on it's zeroeness or non-zeroeness
        return (cgboolean(leftreg, parentASTop, iflabel));
    case A_BREAK:
        cgjump(loopendlabel);
        return (NOREG);
    case A_CONTINUE:
        cgjump(looptoplabel);
        return (NOREG);
    default:
        fatald("Unknown AST operator", n->op);
        return (0);
    }
}

void genpreamble()
{
    cgpreamble();
}

void genpostamble()
{
    cgpostamble();
}

void genfreeregs()
{
    freeall_registers();
}

void genglobsym(struct symtable *node)
{
    cgglobsym(node);
}

// Generate and return a new label number
int genlabel(void)
{
    static int id = 1;
    return (id++);
}

// Generate the code for an IF statement
// and an optional clause
static int genIFAST(struct ASTnode *n, int looptoplabel, int loopendlabel)
{
    int Lfalse, Lend;

    // Generate two labels: one for the
    // false compound statement, and one
    // for the end of the overall IF statement.
    // When there is no ELSE clause, Lfalse _is_
    // the ending label!
    Lfalse = genlabel();
    if (n->right)
        Lend = genlabel();

    // Generate the condition code followed
    // by a zero jump to the false label.
    // We cheat by sending the Lfalse label as a register.
    genAST(n->left, Lfalse, NOLABEL, NOLABEL, n->op);
    genfreeregs();

    // Generate the true compound statement
    genAST(n->mid, NOLABEL, looptoplabel, loopendlabel, n->op);
    genfreeregs();

    // If there is an optional ELSE clause,
    // generate the jump to skip the the end
    if (n->right)
    {
        cgjump(Lend);
    }

    // Now the false label
    cglabel(Lfalse);

    // Optional ELSE clause: generate the
    // false compound statement and the
    // end label
    if (n->right)
    {
        genAST(n->right, NOLABEL, NOLABEL, NOLABEL, n->op);
        genfreeregs();
        cglabel(Lend);
    }

    return (NOREG);
}

// Generate the code for a WHILE statement
static int genWHILE(struct ASTnode *n)
{
    int Lstart, Lend;

    // Generate the start and end labels
    // and output the start label
    Lstart = genlabel();
    Lend = genlabel();
    cglabel(Lstart);

    // Generate the condition code followed
    // by a jump to the end label.
    // We cheat by sending the Lfalse label as a register.
    genAST(n->left, Lend, Lstart, Lend, n->op);
    genfreeregs();

    // Generate the compound statement for the body
    genAST(n->right, NOLABEL, Lstart, Lend, n->op);
    genfreeregs();

    // Finally output the jump back to the condition,
    // and the end label
    cgjump(Lstart);
    cglabel(Lend);
    return NOREG;
}

int genprimsize(int type)
{
    return (cgprimsize(type));
}

int genglobstr(char *strvalue)
{
    int l = genlabel();
    cgglobstr(l, strvalue);
    return (l);
}

// Generate the code to copy the arguments of a
// function call to its parameters, then call the
// function itself. Return the register that holds
// the function's return value.
static int gen_funccall(struct ASTnode *n)
{
    struct ASTnode *gluetree = n->left;
    int reg;
    int numargs = 0;

    // If there is a list of arguments, walk this list
    // from the last argument (right-hand child) to the
    // first
    while (gluetree)
    {
        // Calculate the expression's value
        reg = genAST(gluetree->right, NOLABEL, NOLABEL, NOLABEL, gluetree->op);
        // Copy this into the n'th function parameter: size is 1, 2, 3, ...
        cgcopyarg(reg, gluetree->size);
        // Keep the first (highest) number of arguments
        if (numargs == 0)
            numargs = gluetree->size;

        genfreeregs();
        gluetree = gluetree->left;
    }

    // Call the function, clean up the stack (based on numargs),
    // and return its result
    return (cgcall(n->sym, numargs));
}

int genalign(int type, int offset, int direction)
{
    return (cgalign(type, offset, direction));
}