#include "defs.h"
#define extern_
#include "data.h"
#undef extern_
#include "decl.h"
#include <errno.h>

// Initialise global variables
static void init()
{
    Line = 1;
    Putback = '\n';
}

// List of printable tokens
char *tokstr[] = {"+", "-", "*", "/", "intlit"};

// Loop scanning in all the tokens in the input file.
// Print out details of each token found.
static void scanfile()
{
    struct token T;

    while (scan(&T))
    {
        printf("Token %s", tokstr[T.token]);
        if (T.token == T_INTLIT)
        {
            printf(", value %d", T.intvalue);
        }
        printf("\n");
    }
}

// Print out a usage if started incorrectly
static void usage(char *prog)
{
    fprintf(stderr, "Usage: %s infile\n", prog);
    exit(1);
}

void main(int argc, char *argv[])
{
    struct ASTnode *tree;
    if (argc != 2)
    {
        usage(argv[0]);
    }
    init();

    // Open up the input file
    if ((Infile = fopen(argv[1], "r")) == NULL)
    {
        fprintf(stderr, "Unable to open %s: %s\n", argv[1], strerror(errno));
        exit(0);
    }

    // Create the output file
    if ((Outfile = fopen("out.s", "w")) == NULL)
    {
        fprintf(stderr, "Unable to create out.s: %s\n", strerror(errno));
        exit(1);
    }

    // For now, ensure that void printint() is defined
    addglob("printint", P_CHAR, S_FUNCTION, 0);

    scan(&Token);  // Get the first token from the input
    genpreamble(); // Output the preamble
    while (1)
    {
        tree = function_declaration();
        genAST(tree, NOREG, 0);
        if (Token.token == T_EOF) // Stop when have reached EOF
            break;
    }

    fclose(Outfile); // Close the output file and exit
    exit(0);
}