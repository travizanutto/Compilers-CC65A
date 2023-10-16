%{
#include "../src/ast.h"
%}

%union {
    char s[256];
    struct leaf *l;
    struct context *c;
    struct plan *p;
    struct agent *a;
}

%token<s> NAME AGENT_NAME
%token BELIEFS GOALS PLANS AND OR NOT
%token PERCENT OPEN_CURLY_BRACKET CLOSE_CURLY_BRACKET OPEN_PARENTHESIS CLOSE_PARENTHESIS SEMICOLON

%type<s> agent_name list_element_name plan_name plan_trigger
%type<a> agent_list
%type<l> simple_list beliefs goals plan_actions
%type<p> plans plan_set


%start program

%%
program: {}
    | agent_list program
    ;

agent_list: { $$ = NULL; }
    | agent_name beliefs goals plans PERCENT agent_list { $$ = new_agent($1, $2, $3, $4, $6); }
    ;

agent_name: AGENT_NAME { strlcpy($$, $1, 255); }
    ;

beliefs: BELIEFS OPEN_CURLY_BRACKET simple_list CLOSE_CURLY_BRACKET { $$ = $3; }
    ;

goals: GOALS OPEN_CURLY_BRACKET simple_list CLOSE_CURLY_BRACKET { $$ = $3; }
    ;

simple_list: { $$ = NULL; }
    | list_element_name SEMICOLON simple_list { new_leaf($1, $3); }
    ;

list_element_name: NAME { strlcpy($$, $1, 255); }
    ;

plans: PLANS OPEN_CURLY_BRACKET plan_set CLOSE_CURLY_BRACKET { $$ = $3; }
    ;

plan_set: { $$ = NULL; }
    | plan_name OPEN_PARENTHESIS plan_trigger context plan_actions CLOSE_PARENTHESIS SEMICOLON plan_set { }
    ;

plan_name: NAME { strlcpy($$, $1, 255);1; }
    ;

plan_trigger: NAME SEMICOLON { strlcpy($$, $1, 255); }
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

int main(int argc, int **argv) {
    if (argc < 1) {
        printf("Usage: ./nag_to_jacamo \"list of files\"");
        exit(EXIT_FAILURE);
    }
    yyparse();
}

int yyerror(const char* s, ...) {}