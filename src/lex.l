%option noyywrap
%{
    #include "../build/parse.tab.h"
    #define NAME_MAX_VALUE 256
%}

NUMBER      [0-9]
LETTER      [a-zA-Z]
NAME        {LETTER}({LETTER}|{NUMBER})?*
AGENT_NAME  "#"{NAME}
BELIEFS     "crencas: "
GOALS       "objetivos: "
PLANS       "planos: "
AND         "E"
OR          "OU"
NOT         "NAO"

%%
"%"         { return PERCENT; }
"{"         { return OPEN_CURLY_BRACKET; }
"}"         { return CLOSE_CURLY_BRACKET; }
"("         { return OPEN_PARENTHESIS; }
")"         { return CLOSE_PARENTHESIS; }
";"         { return SEMICOLON; }
\n          { return NEW_LINE; }
{BELIEFS}   { return BELIEFS; }
{GOALS}     { return GOALS; }
{PLANS}     { return PLANS; }
{AND}       { return AND; }
{OR}        { return OR; }
{NOT}       { return NOT; }
{NAME}      { sscanf(yytext, "%s", yylval.s); return NAME; }
{AGENT_NAME} { sscanf(yytext, "%s", yylval.s); return AGENT_NAME; }

%%