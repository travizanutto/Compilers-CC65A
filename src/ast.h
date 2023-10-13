#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

extern int yylineno;
extern FILE* yyin;
int yyerror(const char* s, ...);
int yylex();
#ifdef YYDEBUG
    yydebug = 1;
#endif


/* function by: Jonathan Leffler
https://stackoverflow.com/questions/16136437/clean-all-spaces-and-newlines-c-or-lex-bison
*/

static void squish_whitespace(char *TAG)
{
    int sizeOfArray = strlen(TAG);
    int i, j;
    for (i = j = 0; i < sizeOfArray; i++)
    {
        if (isspace(TAG[i]))
        {
            while (i < sizeOfArray && isspace(TAG[i]))
                i++;
            if (TAG[i] != '\0')
                TAG[j++] = ' ';
        }
        TAG[j++] = TAG[i];
    }
    TAG[j] = '\0';
}