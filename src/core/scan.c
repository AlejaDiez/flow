/********************************************************************************
 * File Name: src/core/scan.c                                                   *
 *                                                                              *
 * Description: Lexical Analyzer, converts the input character stream into a    *
 *              stream of Tokens.                                               *
 * Author: Alejandro Diez Bermejo                                               *
 * Date: 2026-01-01                                                             *
 * Version: 0.0.0                                                               *
 ********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "defs.h"
#include "data.h"
#include "decl.h"

/**
 * Finds the position of a character in a string.
 *
 * @param s The string to search
 * @param c The character to find
 * @return Index of c in s, or -1 if not found.
 */
static int chrpos(char *s, int c)
{
    char *p;

    p = strchr(s, c);
    return (p ? p - s : -1);
}

/**
 * Puts a character back into the input stream buffer.
 *
 * @param c The character to put back
 * @return The previous putback character
 */
static int putback(int c)
{
    int old = Putback;

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

/**
 * Gets the next character from the input stream.
 *
 * @return The next character or EOF.
 */
static int next(void)
{
    int c;

    if (Putback)
    {
        c = putback(0);
    }
    else
    {
        c = fgetc(InputFile);
    }
    Length++;
    return c;
}

/**
 * Skips whitespace, newlines, and comments.
 *
 * @return The first non-skippable character.
 */
static int skip(void)
{
    int c = next();

    while (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '#')
    {
        if (c == '#')
        {
            // Skip comment until end of line
            while ((c = next()) != EOF && c != '\n')
            {
            }
        }

        if (c == '\n')
        {
            Line++;
            Column = 0;
        }
        Column++;
        Length = 0;
        c = next();
    }
    return c;
}

/**
 * Scans an integer literal.
 *
 * @param c The first digit of the integer
 * @return The integer value
 */
static int scanint(int c)
{
    int k, val = 0;

    while ((k = chrpos("0123456789", c)) >= 0)
    {
        val = val * 10 + k;
        c = next();
    }
    putback(c);
    return val;
}

/**
 * Scans an identifier or keyword.
 *
 * @param c The first character
 * @param buf Buffer to store the string
 * @param lim Buffer size limit
 * @return Length of the identifier
 */
static int scanident(int c, char *buf, int lim)
{
    int i = 0;

    while (isalpha(c) || isdigit(c) || c == '_')
    {
        if (i < lim - 1)
        {
            buf[i++] = c;
        }
        else
        {
            fprintf(stderr, "Error: identifier too long at %d:%d\n", Line, Column);
            exit(1);
        }
        c = next();
    }
    putback(c);
    buf[i] = '\0';
    return i;
}

/**
 * Checks if an identifier is a reserved keyword.
 *
 * @param s The identifier string
 * @return The TokenType (keyword or T_IDENT)
 */
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
    case 'c':
        if (!strcmp(s, "const"))
        {
            return T_CONST;
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
        if (!strcmp(s, "fun"))
        {
            return T_FUN;
        }
        break;
    case 'i':
        if (!strcmp(s, "if"))
        {
            return T_IF;
        }
        if (!strcmp(s, "int"))
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
        if (!strcmp(s, "void"))
        {
            return T_VOID;
        }
        break;
    }
    return T_IDENT;
}

/**
 * Main scanning function.
 *
 * @param t Pointer to the Token structure to fill
 * @return 1 if token found, 0 if EOF
 */
int scan(Token *t)
{
    int c;

    // Advance column by the length of the previous token
    Column += Length;
    Length = 0;

    c = skip();

    switch (c)
    {
    case EOF:
        t->type = T_EOF;
        return 0;
    case '+':
        if ((c = next()) == '=')
        {
            t->type = T_ASPLUS;
        }
        else
        {
            putback(c);
            t->type = T_PLUS;
        }
        break;
    case '-':
        if ((c = next()) == '=')
        {
            t->type = T_ASMINUS;
        }
        else
        {
            putback(c);
            t->type = T_MINUS;
        }
        break;
    case '*':
        if ((c = next()) == '=')
        {
            t->type = T_ASSTAR;
        }
        else if (c == '*')
        {
            if ((c = next()) == '=')
            {
                t->type = T_ASDSTAR;
            }
            else
            {
                putback(c);
                t->type = T_DSTAR;
            }
        }
        else
        {
            putback(c);
            t->type = T_STAR;
        }
        break;
    case '/':
        if ((c = next()) == '=')
        {
            t->type = T_ASSLASH;
        }
        else
        {
            putback(c);
            t->type = T_SLASH;
        }
        break;
    case '%':
        if ((c = next()) == '=')
        {
            t->type = T_ASPERCENT;
        }
        else
        {
            putback(c);
            t->type = T_PERCENT;
        }
        break;
    case ';':
        t->type = T_SEMICOLON;
        break;
    case ':':
        t->type = T_COLON;
        break;
    case ',':
        t->type = T_COMMA;
        break;
    case '(':
        t->type = T_LPAREN;
        break;
    case ')':
        t->type = T_RPAREN;
        break;
    case '{':
        t->type = T_LBRACE;
        break;
    case '}':
        t->type = T_RBRACE;
        break;
    case '=':
        if ((c = next()) == '=')
        {
            t->type = T_EQ;
        }
        else
        {
            putback(c);
            t->type = T_ASSIGN;
        }
        break;
    case '!':
        if ((c = next()) == '=')
        {
            t->type = T_NEQ;
        }
        else
        {
            putback(c);
            t->type = T_BANG;
        }
        break;
    case '<':
        if ((c = next()) == '=')
        {
            t->type = T_LE;
        }
        else
        {
            putback(c);
            t->type = T_LT;
        }
        break;
    case '>':
        if ((c = next()) == '=')
        {
            t->type = T_GE;
        }
        else
        {
            putback(c);
            t->type = T_GT;
        }
        break;
    case '&':
        if ((c = next()) == '&')
        {
            if ((c = next()) == '=')
            {
                t->type = T_ASDAMPERSAND;
            }
            else
            {
                putback(c);
                t->type = T_DAMPERSAND;
            }
        }
        else
        {
            fprintf(stderr, "Syntax Error: unknown token '&' at %d:%d\n", Line, Column);
            exit(1);
        }
        break;
    case '|':
        if ((c = next()) == '|')
        {
            if ((c = next()) == '=')
            {
                t->type = T_ASDPIPE;
            }
            else
            {
                putback(c);
                t->type = T_DPIPE;
            }
        }
        else
        {
            fprintf(stderr, "Syntax Error: unknown token '|' at %d:%d\n", Line, Column);
            exit(1);
        }
        break;

    default:
        if (isdigit(c))
        {
            t->type = T_INTLIT;
            t->value.integer = scanint(c);
        }
        else if (isalpha(c) || c == '_')
        {
            char buffer[MAX_LEN];

            scanident(c, buffer, MAX_LEN);
            if (strcmp(buffer, "_") == 0)
            {
                t->type = T_UNDERSCORE;
            }
            else
            {
                int type = keyword(buffer);

                t->type = type;
                if (type == T_IDENT)
                {
                    t->value.string = strdup(buffer);
                }
                else
                {
                    t->value.string = NULL;
                }
            }
        }
        else
        {
            fprintf(stderr, "Syntax Error: unknown character '%c' at %d:%d\n", c, Line, Column);
            exit(1);
        }
        break;
    }
    return 1;
}

/**
 * Verifies the current token matches the expected type, advances the scanner if it matches.
 *
 * @param t Expected Token Type
 */
void match(TokenType t)
{
    if (CurrentToken.type == t)
    {
        scan(&CurrentToken);
    }
    else
    {
        fprintf(stderr, "Syntax Error: expected '%s' but found '%s' at %d:%d\n",
                TokenTypeStr[t], TokenTypeStr[CurrentToken.type], Line, Column);
        exit(1);
    }
}

/**
 * Looks ahead at the next token without consuming it.
 *
 * @return The TokenType of the next token.
 */
TokenType peek(void)
{
    Token tmpToken;
    long pos = ftell(InputFile);
    int oldPutback = Putback;
    int oldLine = Line;
    int oldCol = Column;
    int oldLen = Length;

    scan(&tmpToken);

    fseek(InputFile, pos, SEEK_SET);
    Putback = oldPutback;
    Line = oldLine;
    Column = oldCol;
    Length = oldLen;

    return tmpToken.type;
}
