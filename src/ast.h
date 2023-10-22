#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

// bison and flex related declarations and definitions
extern int yylineno;
extern FILE *yyin;
int yyerror(const char *s, ...);
int yylex();
int yyparse();
#ifdef YYDEBUG
yydebug = 1;
#endif
// ===========================================================

#define NAME_SIZE 33
#define NAME_SIZE_NULL 32

typedef enum
{
    _AND,
    _OR,
    _NOT,
    _NAME,
} CONTEXT_TYPE;

typedef struct Leaf
{
    char leaf_name[NAME_SIZE];
    struct Leaf *next;
} Leaf;

typedef struct Context
{
    char first[NAME_SIZE];
    char second[NAME_SIZE];
    CONTEXT_TYPE type;
} Context;

typedef struct Plan
{
    char plan_name[NAME_SIZE];
    char trigger_name[NAME_SIZE];
    Context *context;
    struct Leaf *actions;
    struct Plan *next;
} Plan;

typedef struct Agent
{
    char agent_name[NAME_SIZE];
    struct Leaf *beliefs;
    struct Leaf *goals;
    struct Plan *plans;
    struct Agent *next;
} Agent;

Leaf *new_leaf(char name[], Leaf *next);
Context *new_context(char first[], char second[], CONTEXT_TYPE type);
Plan *new_plan(char plan_name[], char trigger_name[], Context *context, Leaf *actions, Plan *next);
Agent *new_agent(char name[], Leaf *beliefs, Leaf *goals, Plan *plans, Agent *next);
void eval(Agent *agents);
void agent_to_asl(Agent *agents);
void print_context(FILE *asl_file, Context *context);
void print_actions(FILE *asl_file, Leaf *actions);
void free_ast(void *agents);

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

Leaf *new_leaf(char name[], Leaf *next)
{
    Leaf *tmp = (Leaf *)malloc(sizeof(Leaf));
    if (!tmp)
    {
        printf("Couldn't allocate leaf %s at %i.\n", name, yylineno);
        exit(EXIT_FAILURE);
    }
    strlcpy(tmp->leaf_name, name, NAME_SIZE_NULL);
    tmp->next = next;
    return tmp;
}

Context *new_context(char first[], char second[], CONTEXT_TYPE type)
{
    Context *tmp = (Context *)malloc(sizeof(Context));
    if (!tmp)
    {
        printf("Couldn't allocate context at %i.\n", yylineno);
        exit(EXIT_FAILURE);
    }
    strlcpy(tmp->first, first, NAME_SIZE_NULL);
    if (second)
    {
        strlcpy(tmp->second, second, NAME_SIZE_NULL);
    }
    tmp->type = type;
    return tmp;
}

Plan *new_plan(char plan_name[], char trigger_name[], Context *context, Leaf *actions, Plan *next)
{
    Plan *tmp = (Plan *)malloc(sizeof(Plan));
    if (!tmp)
    {
        printf("Couldn't allocate plan %s at %i.\n", plan_name, yylineno);
        exit(EXIT_FAILURE);
    }
    strlcpy(tmp->plan_name, plan_name, NAME_SIZE_NULL);
    strlcpy(tmp->trigger_name, trigger_name, NAME_SIZE_NULL);
    tmp->context = context;
    tmp->actions = actions;
    tmp->next = next;
    return tmp;
}

Agent *new_agent(char name[], Leaf *beliefs, Leaf *goals, Plan *plans, Agent *next)
{
    Agent *tmp = (Agent *)malloc(sizeof(Agent));
    if (!tmp)
    {
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

void eval(Agent *agent)
{
    if (!agent)
    {
        // unreachable
        printf("Not valid agent.");
        exit(EXIT_FAILURE);
    }
    FILE *jason_file = fopen("../jason/main.mas2j", "w");
    if (!jason_file)
    {
        printf("Could not open mas2j file");
        exit(EXIT_FAILURE);
    }
    fprintf(jason_file, "MAS cc54a {");
    fprintf(jason_file, "\n\tagents: ");
    for (; agent; agent = agent->next)
    {
        fprintf(jason_file, "%s; ", agent->agent_name);
        agent_to_asl(agent);
    }
    fprintf(jason_file, "\n}");
}

void agent_to_asl(Agent *agent)
{
    char buffer[267];
    snprintf(buffer, 267, "../jason/%s.asl", agent->agent_name);
    FILE *asl_file = fopen(buffer, "w");
    if (!asl_file)
    {
        printf("Could not open %s asl file", agent->agent_name);
        exit(EXIT_FAILURE);
    }
    for (; agent->beliefs; agent->beliefs = agent->beliefs->next)
    {
        fprintf(asl_file, "%s.\n", agent->beliefs->leaf_name);
    }
    fprintf(asl_file, "\n");
    for (; agent->goals; agent->goals = agent->goals->next)
    {
        fprintf(asl_file, "!%s.\n", agent->goals->leaf_name);
    }
    fprintf(asl_file, "\n");
    for (; agent->plans; agent->plans = agent->plans->next)
    {
        fprintf(asl_file, "+!%s: ", agent->plans->trigger_name);
        print_context(asl_file, agent->plans->context);
        print_actions(asl_file, agent->plans->actions);
    }
}

void print_context(FILE *asl_file, Context *context)
{
    switch (context->type)
    {
    case _AND:
        fprintf(asl_file, "%s & %s\n", context->first, context->second);
        break;
    case _OR:
        fprintf(asl_file, "%s | %s\n", context->first, context->second);
        break;
    case _NOT:
        fprintf(asl_file, "not %s\n", context->first);
        break;
    case _NAME:
        fprintf(asl_file, "%s\n", context->first);
        break;
    default:
        exit(EXIT_FAILURE); // unreachable
        break;
    }
}

void print_actions(FILE *asl_file, Leaf *actions)
{
    fprintf(asl_file, "\t<- ");
    for (; actions->next; actions = actions->next)
    {
        fprintf(asl_file, "\t%s;\n", actions->leaf_name);
    }
    fprintf(asl_file, "\t%s.\n\n", actions->leaf_name);
}

void free_ast(Agent *agent)
{
    if (!agent)
        return;
}

int main(int argc, char **argv)
{
    yyin = fopen("../src/bob.nag", "r");
    yyparse();
}

int yyerror(char const *, ...)
{
    return 0;
}