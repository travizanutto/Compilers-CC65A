#include "ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>

struct symbol symtab[HASH_SIZE];

static unsigned symhash(char *sym)
{
    unsigned int hash = 0;
    unsigned c;

    while(c = *sym++)
        hash = hash * 9 ^ c;

    return hash;
}

struct symbol *lookup(char *sym) 
{
    struct symbol *sp = &symtab[symhash(sym)%HASH_SIZE];
    int scount = HASH_SIZE;

    while (--scount >= 0) {
        if (sp->name && !strcasecmp(sp->name, sym))
            return sp;
        
        if (!sp->name) {
            sp->name = strdup(sym);
            sp->value = 0;
            sp->func = NULL;
            sp->syms = NULL;
            return sp;
        }

        if(++sp >= symtab+HASH_SIZE)
            sp = symtab;
    }
    yyerror("[ERROR] Symbol table overflow");
    abort();   
}

struct ast *newast(int nodetype, struct ast *l, struct ast *r) 
{
    struct ast *a = malloc(sizeof(struct ast));
    if (!a) {
        yyerror("[ERROR] Out of space");
        exit(0);
    }
    a->nodetype = nodetype;
    a->l = l;
    a->r = r;
    return a;
}   

struct ast *newcmp(int cmptype, struct ast *l, struct ast *r) 
{
    struct ast *a = malloc(sizeof(struct ast));
    if (!a) {
        yyerror("[ERROR] Out of space");
        exit(0);
    }
    a->nodetype = '0' + cmptype;
    a->l = l;
    a->r = r;
    return a;
}

struct ast *newfunc(int functype, struct ast *l) 
{
    struct fncall *a = malloc(sizeof(struct fncall));
    if (!a) {
        yyerror("[ERROR] Out of space");
        exit(0);
    }
    a->nodetype = 'F';
    if (l->nodetype == 'L') {
        a->l = l->l;
        a->r = l->r;
    }
    else {
        a->l = l;
        a->r = NULL;
    }
    a->functype = functype;
    return (struct ast *)a;
}

struct ast *newcall(struct symbol *s, struct ast *l) 
{
    struct ufncall *a = malloc(sizeof(struct ufncall));
    if (!a) {
        yyerror("[ERROR] Out of space");
        exit(0);
    }
    a->nodetype = 'C';
    a->l = l;
    a->s = s;
    return (struct ast *)a;
}

struct ast *newref(struct symbol *s) 
{
    struct symref *a = malloc(sizeof(struct symref));
    if (!a) {
        yyerror("[ERROR] Out of space");
        exit(0);
    }
    a->nodetype = 'N';
    a->s = s;
    return (struct ast *)a;
}

struct ast *newnum(double d) 
{
    struct numval *a = malloc(sizeof(struct numval));
    if (!a) {
        yyerror("[ERROR] Out of space");
        exit(0);
    }
    a->nodetype = 'K';
    a->number = d;
    return (struct ast *)a;
}

struct ast *newflow(int nodetype, struct ast *cond, struct ast *tl, struct ast *el) 
{
    struct flow *a = malloc(sizeof(struct flow));
    if (!a) {
        yyerror("[ERROR] Out of space");
        exit(0);
    }
    a->nodetype = nodetype;
    a->cond = cond;
    a->tl = tl;
    a->el = el;
    return (struct ast *)a;
}

struct ast *newasgn(struct symbol *s, struct ast *v) 
{
    struct symasgn *a = malloc(sizeof(struct symasgn));
    if (!a) {
        yyerror("[ERROR] Out of space");
        exit(0);
    }
    a->nodetype = '=';
    a->s = s;
    a->v = v;
    return (struct ast *)a;
}

struct ast *newfor(struct ast *init, struct ast *cond, struct ast *incr, struct ast *body) {
    struct fornode *f = malloc(sizeof(struct fornode));
    
    if(!f) {
        yyerror("out of space");
        exit(0);
    }

    f->nodetype = 'J';
    f->init = init;
    f->cond = cond;
    f->incr = incr;
    f->body = body;

    return (struct ast *)f;
}

void treefree(struct ast *a) 
{
    switch(a->nodetype) {
        case '+': case '-': case '*': case '/': case '^':
        case '1': case '2': case '3': case '4': case '5': case '6': 
        case 'L': case 'F': case 'C':
            if (a->r)
                treefree(a->r);
            treefree(a->l);
        case 'K': case 'N':
            break;
        case '=':
            free(((struct symasgn *)a)->v);
            break;
        case 'I': case 'W':
            free(((struct flow *)a)->cond);
            if (((struct flow *)a)->tl) free(((struct flow *)a)->tl);
            if (((struct flow *)a)->el) free(((struct flow *)a)->el);
            break;
        case 'J':
            if (((struct fornode *)a)->init) treefree(((struct fornode *)a)->init);
            if (((struct fornode *)a)->cond) treefree(((struct fornode *)a)->cond);
            if (((struct fornode *)a)->incr) treefree(((struct fornode *)a)->incr);
            if (((struct fornode *)a)->body) treefree(((struct fornode *)a)->body);
            break;
        default:
            printf("[ERROR] Free bad node %c\n", a->nodetype);
    }
    free(a);
}

struct symlist *newsymlist(struct symbol *sym, struct symlist *next) 
{
    struct symlist *sl = malloc(sizeof(struct symlist));
    if (!sl) {
        yyerror("[ERROR] Out of space");
        exit(0);
    }
    sl->sym = sym;
    sl->next = next;
    return sl;
}

void symlistfree(struct symlist *sl) 
{
    struct symlist *nsl;
    while(sl) {
        nsl = sl->next;
        free(sl);
        sl = nsl;
    }
}

static double callbuiltin(struct fncall *);
static double calluser(struct ufncall *);

double eval(struct ast *a) 
{
    double v;

    if (!a) {
        yyerror("[ERROR] Internal error, null eval");
        return 0.0;
    }

    switch(a->nodetype) {
        case 'K': v = ((struct numval *)a)->number; break;
        case 'N': v = ((struct symref *)a)->s->value; break;
        case '=': v = ((struct symasgn *)a)->s->value = eval(((struct symasgn *)a)->v); break;
        case '+': v = eval(a->l) + eval(a->r); break;
        case '-': v = eval(a->l) - eval(a->r); break;
        case '*': v = eval(a->l) * eval(a->r); break;
        case '/': v = eval(a->l) / eval(a->r); break;
        case '^': v = pow(eval(a->l), eval(a->r)); break;
        case '1': v = (eval(a->l) > eval(a->r)) ? 1 : 0; break;
        case '2': v = (eval(a->l) < eval(a->r)) ? 1 : 0; break;
        case '3': v = (eval(a->l) != eval(a->r)) ? 1 : 0; break;
        case '4': v = (eval(a->l) == eval(a->r)) ? 1 : 0; break;
        case '5': v = (eval(a->l) >= eval(a->r)) ? 1 : 0; break;
        case '6': v = (eval(a->l) <= eval(a->r)) ? 1 : 0; break;
        case 'I':
            struct flow *i = (struct flow*) a;
            if (eval(i->cond) != 0) {
                if (i->tl) {
                    v = eval(i->tl);
                } else {
                    v = 0.0;
                }
            } else {
                if (i->el) {
                    v = eval(i->el);
                } else {
                    v = 0.0;
                }
            }
            break;
        case 'W':
            struct flow *w = (struct flow*)a;
            v = 0.0;
            if (w->tl) {
                while(eval(w->cond) != 0)
                    v = eval(w->tl);
            }
            break;

        case 'F': v = callbuiltin((struct fncall *)a);
            if (((struct fncall *)a)->functype == B_exit) {
                treefree(a);
                exit(v);
            }
            break;
        case 'C': v = calluser((struct ufncall *)a); break; 
        case 'L': eval(a->l); v = eval(a->r); break;
        case 'A': v = eval(a->l) && eval(a->r); break;
        case 'O': v = eval(a->l) || eval(a->r); break;
        case 'J':  
            struct fornode *f = (struct fornode*)a;
            v = 0.0;
            if(f->init) 
                eval(f->init);

            for(; f->cond == NULL || eval(f->cond) != 0;) {
                if(f->body) 
                    v = eval(f->body);

                if(f->incr) 
                    eval(f->incr); 
            }
            break;
        default: printf("[ERROR] Internal error: bad node %c\n", a->nodetype);
    }
    return v;
}

static double callbuiltin(struct fncall *f)
{
    enum bifs functype = f->functype;
    double v = eval(f->l);
    double r = f->r ? eval(f->r) : 1;

    switch(functype) {
        case B_sqrt:
            return sqrt(v);
        case B_exp:
            return exp(v);
        case B_log:
            return log(v);
        case B_print:
            printf("= %4.4g\n", v);
            return v;
        case B_pow:
            return pow(v, r);
        case B_mod:
            return ((int)v) % ((int)r);
        case B_exit:
            return v;
        default:
            yyerror("[ERROR] Unknown built-in function %d", functype);
            return 0.0;
    }
}

void dodef(struct symbol *name, struct symlist *syms, struct ast *stmts) 
{
    if (name->syms) symlistfree(name->syms);
    if (name->func) treefree(name->func);
    name->syms = syms;
    name->func = stmts;
}

static double calluser(struct ufncall *f)
{
    struct symbol *fn = f->s;
    struct symlist *sl;
    struct ast *args = f->l;
    double *oldval, *newval;
    double v;
    int nargs;
    int i;

    if (!fn->func) {
        yyerror("Call to undefined function", fn->name);
        return 0;
    }

    sl = fn->syms;
    for (nargs = 0; sl; sl = sl->next)
        nargs++;

    oldval = (double *)malloc(nargs * sizeof(double));
    newval = (double *)malloc(nargs * sizeof(double));
    if (!oldval || !newval) {
        yyerror("[ERROR] Out of space in %s", fn->name);
        return 0.0;
    }

    for (i = 0; i < nargs; i++) {
        if (!args) {
            yyerror("[ERROR] Too few args in call to %s", fn->name);
            free(oldval); free(newval);
            return 0.0;
        }

        if (args->nodetype == 'L') {
            newval[i] = eval(args->l);
            args = args->r;
        } else {
            newval[i] = eval(args);
            args = NULL;
        }
    }

    sl = fn->syms;
    for (i = 0; i < nargs; i++) {
        struct symbol *s = sl->sym;
        oldval[i] = s->value;
        s->value = newval[i];
        sl = sl->next;
    }

    free(newval);

    v = eval(fn->func);

    sl = fn->syms;
    for (i = 0; i < nargs; i++) {
        struct symbol *s = sl->sym;
        s->value = oldval[i];
        sl = sl->next;
    }

    free(oldval);
    return v;
}

void yyerror(const char *s, ...) 
{
    va_list ap;
    va_start(ap, s);

    fprintf(stderr, "[ERROR] ");
    vfprintf(stderr, s, ap);
    fprintf(stderr, "\n");
}



int main(int argc, char **argv) {
    if (argc < 2) {
        printf("[ERROR] Too few arguments\n");
        exit(1);
    }
    if (argc == 2) {
        yyin = fopen(argv[1], "a+r");
        if (!yyin) {
            printf("[ERROR] Cannot open file %s\n", argv[1]);
            exit(1);
        }

        fseek(yyin, -1, SEEK_END);
        char last_char = fgetc(yyin);
        if (last_char != '\n') {
            fputc('\n', yyin);
        }

        fseek(yyin, 0, SEEK_SET);
    }

    yyparse();
}