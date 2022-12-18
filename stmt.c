#include "defs.h"
#include "data.h"
#include "decl.h"

// Prototypes
static struct ASTnode *single_statement(void);

// Parse a WHILE statement
// and return its AST
struct ASTnode *while_statement(void)
{
    struct ASTnode *condAST, *bodyAST;

    // Ensure we have 'while' '('
    match(T_WHILE, "while");
    lparen();

    // Parse the following expression
    // and the ')' following. Ensure
    // the tree's operation is a comparison.
    condAST = binexpr(0);
    if (condAST->op < A_EQ || condAST->op > A_GE)
        condAST = mkastunary(A_TOBOOL, condAST->type, condAST, NULL, 0);

    rparen();

    // Get the AST for the compound statement
    // Update the loop depth in the process
    Looplevel++;
    bodyAST = compound_statement();
    Looplevel--;

    // Build ans return the AST for this statement
    return mkastnode(A_WHILE, P_NONE, condAST, NULL, bodyAST, NULL, 0);
}

// Parse an IF statement including
// any optional ELSE clause
// and return its AST
struct ASTnode *if_statement(void)
{
    struct ASTnode *condAST, *trueAST, *falseAST = NULL;

    // Ensure we have 'if' '('
    match(T_IF, "if");
    lparen();

    // Parse the following expression
    // and the ')' following. Ensure
    // the tree's operation is a comparision
    condAST = binexpr(0);

    if (condAST->op < A_EQ || condAST->op > A_GE)
        condAST = mkastunary(A_TOBOOL, condAST->type, condAST, NULL, 0);

    rparen();

    // Get the AST for the compound statement
    trueAST = compound_statement();

    // If we have an 'else', skip it
    // and get the AST for the compound statement
    if (Token.token == T_ELSE)
    {
        scan(&Token);
        falseAST = compound_statement();
    }

    // Build and return the AST for this statement.
    return (mkastnode(A_IF, P_NONE, condAST, trueAST, falseAST, NULL, 0));
}

// Parse a FOR statement
// and return its AST
static struct ASTnode *for_statement(void)
{
    struct ASTnode *condAST, *bodyAST;
    struct ASTnode *preopAST, *postopAST;
    struct ASTnode *tree;

    // Ensure we have 'for' '('
    match(T_FOR, "for");
    lparen();

    // Get the pre_op statement and the ';'
    preopAST = single_statement();
    semi();

    // Get the condition and the ';'
    condAST = binexpr(0);
    if (condAST->op < A_EQ || condAST->op > A_GE)
        condAST = mkastunary(A_TOBOOL, condAST->type, condAST, NULL, 0);

    semi();

    // Get the post_op statement and ')'
    postopAST = single_statement();
    rparen();

    // Get the compound statement
    // Update the loop depth in the process
    Looplevel++;
    bodyAST = compound_statement();
    Looplevel--;

    // For now, all four sub-trees have to be non-NULL.
    // Later on, we'll change the semantics for when some are missing

    // Glue the compound statement and the postop tree
    tree = mkastnode(A_GLUE, P_NONE, bodyAST, NULL, postopAST, NULL, 0);

    // Make a WHILE loop with the condition and this new body
    tree = mkastnode(A_WHILE, P_NONE, condAST, NULL, tree, NULL, 0);

    // And glue the preop tree to the A_WHILE tree
    return mkastnode(A_GLUE, P_NONE, preopAST, NULL, tree, NULL, 0);
}

// Parse a return statement and return its AST
static struct ASTnode *return_statement(void)
{
    struct ASTnode *tree;

    // Can't return a value if function returns P_VOID
    if (Functionid->type == P_VOID)
        fatal("Can't return from a void function");

    // Ensure we have 'return' '('
    match(T_RETURN, "return");
    lparen();

    // Parse the following expression
    tree = binexpr(0);

    // Ensure this is compatible with the function's type
    tree = modify_type(tree, Functionid->type, 0);
    if (tree == NULL)
        fatal("Incompatible type to return");

    // Add on the A_RETURN node
    tree = mkastunary(A_RETURN, P_NONE, tree, NULL, 0);

    // Get the ')'
    rparen();
    return tree;
}

// break_statement: 'break' ;
//
// Parse a break statement and return its AST
static struct ASTnode *break_statement(void)
{
    if (Looplevel == 0 && Switchlevel == 0)
        fatal("no loop or switch to break out from");
    scan(&Token);
    return (mkastleaf(A_BREAK, 0, NULL, 0));
}

// continue_statement: 'continue' ;
//
// Parse a continue statement and return its AST
static struct ASTnode *continue_statement(void)
{

    if (Looplevel == 0)
        fatal("no loop to continue to");
    scan(&Token);
    return (mkastleaf(A_CONTINUE, 0, NULL, 0));
}

// Parse a switch statement and return its AST
static struct ASTnode *switch_statement(void)
{
    struct ASTnode *left, *n, *c, *casetree = NULL, *casetail;
    int inloop = 1, casecount = 0;
    int seendefault = 0;
    int ASTop, casevalue;

    // Skip the 'switch' and '('
    scan(&Token);
    lparen();

    // Get the switch expression, the ')' and the the '{'
    left = binexpr(0);
    rparen();
    lbrace();

    // Ensure that this is of int type
    if (!inttype(left->type))
        fatal("Switch expression is not of integer type");

    // Build a A_SWITCH subtree with the expression as
    // the child
    n = mkastunary(A_SWITCH, 0, left, NULL, 0);

    // Now parse the cases
    Switchlevel++;
    while (inloop)
    {
        switch (Token.token)
        {
        // Leave the loop when we hit a '}'
        case T_RBRACE:
            if (casecount == 0)
                fatal("No cases in switch");
            inloop = 0;
            break;
        case T_CASE:
        case T_DEFAULT:
            // Ensure this isn't after a previous 'default'
            if (seendefault)
                fatal("case or default after existing default");

            // Set the AST operation. Scan the case value if required
            if (Token.token == T_DEFAULT)
            {
                ASTop = A_DEFAULT;
                seendefault = 1;
                scan(&Token);
            }
            else
            {
                ASTop = A_CASE;
                scan(&Token);
                left = binexpr(0);
                // Ensure the case value is an integer literal
                if (left->op != A_INTLIT)
                    fatal("Expecting integer literal for case value");
                casevalue = left->intvalue;

                // Walk the list of existing case values to ensure
                // that there isn't a duplicate case value
                for (c = casetree; c != NULL; c = c->right)
                {
                    if (casevalue == c->intvalue)
                        fatal("Duplicate case value");
                }
            }

            // Scan the ':' and get the compound expression
            match(T_COLON, ":");
            left = compound_statement();
            casecount++;

            // Build a sub-tree with the compound statement as the left child
            // and link it in to the growing A_CASE tree
            if (casetree == NULL)
            {
                casetree = casetail = mkastunary(ASTop, 0, left, NULL, casevalue);
            }
            else
            {
                casetail->right = mkastunary(ASTop, 0, left, NULL, casevalue);
                casetail = casetail->right;
            }
            break;
        default:
            fatald("Unexpected token in switch", Token.token);
        }
    }
    Switchlevel--;

    // We have a sub-tree with the cases and any default. Put the
    // case count into the A_SWITCH node and attach the case tree.
    n->intvalue = casecount;
    n->right = casetree;
    rbrace();

    return (n);
}

// Parse a single statement
// and return its AST
static struct ASTnode *single_statement(void)
{
    int type, class = C_LOCAL;
    struct symtable *ctype;

    switch (Token.token)
    {
    case T_IDENT:
        // We have to see if the identifier matches a typedef.
        // If not do the default code in this switch statement.
        // Otherwise, fall down to the parse_type() call.
        if (findtypedef(Text) == NULL)
            return (binexpr(0));
    case T_CHAR:
    case T_INT:
    case T_LONG:
    case T_STRUCT:
    case T_UNION:
    case T_ENUM:
    case T_TYPEDEF:
        // The beginning of a variable declaration.
        // Parse the type and get the identifier.
        // Then parse the rest of the declaration.
        type = parse_type(&ctype, &class);
        ident();
        var_declaration(type, ctype, class);
        semi();
        return (NULL); // No AST generated here
    case T_IF:
        return (if_statement());
    case T_WHILE:
        return (while_statement());
    case T_FOR:
        return (for_statement());
    case T_RETURN:
        return (return_statement());
    case T_BREAK:
        return (break_statement());
    case T_CONTINUE:
        return (continue_statement());
    case T_SWITCH:
        return (switch_statement());
    default:
        // For now, see if this is an expression.
        // This caches assignment statements.
        return (binexpr(0));
    }
    return (NULL); // Keep -Wall happy
}

// Parse a computed statement
// and return its AST
struct ASTnode *compound_statement(void)
{
    struct ASTnode *left = NULL;
    struct ASTnode *tree;

    // Require a left curly bracket
    lbrace();

    while (1)
    {
        // Parse a single statement
        tree = single_statement();

        if (tree != NULL &&
            (tree->op == A_ASSIGN || tree->op == A_RETURN ||
             tree->op == A_FUNCCALL || tree->op == A_BREAK ||
             tree->op == A_CONTINUE))
        {
            semi();
        }

        // For each new tree, either save it in left
        // if left is empty, or glue the left and the
        // new tree together
        if (tree)
        {
            if (left == NULL)
                left = tree;
            else
                left = mkastnode(A_GLUE, P_NONE, left, NULL, tree, NULL, 0);
        }

        // When we hit a right curly bracket,
        // skip past it and return the AST
        if (Token.token == T_RBRACE)
        {
            rbrace();
            return left;
        }
    }
}
