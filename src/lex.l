%option noyywrap
%{
    #include "../build/parse.tab.h"
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

{NAME}      { return NAME; }
{BELIEFS}   { return BELIEFS; }
{GOALS}     { return GOALS; }
{PLANS}     { return PLANS; }
{AND}       { return AND; }
{OR}        { return OR; }
{NOT}       { return NOT; }

%%