%{
#include "../src/ast.h"
%}

%union {
    char s[256];
    struct Leaf *l;
    struct Context *e;
    struct Plan *p;
    struct Agent *a;
}

%token<s> NAME AGENT_NAME
%token BELIEFS GOALS PLANS AND OR NOT
%token PERCENT OPEN_CURLY_BRACKET CLOSE_CURLY_BRACKET OPEN_PARENTHESIS CLOSE_PARENTHESIS SEMICOLON

%type<s> agent_name list_element_name plan_name trigger
%type<l> simple_list beliefs goals actions
%type<e> context
%type<p> plans plan_set
%type<a> agent_list

%start program

%%
program: agent_list { eval($1); free_ast($1); }

agent_list: { $$ = NULL; }
    | agent_name beliefs goals plans PERCENT agent_list { $$ = new_agent($1, $2, $3, $4, $6); }
    ;

agent_name: AGENT_NAME { strlcpy($$, yyval.s, NAME_SIZE_NULL); }
    ;

beliefs: BELIEFS OPEN_CURLY_BRACKET simple_list CLOSE_CURLY_BRACKET { $$ = $3; }
    ;

goals: GOALS OPEN_CURLY_BRACKET simple_list CLOSE_CURLY_BRACKET { $$ = $3; }
    ;

simple_list: { $$ = NULL; }
    | list_element_name SEMICOLON simple_list { $$ = new_leaf(yyval.s, $3); }
    ;

list_element_name: NAME { strlcpy($$, yyval.s, NAME_SIZE_NULL); }
    ;

plans: PLANS OPEN_CURLY_BRACKET plan_set CLOSE_CURLY_BRACKET { $$ = $3; }
    ;

plan_set: { $$ = NULL; }
    | plan_name OPEN_PARENTHESIS trigger context actions CLOSE_PARENTHESIS SEMICOLON plan_set { $$ = new_plan($1, $3, $4, $5, $8); }
    ;

plan_name: NAME { strlcpy($$, yyval.s, NAME_SIZE_NULL); }
    ;

trigger: NAME SEMICOLON { strlcpy($$, yyval.s, NAME_SIZE_NULL); }
    ;

context: NAME AND NAME SEMICOLON { $$ = new_context($1, $3, _AND); }
    | NAME OR NAME SEMICOLON { $$ = new_context($1, $3, _OR); }
    | NOT NAME SEMICOLON { $$ = new_context($2, NULL, _NOT); }
    | NAME { $$ = new_context($1, NULL, _NAME); }
    ;

actions: OPEN_CURLY_BRACKET simple_list CLOSE_CURLY_BRACKET { $$ = $2; }
    ;
%%