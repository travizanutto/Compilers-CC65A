#include "ast.h"

Leaf *new_leaf(char name[], Leaf *next)
{
    Leaf *tmp = (Leaf *)malloc(sizeof(Leaf));
    if (!tmp)
    {
        printf("Couldn't allocate leaf %s at %i.\n", name, yylineno);
        exit(EXIT_FAILURE);
    }
    strncpy(tmp->leaf_name, name, NAME_SIZE);
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
    strncpy(tmp->first, first, NAME_SIZE);
    if (second)
    {
        strncpy(tmp->second, second, NAME_SIZE);
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
    strncpy(tmp->plan_name, plan_name, NAME_SIZE);
    strncpy(tmp->trigger_name, trigger_name, NAME_SIZE);
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
    strncpy(tmp->agent_name, name, NAME_SIZE);
    tmp->beliefs = beliefs;
    tmp->goals = goals;
    tmp->plans = plans;
    tmp->next = next;
    return tmp;
}

void eval(Agent *agent)
{
    fclose(yyin);
    if (!agent)
    {
        // unreachable
        printf("Not valid agent.");
        exit(EXIT_FAILURE);
    }
    FILE *jason_file = fopen("./jason/main.mas2j", "w");
    if (!jason_file)
    {
        printf("Could not open mas2j file");
        exit(EXIT_FAILURE);
    }
    fprintf(jason_file, "MAS cc64a {");
    fprintf(jason_file, "\n\tagents: ");
    Agent *agent_tmp = agent;
    for (; agent; agent = agent->next)
    {
        fprintf(jason_file, "%s; ", agent->agent_name);
        agent_to_asl(agent);
        free_ast(agent);
    }
    free_agents(agent_tmp);
    fprintf(jason_file, "\n}");
    fclose(jason_file);
}

void agent_to_asl(Agent *agent)
{
    char buffer[64];
    char jason_path[] = "./jason/";
    int string_len = strlen(jason_path) + NAME_SIZE + strlen(".asl");
    snprintf(buffer, string_len, "%s%s.asl", jason_path, agent->agent_name);
    FILE *asl_file = fopen(buffer, "w");
    if (!asl_file)
    {
        printf("Could not open %s asl file", agent->agent_name);
        exit(EXIT_FAILURE);
    }
    for (Leaf *beliefs = agent->beliefs; beliefs; beliefs = beliefs->next)
    {
        fprintf(asl_file, "%s.\n", beliefs->leaf_name);
    }
    fprintf(asl_file, "\n");
    for (Leaf *goals = agent->goals; goals; goals = goals->next)
    {
        fprintf(asl_file, "!%s.\n", goals->leaf_name);
    }
    fprintf(asl_file, "\n");
    for (Plan *plans = agent->plans; plans; plans = plans->next)
    {
        fprintf(asl_file, "+!%s: ", plans->trigger_name);
        print_context(asl_file, plans->context);
        print_actions(asl_file, plans->actions);
    }
    fclose(asl_file);
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
        printf("bad context type"); // unreachable
        exit(EXIT_FAILURE);
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

void *free_ast(Agent *agent)
{
    if (!agent)
        return NULL;
    agent->beliefs = free_list(agent->beliefs);
    agent->goals = free_list(agent->goals);
    agent->plans = free_plans(agent->plans);
    return NULL;
}

void *free_list(Leaf *list)
{
    if (!list)
        return NULL;
    if (list->next)
        list->next = free_list(list->next);
    free(list);
    return NULL;
}

void *free_plans(Plan *plans)
{
    if (!plans)
        return NULL;
    if (plans->next)
        plans->next = free_plans(plans->next);
    plans->actions = free_list(plans->actions);
    free(plans->context);
    free(plans);
    return NULL;
}

void *free_agents(Agent *agent)
{
    if (!agent)
        return NULL;
    if (agent->next)
        agent->next = (agent->next);
    free(agent);
    return NULL;
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("Usage ./cc64a <FILENAME>");
        exit(EXIT_FAILURE);
    }
    yyin = fopen(argv[1], "r");
    if (!yyin)
    {
        printf("Bad nag file");
        exit(EXIT_FAILURE);
    }
    yyparse();
}

int yyerror(char const *, ...)
{
    return 0;
}
