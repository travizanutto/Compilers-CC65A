#include <stdio.h>
#include <stdlib.h>

extern int yylineno;
int yyerror(const char* s, ...);
int yylex();
