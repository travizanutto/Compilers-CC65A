%{
#include "../src/ast.h"
%}

%union {
    char s[256];
}

%token <s> NAME AGENT_NAME
%token BELIEFS GOALS PLANS AND OR NOT
%token PERCENT OPEN_CURLY_BRACKET CLOSE_CURLY_BRACKET OPEN_PARENTHESIS CLOSE_PARENTHESIS SEMICOLON NEW_LINE

%start program

%%
program: 
    | agent_list program
    ;

agent_list: 
    | agent PERCENT agent_list 
    ;
// agent_name beliefs goals
agent: agent_name beliefs goals plans { printf("agent\n"); }
    ;

agent_name: AGENT_NAME
    ;

beliefs: BELIEFS OPEN_CURLY_BRACKET simple_list CLOSE_CURLY_BRACKET
    ;

goals: GOALS OPEN_CURLY_BRACKET simple_list CLOSE_CURLY_BRACKET
    ;

simple_list:
    | list_element_name SEMICOLON simple_list
    ;

list_element_name: NAME { }
    ;

plans: PLANS OPEN_CURLY_BRACKET plan_set CLOSE_CURLY_BRACKET
    ;

plan_set: 
    | plan_name OPEN_PARENTHESIS plan_trigger context plan_actions CLOSE_PARENTHESIS SEMICOLON plan_set
    ;

plan_name: NAME { }
    ;

plan_trigger: NAME SEMICOLON { }
    ;

context: context_and AND context_and SEMICOLON
    | context_or OR context_or SEMICOLON
    | NOT context_not SEMICOLON
    | context_name SEMICOLON
    ;

context_and: NAME
    | NOT NAME
    ;

context_or: NAME
    | NOT NAME
    ;

context_not: NAME
    | NOT NAME
    ;

context_name: NAME
    | NOT NAME
    ;

plan_actions: OPEN_CURLY_BRACKET simple_list CLOSE_CURLY_BRACKET
    ;
%%

// ---------------------------------

int main() {
    yyparse();
}

int yyerror(const char* s, ...) {}