%{
    #include <stdio.h>
    int yyerror(const char*);
    int yylex();
    #define YYDEBUG 1

%}

%token NAME BELIEFS GOALS PLANS AND OR NOT
%start program

%%
program: agent_list
    ;

agent_list: 
    | agent '%' agent_list
    ;

agent: '#' NAME BELIEFS '{' simple_list '}' GOALS '{' simple_list '}' PLANS '{' plan_set '}'
    ;

simple_list:
    | NAME ';' simple_list
    ;

plan_set: 
    | NAME plan_tuple ';' plan_set
    ; 

plan_tuple: '(' NAME ';' context ';' '{' simple_list '}' ')'
    ;

context: NAME AND NAME
    | NAME OR NAME
    | NOT NAME
    | NAME
    ;
%%

// ---------------------------------

int main() {
    yyparse();
}

int yyerror(const char* s) {}