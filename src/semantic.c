//Inês Martins Marçal 2019215917
//Noémia Quintano Mora Gonçalves 2019219433

#include "semantic.h"

symbol_table_t *create_symbol_table(char *name)
{
    symbol_table_t *symbol_table = (symbol_table_t *)malloc(sizeof(symbol_table_t));
    symbol_table->symbols = vector();
    symbol_table->parameters = vector();
    symbol_table->id = name;
    return symbol_table;
}

void add_symbol(symbol_table_t *symbol_table, symbol_t *symbol)
{
    if (!symbol_table || !symbol)
        return;
    push_back(symbol_table->symbols, symbol);
}

void add_table(symbol_table_t *table)
{
    if (!table)
        return;
    push_back(symbol_table, table);
}

void print_symbol_table_vector(vector_t *symbol_table_vector)
{
    if (!symbol_table_vector)
        return;
    for (int i = 0; i < symbol_table_vector->size; i++)
    {
        print_symbol_table((symbol_table_t *)symbol_table_vector->data[i]);
        printf("\n");
    }
}

void print_symbol_table(symbol_table_t *symbol_table)
{
    if (!symbol_table)
    {
        return;
    }
    if (!strcmp(symbol_table->id, "Global"))
        printf("===== %s Symbol Table =====\n", symbol_table->id);
    else
    {
        printf("===== Function %s(", symbol_table->id);
        if (symbol_table->parameters->size == 1)
        {
            printf("%s", (char *)symbol_table->parameters->data[0]);
        }
        else
            for (int i = 0; i < symbol_table->parameters->size; i++)
            {
                if(strcmp((char *)symbol_table->parameters->data[i],"none")){
                    printf("%s", (char *)symbol_table->parameters->data[i]);
                    if (i != symbol_table->parameters->size - 1)
                        printf(",");
                }
                
            }
        printf(") Symbol Table =====\n");
    }

    for (int i = 0; i < symbol_table->symbols->size; i++)
    {
        print_symbol((symbol_t *)symbol_table->symbols->data[i]);
    }
}

void print_symbol(symbol_t *symbol)
{
    if (!symbol)
        return;

    printf("%s\t", symbol->id);
    if (symbol->parameters)
    {
        printf("(");
        if (symbol->parameters->size == 1)
        {
            printf("%s", (char *)symbol->parameters->data[0]);
        }
        else
            for (int i = 0; i < symbol->parameters->size; i++)
            {
                if(strcmp((char *)symbol->parameters->data[i],"none")){
                    printf("%s", (char *)symbol->parameters->data[i]);
                    if (i != symbol->parameters->size - 1)
                        printf(",");
                }
            }
        printf(")");
    }

    printf("\t%s", symbol->type);

    if (symbol->is_param)
    {
        printf("\tparam");
    }
    printf("\n");
}

void check_semantics(ast_node_t *node)
{

    if (!strcmp("Program", node->id))
    {
        add_table(create_symbol_table("Global"));
        for (int i = 0; i < node->children->size; i++)
        {
            if (!strcmp("FuncDecl", ((ast_node_t *)node->children->data[i])->id))
            {
                if( search_table((symbol_table_t *)symbol_table->data[0],((ast_node_t *)(((ast_node_t *)((ast_node_t *)node->children->data[i])->children->data[0])->children->data[0]))->token.value,0)){
                    print_error((ast_node_t *)(((ast_node_t *)((ast_node_t *)node->children->data[i])->children->data[0])->children->data[0]),0);
                   
                }else{
                    function_header((ast_node_t *)((ast_node_t *)node->children->data[i])->children->data[0]);
                }
                
            }
            else
            {
                if (!strcmp("VarDecl", ((ast_node_t *)node->children->data[i])->id))
                {
                   if(search_table((symbol_table_t *)symbol_table->data[0], (char *)(((ast_node_t *)(((ast_node_t *)node->children->data[i])->children->data[1]))->token.value),1)){
                       print_error(((ast_node_t *)(((ast_node_t *)node->children->data[i])->children->data[1])), 0);
    
                   }else{
                        add_symbol((symbol_table_t *)symbol_table->data[0], create_symbol( (char *)(((ast_node_t *)(((ast_node_t *)node->children->data[i])->children->data[1]))->token.value), 0,
                                                                                      verify_type(((ast_node_t *)(((ast_node_t *)node->children->data[i])->children->data[0]))->id), NULL));
                   }

                   
                }
            }
        }
    }
    if (!strcmp("FuncDecl", node->id))
    {
        symbol_table_t  * function = search_function_table( ((ast_node_t *)(((ast_node_t *)(node->children->data[0]))->children->data[0]))->token.value);
        if (function){
            if (!function->is_defined)
                function_body((ast_node_t *)node->children->data[1], function ,1);
            function->is_defined=1;  
            
            //verificar nao usados
            ast_node_t * funcbody = (ast_node_t *) node->children->data[1];
            for(int i = 0;i < function->symbols->size;i++){
                 if(!((symbol_t *)(function->symbols->data[i]))->is_used && !((symbol_t *)(function->symbols->data[i]))->is_param && strcmp(((symbol_t *)(function->symbols->data[i]))->id,"return")){
                      for (int j = 0;j<funcbody->children->size;j++){
                        if(!strcmp("VarDecl",((ast_node_t*)(funcbody->children->data[j]))->id) &&
                           !strcmp(((symbol_t *)(function->symbols->data[i]))->id,((ast_node_t *)(((ast_node_t*)(funcbody->children->data[j]))->children->data[1]))->token.value)){
                            print_error(((ast_node_t*)(funcbody->children->data[j]))->children->data[1],5);
                        }  
                 }  
                } 
                    
            }  
        }
        return;
    }
    

    if (node)
    {
        for (int i = 0; i < node->children->size; ++i)
        {
            check_semantics((ast_node_t *)node->children->data[i]);
        }
    }
}

void function_header(ast_node_t *node)
{
    symbol_t *return_symbol;
    int param_node;
    //nome
    symbol_table_t *function = create_symbol_table(((ast_node_t *)(node->children->data[0]))->token.value);
    
    //return type
    if (node->children->size == 2)
    {
        return_symbol = create_symbol("return", 0, "none", NULL);
        param_node = 1;
    }
    else
    {
        return_symbol = create_symbol("return", 0, verify_type(((ast_node_t *)(node->children->data[1]))->id), NULL);
        param_node = 2;
    }
    add_symbol(function, return_symbol);
    //parameters
    ast_node_t *id;
    char *type;
    for (int i = 0; i < ((ast_node_t *)(node->children->data[param_node]))->children->size; i++)
    {
        id = (ast_node_t *)((ast_node_t *)((ast_node_t *)(node->children->data[param_node]))->children->data[i])->children->data[1];
        type = (char *)((ast_node_t *)((ast_node_t *)((ast_node_t *)(node->children->data[param_node]))->children->data[i])->children->data[0])->id;
        if( search_table(function,id->token.value,0)){
            print_error(id,0);
        }
        add_param(function, verify_type(type));
        add_symbol(function, create_symbol(id->token.value, 1, verify_type(type), NULL));
    }
    function->is_defined=0;
    add_table(function);
    add_function_to_global(function);
    
}

symbol_t *create_symbol(char *id, int is_param, char *type, vector_t *param)
{
    symbol_t *symbol = (symbol_t *)malloc(sizeof(symbol_t));
    symbol->id = id;
    symbol->is_param = is_param;
    symbol->type = type;
    symbol->is_func = 0;
    symbol->parameters = param;
    symbol->is_used=0;
    symbol->temp=-1;
    return symbol;
}

void add_param(symbol_table_t *table, char *type)
{
    if (!table || !type)
        return;
    push_back(table->parameters, type);
}

void add_function_to_global(symbol_table_t *table)
{
    symbol_t *symbol = create_symbol(table->id, 0, ((symbol_t *)table->symbols->data[0])->type, table->parameters);
    symbol->is_func = 1;
    add_symbol((symbol_table_t *)symbol_table->data[0], symbol);
}

void function_body(ast_node_t *node, symbol_table_t *function,int print_error_id)
{

    if (!node)
        return;

    if (!strcmp(node->id, "VarDecl"))
    {
        if(search_table(function,(char *)((ast_node_t *)(node->children->data[1]))->token.value,0)){
            print_error((ast_node_t *)(node->children->data[1]),0);
        }else{ 
        add_symbol(function, create_symbol((char *)((ast_node_t *)(node->children->data[1]))->token.value, 0,
                                           verify_type(((ast_node_t *)node->children->data[0])->id), NULL));
        }
        return;
    }

    if (!strcmp(node->id, "Call"))
    {

        symbol_t *var2 = search_table(function, ((ast_node_t *)(node->children->data[0]))->token.value, 1);
        if (var2)
        {
            if (var2->is_func)
            {

                if (var2->parameters->size == node->children->size - 1)
                {
                    for (int i = 0; i < node->children->size; ++i)
                    {
                        function_body((ast_node_t *)node->children->data[i], function, 0);
                    }
                    int erro = 0;
                    for (int i = 0; i < var2->parameters->size; i++)
                    {

                        if (((ast_node_t *)(node->children->data[i + 1])))
                        {

                            if (strcmp((char *)(var2->parameters->data[i]), ((ast_node_t *)(node->children->data[i + 1]))->type))
                            {
                                erro = 1;
                                
                            }
                        }
                    }
                    if (erro)
                    {
                        print_error((ast_node_t *)node, 3);
                        add_type_to_node(node, "undef");
                    }
                    else
                    {
                        add_type_to_node(node, var2->type);
                    }
                }
                else
                {

                    for (int i = 0; i < node->children->size; ++i)
                    {
                        function_body((ast_node_t *)node->children->data[i], function, 0);
                    }
                    print_error((ast_node_t *)node, 3);
                    add_type_to_node(node, "undef");
                }
            }
        }
        else
        {
            for (int i = 0; i < node->children->size; ++i)
            {
                function_body((ast_node_t *)node->children->data[i], function, 0);
            }
            print_error((ast_node_t *)node, 3);
            add_type_to_node(node, "undef");
        }
        return;
    }

    for (int i = 0; i < node->children->size; ++i)
    {
        function_body((ast_node_t *)node->children->data[i], function,1);
    }

    if (!strcmp(node->id, "IntLit"))
    {
        add_type_to_node(node, "int");
    }

    if (!strcmp(node->id, "RealLit"))
    {
        add_type_to_node(node, "float32");
    }

    if (!strcmp(node->id, "Id"))
    {
        symbol_t *var = search_table(function, node->token.value,1);
        if (var)
        {
            var->is_used=1;
            if (var->is_func)
            {   //caso de o id ser de uma funcao mas estiver a ser utilizado como variavel error
                if(print_error_id){
                    add_type_to_node(node, "undef");
                    print_error(node,1);
                    return;
                   
                }
                char *param = "(";
                if (var->parameters->size == 1)
                {
                    param = concatenate_strings(param, var->parameters->data[0]);
                }
                else{
                    for (int i = 0; i < var->parameters->size; i++)
                    {
                        param = concatenate_strings(param, var->parameters->data[i]);
                        if (i != var->parameters->size - 1)
                            param = concatenate_strings(param, ",");
                    }
                }
                    
                param = concatenate_strings(param, ")");
                add_type_to_node(node, param);
            }else{
               
                add_type_to_node(node, var->type);
            }

            
        }else{
            add_type_to_node(node, "undef");
            if(print_error_id)
                print_error(node,1);
        }
        return;
    }

    if (!strcmp(node->id, "Assign"))
    {
            if (!strcmp(((ast_node_t *)(node->children->data[0]))->type, ((ast_node_t *)(node->children->data[1]))->type) && strcmp(((ast_node_t *)(node->children->data[1]))->type,"undef"))
            {
                add_type_to_node(node, (char *)((ast_node_t *)(node->children->data[0]))->type);
            }
            else
            {
                print_error(node,4);
                add_type_to_node(node, "undef");
            }
        
        return;
    }

    if (!strcmp(node->id, "Or"))
    {
        
        if (strcmp(((ast_node_t *)(node->children->data[0]))->type, ((ast_node_t *)(node->children->data[1]))->type) || strcmp(((ast_node_t *)(node->children->data[0]))->type,"bool") )
            print_error(node,4);
            
        add_type_to_node(node, "bool"); 
        
        return;
    }

    if (!strcmp(node->id, "And"))
    {
        if (strcmp(((ast_node_t *)(node->children->data[0]))->type, ((ast_node_t *)(node->children->data[1]))->type)|| strcmp(((ast_node_t *)(node->children->data[0]))->type,"bool") )
            print_error(node,4);
            
        add_type_to_node(node, "bool"); 
        
        return;
    }

    if (!strcmp(node->id, "Not"))
    {
        if (strcmp(((ast_node_t *)(node->children->data[0]))->type,"bool")){
            add_type_to_node(node, "bool");
            print_error(node,2);
        }else{
            add_type_to_node(node, "bool");
        }
        
        return;
    }

    if (!strcmp(node->id, "Mod"))
    {
        if (((ast_node_t *)(node->children->data[0]))->type && ((ast_node_t *)(node->children->data[1]))->type)
        {
            if (!strcmp(((ast_node_t *)(node->children->data[0]))->type, ((ast_node_t *)(node->children->data[1]))->type) && !strcmp(((ast_node_t *)(node->children->data[1]))->type, "int"))
            {
                add_type_to_node(node, (char *)((ast_node_t *)(node->children->data[0]))->type);
            }
            else
            {
                print_error(node,4);
                add_type_to_node(node, "undef");
            }
        }
        return;
    }

    if (!strcmp(node->id, "Mul"))
    {
        if (((ast_node_t *)(node->children->data[0]))->type && ((ast_node_t *)(node->children->data[1]))->type)
        {
            if (!strcmp(((ast_node_t *)(node->children->data[0]))->type, ((ast_node_t *)(node->children->data[1]))->type) && (!strcmp("int",((ast_node_t *)(node->children->data[0]))->type) ||  !strcmp("float32",((ast_node_t *)(node->children->data[0]))->type)))
            {
                add_type_to_node(node, (char *)((ast_node_t *)(node->children->data[0]))->type);
            }
            else
            {
                print_error(node,4);
                add_type_to_node(node, "undef");
            }
        }
        else
        {
            add_type_to_node(node, "undef");
        }
        return;
    }
    if (!strcmp(node->id, "Div"))
    {
        if (((ast_node_t *)(node->children->data[0]))->type && ((ast_node_t *)(node->children->data[1]))->type)
        {
            if (!strcmp(((ast_node_t *)(node->children->data[0]))->type, ((ast_node_t *)(node->children->data[1]))->type) && (!strcmp("int",((ast_node_t *)(node->children->data[0]))->type) ||  !strcmp("float32",((ast_node_t *)(node->children->data[0]))->type)))
            {
                add_type_to_node(node, (char *)((ast_node_t *)(node->children->data[0]))->type);
            }
            else
            {
                 print_error(node,4);
                add_type_to_node(node, "undef");
            }
        }
        else
        {
            add_type_to_node(node, "undef");
        }
        return;
    }
    if (!strcmp(node->id, "Add"))
    {
        if (((ast_node_t *)(node->children->data[0]))->type && ((ast_node_t *)(node->children->data[1]))->type)
        {
            if (!strcmp(((ast_node_t *)(node->children->data[0]))->type, ((ast_node_t *)(node->children->data[1]))->type) && (!strcmp("int",((ast_node_t *)(node->children->data[0]))->type) || !strcmp("string",((ast_node_t *)(node->children->data[0]))->type) || !strcmp("float32",((ast_node_t *)(node->children->data[0]))->type)))
            {
                add_type_to_node(node, (char *)((ast_node_t *)(node->children->data[0]))->type);
            }
            else
            {
                print_error(node,4);
                add_type_to_node(node, "undef");
            }
        }
        else
        {
            add_type_to_node(node, "undef");
        }
        return;
    }
    if (!strcmp(node->id, "Sub"))
    {
        if (((ast_node_t *)(node->children->data[0]))->type && ((ast_node_t *)(node->children->data[1]))->type)
        {
            if (!strcmp(((ast_node_t *)(node->children->data[0]))->type, ((ast_node_t *)(node->children->data[1]))->type) && (!strcmp("int",((ast_node_t *)(node->children->data[0]))->type) ||  !strcmp("float32",((ast_node_t *)(node->children->data[0]))->type)))
            {
                add_type_to_node(node, (char *)((ast_node_t *)(node->children->data[0]))->type);
            }
            else
            {
                print_error(node,4);
                add_type_to_node(node, "undef");
            }
        }
        else
        {
            add_type_to_node(node, "undef");
        }
        return;
    }

    if (!strcmp(node->id, "Plus"))
    {
        if (((ast_node_t *)(node->children->data[0]))->type && (!strcmp(((ast_node_t *)(node->children->data[0]))->type,"int")||!strcmp(((ast_node_t *)(node->children->data[0]))->type,"float32")))
        {
            add_type_to_node(node, (char *)((ast_node_t *)(node->children->data[0]))->type);
        }
        else
        {
            print_error(node,2),
            add_type_to_node(node, "undef");
        }
        return;
    }

    if (!strcmp(node->id, "Minus"))
    {
        if (((ast_node_t *)(node->children->data[0]))->type && (!strcmp(((ast_node_t *)(node->children->data[0]))->type,"int")||!strcmp(((ast_node_t *)(node->children->data[0]))->type,"float32")))
        {
            add_type_to_node(node, (char *)((ast_node_t *)(node->children->data[0]))->type);
        }
        else
        {
            print_error(node,2),
            add_type_to_node(node, "undef");
        }
        return;
    }

    if (!strcmp(node->id, "Eq"))
    {
        if (((ast_node_t *)(node->children->data[0]))->type && ((ast_node_t *)(node->children->data[1]))->type)
        {
            if (!strcmp(((ast_node_t *)(node->children->data[0]))->type, ((ast_node_t *)(node->children->data[1]))->type) && (!strcmp("int",((ast_node_t *)(node->children->data[0]))->type) || !strcmp("string",((ast_node_t *)(node->children->data[0]))->type) || !strcmp("float32",((ast_node_t *)(node->children->data[0]))->type)|| !strcmp("bool",((ast_node_t *)(node->children->data[0]))->type)))
            {
                add_type_to_node(node, "bool");
            }
            else
            {
                  print_error(node,4);
                add_type_to_node(node, "bool");
            }
        }
        else
        {
            add_type_to_node(node, "bool");
        }
        return;
    }

    if (!strcmp(node->id, "Ne"))
    {
        if (((ast_node_t *)(node->children->data[0]))->type && ((ast_node_t *)(node->children->data[1]))->type)
        {
            if (!strcmp(((ast_node_t *)(node->children->data[0]))->type, ((ast_node_t *)(node->children->data[1]))->type) && (!strcmp("int",((ast_node_t *)(node->children->data[0]))->type) || !strcmp("string",((ast_node_t *)(node->children->data[0]))->type) || !strcmp("float32",((ast_node_t *)(node->children->data[0]))->type)|| !strcmp("bool",((ast_node_t *)(node->children->data[0]))->type))) 
            {
                add_type_to_node(node, "bool");
            }
            else
            {
                print_error(node,4);
                add_type_to_node(node, "bool");
            }
        }
        else
        {
            add_type_to_node(node, "bool");
        }
        return;
    }

      if (!strcmp(node->id, "Le"))
    {
        if (((ast_node_t *)(node->children->data[0]))->type && ((ast_node_t *)(node->children->data[1]))->type)
        {
            if (!strcmp(((ast_node_t *)(node->children->data[0]))->type, ((ast_node_t *)(node->children->data[1]))->type) && (!strcmp("int",((ast_node_t *)(node->children->data[0]))->type) || !strcmp("string",((ast_node_t *)(node->children->data[0]))->type) || !strcmp("float32",((ast_node_t *)(node->children->data[0]))->type)))
            {
                add_type_to_node(node, "bool");
            }
            else
            {
                print_error(node,4);
                add_type_to_node(node, "bool");
            }
        }
        else
        {
            add_type_to_node(node, "bool");
        }
        return;
    }

      if (!strcmp(node->id, "Lt"))
    {
        if (((ast_node_t *)(node->children->data[0]))->type && ((ast_node_t *)(node->children->data[1]))->type)
        {
            if (!strcmp(((ast_node_t *)(node->children->data[0]))->type, ((ast_node_t *)(node->children->data[1]))->type) && (!strcmp("int",((ast_node_t *)(node->children->data[0]))->type) || !strcmp("string",((ast_node_t *)(node->children->data[0]))->type) || !strcmp("float32",((ast_node_t *)(node->children->data[0]))->type)))
            {
                add_type_to_node(node, "bool");
            }
            else
            {
                print_error(node,4);
                add_type_to_node(node, "bool");
            }
        }
        else
        {
            add_type_to_node(node, "bool");
        }
        return;
    }
      if (!strcmp(node->id, "Ge"))
    {
        if (((ast_node_t *)(node->children->data[0]))->type && ((ast_node_t *)(node->children->data[1]))->type)
        {
            if (!strcmp(((ast_node_t *)(node->children->data[0]))->type, ((ast_node_t *)(node->children->data[1]))->type) && (!strcmp("int",((ast_node_t *)(node->children->data[0]))->type) || !strcmp("string",((ast_node_t *)(node->children->data[0]))->type) || !strcmp("float32",((ast_node_t *)(node->children->data[0]))->type))) 
            {
                add_type_to_node(node, "bool");
            }
            else
            {
                print_error(node,4);
                add_type_to_node(node, "bool");
            }
        }
        else
        {
            add_type_to_node(node, "bool");
        }
        return;
    }
      if (!strcmp(node->id, "Gt"))
    {
        if (((ast_node_t *)(node->children->data[0]))->type && ((ast_node_t *)(node->children->data[1]))->type)
        {
            if (!strcmp(((ast_node_t *)(node->children->data[0]))->type, ((ast_node_t *)(node->children->data[1]))->type) && (!strcmp("int",((ast_node_t *)(node->children->data[0]))->type) || !strcmp("string",((ast_node_t *)(node->children->data[0]))->type) || !strcmp("float32",((ast_node_t *)(node->children->data[0]))->type)))
            {
                add_type_to_node(node, "bool");
            }
            else
            {
                print_error(node,4);
                add_type_to_node(node, "bool");
            }
        }
        else
        {
            add_type_to_node(node, "bool");
        }
        return;
    }
    if (!strcmp(node->id, "ParseArgs")){
        if (((ast_node_t *)(node->children->data[0]))->type && ((ast_node_t *)(node->children->data[1]))->type)
        {
            if (!strcmp(((ast_node_t *)(node->children->data[0]))->type,"int") && (!strcmp(((ast_node_t *)(node->children->data[1]))->type,"string") || !strcmp(((ast_node_t *)(node->children->data[1]))->type,"int"))){
                add_type_to_node(node, "int");
                
            }else{
            
                print_error(node,4);
            }
        }
        return;
    }

    if (!strcmp(node->id, "Return")){
        if(node->children->size == 0){
            if(strcmp(((symbol_t *)function->symbols->data[0])->type,"none"))
                print_error(node,6);
        }else{
             if(strcmp(((symbol_t *)function->symbols->data[0])->type,((ast_node_t *)node->children->data[0])->type))
                print_error(node,6);
        }
        return;
    }

     if (!strcmp(node->id, "For")){
         if (node->children->size == 2  && strcmp(((ast_node_t *) (node->children->data[0]))->type,"bool")){
             print_error(node, 6);
        }
        return;

     }
    
    if (!strcmp(node->id, "If")){
         if (node->children->size == 3  && strcmp(((ast_node_t *) (node->children->data[0]))->type,"bool")){
             print_error(node, 6);
        }
        return;
     } 

      if (!strcmp(node->id, "Print")){
        if(((ast_node_t *) (node->children->data[0]))->type)
          if(!strcmp(((ast_node_t *) (node->children->data[0]))->type,"undef"))
            print_error(node, 6);
        return;
     }
  
   
}

char *verify_type(const char *type)
{
    if (!strcmp(type, "Int"))
    {
        return "int";
    }
    if (!strcmp(type, "Bool"))
    {
        return "bool";
    }
    if (!strcmp(type, "Float32"))
    {
        return "float32";
    }
    if (!strcmp(type, "String"))
    {
        return "string";
    }
    return NULL;
}

void add_type_to_node(ast_node_t *node, char *type)
{
    node->type = type;
}

symbol_t *search_table(symbol_table_t *table, char *name,int global_search)
{

    for (int i = 0; i < table->symbols->size; i++)
    {
        if (!strcmp(((symbol_t *)(table->symbols->data[i]))->id, name))
        {
            return (symbol_t *)(table->symbols->data[i]);
        }
    }
    if (global_search){
        for (int i = 0; i < ((symbol_table_t *)(symbol_table->data[0]))->symbols->size; i++)
        {
        if (!strcmp(((symbol_t *)(((symbol_table_t *)(symbol_table->data[0]))->symbols->data[i]))->id, name))
            return (symbol_t *)(((symbol_table_t *)(symbol_table->data[0]))->symbols->data[i]);
        }
    }


    return NULL;
}

char *concatenate_strings(char *str1, char *str2)
{
    char *str3 = (char *)malloc(1 + strlen(str1) + strlen(str2));
    strcpy(str3, str1);
    strcat(str3, str2);
    return str3;
}

symbol_table_t * search_function_table(const char * name){
    for(int i = 0; i < symbol_table->size;i++){
        if (!strcmp(name,((symbol_table_t *)(symbol_table->data[i]))->id)){
            return (symbol_table_t *)(symbol_table->data[i]);
        }
    }
    return NULL;
}

void print_error(ast_node_t* node, int type ){
   errors_ += 1;
    switch(type){
        char * operator;
        case 0:
            printf("Line %d, column %d: Symbol %s already defined\n", node->token.line, node->token.column, node->token.value);
            break;
        
        case 1:
            printf("Line %d, column %d: Cannot find symbol %s\n", node->token.line, node->token.column, node->token.value);
        break;
        
        case 2:
           
            if (!strcmp(node->id,"Not"))
                operator="!";
            if(!strcmp(node->id,"Minus"))
                operator = "-";
            if(!strcmp(node->id,"Plus"))
                operator = "+";
            printf("Line %d, column %d: Operator %s cannot be applied to type %s\n",node->token.line, node->token.column,operator , ((ast_node_t *)node->children->data[0])->type);
        break;

        case 3:
            printf("Line %d, column %d: Cannot find symbol %s(", ((ast_node_t *)(node->children->data[0]))->token.line, ((ast_node_t *)(node->children->data[0]))->token.column, ((ast_node_t *)(node->children->data[0]))->token.value);
            for(int i = 1; i < node->children->size;i++){
                printf("%s", ((ast_node_t *)(node->children->data[i]))->type);
                if(i != node->children->size-1)
                    printf(",");
            }
            
            printf(")\n");
        break;

        case 4:
            if (!strcmp(node->id,"Eq"))
                operator = "==";
            if(!strcmp(node->id,"Ne"))
                operator = "!=";

            if(!strcmp(node->id,"Lt"))
                operator = "<";
            if (!strcmp(node->id,"Le"))
                operator="<=";
            if(!strcmp(node->id,"Gt"))
                operator = ">";
            if(!strcmp(node->id,"Ge"))
                operator = ">=";

            if(!strcmp(node->id,"Assign"))
                operator = "=";    

            if(!strcmp(node->id,"And"))
                operator = "&&";
            if(!strcmp(node->id,"Or"))
                operator = "||";
                
            if(!strcmp(node->id,"Add"))
                operator = "+";
            if(!strcmp(node->id,"Sub"))
                operator = "-";
            if(!strcmp(node->id,"Div"))
                operator = "/";
            if(!strcmp(node->id,"Mul"))
                operator = "*";
            if(!strcmp(node->id,"Mod")){
                 printf("Line %d, column %d: Operator %c cannot be applied to types %s, %s\n",node->token.line, node->token.column,'%',((ast_node_t *)(node->children->data[0]))->type,((ast_node_t *)(node->children->data[1]))->type);
                 break;
            }
            if(!strcmp(node->id,"ParseArgs")){
                operator = "strconv.Atoi";
            }
                
            printf("Line %d, column %d: Operator %s cannot be applied to types %s, %s\n",node->token.line, node->token.column,operator,((ast_node_t *)(node->children->data[0]))->type,((ast_node_t *)(node->children->data[1]))->type);
        break;
       case 5:
            printf("Line %d, column %d: Symbol %s declared but never used\n",node->token.line,node->token.column,node->token.value);
        break;

        case 6:
            if (!strcmp(node->id, "Return"))
            {
                if (node->children->size != 0)
                {
                    printf("Line %d, column %d: Incompatible type %s in return statement\n", ((ast_node_t *)(node->children->data[0]))->token.line, ((ast_node_t *)(node->children->data[0]))->token.column, ((ast_node_t *)(node->children->data[0]))->type);
                }
                else
                {
                    printf("Line %d, column %d: Incompatible type none in return statement\n", node->token.line, node->token.column);
                }
            }
            if (!strcmp(node->id, "For")){
                printf("Line %d, column %d: Incompatible type %s in for statement\n", ((ast_node_t *) (node->children->data[0]))->token.line, ((ast_node_t *) (node->children->data[0]))->token.column, ((ast_node_t *) (node->children->data[0]))->type);
               
            }
            if (!strcmp(node->id, "If")){
                printf("Line %d, column %d: Incompatible type %s in if statement\n", ((ast_node_t *) (node->children->data[0]))->token.line, ((ast_node_t *) (node->children->data[0]))->token.column, ((ast_node_t *) (node->children->data[0]))->type);
               
            }
            if (!strcmp(node->id, "Print")){
                if (!strcmp(((ast_node_t *) (node->children->data[0]))->id,"Call")){
                    printf("Line %d, column %d: Incompatible type %s in fmt.Println statement\n", ((ast_node_t *)(((ast_node_t *) (node->children->data[0]))->children->data[0]))->token.line, ((ast_node_t *)(((ast_node_t *) (node->children->data[0]))->children->data[0]))->token.column, ((ast_node_t *)(((ast_node_t *) (node->children->data[0]))->children->data[0]))->type);           
                }else{
                     printf("Line %d, column %d: Incompatible type %s in fmt.Println statement\n", ((ast_node_t *) (node->children->data[0]))->token.line, ((ast_node_t *) (node->children->data[0]))->token.column, ((ast_node_t *) (node->children->data[0]))->type);
                }
                    
               
               
            }
        

        break;
    }
}
