#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// bison/yacc and flex/lex related declarations
void yyerror(const char *s, ...);
int yylex();
int yyparse();
extern int yylineno;
extern FILE *yyin;
extern int yylineno;
extern FILE *yyin;

struct symbol { 
    char *name;
    double value;
    struct ast *func;
    struct symlist *syms;
};

#define HASH_SIZE 9997

struct symbol *lookup(char*);

struct symlist {
    struct symbol *sym;
    struct symlist *next;
};

struct symlist *newsymlist(struct symbol *sym, struct symlist *next);
void symlistfree(struct symlist *sl);

enum bifs {
    B_sqrt = 1,
    B_exp,
    B_log,
    B_print,
    B_pow,
    B_mod,
    B_exit
};

struct ast {
    int nodetype;
    struct ast *l;
    struct ast *r;
};

struct fncall {
    int nodetype;
    struct ast *l;
    struct ast *r;
    enum bifs functype;
};

struct ufncall {
    int nodetype;
    struct ast *l;
    struct symbol *s;
};

struct flow {
    int nodetype;
    struct ast *cond;
    struct ast *tl;
    struct ast *el;
};

struct fornode {
    int nodetype; 
    struct ast *init;
    struct ast *cond;
    struct ast *incr;
    struct ast *body;
};

struct numval {
    int nodetype;
    double number;
};

struct symref {
    int nodetype;
    struct symbol *s;
};

struct symasgn {
    int nodetype;
    struct symbol *s;
    struct ast *v;
};

struct ast *newast(int nodetype, struct ast *l, struct ast *r);
struct ast *newcmp(int cmptype, struct ast *l, struct ast *r);
struct ast *newfunc(int functype, struct ast *l);
struct ast *newcall(struct symbol *s, struct ast *l);
struct ast *newref(struct symbol *s);
struct ast *newnum(double d);
struct ast *newflow(int nodetype, struct ast *cond, struct ast *tl, struct ast *el);
struct ast *newasgn(struct symbol *s, struct ast *v);
struct ast *newfor(struct ast *init, struct ast *cond, struct ast *incr, struct ast *body);

void dodef(struct symbol *name, struct symlist *syms, struct ast *stmts);
void treefree(struct ast *);
double eval(struct ast *);

