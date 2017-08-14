
%{
/* A COMPACT GUIDE TO LEX & YACC by Tom Niemann */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h> /* offsetof */
#include "pp_calc_inc.h"

static node_type *alloc_constant(int value);
static node_type *alloc_variable(int index);
static node_type *alloc_operator(int oprtr,int number_of_operands,...);
static void       free_node(node_type *node);
static int        exec_node(node_type *node);

int pp_calc_variables[26];
%}

%union {
    int        value;
    char       index;
    node_type *node;
};

%token <value> INTEGER
%token <index> VARIABLE
%token WHILE IF PRINT
%token DELAY EXIT
%nonassoc IFX
%nonassoc ELSE

%left GE LE EQ NE '>' '<'
%left '+' '-'
%left '*' '/'
%nonassoc UMINUS

%type <node> statement expression statement_list
    
%%

program:
        function { exit(0); }
    ;
    
function:
        function statement { exec_node($2); free_node($2); }
    |   /* NULL */
    ;
    
statement:
        ';'                                            { $$ = alloc_operator(';',2,NULL,NULL); }
    |   expression ';'                                 { $$ = $1; }
    |   PRINT expression ';'                           { $$ = alloc_operator(PRINT,1,$2); }
    |   DELAY expression ';'                           { $$ = alloc_operator(DELAY,1,$2); }
    |   EXIT expression ';'                            { $$ = alloc_operator(EXIT,1,$2); }   
    |   VARIABLE '=' expression ';'                    { $$ = alloc_operator('=',2,alloc_variable($1),$3); }
    |   WHILE '(' expression ')' statement             { $$ = alloc_operator(WHILE,2,$3,$5); }
    |   IF '(' expression ')' statement %prec IFX      { $$ = alloc_operator(IF,2,$3,$5); }
    |   IF '(' expression ')' statement ELSE statement { $$ = alloc_operator(IF,3,$3,$5,$7); }
    |   '{' statement_list '}'                         { $$ = $2; }
    ;            

statement_list:
        statement                { $$ = $1; }
    |   statement_list statement { $$ = alloc_operator(';',2,$1,$2); }
    ;
    
expression:
        INTEGER                     { $$ = alloc_constant($1); }
    |   VARIABLE                    { $$ = alloc_variable($1); }
    |   '-' expression %prec UMINUS { $$ = alloc_operator(UMINUS,1,$2); }
    |   expression '+' expression   { $$ = alloc_operator('+',2,$1,$3); }
    |   expression '-' expression   { $$ = alloc_operator('-',2,$1,$3); }
    |   expression '*' expression   { $$ = alloc_operator('*',2,$1,$3); }
    |   expression '/' expression   { $$ = alloc_operator('/',2,$1,$3); }
    |   expression '<' expression   { $$ = alloc_operator('<',2,$1,$3); }
    |   expression '>' expression   { $$ = alloc_operator('>',2,$1,$3); }
    |   expression GE expression    { $$ = alloc_operator(GE,2,$1,$3); }
    |   expression LE expression    { $$ = alloc_operator(LE,2,$1,$3); } 
    |   expression NE expression    { $$ = alloc_operator(NE,2,$1,$3); } 
    |   expression EQ expression    { $$ = alloc_operator(EQ,2,$1,$3); }                      
    |   '(' expression ')'          { $$ = $2; }
    ;
                            
%%

static node_type *alloc_constant(int value)
{
    node_type *node;
    size_t     node_size;
    
    node_size = offsetof(struct node_type_struct,c) + sizeof(constant_node_type);
    node = malloc(node_size);
    
    node->t       = type_constant;
    node->c.value = value;
    
    return node;
}

static node_type *alloc_variable(int index)
{
    node_type *node;
    size_t     node_size;

    node_size = offsetof(struct node_type_struct,c) + sizeof(variable_node_type);
    node = malloc(node_size);
    
    node->t       = type_variable;
    node->v.index = index;
        
    return node;
}

static node_type *alloc_operator(int oprtr,int number_of_operands,...)
{
    va_list    ap;
    node_type *node;
    size_t     node_size;
    int        ix;
    
    node_size = offsetof(struct node_type_struct,c) + sizeof(operator_node_type) +
                (number_of_operands-1)*sizeof(node_type*);
    node = malloc(node_size);
                    
    node->t                    = type_operator;
    node->o.oprtr              = oprtr;
    node->o.number_of_operands = number_of_operands;
    
    va_start(ap,number_of_operands);
    for (ix=0; ix<number_of_operands; ix++)
        node->o.operands[ix] = va_arg(ap,node_type*);
    va_end(ap);
    
    return node;
}

static void free_node(node_type *node)
{
    int ix;
    
    if (!node)
        return;
        
    if (node->t == type_operator)
    {
        for (ix=0; ix<node->o.number_of_operands; ix++)
        {
            free_node(node->o.operands[ix]);
        }
    }
    
    free(node);        
}

static int exec_node(node_type *node)
{
    if (!node)
        return 0;
        
    if (node->t == type_constant)
    {
        return node->c.value;
    }
    else if (node->t == type_variable)
    {
        return pp_calc_variables[node->v.index];
    }
    else if (node->t == type_operator)
    {
        switch(node->o.oprtr)
        {
        case WHILE:
            while (exec_node(node->o.operands[0]))
                exec_node(node->o.operands[1]);                            
            return 0;
        case IF:
            if (exec_node(node->o.operands[0]))
                exec_node(node->o.operands[1]);
            else if (node->o.number_of_operands > 2)
                exec_node(node->o.operands[2]);
            return 0; 
        case PRINT: 
            printf("%d\n",exec_node(node->o.operands[0])); 
            return 0;
            
        case DELAY:
            sleep(exec_node(node->o.operands[0]));
            return 0;            
        case EXIT:
            exit(exec_node(node->o.operands[0]));
            return 0;   
                        
        case ';': 
            exec_node(node->o.operands[0]); 
            return exec_node(node->o.operands[1]);
        case '=': 
            return pp_calc_variables[node->o.operands[0]->v.index] = exec_node(node->o.operands[1]);
        case UMINUS: 
            return -exec_node(node->o.operands[0]);
        case '+': 
            return exec_node(node->o.operands[0]) + exec_node(node->o.operands[1]);
        case '-': 
            return exec_node(node->o.operands[0]) - exec_node(node->o.operands[1]);
        case '*': 
            return exec_node(node->o.operands[0]) * exec_node(node->o.operands[1]);
        case '/': 
            return exec_node(node->o.operands[0]) / exec_node(node->o.operands[1]);
        case '<': 
            return exec_node(node->o.operands[0]) < exec_node(node->o.operands[1]);
        case '>': 
            return exec_node(node->o.operands[0]) > exec_node(node->o.operands[1]);
        case GE: 
            return exec_node(node->o.operands[0]) >= exec_node(node->o.operands[1]);
        case LE: 
            return exec_node(node->o.operands[0]) <= exec_node(node->o.operands[1]);
        case NE: 
            return exec_node(node->o.operands[0]) != exec_node(node->o.operands[1]);
        case EQ: 
            return exec_node(node->o.operands[0]) == exec_node(node->o.operands[1]);
        default:
            return 0;                              
        }
    }
    else
    {
    }
        
    return 0;   
}

void pp_calc()
{
    yyparse();
}

void pp_calc_init()
{
}


