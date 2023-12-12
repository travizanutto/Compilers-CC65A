%option noyywrap nodefault yylineno
%{
#include "parse.tab.h"
#include "../src/ast.h"
%}

EXP ([Ee][-+]?[0-9]+)

%%
"+" |
"-" |
"*" |
"/" |
"=" |
"," |
";" |
"(" |
")" |
"^"     { return yytext[0]; }

"#"     { while(input() != '\n'); return EOL; }

">"     { yylval.fn = 1; return CMP; }
"<"     { yylval.fn = 2; return CMP; }
"!="    { yylval.fn = 3; return CMP; }
"=="    { yylval.fn = 4; return CMP; }
">="    { yylval.fn = 5; return CMP; }
"<="    { yylval.fn = 6; return CMP; }

"if"    { return IF; }
"then"  { return THEN; }
"else"  { return ELSE; }
"while" { return WHILE; }
"for"   { return FOR; }
"do"    { return DO; }
"let"   { return LET; }
"and"   { return AND; }
"or"    { return OR; }


"sqrt"  { yylval.fn = B_sqrt; return FUNC; }
"exp"   { yylval.fn = B_exp; return FUNC; }
"log"   { yylval.fn = B_log; return FUNC; }
"print" { yylval.fn = B_print; return FUNC; }

"pow"   { yylval.fn = B_pow; return FUNC; }
"mod"   { yylval.fn = B_mod; return FUNC; }
"exit"  { yylval.fn = B_exit; return FUNC; }

[a-zA-Z][a-zA-Z0-9]*    { yylval.s = lookup(yytext); return NAME; }

[0-9]+"."[0-9]*{EXP}? |
"."?[0-9]+{EXP}?        { yylval.d = atof(yytext); return NUMBER; }

"//".*
[ \t]

\\\n    { printf("> "); }
\n      { return EOL; }

.       { yyerror("[ERROR] UNKOWN: %c\n", *yytext); }
%%