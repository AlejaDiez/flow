#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "defs.h"
#include "data.h"
#include "decl.h"

// Return the position of character c in string s, or -1 if not found
static int chrpos(char *s, int c)
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
static int next(void)
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
static int skip(void)
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

// Scan an identifier
static int scanident(int c, char *buf, int lim)
{
    int i = 0;

    // Scan until we hit a character that isn't a letter, digit or underscore
    while (isalpha(c) || isdigit(c) || c == '_')
    {
        if (lim - 1 == i)
        {
            fprintf(stderr, "Error: out of memory (%d:%d)\n", Line, Column);
            exit(1);
        }
        else if (i < lim - 1)
        {
            buf[i++] = c;
        }
        c = next();
    }
    // Put back the character that stopped the loop
    putback(c);
    buf[i] = '\0';

    return i;
}

// Check if the identifier is a keyword and return the keyword token
static int keyword(char *s)
{
    switch (*s)
    {
    case 'b':
        if (!strcmp(s, "bool"))
        {
            return T_BOOL;
        }
        break;
    case 'e':
        if (!strcmp(s, "else"))
        {
            return T_ELSE;
        }
        break;
    case 'f':
        if (!strcmp(s, "false"))
        {
            return T_FALSE;
        }
        else if (!strcmp(s, "fun"))
        {
            return T_FUN;
        }
        break;
    case 'i':
        if (!strcmp(s, "if"))
        {
            return T_IF;
        }
        else if (!strcmp(s, "int"))
        {
            return T_INT;
        }
        break;
    case 'l':
        if (!strcmp(s, "loop"))
        {
            return T_LOOP;
        }
        break;
    case 'm':
        if (!strcmp(s, "match"))
        {
            return T_MATCH;
        }
        break;
    case 'n':
        if (!strcmp(s, "next"))
        {
            return T_NEXT;
        }
        break;
    case 'p':
        if (!strcmp(s, "print"))
        {
            return T_PRINT;
        }
        break;
    case 'r':
        if (!strcmp(s, "return"))
        {
            return T_RETURN;
        }
        break;
    case 's':
        if (!strcmp(s, "stop"))
        {
            return T_STOP;
        }
        break;
    case 't':
        if (!strcmp(s, "true"))
        {
            return T_TRUE;
        }
        break;
    case 'v':
        if (!strcmp(s, "var"))
        {
            return T_VAR;
        }
        else if (!strcmp(s, "void"))
        {
            return T_VOID;
        }
        break;
    }
    return T_IDENT;
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
        if ((c = next()) == '=')
        {
            t->type = T_ASPLUS;
            t->value = NO_VALUE;
        }
        else
        {
            putback(c);
            t->type = T_PLUS;
            t->value = NO_VALUE;
        }
        break;
    case '-':
        if ((c = next()) == '=')
        {
            t->type = T_ASMINUS;
            t->value = NO_VALUE;
        }
        else
        {
            putback(c);
            t->type = T_MINUS;
            t->value = NO_VALUE;
        }
        break;
    case '*':
        if ((c = next()) == '*')
        {
            if ((c = next()) == '=')
            {
                t->type = T_ASDSTAR;
                t->value = NO_VALUE;
            }
            else
            {
                putback(c);
                t->type = T_DSTAR;
                t->value = NO_VALUE;
            }
        }
        else if (c == '=')
        {
            t->type = T_ASSTAR;
            t->value = NO_VALUE;
        }
        else
        {
            putback(c);
            t->type = T_STAR;
            t->value = NO_VALUE;
        }
        break;
    case '/':
        if ((c = next()) == '=')
        {
            t->type = T_ASSLASH;
            t->value = NO_VALUE;
        }
        else
        {
            putback(c);
            t->type = T_SLASH;
            t->value = NO_VALUE;
        }
        break;
    case '%':
        if ((c = next()) == '=')
        {
            t->type = T_ASPERCENT;
            t->value = NO_VALUE;
        }
        else
        {
            putback(c);
            t->type = T_PERCENT;
            t->value = NO_VALUE;
        }
        break;
    case '=':
        if ((c = next()) == '=')
        {
            t->type = T_EQ;
            t->value = NO_VALUE;
        }
        else
        {
            putback(c);
            t->type = T_ASSIGN;
            t->value = NO_VALUE;
        }
        break;
    case '<':
        if ((c = next()) == '=')
        {
            t->type = T_LE;
            t->value = NO_VALUE;
        }
        else
        {
            putback(c);
            t->type = T_LT;
            t->value = NO_VALUE;
        }
        break;
    case '>':
        if ((c = next()) == '=')
        {
            t->type = T_GE;
            t->value = NO_VALUE;
        }
        else
        {
            putback(c);
            t->type = T_GT;
            t->value = NO_VALUE;
        }
        break;
    case '!':
        if ((c = next()) == '=')
        {
            t->type = T_NEQ;
            t->value = NO_VALUE;
        }
        else
        {
            putback(c);
            t->type = T_BANG;
            t->value = NO_VALUE;
        }
        break;
    case '&':
        if ((c = next()) == '&')
        {
            if ((c = next()) == '=')
            {
                t->type = T_ASDAMPERSAND;
                t->value = NO_VALUE;
            }
            else
            {
                putback(c);
                t->type = T_DAMPERSAND;
                t->value = NO_VALUE;
            }
        }
        else
        {
            putback(c);
            fprintf(stderr, "Syntax Error: unrecognized character '%c' (%d:%d)\n", c, Line, Column);
            exit(1);
        }
        break;
    case '|':
        if ((c = next()) == '|')
        {
            if ((c = next()) == '=')
            {
                t->type = T_ASDPIPE;
                t->value = NO_VALUE;
            }
            else
            {
                putback(c);
                t->type = T_DPIPE;
                t->value = NO_VALUE;
            }
        }
        else
        {
            putback(c);
            fprintf(stderr, "Syntax Error: unrecognized character '%c' (%d:%d)\n", c, Line, Column);
            exit(1);
        }
        break;
    case ':':
        t->type = T_COLON;
        t->value = NO_VALUE;
        break;
    case ';':
        t->type = T_SEMICOLON;
        t->value = NO_VALUE;
        break;
    case '(':
        t->type = T_LPAREN;
        t->value = NO_VALUE;
        break;
    case ')':
        t->type = T_RPAREN;
        t->value = NO_VALUE;
        break;
    case '{':
        t->type = T_LBRACE;
        t->value = NO_VALUE;
        break;
    case '}':
        t->type = T_RBRACE;
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
        // If it's a letter or underscore, scan the identifier
        else if (isalpha(c) || '_' == c)
        {
            char buffer[MAX_LEN];

            scanident(c, buffer, MAX_LEN - 1);

            if (strcmp(buffer, "_") == 0)
            {
                t->type = T_UNDERSCORE;
                t->value = NO_VALUE;
            }
            else
            {
                int tokentype = keyword(buffer);

                t->type = tokentype;
                if (tokentype == T_IDENT)
                {
                    t->value.string = strdup(buffer);
                }
            }
            break;
        }
        fprintf(stderr, "Syntax Error: unrecognized character '%c' (%d:%d)\n", c, Line, Column);
        exit(1);
    }
    return 1;
}

// Check that the current token is 't', and fetch the next one.
void match(TokenType t, char *what)
{
    if (CurrentToken.type == t)
    {
        scan(&CurrentToken);
    }
    else
    {
        fprintf(stderr, "Syntax Error: expected %s but found other token (%d:%d)\n", what, Line, Column);
        exit(1);
    }
}

// Look ahead in the input to see the next tokens without consuming them
int peek(void)
{
    Token tmpToken;

    // Save current state
    struct
    {
        int putback;
        int line;
        int column;
        int length;
    } state = {Putback, Line, Column, Length};
    long pos = ftell(InputFile);

    // Scan the next token
    scan(&tmpToken);

    // Restore state
    fseek(InputFile, pos, SEEK_SET);
    Putback = state.putback;
    Line = state.line;
    Column = state.column;
    Length = state.length;

    // Return the token
    return tmpToken.type;
}
