//Inês Martins Marçal 2019215917
//Noémia Quintano Mora Gonçalves 2019219433

#ifndef AST_H
#define AST_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct Token
{
  char *value;
  int column, line;
} token_t;

typedef struct Vector
{
  void **data;
  int size;
  int capacity;
} vector_t;

typedef struct Node
{
  const char *id;
  const char * type;
  token_t token;
  vector_t *children;
  int temp;
} ast_node_t;

vector_t *vector();
void push_back(vector_t *vector, void *elem);

void print_ast(ast_node_t *node, int depth);
void free_ast(ast_node_t *node);

ast_node_t *ast_node(const char *id, token_t *token);
void add_child(ast_node_t *parent, ast_node_t *child);
;
void add_as_siblings(ast_node_t *parent, ast_node_t *node);
int get_size(ast_node_t *node);

#endif // AST_H
