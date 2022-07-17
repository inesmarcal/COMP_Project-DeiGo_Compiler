//Inês Martins Marçal 2019215917
//Noémia Quintano Mora Gonçalves 2019219433



#include "ast.h"

vector_t *vector()
{
  vector_t *vector = (vector_t *)malloc(sizeof(vector_t));
  vector->size = 0;
  vector->capacity = 1;
  vector->data = (void **)malloc(sizeof(void *) * vector->capacity);
  return vector;
}

void free_vector(vector_t *vector)
{
  if (vector != NULL)
  {
    free(vector->data);
    free(vector);
  }
}

ast_node_t *ast_node(const char *id, token_t *token)
{
  ast_node_t *node = (ast_node_t *)malloc(sizeof(ast_node_t));
  node->children = vector();
  node->id = id;
  node->temp = -1;
  if (token){
    node->token=*token;
  }else{
    node->token.value=NULL;
  }
  
  node->type = NULL;
  return node;
}

void free_ast_node(ast_node_t *node)
{
  if (node != NULL)
  {
    free_vector(node->children);
    if (node->token.value!= NULL)
    {
      free(node->token.value);
    }
    free(node);
  }
}

void push_back(vector_t *vector, void *elem)
{
  if (!vector || !elem)
    return;
  if (vector->size == vector->capacity)
  {
    vector->capacity *= 2;
    vector->data =
        (void **)realloc(vector->data, sizeof(void *) * vector->capacity);
  }
  vector->data[vector->size++] = elem;
}

void add_child(ast_node_t *parent, ast_node_t *child)
{
  if (!parent || !child)
    return;
  push_back(parent->children, child);
}

void add_as_siblings(ast_node_t *parent, ast_node_t *node)
{
  if (!parent || !node)
    return;
  for (int i = 0; i < node->children->size; ++i)
  {
    add_child(parent, (ast_node_t *)node->children->data[i]);
    node->children->data[i] = NULL;
  }
  free_ast_node(node);
}

void print_node(ast_node_t *node, int depth)
{
  for (int i = 0; i < depth; ++i)
  {
    printf("..");
  }
  printf("%s", node->id);
  if (node->token.value != NULL)
  {
    printf("(%s)", node->token.value);
  }
  if (node->type && strcmp(node->type,"none"))
    printf(" - %s",node->type);
  printf("\n");
}

void print_ast(ast_node_t *node, int depth)
{
  if (node != NULL)
  {
    print_node(node, depth);
    ast_node_t **children = (ast_node_t **)node->children->data;
    for (int i = 0; i < node->children->size; ++i)
    {
      print_ast((ast_node_t *)children[i], depth + 1);
    }
  }
}

void free_ast(ast_node_t *node)
{
  if (node != NULL)
  {
    ast_node_t **children = (ast_node_t **)node->children->data;
    for (int i = 0; i < node->children->size; ++i)
    {
      free_ast((ast_node_t *)children[i]);
    }
    free_ast_node(node);
  }
}

int get_size(ast_node_t *node)
{
  if (node)
  {
    return node->children->size;
  }
  return 0;
}
