#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "defs.h"
#include "data.h"
#include "decl.h"

// Return the position of character c in string s, or -1 if not found
static int chrpos(const char *s, int c)
{
    const char *p;

    p = strchr(s, c);
    return (p ? p - s : -1);
}

// Putback a character that we don't need to deal with yet, return the old character
static int putback(int c)
{
    int old;

    old = Putback;
    Putback = c;
    if (c == 0)
    {
        Length = 0;
    }
    else
    {
        Length--;
    }
    return old;
}

// Get the next character from the input
static int next()
{
    int c;

    // Check if we have a character to put back
    if (Putback != 0)
    {
        c = putback(0); // Get from putback
    }
    else
    {
        c = fgetc(InputFile); // Read from the input file
    }
    Length++;
    return c;
}

// Skip any input that we don't need to deal with
static int skip()
{
    int c;

    // Get the next character
    c = next();
    // Skip whitespace, newlines, tabs, comments, etc...
    while (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '#')
    {
        if (c == '#')
        {
            // Skip the comment
            while ((c = next()) != EOF && c != '\n')
                ;
        }
        // Check for newline to update line and column numbers
        if (c == '\n')
        {
            Line++;
            Column = 0;
        }
        Column++;
        Length = 0;
        // Fetch the next character
        c = next();
    }
    return c;
}

// Scan an integer from the input
static int scanint(int c)
{
    int k, val = 0;

    // Convert each character into an int value
    while ((k = chrpos("0123456789", c)) != -1)
    {
        val = val * 10 + k;
        c = next();
    }
    // We hit a character that isn't a digit, put it back
    putback(c);
    return val;
}

// Scan the next token from the input, and put it in the Token structure
int scan(Token *t)
{
    int c;

    // Update the column number and reset the token length
    Column += Length;
    Length = 0;
    // Skip whitespace, newlines, tabs, etc...
    c = skip();
    // Determine the token based on the input
    switch (c)
    {
    case EOF:
        t->type = T_EOF;
        t->value = NO_VALUE;
        return 0;
    case '+':
        t->type = T_PLUS;
        t->value = NO_VALUE;
        break;
    case '-':
        t->type = T_MINUS;
        t->value = NO_VALUE;
        break;
    case '*':
        t->type = T_STAR;
        t->value = NO_VALUE;
        break;
    case '/':
        t->type = T_SLASH;
        t->value = NO_VALUE;
        break;
    default:
        // If it's a digit, scan the integer literal
        if (isdigit(c))
        {
            t->type = T_INTLIT;
            t->value.integer = scanint(c);
            break;
        }
        fprintf(stderr, "Syntax Error: unrecognized character '%c' at line %d column %d\n", c, Line, Column);
        exit(1);
    }
    return 1;
}