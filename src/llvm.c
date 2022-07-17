//Inês Martins Marçal 2019215917
//Noémia Quintano Mora Gonçalves 2019219433

#include "llvm.h"

void generate_llvm(ast_node_t *node)
{
    if (!node)
        return;

    if (node->children->size == 0)
    {
        printf("define i32 @main(){\n\t ret i32 0\n}");
        return;
    }
    strlit_count = 0;
    printf("declare i32 @printf(i8*, ...)\n");
    printf("declare i32 @atoi(i8*)\n");
    printf("\n");
    printf("@.intlit = private unnamed_addr constant [4 x i8] c\"%%d\\0A\\00\"\n");
    printf("@.strlit = private unnamed_addr constant [4 x i8] c\"%%s\\0A\\00\"\n");
    printf("@.float = private unnamed_addr constant [7 x i8] c\"%%.08f\\0A\\00\"\n");
    printf("@.true = private unnamed_addr constant [6 x i8] c\"true\\0A\\00\"\n");
    printf("@.false = private unnamed_addr constant [7 x i8] c\"false\\0A\\00\"\n");
    printf("\n");
    declare_const_str(node);

    strlit_count = 0;
    generate_program(node);
}
void generate_program(ast_node_t *node)
{
    if (!node)
        return;

    if (!strcmp(node->id, "FuncDecl"))
    {
        generate_func(node);
        return;
    }
    if (!strcmp(node->id, "VarDecl"))
    {
        generate_global_var(node);
        return;
    }
    if (!strcmp(node->id, "Call"))
    {
        generate_call(node);
    }

    for (int i = 0; i < node->children->size; i++)
    {
        generate_program((ast_node_t *)node->children->data[i]);
    }
}
void generate_global_var(ast_node_t *node)
{
    char *id = (char *)((ast_node_t *)node->children->data[1])->token.value;
    char *type = (char *)((ast_node_t *)node->children->data[0])->id;

    printf("@%s = global %s %s\n", id, get_llvm_type(type), get_default_value(type));
}
void generate_func(ast_node_t *node)
{
    strlit_count = 0;
    temp = 1;
    return_found = 0;
    bool_count = 0;
    ast_node_t *header = (ast_node_t *)node->children->data[0];
    ast_node_t *body = (ast_node_t *)node->children->data[1];
    int param_node = 0;
    char *return_type;
    function_name = (char *)((ast_node_t *)header->children->data[0])->token.value;
    if (!strcmp(function_name, "main"))
    {
        temp = 5;
        printf("define i32 @main(i32, i8**){\n");
        printf("\t%%3 = alloca i32\n");
        printf("\t%%4 = alloca i8**\n");
        printf("\tstore i32 %%0, i32* %%3\n");
        printf("\tstore i8** %%1, i8*** %%4\n");
        generate_body(body);
        printf("\tret i32 0\n");
        printf("}\n");
    }
    else
    {
        if (header->children->size == 2)
        {
            return_type = "void";
            param_node = 1;
        }
        else
        {
            return_type = get_llvm_type((char *)((ast_node_t *)(header->children->data[1]))->id);
            param_node = 2;
        }
        printf("define %s @%s(", return_type, function_name);
        symbol_t *symbol;
        char *id, *type;
        for (int i = 0; i < ((ast_node_t *)(header->children->data[param_node]))->children->size; i++)
        {
            id = ((ast_node_t *)((ast_node_t *)((ast_node_t *)(header->children->data[param_node]))->children->data[i])->children->data[1])->token.value;
            type = (char *)((ast_node_t *)((ast_node_t *)((ast_node_t *)(header->children->data[param_node]))->children->data[i])->children->data[0])->id;
            printf("%s", get_llvm_type(type));
            if (i != ((ast_node_t *)(header->children->data[param_node]))->children->size - 1)
                printf(", ");
        }
        printf(") {\n");
        temp = ((ast_node_t *)(header->children->data[param_node]))->children->size + 1;
        for (int i = 0; i < ((ast_node_t *)(header->children->data[param_node]))->children->size; i++)
        {
            id = ((ast_node_t *)((ast_node_t *)((ast_node_t *)(header->children->data[param_node]))->children->data[i])->children->data[1])->token.value;
            type = (char *)((ast_node_t *)((ast_node_t *)((ast_node_t *)(header->children->data[param_node]))->children->data[i])->children->data[0])->id;
            printf("\t%%%d = alloca %s\n", temp, get_llvm_type(type));
            symbol = search_table(search_function_table((const char *)function_name), id, 0);
            printf("\tstore %s %%%d, %s* %%%d\n", get_llvm_type(type), i, get_llvm_type(type), temp);
            if (symbol)
                symbol->temp = temp;
            temp += 1;
        }
        generate_body(body);
        if (!return_found)
        {
            if (!strcmp(return_type, "void"))
            {
                printf("\tret void\n");
            }
        }

        printf("}\n\n");
    }
}

void generate_body(ast_node_t *node)
{
    if (!node)
        return;
    ast_node_t *child;
    for (int i = 0; i < node->children->size; i++)
    {
        child = (ast_node_t *)node->children->data[i];
        if (!strcmp(child->id, "VarDecl"))
        {
            generate_local_var(child);
        }

        if (!strcmp(child->id, "Assign"))
        {
            generate_assign(child);
        }

        if (!strcmp(child->id, "Print"))
        {
            generate_print(child);
        }

        if (!strcmp(child->id, "ParseArgs"))
        {
            parse_args(child);
        }
        if (!strcmp(child->id, "Call"))
        {
            generate_expression(child);
        }

        if (!strcmp(child->id, "If"))
        {
            generate_if(child);
        }
        if (!strcmp(child->id, "For"))
        {
            generate_for(child);
        }
        if (!strcmp(child->id, "Return"))
        {
            return_found = 1;
            generate_return(child);
            return;
        }
    }
}
void generate_return(ast_node_t *node)
{
    if (node->children->size == 0)
    {
        printf("\tret void\n");
    }
    else
    {
        ast_node_t *expr = (ast_node_t *)node->children->data[0];

        generate_expression(expr);
        if (expr->temp != -1)
        {
            printf("\tret %s %%%d\n", get_llvm_type((char *)expr->type), expr->temp);
        }
        else
        {
            if (!strcmp(expr->id, "Id"))
            {
                symbol_t *symbol = search_table(search_function_table((const char *)function_name), expr->token.value, 0);
                if (!symbol)
                {
                    symbol = search_table(search_function_table((const char *)function_name), expr->token.value, 1);
                    printf("\t%%%d = load %s, %s* @%s\n", temp, get_llvm_type(symbol->type), get_llvm_type(symbol->type), symbol->id);
                }
                else
                {
                    printf("\t%%%d = load %s, %s* %%%d\n", temp, get_llvm_type(symbol->type), get_llvm_type(symbol->type), symbol->temp);
                }
                printf("\tret %s %%%d\n", get_llvm_type((char *)expr->type), temp);
                temp += 1;
                return;
            }
            else
            {
                if (expr->token.value)
                {
                    if (!strcmp(expr->type, "int"))
                    {
                        printf("\t%%%d = add i32 0, %s\n", temp, expr->token.value);
                    }
                    else
                    {

                        printf("\t%%%d = fadd double 0.0, ", temp);
                        convert_to_float((char *)expr->token.value);
                        printf("\n");
                    }
                    printf("\tret %s %%%d\n", get_llvm_type((char *)expr->type), temp);
                    temp += 1;
                }
            }
        }
    }
    temp += 1;
}

void generate_assign(ast_node_t *node)
{
    ast_node_t *left = (ast_node_t *)node->children->data[0];
    ast_node_t *right = (ast_node_t *)node->children->data[1];

    symbol_t *symbol_left = search_table(search_function_table((const char *)function_name), left->token.value, 0);
    if (!strcmp(right->id, "IntLit"))
    {
        if (!symbol_left)
        {
            symbol_left = search_table(search_function_table((const char *)function_name), left->token.value, 1);
            printf("\tstore %s %s,%s* @%s\n", get_llvm_type((char *)left->type), right->token.value, get_llvm_type(symbol_left->type), symbol_left->id);
        }
        else
        {
            printf("\tstore %s %s,%s* %%%d\n", get_llvm_type((char *)left->type), right->token.value, get_llvm_type(symbol_left->type), symbol_left->temp);
        }
        return;
    }

    if (!strcmp(right->id, "RealLit"))
    {
        if (!symbol_left)
        {
            symbol_left = search_table(search_function_table((const char *)function_name), left->token.value, 1);
            printf("\tstore %s ", get_llvm_type((char *)right->type));
            convert_to_float((char *)right->token.value);
            printf(",%s* @%s\n", get_llvm_type((char *)right->type), symbol_left->id);
        }
        else
        {
            printf("\tstore %s ", get_llvm_type((char *)right->type));
            convert_to_float((char *)right->token.value);
            printf(",%s* %%%d\n", get_llvm_type((char *)right->type), symbol_left->temp);
        }
        return;
    }

    if (!strcmp(right->id, "Id"))
    {
        symbol_t *symbol_right = search_table(search_function_table((const char *)function_name), right->token.value, 0);
        //direita global
        if (!symbol_right)
        {
            symbol_right = search_table(search_function_table((const char *)function_name), right->token.value, 1);
            printf("\t%%%d = load %s, %s* @%s\n", temp, get_llvm_type(symbol_right->type), get_llvm_type(symbol_right->type), symbol_right->id);
            if (!symbol_left)
            {
                symbol_left = search_table(search_function_table((const char *)function_name), left->token.value, 1);
                printf("\tstore %s %%%d, %s* @%s\n", get_llvm_type(symbol_left->type), temp, get_llvm_type(symbol_left->type), symbol_left->id);
            }
            else
            {
                printf("\tstore %s %%%d, %s* %%%d\n", get_llvm_type(symbol_left->type), temp, get_llvm_type(symbol_left->type), symbol_left->temp);
            }
            temp += 1;
            //direita local
        }
        else
        {
            printf("\t%%%d = load %s, %s* %%%d\n", temp, get_llvm_type(symbol_right->type), get_llvm_type(symbol_right->type), symbol_right->temp);
            if (!symbol_left)
            {
                symbol_left = search_table(search_function_table((const char *)function_name), left->token.value, 1);
                printf("\tstore %s %%%d, %s* @%s\n", get_llvm_type(symbol_left->type), temp, get_llvm_type(symbol_left->type), symbol_left->id);
            }
            else
            {
                printf("\tstore %s %%%d, %s* %%%d\n", get_llvm_type(symbol_left->type), temp, get_llvm_type(symbol_left->type), symbol_left->temp);
            }
            temp += 1;
        }
        return;
    }

    generate_expression(right);
    if (right->temp != -1)
    {
        if (!symbol_left)
        {
            symbol_left = search_table(search_function_table((const char *)function_name), left->token.value, 1);
            printf("\tstore %s %%%d, %s* @%s\n", get_llvm_type(symbol_left->type), right->temp, get_llvm_type(symbol_left->type), symbol_left->id);
        }
        else
        {
            printf("\tstore %s %%%d, %s* %%%d\n", get_llvm_type(symbol_left->type), right->temp, get_llvm_type(symbol_left->type), symbol_left->temp);
        }
    }
}

void generate_for(ast_node_t *node)
{
    if (node->children->size == 1)
    {
        generate_body((ast_node_t *)node->children->data[0]);
    }
    else
    {
        ast_node_t *expr = (ast_node_t *)node->children->data[0];
        int for_temp = bool_count;
        bool_count += 1;
        printf("br label %%for_load_%d\n", for_temp);

        printf("for_load_%d:\n", for_temp);
        generate_expression(expr);
        if (expr->temp != -1)
        {
            printf("\t%%%d = icmp ne i1 %%%d, 0 \n", temp, expr->temp);
            temp += 1;
        }
        else
        {
            if (!strcmp(expr->id, "Id"))
            {
                symbol_t *symbol = search_table(search_function_table((const char *)function_name), expr->token.value, 0);
                if (!symbol)
                {
                    symbol = search_table(search_function_table((const char *)function_name), expr->token.value, 1);
                    printf("\t%%%d = load %s, %s* @%s\n", temp, get_llvm_type(symbol->type), get_llvm_type(symbol->type), symbol->id);
                    temp += 1;
                }
                else
                {
                    printf("\t%%%d = load %s, %s* %%%d\n", temp, get_llvm_type(symbol->type), get_llvm_type(symbol->type), symbol->temp);
                    temp += 1;
                }
                if (!strcmp(expr->type, "int"))
                {
                    printf("\t%%%d = icmp ne i32 %%%d, 0 \n", temp, temp - 1);
                }
                if (!strcmp(expr->type, "float32"))
                {
                    printf("\t%%%d = fcmp one double %%%d, 0.0 \n", temp, temp - 1);
                }
                if (!strcmp(expr->type, "float32"))
                {
                    printf("\t%%%d = icmp ne i1 %%%d, 0 \n", temp, expr->temp);
                }
                temp += 1;
            }
            else
            {
                if (expr->token.value)
                {
                    if (!strcmp(expr->type, "int"))
                    {
                        printf("\t%%%d = add i32 0, %s\n", temp, expr->token.value);
                        temp += 1;
                        printf("\t%%%d = icmp one i32 %%%d, 0 \n", temp, temp - 1);
                    }
                    else
                    {

                        printf("\t%%%d = fadd double 0.0, ", temp);
                        temp += 1;
                        convert_to_float((char *)expr->token.value);
                        printf("\t%%%d = fcmp ne double %%%d, 0.0 \n", temp, temp - 1);
                        printf("\n");
                    }
                    temp += 1;
                }
            }
        }
        printf("br i1 %%%d, label %%for_%d, label %%continue_for_%d\n", expr->temp, for_temp, for_temp);

        printf("for_%d:\n", for_temp);
        generate_body((ast_node_t *)node->children->data[1]);
        printf("br label %%for_load_%d\n", for_temp);

        printf("continue_for_%d:", for_temp);
        return;
    }
}
void generate_if(ast_node_t *node)
{
    int bool_if;
    ast_node_t *expr = (ast_node_t *)node->children->data[0];
    generate_expression(expr);
    if (expr->temp != -1)
    {
        bool_if = bool_count;
        printf("\t%%%d = icmp ne i1 %%%d, 0 \n", temp, expr->temp);
        printf("\tbr i1 %%%d, label %%if_%d, label %%else_%d\n", temp, bool_count, bool_count);
        temp += 1;
        printf("\tif_%d:\n", bool_count);
        bool_count += 1;
        generate_body((ast_node_t *)node->children->data[1]);
        printf("\tbr label %%continue_if_%d\n", bool_if);
        printf("\telse_%d:\n", bool_if);
        generate_body((ast_node_t *)node->children->data[2]);
        printf("\tbr label %%continue_if_%d\n", bool_if);
        printf("\tcontinue_if_%d:\n", bool_if);

        return;
    }
}

void generate_print(ast_node_t *node)
{
    ast_node_t *data = (ast_node_t *)node->children->data[0];

    if (!strcmp(data->id, "IntLit"))
    {
        printf("\t%%%d = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.intlit, i32 0, i32 0), i32 %s)\n", temp, data->token.value);
        temp += 1;
        return;
    }

    if (!strcmp(data->id, "StrLit"))
    {
        printf("\t%%%d = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.strlit, i32 0, i32 0), i8* getelementptr inbounds ([%d x i8], [%d x i8]* @.%s.%d, i32 0, i32 0))\n", temp, count_escape(data->token.value) + count_regular(data->token.value) + 1, count_escape(data->token.value) + count_regular(data->token.value) + 1, function_name, strlit_count);
        temp += 1;
        strlit_count += 1;
        return;
    }

    if (!strcmp(data->id, "RealLit"))
    {

        printf("\t%%%d = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([7 x i8], [7 x i8]* @.float, i32 0, i32 0), double ", temp);
        convert_to_float((char *)data->token.value);
        printf(")\n");
        temp += 1;
        return;
    }

    if (!strcmp(data->id, "Id"))
    {
        symbol_t *symbol = search_table(search_function_table((const char *)function_name), data->token.value, 0);
        if (!strcmp(data->type, "int"))
        {
            if (!symbol)
            {
                symbol = search_table(search_function_table((const char *)function_name), data->token.value, 1);
                printf("\t%%%d = load %s, %s* @%s\n", temp, get_llvm_type(symbol->type), get_llvm_type(symbol->type), symbol->id);
                temp += 1;
                printf("\t%%%d = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.intlit, i32 0, i32 0), i32 %%%d)\n", temp, temp - 1);
                temp += 1;
                return;
            }
            else
            {
                printf("\t%%%d = load %s, %s* %%%d\n", temp, get_llvm_type(symbol->type), get_llvm_type(symbol->type), symbol->temp);
                temp += 1;
                printf("\t%%%d = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.intlit, i32 0, i32 0), i32 %%%d)\n", temp, temp - 1);
                temp += 1;
                return;
            }
        }

        if (!strcmp(data->type, "float32"))
        {
            if (!symbol)
            {
                symbol = search_table(search_function_table((const char *)function_name), data->token.value, 1);
                printf("\t%%%d = load %s, %s* @%s\n", temp, get_llvm_type(symbol->type), get_llvm_type(symbol->type), symbol->id);
                temp += 1;
                printf("\t%%%d = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([7 x i8], [7 x i8]* @.float, i32 0, i32 0), double %%%d)\n", temp, temp - 1);
                temp += 1;
                return;
            }
            else
            {
                printf("\t%%%d = load %s, %s* %%%d\n", temp, get_llvm_type(symbol->type), get_llvm_type(symbol->type), symbol->temp);
                temp += 1;
                printf("\t%%%d = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([7 x i8], [7 x i8]* @.float, i32 0, i32 0), double %%%d)\n", temp, temp - 1);
                temp += 1;
                return;
            }
        }

        if (!strcmp(data->type, "bool"))
        {
            if (!symbol)
            {
                symbol = search_table(search_function_table((const char *)function_name), data->token.value, 1);
                printf("\t%%%d = load %s, %s* @%s\n", temp, get_llvm_type(symbol->type), get_llvm_type(symbol->type), symbol->id);
                temp += 1;
                printf("%%%d = icmp eq i1 %%%d, 1\n", temp, temp - 1);
                printf("br i1 %%%d, label %%true_%d, label %%false_%d\n", temp, bool_count, bool_count);
                temp += 1;
                printf("true_%d:\n", bool_count);
                printf("%%%d = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @.true, i32 0, i32 0))\n", temp);
                temp += 1;
                printf("br label %%bool_continue_%d\n", bool_count);
                printf("false_%d:\n", bool_count);
                printf("%%%d = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([7 x i8], [7 x i8]* @.false, i32 0, i32 0))\n", temp);
                temp += 1;
                printf("br label %%bool_continue_%d\n", bool_count);
                printf("bool_continue_%d:\n", bool_count);
                bool_count += 1;
                return;
            }
            else
            {
                printf("\t%%%d = load %s, %s* %%%d\n", temp, get_llvm_type(symbol->type), get_llvm_type(symbol->type), symbol->temp);
                temp += 1;
                printf("%%%d = icmp eq i1 %%%d, 1\n", temp, temp - 1);
                printf("br i1 %%%d, label %%true_%d, label %%false_%d\n", temp, bool_count, bool_count);
                temp += 1;
                printf("true_%d:\n", bool_count);
                printf("%%%d = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @.true, i32 0, i32 0))\n", temp);
                temp += 1;
                printf("br label %%bool_continue_%d\n", bool_count);
                printf("false_%d:\n", bool_count);
                printf("%%%d = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([7 x i8], [7 x i8]* @.false, i32 0, i32 0))\n", temp);
                temp += 1;
                printf("br label %%bool_continue_%d\n", bool_count);
                printf("bool_continue_%d:\n", bool_count);
                bool_count += 1;
                return;
            }
        }
    }
    generate_expression(data);

    if (data->temp != -1)
    {
        if (!strcmp(data->type, "int"))
        {
            printf("\t%%%d = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.intlit, i32 0, i32 0), i32 %%%d)\n", temp, data->temp);
        }
        if (!strcmp(data->type, "float32"))
        {
            printf("\t%%%d = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([7 x i8], [7 x i8]* @.float, i32 0, i32 0), double %%%d)\n", temp, data->temp);
        }
        if (!strcmp(data->type, "bool"))
        {

            printf("%%%d = icmp eq i1 %%%d, 1\n", temp, data->temp);
            printf("br i1 %%%d, label %%true_%d, label %%false_%d\n", temp, bool_count, bool_count);
            temp += 1;
            printf("true_%d:\n", bool_count);
            printf("%%%d = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @.true, i32 0, i32 0))\n", temp);
            temp += 1;
            printf("br label %%bool_continue_%d\n", bool_count);
            printf("false_%d:\n", bool_count);
            printf("%%%d = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([7 x i8], [7 x i8]* @.false, i32 0, i32 0))\n", temp);
            printf("br label %%bool_continue_%d\n", bool_count);
            printf("bool_continue_%d:\n", bool_count);
            bool_count += 1;
        }
        temp += 1;
    }
}

void parse_args(ast_node_t *node)
{
    ast_node_t *node_left = (ast_node_t *)node->children->data[0];
    ast_node_t *node_right = (ast_node_t *)node->children->data[1];

    symbol_t *symbol_left = search_table(search_function_table(function_name), (char *)node_left->token.value, 0);

    if (!strcmp(node_right->id, "IntLit"))
    {
        printf("\t%%%d = load i8**, i8*** %%4\n", temp);
        temp += 1;
        printf("\t%%%d = getelementptr inbounds i8*, i8** %%%d, %s %s\n", temp, temp - 1, get_llvm_type((char *)node_right->type), node_right->token.value);
        temp += 1;
        printf("\t%%%d = load i8*, i8** %%%d, align 8\n", temp, temp - 1);
        temp += 1;
        printf("\t%%%d = call i32 @atoi(i8* %%%d)\n", temp, temp - 1);
        if (!symbol_left)
        {
            symbol_left = search_table(search_function_table(function_name), (char *)node_left->token.value, 1);
            printf("\tstore i32 %%%d, i32* @%s, align 8\n", temp, symbol_left->id);
        }
        else
        {
            printf("\tstore i32 %%%d, i32* %%%d, align 8\n", temp, symbol_left->temp);
        }
        temp += 1;
        return;
    }
    if (!strcmp(node_right->id, "Id"))
    {
        symbol_t *symbol_right = search_table(search_function_table(function_name), (char *)node_right->token.value, 0);
        if (!symbol_right)
        {
            symbol_right = search_table(search_function_table(function_name), (char *)node_right->token.value, 1);
            printf("\t%%%d = load %s, %s* @%s\n", temp, get_llvm_type(symbol_right->type), get_llvm_type(symbol_right->type), symbol_right->id);
            temp += 1;
            printf("\t%%%d = load i8**, i8*** %%4\n", temp);
            temp += 1;
            printf("\t%%%d = getelementptr inbounds i8*, i8** %%%d, %s %%%d\n", temp, temp - 1, get_llvm_type(symbol_right->type), temp - 2);
            temp += 1;
            printf("\t%%%d = load i8*, i8** %%%d, align 8\n", temp, temp - 1);
            temp += 1;
            printf("\t%%%d = call i32 @atoi(i8* %%%d)\n", temp, temp - 1);
            if (!symbol_left)
            {
                symbol_left = search_table(search_function_table(function_name), (char *)node_left->token.value, 1);
                printf("\tstore i32 %%%d, i32* @%s, align 8\n", temp, symbol_left->id);
            }
            else
            {
                printf("\tstore i32 %%%d, i32* %%%d, align 8\n", temp, symbol_left->temp);
            }
            temp += 1;
        }
        else
        {
            printf("\t%%%d = load %s, %s* %%%d\n", temp, get_llvm_type(symbol_right->type), get_llvm_type(symbol_right->type), symbol_right->temp);
            temp += 1;
            printf("\t%%%d = load i8**, i8*** %%4\n", temp);
            temp += 1;
            printf("\t%%%d = getelementptr inbounds i8*, i8** %%%d, %s %%%d\n", temp, temp - 1, get_llvm_type(symbol_right->type), temp - 2);
            temp += 1;
            printf("\t%%%d = load i8*, i8** %%%d, align 8\n", temp, temp - 1);
            temp += 1;
            printf("\t%%%d = call i32 @atoi(i8* %%%d)\n", temp, temp - 1);
            if (!symbol_left)
            {
                symbol_left = search_table(search_function_table(function_name), (char *)node_left->token.value, 1);
                printf("\tstore i32 %%%d, i32* @%s, align 8\n", temp, symbol_left->id);
            }
            else
            {
                printf("\tstore i32 %%%d, i32* %%%d, align 8\n", temp, symbol_left->temp);
            }
            temp += 1;
        }
        return;
    }
}

void generate_local_var(ast_node_t *node)
{
    char *id = (char *)((ast_node_t *)node->children->data[1])->token.value;
    char *type = (char *)((ast_node_t *)node->children->data[0])->id;

    symbol_t *symbol = search_table(search_function_table((const char *)function_name), id, 0);
    printf("\t%%%d = alloca %s\n", temp, get_llvm_type(type));
    printf("\tstore %s %s, %s* %%%d\n", get_llvm_type(type), get_default_value(type), get_llvm_type(type), temp);
    symbol->temp = temp;
    temp += 1;
}

void declare_const_str(ast_node_t *node)
{
    if (!node)
        return;

    if (!strcmp(node->id, "FuncDecl"))
    {
        ast_node_t *header = (ast_node_t *)node->children->data[0];
        function_name = ((ast_node_t *)header->children->data[0])->token.value;
        strlit_count = 0;
    }
    if (!strcmp(node->id, "StrLit"))
    {

        printf("@.%s.%d = ", function_name, strlit_count);
        strlit_count += 1;
        char *aux = node->token.value;
        for (int i = 1; i < strlen(aux); i++)
        {
            aux[i - 1] = aux[i];
        }
        aux[strlen(aux) - 2] = '\0';

        printf(" private unnamed_addr constant [%d x i8] c\"", count_regular(aux) + count_escape(aux) + 1);
        for (int i = 0; i < strlen(aux); i++)
        {
            if (aux[i] == '\\')
            {
                switch (aux[i + 1])
                {
                case 'n':
                    printf("\\0A");
                    break;
                case '\\':
                    printf("\\5C");
                    break;
                case 'f':
                    printf("\\0C");
                    break;
                case 't':
                    printf("\\09");
                    break;
                case '"':
                    printf("\\22");
                    break;

                case 'r':
                    printf("\\0D");
                    break;
                }
                i++;
            }
            else
            {
                printf("%c", aux[i]);
            }
        }
        printf("\\00\"\n");
    }

    for (int i = 0; i < node->children->size; i++)
    {
        declare_const_str((ast_node_t *)node->children->data[i]);
    }
}

int count_escape(char *token)
{
    int c = 0;
    for (int i = 0; i < strlen(token); i++)
    {
        if (token[i] == '\\')
        {
            i++;
            c++;
        }
    }
    return c;
}
int count_regular(char *token)
{
    int c = 0;
    for (int i = 0; i < strlen(token); i++)
    {
        if (token[i] == '\\')
        {
            i++;
        }
        else
        {
            c++;
        }
    }
    return c;
}
char *get_default_value(char *type)
{
    if (!strcmp(type, "Int") || !strcmp(type, "int"))
        return "0";

    if (!strcmp(type, "Bool") || !strcmp(type, "bool"))
        return "false";

    if (!strcmp(type, "String") || !strcmp(type, "string"))
        return "null";

    if (!strcmp(type, "Float32") || !strcmp(type, "float32"))
        return "0.0";

    return NULL;
}
char *get_llvm_type(char *type)
{
    if (!strcmp(type, "Int") || !strcmp(type, "int"))
        return "i32";

    if (!strcmp(type, "Bool") || !strcmp(type, "bool"))
        return "i1";

    if (!strcmp(type, "String") || !strcmp(type, "string"))
        return "i8*";

    if (!strcmp(type, "Float32") || !strcmp(type, "float32"))
        return "double";

    return NULL;
}

void generate_expression(ast_node_t *node)
{

    if (!node)
        return;

    for (int i = 0; i < node->children->size; i++)
    {
        generate_expression((ast_node_t *)node->children->data[i]);
    }

    if (generate_plus_minus_not(node) == 1)
        return;

    if (generate_operation(node) == 1)
    {
        return;
    }

    if (generate_boolean(node) == 1)
    {
        return;
    }

    if (generate_call(node) == 1)
    {
        return;
    }
}

int generate_operation(ast_node_t *node)
{
    char *operator= NULL;
    if (node->children->size < 2)
        return 0;

    ast_node_t *left = (ast_node_t *)node->children->data[0];
    ast_node_t *right = (ast_node_t *)node->children->data[1];

    int left_op_temp = -1;
    int right_op_temp = -1;
    if (!strcmp(node->id, "Div") && !strcmp(node->type, "float32"))
    {
        operator= "fdiv";
    }

    if (!strcmp(node->id, "Div") && !strcmp(node->type, "int"))
    {
        operator= "sdiv";
    }

    if (!strcmp(node->id, "Add") && !strcmp(node->type, "float32"))
    {
        operator= "fadd";
    }

    if (!strcmp(node->id, "Add") && !strcmp(node->type, "int"))
    {
        operator= "add";
    }

    if (!strcmp(node->id, "Mul") && !strcmp(node->type, "float32"))
    {
        operator= "fmul";
    }

    if (!strcmp(node->id, "Mul") && !strcmp(node->type, "int"))
    {
        operator= "mul";
    }

    if (!strcmp(node->id, "Sub") && !strcmp(node->type, "float32"))
    {
        operator= "fsub";
    }

    if (!strcmp(node->id, "Sub") && !strcmp(node->type, "int"))
    {
        operator= "sub";
    }

    if (!strcmp(node->id, "Mod") && !strcmp(node->type, "int"))
        operator= "srem";

    if (!strcmp(node->id, "Mod") && !strcmp(node->type, "float32"))
    {
        operator= "frem";
    }
    if (!strcmp(node->id, "And"))
    {
        operator= "and";
    }
    if (!strcmp(node->id, "Or"))
    {
        operator= "or";
    }

    if (operator)
    {

        //terminal op terminal
        if (left->temp == -1 && right->temp == -1)
        { //load left
            if (!strcmp(left->id, "Id"))
            {

                symbol_t *left_symbol = search_table(search_function_table((const char *)function_name), left->token.value, 0);
                if (!left_symbol)
                {
                    left_symbol = search_table(search_function_table((const char *)function_name), left->token.value, 1);
                    printf("\t%%%d = load %s, %s* @%s\n", temp, get_llvm_type(left_symbol->type), get_llvm_type(left_symbol->type), left_symbol->id);
                    left_op_temp = temp;
                    temp += 1;
                }
                else
                {
                    printf("\t%%%d = load %s, %s* %%%d\n", temp, get_llvm_type(left_symbol->type), get_llvm_type(left_symbol->type), left_symbol->temp);
                    left_op_temp = temp;
                    temp += 1;
                }
            }
            else
            {
                if (left->token.value)
                {
                    if (!strcmp(left->type, "int"))
                    {
                        printf("\t%%%d = add i32 0, %s\n", temp, left->token.value);
                    }
                    else
                    {

                        printf("\t%%%d = fadd double 0.0, ", temp);
                        convert_to_float((char *)left->token.value);
                        printf("\n");
                    }
                    left_op_temp = temp;
                    temp += 1;
                }
            }

            //load right
            if (!strcmp(right->id, "Id"))
            {

                symbol_t *right_symbol = search_table(search_function_table((const char *)function_name), right->token.value, 0);
                if (!right_symbol)
                {
                    right_symbol = search_table(search_function_table((const char *)function_name), right->token.value, 1);
                    printf("\t%%%d = load %s, %s* @%s\n", temp, get_llvm_type(right_symbol->type), get_llvm_type(right_symbol->type), right_symbol->id);
                    right_op_temp = temp;
                    temp += 1;
                }
                else
                {
                    printf("\t%%%d = load %s, %s* %%%d\n", temp, get_llvm_type(right_symbol->type), get_llvm_type(right_symbol->type), right_symbol->temp);
                    right_op_temp = temp;
                    temp += 1;
                }
            }
            else
            {
                if (right->token.value)
                {
                    if (!strcmp(right->type, "int"))
                    {
                        printf("\t%%%d = add i32 0, %s\n", temp, right->token.value);
                    }
                    else
                    {
                        printf("\t%%%d = fadd double 0.0, ", temp);
                        convert_to_float((char *)right->token.value);
                        printf("\n");
                    }
                    right_op_temp = temp;
                    temp += 1;
                }
            }

            //operation
            if (right_op_temp != -1 && left_op_temp != -1)
            {
                printf("\t%%%d = %s %s %%%d, %%%d\n", temp, operator, get_llvm_type((char *)node->type), left_op_temp, right_op_temp);
                node->temp = temp;
                temp += 1;
            }
        }

        //terminal op op

        if ((left->temp == -1) && (right->temp != -1))
        {

            //load left
            if (!strcmp(left->id, "Id"))
            {

                symbol_t *left_symbol = search_table(search_function_table((const char *)function_name), left->token.value, 0);
                if (!left_symbol)
                {
                    left_symbol = search_table(search_function_table((const char *)function_name), left->token.value, 1);
                    printf("\t%%%d = load %s, %s* @%s\n", temp, get_llvm_type(left_symbol->type), get_llvm_type(left_symbol->type), left_symbol->id);
                    left_op_temp = temp;
                    temp += 1;
                }
                else
                {
                    printf("\t%%%d = load %s, %s* %%%d\n", temp, get_llvm_type(left_symbol->type), get_llvm_type(left_symbol->type), left_symbol->temp);
                    left_op_temp = temp;
                    temp += 1;
                }
            }
            else
            {
                if (left->token.value)
                {
                    if (!strcmp(left->type, "int"))
                    {
                        printf("\t%%%d = add i32 0, %s\n", temp, left->token.value);
                    }
                    else
                    {

                        printf("\t%%%d = fadd double 0.0, ", temp);
                        convert_to_float((char *)left->token.value);
                        printf("\n");
                    }
                    left_op_temp = temp;
                    temp += 1;
                }
            }

            //operation
            if ((left_op_temp != -1) && (right->temp != -1))
            {
                printf("\t%%%d = %s %s %%%d, %%%d\n", temp, operator, get_llvm_type((char *)node->type), left_op_temp, right->temp);
                node->temp = temp;
                temp += 1;
            }
        }

        //op op terminal

        if (left->temp != -1 && right->temp == -1)
        {
            //load right
            if (!strcmp(right->id, "Id"))
            {

                symbol_t *right_symbol = search_table(search_function_table((const char *)function_name), right->token.value, 0);
                if (!right_symbol)
                {
                    right_symbol = search_table(search_function_table((const char *)function_name), right->token.value, 1);
                    printf("\t%%%d = load %s, %s* @%s\n", temp, get_llvm_type(right_symbol->type), get_llvm_type(right_symbol->type), right_symbol->id);
                    right_op_temp = temp;
                    temp += 1;
                }
                else
                {
                    printf("\t%%%d = load %s, %s* %%%d\n", temp, get_llvm_type(right_symbol->type), get_llvm_type(right_symbol->type), right_symbol->temp);
                    right_op_temp = temp;
                    temp += 1;
                }
            }
            else
            {
                if (right->token.value)
                {
                    if (!strcmp(right->type, "int"))
                    {
                        printf("\t%%%d = add i32 0, %s\n", temp, right->token.value);
                    }
                    else
                    {
                        printf("\t%%%d = fadd double 0.0, ", temp);
                        convert_to_float((char *)right->token.value);
                        printf("\n");
                    }
                    right_op_temp = temp;
                    temp += 1;
                }
            }

            //operation
            if (left->temp != -1 && right_op_temp != -1)
            {
                printf("\t%%%d = %s %s %%%d, %%%d\n", temp, operator, get_llvm_type((char *)node->type), left->temp, right_op_temp);
                node->temp = temp;
                temp += 1;
            }
        }

        //operator op operator
        if (left->temp != -1 && right->temp != -1)
        {
            printf("\t%%%d = %s %s %%%d, %%%d\n", temp, operator, get_llvm_type((char *)node->type), left->temp, right->temp);
            node->temp = temp;
            temp += 1;
        }

        return 1;
    }
    return 0;
}

int generate_plus_minus_not(ast_node_t *node)
{
    if (!strcmp(node->id, "Plus"))
    {
        ast_node_t *data = (ast_node_t *)node->children->data[0];

        if (data->temp == -1)
        {

            if (!strcmp(data->id, "Id"))
            {
                symbol_t *symbol = search_table(search_function_table((const char *)function_name), data->token.value, 0);
                if (!symbol)
                {
                    symbol = search_table(search_function_table((const char *)function_name), data->token.value, 1);
                    printf("\t%%%d = load %s, %s* @%s\n", temp, get_llvm_type(symbol->type), get_llvm_type(symbol->type), symbol->id);
                    node->temp = temp;
                    temp += 1;
                }
                else
                {
                    printf("\t%%%d = load %s, %s* %%%d\n", temp, get_llvm_type(symbol->type), get_llvm_type(symbol->type), symbol->temp);
                    temp += 1;
                    if (!strcmp(data->type, "int"))
                    {
                        printf("\t%%%d = add i32 0, %%%d\n", temp, temp - 1);
                    }
                    else
                    {
                        printf("\t%%%d = fadd double 0.0, %%%d\n", temp, temp - 1);
                    }
                    node->temp = temp;
                    temp += 1;
                }
            }
            else
            {
                if (data->token.value)
                {
                    if (!strcmp(data->type, "int"))
                    {
                        printf("\t%%%d = add i32 0, %s\n", temp, data->token.value);
                    }
                    else
                    {
                        printf("\t%%%d = fadd double 0.0, ", temp);
                        convert_to_float((char *)data->token.value);
                        printf("\n");
                    }
                    node->temp = temp;
                    temp += 1;
                }
            }
        }
        else
        {
            if (!strcmp(data->type, "int"))
            {
                printf("\t%%%d = add i32 0, %%%d\n", temp, data->temp);
            }
            else
            {
                printf("\t%%%d = fadd double 0.0, %%%d\n", temp, data->temp);
            }
            node->temp = temp;
            temp += 1;
        }

        return 1;
    }

    if (!strcmp(node->id, "Minus"))
    {
        ast_node_t *data = (ast_node_t *)node->children->data[0];

        if (data->temp == -1)
        {
            if (!strcmp(data->id, "Id"))
            {
                symbol_t *symbol = search_table(search_function_table((const char *)function_name), data->token.value, 0);
                if (!symbol)
                {
                    symbol = search_table(search_function_table((const char *)function_name), data->token.value, 1);
                    printf("\t%%%d = load %s, %s* @%s\n", temp, get_llvm_type(symbol->type), get_llvm_type(symbol->type), symbol->id);
                    temp += 1;
                    if (!strcmp(data->type, "int"))
                    {
                        printf("\t%%%d = sub i32 0, %%%d\n", temp, temp - 1);
                    }
                    else
                    {
                        printf("\t%%%d = fsub double 0.0, %%%d\n", temp, temp - 1);
                    }
                    node->temp = temp;
                    temp += 1;
                }
                else
                {
                    printf("\t%%%d = load %s, %s* %%%d\n", temp, get_llvm_type(symbol->type), get_llvm_type(symbol->type), symbol->temp);
                    temp += 1;
                    if (!strcmp(data->type, "int"))
                    {
                        printf("\t%%%d = sub i32 0, %%%d\n", temp, temp - 1);
                    }
                    else
                    {
                        printf("\t%%%d = fsub double 0.0, %%%d\n", temp, temp - 1);
                    }
                    node->temp = temp;
                    temp += 1;
                }
            }
            else
            {
                if (data->token.value)
                {
                    if (!strcmp(data->type, "int"))
                    {
                        printf("\t%%%d = sub i32 0, %s\n", temp, data->token.value);
                    }
                    else
                    {

                        printf("\t%%%d = fsub double 0.0, ", temp);
                        convert_to_float((char *)data->token.value);
                        printf("\n");
                    }
                    node->temp = temp;
                    temp += 1;
                }
            }
        }
        else
        {

            if (!strcmp(data->type, "int"))
            {
                printf("\t%%%d = sub i32 0, %%%d\n", temp, data->temp);
            }
            else
            {
                printf("\t%%%d = fsub double 0.0, %%%d\n", temp, data->temp);
            }
            node->temp = temp;
            temp += 1;
        }

        return 1;
    }

    if (!strcmp(node->id, "Not"))
    {

        ast_node_t *expr = (ast_node_t *)node->children->data[0];

        if (expr->temp != -1)
        {
            printf("%%%d = icmp eq i1 %%%d, 1\n", temp, expr->temp);
        }
        else
        {
            symbol_t *symbol = search_table(search_function_table(function_name), expr->token.value, 0);
            if (!symbol)
            {
                symbol_t *symbol = search_table(search_function_table(function_name), expr->token.value, 1);
                printf("%%%d = load %s, %s* @%s\n", temp, get_llvm_type((char *)expr->type), get_llvm_type((char *)expr->type), symbol->id);
                temp += 1;
                printf("%%%d = icmp eq i1 %%%d, 1\n", temp, temp - 1);
            }
            else
            {
                printf("%%%d = load %s, %s* %%%d\n", temp, get_llvm_type((char *)expr->type), get_llvm_type((char *)expr->type), symbol->temp);
                temp += 1;
                printf("%%%d = icmp eq i1 %%%d, 1\n", temp, temp - 1);
            }
        }
        printf("br i1 %%%d, label %%true_not%d, label %%false_not%d\n", temp, bool_count, bool_count);
        temp += 1;
        printf("true_not%d:\n", bool_count);
        printf("%%%d = add i1 0, 0\n", temp);
        temp += 1;
        printf("br label %%bool_continue_not%d\n", bool_count);
        printf("false_not%d:\n", bool_count);
        printf("%%%d = add i1 1, 0\n", temp);
        temp += 1;
        printf("br label %%bool_continue_not%d\n", bool_count);
        printf("bool_continue_not%d:\n", bool_count);
        printf("%%%d = phi i1 [ %%%d, %%true_not%d ], [ %%%d, %%false_not%d ]\n", temp, temp - 2, bool_count, temp - 1, bool_count);
        node->temp = temp;
        temp += 1;
        bool_count += 1;
        return 1;
    }
    return 0;
}

int generate_boolean(ast_node_t *node)
{
    char *operator= NULL;
    char *operation = NULL;
    if (node->children->size < 2)
    {
        return 0;
    }
    ast_node_t *left = (ast_node_t *)node->children->data[0];
    ast_node_t *right = (ast_node_t *)node->children->data[1];
    ///adicionar bools
    if (!strcmp(left->type, "int") || !strcmp(left->type, "bool"))
    {
        operation = "icmp";
        if (!strcmp(node->id, "Lt"))
            operator= "slt";

        if (!strcmp(node->id, "Le"))
            operator= "sle";

        if (!strcmp(node->id, "Gt"))
            operator= "sgt";

        if (!strcmp(node->id, "Ge"))
            operator= "sge";

        if (!strcmp(node->id, "Eq"))
            operator= "eq";

        if (!strcmp(node->id, "Ne"))
            operator= "ne";
    }

    if (!strcmp(left->type, "float32"))
    {
        operation = "fcmp";
        if (!strcmp(node->id, "Lt"))
            operator= "olt";

        if (!strcmp(node->id, "Le"))
            operator= "ole";

        if (!strcmp(node->id, "Gt"))
            operator= "ogt";

        if (!strcmp(node->id, "Ge"))
            operator= "oge";

        if (!strcmp(node->id, "Eq"))
            operator= "oeq";

        if (!strcmp(node->id, "Ne"))
            operator= "one";
    }
    if (operator)
    {
        int left_op_temp = -1;
        int right_op_temp = -1;

        //literal cmp literal
        if (left->temp == -1 && right->temp == -1)
        { //load left
            if (!strcmp(left->id, "Id"))
            {

                symbol_t *left_symbol = search_table(search_function_table((const char *)function_name), left->token.value, 0);
                if (!left_symbol)
                {
                    left_symbol = search_table(search_function_table((const char *)function_name), left->token.value, 1);
                    printf("\t%%%d = load %s, %s* @%s\n", temp, get_llvm_type(left_symbol->type), get_llvm_type(left_symbol->type), left_symbol->id);
                    left_op_temp = temp;
                    temp += 1;
                }
                else
                {
                    printf("\t%%%d = load %s, %s* %%%d\n", temp, get_llvm_type(left_symbol->type), get_llvm_type(left_symbol->type), left_symbol->temp);
                    left_op_temp = temp;
                    temp += 1;
                }
            }
            else
            {
                if (left->token.value)
                {
                    if (!strcmp(left->type, "int"))
                    {
                        printf("\t%%%d = add i32 0, %s\n", temp, left->token.value);
                    }
                    else
                    {

                        printf("\t%%%d = fadd double 0.0, ", temp);
                        convert_to_float((char *)left->token.value);
                        printf("\n");
                    }
                    left_op_temp = temp;
                    temp += 1;
                }
            }

            //load right
            if (!strcmp(right->id, "Id"))
            {

                symbol_t *right_symbol = search_table(search_function_table((const char *)function_name), right->token.value, 0);
                if (!right_symbol)
                {
                    right_symbol = search_table(search_function_table((const char *)function_name), right->token.value, 1);
                    printf("\t%%%d = load %s, %s* @%s\n", temp, get_llvm_type(right_symbol->type), get_llvm_type(right_symbol->type), right_symbol->id);
                    right_op_temp = temp;
                    temp += 1;
                }
                else
                {
                    printf("\t%%%d = load %s, %s* %%%d\n", temp, get_llvm_type(right_symbol->type), get_llvm_type(right_symbol->type), right_symbol->temp);
                    right_op_temp = temp;
                    temp += 1;
                }
            }
            else
            {
                if (right->token.value)
                {
                    if (!strcmp(right->type, "int"))
                    {
                        printf("\t%%%d = add i32 0, %s\n", temp, right->token.value);
                    }
                    else
                    {
                        printf("\t%%%d = fadd double 0.0, ", temp);
                        convert_to_float((char *)right->token.value);
                        printf("\n");
                    }
                    right_op_temp = temp;
                    temp += 1;
                }
            }
            //operation
            if (left_op_temp != -1 && right_op_temp != -1)
            {
                printf("\t%%%d = %s %s %s %%%d, %%%d\n", temp, operation, operator, get_llvm_type((char *)left->type), left_op_temp, right_op_temp);
                node->temp = temp;
                temp += 1;
            }
        }

        // literal cmp op
        if ((left->temp == -1) && (right->temp != -1))
        {

            //load left
            if (!strcmp(left->id, "Id"))
            {

                symbol_t *left_symbol = search_table(search_function_table((const char *)function_name), left->token.value, 0);
                if (!left_symbol)
                {
                    left_symbol = search_table(search_function_table((const char *)function_name), left->token.value, 1);
                    printf("\t%%%d = load %s, %s* @%s\n", temp, get_llvm_type(left_symbol->type), get_llvm_type(left_symbol->type), left_symbol->id);
                    left_op_temp = temp;
                    temp += 1;
                }
                else
                {
                    printf("\t%%%d = load %s, %s* %%%d\n", temp, get_llvm_type(left_symbol->type), get_llvm_type(left_symbol->type), left_symbol->temp);
                    left_op_temp = temp;
                    temp += 1;
                }
            }
            else
            {
                if (left->token.value)
                {
                    if (!strcmp(left->type, "int"))
                    {
                        printf("\t%%%d = add i32 0, %s\n", temp, left->token.value);
                    }
                    else
                    {

                        printf("\t%%%d = fadd double 0.0, ", temp);
                        convert_to_float((char *)left->token.value);
                        printf("\n");
                    }
                    left_op_temp = temp;
                    temp += 1;
                }
            }

            //operation
            if ((left_op_temp != -1) && (right->temp != -1))
            {
                printf("\t%%%d = %s %s %s %%%d, %%%d\n", temp, operation, operator, get_llvm_type((char *)left->type), left_op_temp, right->temp);
                node->temp = temp;
                temp += 1;
            }
        }

        //op cmp literal
        if (left->temp != -1 && right->temp == -1)
        {
            //load right
            if (!strcmp(right->id, "Id"))
            {

                symbol_t *right_symbol = search_table(search_function_table((const char *)function_name), right->token.value, 0);
                if (!right_symbol)
                {
                    right_symbol = search_table(search_function_table((const char *)function_name), right->token.value, 1);
                    printf("\t%%%d = load %s, %s* @%s\n", temp, get_llvm_type(right_symbol->type), get_llvm_type(right_symbol->type), right_symbol->id);
                    right_op_temp = temp;
                    temp += 1;
                }
                else
                {
                    printf("\t%%%d = load %s, %s* %%%d\n", temp, get_llvm_type(right_symbol->type), get_llvm_type(right_symbol->type), right_symbol->temp);
                    right_op_temp = temp;
                    temp += 1;
                }
            }
            else
            {
                if (right->token.value)
                {
                    if (!strcmp(right->type, "int"))
                    {
                        printf("\t%%%d = add i32 0, %s\n", temp, right->token.value);
                    }
                    else
                    {
                        printf("\t%%%d = fadd double 0.0, ", temp);
                        convert_to_float((char *)right->token.value);
                        printf("\n");
                    }
                    right_op_temp = temp;
                    temp += 1;
                }
            }

            //operation
            if (left->temp != -1 && right_op_temp != -1)
            {
                printf("\t%%%d = %s %s %s %%%d, %%%d\n", temp, operation, operator, get_llvm_type((char *)left->type), left->temp, right_op_temp);
                node->temp = temp;
                temp += 1;
            }
        }

        //op cmp op
        if (left->temp != -1 && right->temp != -1)
        {
            printf("\t%%%d = %s %s %s %%%d, %%%d\n", temp, operation, operator, get_llvm_type((char *)left->type), left->temp, right->temp);
            node->temp = temp;
            temp += 1;
        }
        return 1;
    }
    return 0;
}

int generate_call(ast_node_t *node)
{
    if (!strcmp(node->id, "Call"))
    {
        int param_array[node->children->size - 1];
        ast_node_t *param;
        for (int i = 1; i < node->children->size; i++)
        {
            param = (ast_node_t *)node->children->data[i];
            if (param->temp == -1)
            {
                if (!strcmp(param->id, "Id"))
                {
                    symbol_t *symbol = search_table(search_function_table(function_name), param->token.value, 0);
                    if (!symbol)
                    {
                        symbol = search_table(search_function_table((const char *)function_name), param->token.value, 1);
                        printf("\t%%%d = load %s, %s* @%s\n", temp, get_llvm_type(symbol->type), get_llvm_type(symbol->type), symbol->id);
                        param_array[i - 1] = temp;
                        temp += 1;
                    }
                    else
                    {
                        printf("\t%%%d = load %s, %s* %%%d\n", temp, get_llvm_type(symbol->type), get_llvm_type(symbol->type), symbol->temp);
                        param_array[i - 1] = temp;
                        temp += 1;
                    }
                }
                else
                {
                    if (param->token.value)
                    {
                        if (!strcmp(param->type, "int"))
                        {
                            printf("\t%%%d = add i32 0, %s\n", temp, param->token.value);
                        }
                        else
                        {
                            printf("\t%%%d = fadd double 0.0, ", temp);
                            convert_to_float((char *)param->token.value);
                            printf("\n");
                        }
                        param_array[i - 1] = temp;
                        temp += 1;
                    }
                }
            }
            else
            {
                param_array[i - 1] = param->temp;
            }
        }

        if (!strcmp(node->type, "none"))
        {
            printf("\tcall void @%s(", ((ast_node_t *)node->children->data[0])->token.value);
        }
        else
        {
            printf("\t%%%d = call %s @%s(", temp, get_llvm_type((char *)node->type), ((ast_node_t *)node->children->data[0])->token.value);
            node->temp = temp;
            temp += 1;
        }

        for (int i = 1; i < node->children->size; i++)
        {
            param = (ast_node_t *)node->children->data[i];
            printf("%s %%%d", get_llvm_type((char *)param->type), param_array[i - 1]);
            if (i < node->children->size - 1)
            {
                printf(",");
            }
        }
        printf(")\n");
        return 1;
    }
    return 0;
}

void convert_to_float(char *value)
{
    if (value[0] == '.')
    {
        printf("0");
    }
    printf("%s", value);
}