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
// ===========================================================

#define NAME_SIZE 33
#define NAME_SIZE_NULL 32

typedef enum {
    _AND,
    _OR,
    _NOT,
    _NAME,
} CONTEXT_TYPE ;

typedef struct leaf {
    char leaf_name[NAME_SIZE];
    struct leaf *next;
} leaf;

typedef struct context {
    char first[NAME_SIZE];
    char second[NAME_SIZE];
    CONTEXT_TYPE type;
} context;

typedef struct plan {
    char plan_name[NAME_SIZE];
    char trigger_name[NAME_SIZE];
    context *contexts;
    struct leaf *actions;
    struct plan *next;
} plan;

typedef struct agent {
    char agent_name[NAME_SIZE];
    struct leaf *beliefs;
    struct leaf *goals;
    struct plan *plans;
    struct agent *next;
} agent;

leaf* new_leaf(char name[], leaf* next);
context* new_context(char first[], char second[], CONTEXT_TYPE type);
plan* new_plan(char plan_name[], char trigger_name[], context *contexts, leaf *actions, plan *next);
agent* new_agent(char name[], leaf *beliefs, leaf *goals, plan *plans, agent *next);
void eval(agent *agent);
void agent_to_asl(agent *agent);
void free_ast(void *agent);

static void squish_whitespace(char *TAG)
{
    /* function by: Jonathan Leffler
    https://stackoverflow.com/questions/16136437/clean-all-spaces-and-newlines-c-or-lex-bison */
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

leaf* new_leaf(char name[], leaf* next) {
    leaf *tmp = (leaf*) malloc(sizeof(leaf));
    if (!tmp) {
        printf("Couldn't allocate leaf %s at %i.\n", name, yylineno);
        exit(EXIT_FAILURE);
    }
    strlcpy(tmp->leaf_name, name, NAME_SIZE_NULL);
    tmp->next = next;
    return tmp;
}

context* new_context(char first[], char second[], CONTEXT_TYPE type) {
    context *tmp = (context*) malloc(sizeof(context));
    if (!tmp) {
        printf("Couldn't allocate context at %i.\n", yylineno);
        exit(EXIT_FAILURE);
    }
    strlcpy(tmp->first, first, NAME_SIZE_NULL);
    if (second) {
        strlcpy(tmp->second, second, NAME_SIZE_NULL);
    }
    tmp->type = type;
    return tmp;
}

plan* new_plan(char plan_name[], char trigger_name[], context *contexts, leaf *actions, plan *next) {
    plan *tmp = (plan*) malloc(sizeof(plan));
        if (!tmp) {
        printf("Couldn't allocate plan %s at %i.\n", plan_name, yylineno);
        exit(EXIT_FAILURE);
    }
    strlcpy(tmp->plan_name, plan_name, NAME_SIZE_NULL);
    strlcpy(tmp->trigger_name, trigger_name, NAME_SIZE_NULL);
    tmp->contexts = contexts;
    tmp->actions = actions;
    tmp->next = next;
    return tmp;
}

agent* new_agent(char name[], leaf *beliefs, leaf *goals, plan *plans, agent *next) {
    agent *tmp = (agent*) malloc(sizeof(agent));
    if (!tmp) {
        printf("Couldn't allocate agent %s at %i.\n", name, yylineno);
        exit(EXIT_FAILURE);
    }
    strlcpy(tmp->agent_name, name, NAME_SIZE_NULL);
    tmp->beliefs = beliefs;
    tmp->goals = goals;
    tmp->plans = plans;
    tmp->next = next;
    return tmp;
}

void eval(agent *agent) {
    if (!agent) {
        // unreachable
        printf("Not valid agent.");
        exit(EXIT_FAILURE);
    }
    FILE *jacamo_file = fopen("../jacamo/main.jcm", "w");
    fprintf(jacamo_file, "mas cc54a {");
    for(; agent; agent = agent->next) {
        fprintf(jacamo_file, "\n\tagent %s {}", agent->agent_name);
        agent_to_asl(agent);
    }
    fprintf(jacamo_file, "\n}");
}

void agent_to_asl(agent *agent) {
    char buffer[267];
    snprintf(buffer, 267, "../jacamo/%s.asl", agent->agent_name);
    FILE *asl_file = fopen(buffer, "w");
    for(; agent->beliefs; agent->beliefs = agent->beliefs->next) {
        fprintf(asl_file, "%s.\n", agent->beliefs->leaf_name);
    }
    fprintf(asl_file, "\n");
    for(; agent->goals; agent->goals = agent->goals->next) {
        fprintf(asl_file, "!%s.\n", agent->goals->leaf_name);
    }
    fprintf(asl_file, "\n");
    for(; agent->plans; agent->plans = agent->plans->next) {
        fprintf(asl_file, "+!%s: true <- .print(\"hello\").", agent->plans->plan_name);
    }
}

void free_ast(agent *agent) {
    if(!agent)
        return;
}

int main(int argc, char **argv) {
    yyin = fopen("../src/bob.nag", "r");
    yyparse();
}

int yyerror(char const*, ...) {
    return 0;
}