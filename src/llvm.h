//Inês Martins Marçal 2019215917
//Noémia Quintano Mora Gonçalves 2019219433

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "ast.h"
#include "semantic.h"
int temp;
int strlit_count;
char * function_name;
int bool_count;
int return_found;
void generate_llvm(ast_node_t * node);
void declare_const_str(ast_node_t *node);
int count_escape(char *token);
int count_regular(char * token);
void generate_func(ast_node_t * node);
void generate_program(ast_node_t * node);
void generate_body(ast_node_t* node);
void generate_global_var(ast_node_t * node);
char * get_llvm_type(char * type);
char *get_default_value(char * type);
void generate_local_var(ast_node_t * node);
void generate_assign(ast_node_t * node);
void generate_print(ast_node_t * node);
void parse_args(ast_node_t * node);
void generate_expression(ast_node_t * node);
int generate_plus_minus_not(ast_node_t * node);
int generate_operation(ast_node_t* node);
void convert_to_float(char * value);
int generate_boolean(ast_node_t * node);
int generate_call(ast_node_t* node);
void generate_return(ast_node_t * node);
void generate_if(ast_node_t * node);
void generate_for(ast_node_t * node);