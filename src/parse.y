%{
#include "../src/ast.h"
%}

%union {
    char s[33];
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
program: agent_list { eval($1); }

agent_list: { $$ = NULL; }
    | agent_name beliefs goals plans PERCENT agent_list { $$ = new_agent($1, $2, $3, $4, $6); }
    ;

beliefs: BELIEFS OPEN_CURLY_BRACKET simple_list CLOSE_CURLY_BRACKET { $$ = $3; }
    ;

goals: GOALS OPEN_CURLY_BRACKET simple_list CLOSE_CURLY_BRACKET { $$ = $3; }
    ;

plans: PLANS OPEN_CURLY_BRACKET plan_set CLOSE_CURLY_BRACKET { $$ = $3; }
    ;

plan_set: { $$ = NULL; }
    | plan_name OPEN_PARENTHESIS trigger context actions CLOSE_PARENTHESIS SEMICOLON plan_set { $$ = new_plan($1, $3, $4, $5, $8); }
    ;

trigger: NAME SEMICOLON
    ;

context: NAME AND NAME SEMICOLON { $$ = new_context($1, $3, _AND); }
    | NAME OR NAME SEMICOLON { $$ = new_context($1, $3, _OR); }
    | NOT NAME SEMICOLON { $$ = new_context($2, NULL, _NOT); }
    | NAME { $$ = new_context($1, NULL, _NAME); }
    ;

actions: OPEN_CURLY_BRACKET simple_list CLOSE_CURLY_BRACKET { $$ = $2; }
    ;

simple_list: { $$ = NULL; }
    | list_element_name SEMICOLON simple_list { $$ = new_leaf(yyval.s, $3); }
    ;

plan_name: NAME
    ;
    
agent_name: AGENT_NAME
    ;

list_element_name: NAME
    ;
%%