#include "defs.h"
#include "data.h"
#include "decl.h"

// A pointer to a rejected token
static struct token *Rejtoken = NULL;

// Reject the token that we just scanned
void reject_token(struct token *t)
{
    if (Rejtoken != NULL)
        fatal("Can't reject token twice");

    Rejtoken = t;
}

// Get the next character from the input file.
static int next(void)
{
    int c;

    if (Putback)
    {                // Use the character put
        c = Putback; // back if there is one
        Putback = 0;
        return c;
    }

    c = fgetc(Infile); // Read from input file
    if ('\n' == c)
    {
        Line++; // Increment line count
    }

    return c;
}

// Put back an unwanted character
static void putback(int c)
{
    Putback = c;
}

// Skip past input that we don't need to deal with,
// i.e. whitespace, newlines. Return the first
// character we do need to deal with
static int skip(void)
{
    int c;

    c = next();
    while (' ' == c || '\t' == c || '\n' == c || '\r' == c || '\f' == c)
    {
        c = next();
    }

    return c;
}

// Return the position of character c
// in string s, or -1 if c not found
static int chrpos(char *s, int c)
{
    char *p;

    p = strchr(s, c);
    return (p ? p - s : -1);
}

// Given a word from the input, return the matching
// keyword token number or 0 if it's not a keyword.
// Switch on the first letter so that we don't have
// to waste time strcmp()ing against all the keywords.
static int keyword(char *s)
{
    switch (*s)
    {
    case 'i':
        if (!strcmp(s, "int"))
            return T_INT;
        if (!strcmp(s, "if"))
            return T_IF;
        break;
    case 'p':
        if (!strcmp(s, "print"))
            return T_PRINT;
        break;
    case 'e':
        if (!strcmp(s, "else"))
            return T_ELSE;
        break;
    case 'w':
        if (!strcmp(s, "while"))
            return T_WHILE;
        break;
    case 'f':
        if (!strcmp(s, "for"))
            return T_FOR;
    case 'v':
        if (!strcmp(s, "void"))
            return T_VOID;
        break;
    case 'c':
        if (!strcmp(s, "char"))
            return T_CHAR;
    case 'l':
        if (!strcmp(s, "long"))
            return T_LONG;
    case 'r':
        if (!strcmp(s, "return"))
            return T_RETURN;
        break;
    }

    return 0;
}

// Scan an identifier from the input file and
// store it in buf[]. Return the identifier's length
static int scanident(int c, char *buf, int lim)
{
    int i = 0;

    // Allow digits, alpha and underscores
    while (isalpha(c) || isdigit(c) || '_' == c)
    {
        // Error if we hit the identifier length limit,
        // else append to buf[] and get next character
        if (lim - 1 == i)
        {
            printf("identifier too long on line %d\n", Line);
            exit(0);
        }
        else if (i < lim - 1)
        {
            buf[i++] = c;
        }
        c = next();
    }

    // We hit a non-valid character, put it back.
    // NUL-terminate the buf[] and return the length
    putback(c);
    buf[i] = '\0';
    return i;
}

// Scan and return an integer literal
// value from the input file. Store
// the value as a string in Text.
static int scanint(int c)
{
    int k, val = 0;

    // Convert each character into an int value
    while ((k = chrpos("0123456789", c)) >= 0)
    {
        val = val * 10 + k;
        c = next();
    }

    putback(c);
    return val;
}

// Scan and return the next token found in the input.
// Return 1 if token valid, 0 if no tokens left.
int scan(struct token *t)
{
    int c, tokentype;

    // If we have any rejected token, return it
    if (Rejtoken != NULL)
    {
        t = Rejtoken;
        Rejtoken = NULL;
        return 1;
    }

    // Skip whitespace
    c = skip();

    // Determine the token base on
    // the input character
    switch (c)
    {
    case EOF:
        t->token = T_EOF;
        return 0;
    case '+':
        t->token = T_PLUS;
        break;
    case '-':
        t->token = T_MINUS;
        break;
    case '*':
        t->token = T_STAR;
        break;
    case '/':
        t->token = T_SLASH;
        break;
    case ';':
        t->token = T_SEMI;
        break;
    case '{':
        t->token = T_LBRACE;
        break;
    case '}':
        t->token = T_RBRACE;
        break;
    case '(':
        t->token = T_LPAREN;
        break;
    case ')':
        t->token = T_RPAREN;
        break;
    case ',':
        t->token = T_COMMA;
        break;
    case '=':
        if ((c = next()) == '=')
        {
            t->token = T_EQ;
        }
        else
        {
            putback(c);
            t->token = T_ASSIGN;
        }
        break;
    case '!':
        if ((c = next()) == '=')
        {
            t->token = T_NE;
        }
        else
        {
            fatalc("Unrecognised character", c);
        }
        break;
    case '<':
        if ((c = next()) == '=')
        {
            t->token = T_LE;
        }
        else
        {
            putback(c);
            t->token = T_LT;
        }
        break;
    case '>':
        if ((c = next()) == '=')
        {
            t->token = T_GE;
        }
        else
        {
            putback(c);
            t->token = T_GT;
        }
        break;
    case '&':
        if ((c = next()) == '&')
        {
            t->token = T_LOGAND;
        }
        else
        {
            putback(c);
            t->token = T_AMPER;
        }
        break;
    default:
        // If it's a digit, scan the
        // literal integer value in
        if (isdigit(c))
        {
            t->intvalue = scanint(c);
            t->token = T_INTLIT;
            break;
        }
        else if (isalpha(c) || '_' == c)
        {
            // Read in a keyword or identifier
            scanident(c, Text, TEXTLEN);

            // If it's a recognised keyword, return that token
            if ((tokentype = keyword(Text)))
            {
                t->token = tokentype;
                break;
            }

            // Not a recognised keyword, so it must be an identifier
            t->token = T_IDENT;
            break;
        }

        printf("Unrecognised character %c on line %d\n", c, Line);
        exit(1);
    }

    return 1;
}