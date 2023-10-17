#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

// bison and flex related declarations and definitions
extern int yylineno;
extern FILE* yyin;
int yyerror(const char* s, ...);
int yylex();
int yyparse();
#ifdef YYDEBUG
    yydebug = 1;
#endif

typedef enum {
    _AND,
    _OR,
    _NOT,
} CONTEXT_TYPE ;

typedef struct leaf {
    char leaf_name[256];
    struct leaf *next;
} leaf;

typedef struct context {
    char first[256];
    char second[256];
    CONTEXT_TYPE type;
} context;

typedef struct plan {
    char plan_name[256];
    char trigger_name[256];
    context *contexts;
    struct leaf *actions;
    struct plan *next;
} plan;

typedef struct agent {
    char agent_name[256];
    struct leaf *beliefs;
    struct leaf *goals;
    struct plan *plans;
    struct agent *next;
} agent;

/* function by: Jonathan Leffler
https://stackoverflow.com/questions/16136437/clean-all-spaces-and-newlines-c-or-lex-bison */
static void squish_whitespace(char *TAG)
{
    int sizeOfArray = strlen(TAG);
    int i, j;
    for (i = j = 0; i < sizeOfArray; i++)
    {
        if (isspace(TAG[i]))
        {
            while (i < sizeOfArray && isspace(TAG[i]))
                i++;
            if (TAG[i] != '\0')
                TAG[j++] = ' ';
        }
        TAG[j++] = TAG[i];
    }
    TAG[j] = '\0';
}

leaf* new_leaf(char name[256], leaf* next) {
    leaf *tmp = (leaf*) malloc(sizeof(leaf));
    if (!tmp) {
        printf("Couldn't allocate leaf %s at %i.\n", name, yylineno);
        exit(EXIT_FAILURE);
    }
    strlcpy(tmp->leaf_name, name, 255);
    tmp->next = next;
    return tmp;
}

context* new_context(char first[256], char second[256], CONTEXT_TYPE type) {
    context *tmp = (context*) malloc(sizeof(context));
    if (!tmp) {
        printf("Couldn't allocate context at %i.\n", yylineno);
        exit(EXIT_FAILURE);
    }
    strlcpy(tmp->first, first, 255);
    if (second) {
        strlcpy(tmp->second, second, 255);
    }
    tmp->type = type;
    return tmp;
}

plan* new_plan(char plan_name[256], char trigger_name[256], context *contexts, leaf *actions, plan *next) {
    plan *tmp = (plan*) malloc(sizeof(plan));
        if (!tmp) {
        printf("Couldn't allocate plan %s at %i.\n", plan_name, yylineno);
        exit(EXIT_FAILURE);
    }
    strlcpy(tmp->plan_name, plan_name, 255);
    strlcpy(tmp->trigger_name, trigger_name, 255);
    tmp->contexts = contexts;
    tmp->actions = actions;
    tmp->next = next;
    return tmp;
}

agent* new_agent(char name[256], leaf *beliefs, leaf *goals, plan *plans, agent *next) {
    agent *tmp = (agent*) malloc(sizeof(agent));
    if (!tmp) {
        printf("Couldn't allocate agent %s at %i.\n", name, yylineno);
        exit(EXIT_FAILURE);
    }
    strlcpy(tmp->agent_name, name, 255);
    tmp->beliefs = beliefs;
    tmp->goals = goals;
    tmp->plans = plans;
    tmp->next = next;
    return tmp;
}

void eval(agent *agent);

int main(int argc, char **argv) {
    yyparse();
}