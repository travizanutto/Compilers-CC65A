%option noyywrap
%{
    #include "../build/parse.tab.h"
    #define NAME_MAX_VALUE 256
%}

NUMBER      [0-9]
LETTER      [a-zA-Z]
NAME        {LETTER}({LETTER}|{NUMBER})?*
BELIEFS     "crencas: "
GOALS       "objetivos: "
PLANS       "planos: "
AND         "E"
OR          "OU"
NOT         "NAO"

%%
"%"         { return PERCENT; }
"#"         { return NUMBER_SIGN; }
"{"         { return OPEN_CURLY_BRACKET; }
"}"         { return CLOSE_CURLY_BRACKET; }
"("         { return OPEN_PARENTHESIS; }
")"         { return CLOSE_PARENTHESIS; }
";"         { return SEMICOLON; }
{BELIEFS}   { return BELIEFS; }
{GOALS}     { return GOALS; }
{PLANS}     { return PLANS; }
{AND}       { return AND; }
{OR}        { return OR; }
{NOT}       { return NOT; }
{NAME}      { sscanf(yytext, "%s", yylval.s); return NAME; }

%%