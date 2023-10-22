#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// bison and flex related declarations and definitions
extern int yylineno;
extern FILE *yyin;
int yyerror(const char *s, ...);
int yylex();
int yyparse();

#define NAME_SIZE 33

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
void free_ast(Agent *agent);
void free_list(Leaf *list);
void free_plans(Plan *plans);
void free_agents(Agent *agent);