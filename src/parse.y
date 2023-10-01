%{
    #include <stdio.h>
    int yyerror(const char*);
    int yylex();

%}

%%
test:
    ;
%%

int main() {
    yyparse();
}

int yyerror(const char* s) {}