%{
#include "../src/ast.h"

#ifdef YYDEBUG
    yydebug = 1;
#endif
%}

%union {
    const char* s;
}

%token <s> NAME 
%token BELIEFS GOALS PLANS AND OR NOT
%start program

%%
program: agent_list { }
    ;

agent_list: 
    | agent '%' agent_list
    ;

agent: '#' agent_name BELIEFS '{' simple_list '}' GOALS '{' simple_list '}' PLANS '{' plan_set '}'
    ;

agent_name: NAME
    ;

simple_list:
    | list_element_name ';' simple_list
    ;

list_element_name: NAME
    ;

plan_set: 
    | plan_name plan_tuple ';' plan_set
    ;

plan_name: NAME
    ;

plan_tuple: '(' plan_trigger ';' context ';' '{' simple_list '}' ')'
    ;

plan_trigger: NAME
    ;

context: context_and AND context_and
    | context_or OR context_or
    | NOT context_not
    | context_name
    ;

context_and: NAME
    ;

context_or: NAME
    ;

context_not: NAME
    ;

context_name: NAME
    ;
%%

// ---------------------------------

int main() {
    //yyin = fopen("/home/cainan/Compilers-CC65A/tests/bob.nag", "r");
    printf("%i\n", yyparse());
}

int yyerror(const char* s, ...) {}