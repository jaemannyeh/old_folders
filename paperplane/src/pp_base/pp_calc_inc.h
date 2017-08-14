
/* A COMPACT GUIDE TO LEX & YACC by Tom Niemann */

#ifndef _PP_CALC_INC_H_
#define _PP_CALC_INC_H_

typedef enum { type_constant, type_variable, type_operator } node_enum;

typedef struct
{
    int value;
} constant_node_type;

typedef struct
{
    int index;
} variable_node_type;

typedef struct
{
    int                      oprtr;
    int                      number_of_operands;
    struct node_type_struct *operands[1];
} operator_node_type;

typedef struct node_type_struct
{
    node_enum t;

    union
    {
        constant_node_type c;
        variable_node_type v;
        operator_node_type o; 
    };
} node_type;

extern int pp_calc_variables[26];

#endif /* _PP_CALC_INC_H_ */
