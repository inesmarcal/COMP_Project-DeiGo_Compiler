//Inês Martins Marçal 2019215917
//Noémia Quintano Mora Gonçalves 2019219433

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "ast.h"


typedef struct {  
    char *id;
    int is_param;
    int is_func;
    int is_used;
    vector_t * parameters;
    char * type;
    int temp;
     

} symbol_t ;

typedef struct {
    char * id; 
    int is_defined;
    vector_t * symbols, * parameters;
    
} symbol_table_t;

vector_t * symbol_table;  
int errors_ ; 

symbol_table_t * create_symbol_table(char * name);
symbol_t *create_symbol(char * id, int is_param, char * type,vector_t * param);
void print_symbol_table_vector(vector_t * symbol_table_vector);
void print_symbol_table(symbol_table_t * symbol_table);
void print_symbol(symbol_t * symbol);
void add_symbol(symbol_table_t * symbol_table, symbol_t * symbol);
void add_table(symbol_table_t * symbol_table);
void check_semantics(ast_node_t* node);
void function_header(ast_node_t * node);
void add_param(symbol_table_t * table, char * type );
void add_function_to_global(symbol_table_t * table);
void function_body(ast_node_t * node, symbol_table_t * function, int print_error_id);
char * verify_type(const char * type);
void add_type_to_node(ast_node_t * node, char * type );
symbol_t * search_table(symbol_table_t * table, char * name,int global_search);
char * concatenate_strings(char * str1,char * str2);
symbol_table_t * search_function_table(const char * name);
void print_error(ast_node_t* node, int type);
ast_node_t * search_node(char * name, ast_node_t * node);
