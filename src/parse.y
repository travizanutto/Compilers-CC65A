%{
#include "../src/ast.h"

#ifdef YYDEBUG
    yydebug = 1;
#endif
%}

%union {
    char s[256];
}

%token <s> NAME 
%token <s> BELIEFS GOALS PLANS AND OR NOT
%token PERCENT NUMBER_SIGN OPEN_CURLY_BRACKET CLOSE_CURLY_BRACKET OPEN_PARENTHESIS CLOSE_PARENTHESIS SEMICOLON

%start program

%%
program: 
    | agent_list program
    ;

agent_list: 
    | agent PERCENT agent_list { }
    ;

agent: NUMBER_SIGN agent_name BELIEFS OPEN_CURLY_BRACKET simple_list CLOSE_CURLY_BRACKET GOALS OPEN_CURLY_BRACKET simple_list CLOSE_CURLY_BRACKET PLANS OPEN_CURLY_BRACKET plan_set CLOSE_CURLY_BRACKET
    ;

agent_name: NAME
    ;

simple_list:
    | list_element_name SEMICOLON simple_list
    ;

list_element_name: NAME
    ;

plan_set: 
    | plan_name plan_tuple SEMICOLON plan_set
    ;

plan_name: NAME
    ;

plan_tuple: OPEN_PARENTHESIS plan_trigger CLOSE_PARENTHESIS context SEMICOLON OPEN_CURLY_BRACKET simple_list CLOSE_CURLY_BRACKET CLOSE_PARENTHESIS
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
    yyparse();
}

int yyerror(const char* s, ...) {}