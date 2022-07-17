%{  
    //Inês Martins Marçal 2019215917
    //Noémia Quintano Mora Gonçalves 2019219433

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include "llvm.h"
    #include "y.tab.h"  
    
    
    int yylex(void);
    void yyerror (const char *s);
    int yylex_destroy(void);
    void flag_l();
    int ast_flag = 0;
    int note_ast_flag=0; 
    int syntax_errors=0;
    int var_type = 0;
    char *type_array[] = {"Int", "Float32", "Bool", "String"};
    ast_node_t *program = NULL;
    extern vector_t *symbol_table;
    extern  int errors_;

    void add_type(ast_node_t * node){
    ast_node_t * n;
    ast_node_t * n1;
    for (int i = 0; i < node->children->size; i++ ){
       n = (ast_node_t *) (node->children->data[i]);
       n1 =(ast_node_t *) n->children->data[0];
       n1->id = type_array[var_type]; 
  
    }
}
   
%}

%union {  
  token_t token;
  ast_node_t *node;
}

%token <token> BLANKID ID SEMICOLON COMMA ASSIGN STAR DIV MINUS PLUS GT 
%token <token> LBRACE LPAR LSQ LT MOD NOT RBRACE RPAR RSQ INT FLOAT32 
%token <token> INTLIT REALLIT EQ GE LE NE AND OR PACKAGE RETURN 
%token <token> ELSE FOR IF VAR BOOL STRING PRINT PARSEINT FUNC 
%token <token> CMDARGS RESERVED STRLIT

%type <node> Program Declarations VarDeclaration FuncDeclaration Aux 
%type <node> Type Parameters Aux2 FuncBody VarsAndStatements  
%type <node> Statement Aux4 ParseArgs FuncInvocation aux3 Expr 

%left COMMA 
%right ASSIGN
%left OR 
%left AND 
%left EQ NE GT GE LT LE
%left PLUS MINUS  
%left STAR DIV MOD 
%right NOT
%nonassoc NOELSE
%nonassoc ELSE 

%nonassoc RPAR LPAR LSQ RSQ  

%%

Program: PACKAGE ID SEMICOLON Declarations                                                  {$$ = program = ast_node("Program", NULL);  add_as_siblings($$, $4); }
    |  PACKAGE ID SEMICOLON  {;}                                                            {$$ = program = ast_node("Program", NULL); } 
    ;

Declarations: VarDeclaration SEMICOLON                                                      {$$ = ast_node("Declarations", NULL); add_as_siblings($$, $1);} 
        | FuncDeclaration SEMICOLON                                                         {$$ = ast_node("Declarations", NULL); add_child($$, $1);} 
        | Declarations VarDeclaration SEMICOLON                                             {if(!$$) {$$=ast_node("Declarations", NULL);}  add_as_siblings($$, $2);}
        | Declarations FuncDeclaration SEMICOLON                                            {if(!$$) {$$ = ast_node("Declarations", NULL);}  add_child($$, $2);} 
        ;

VarDeclaration: VAR ID Aux Type                                                             {$$ = ast_node("aux", NULL);  ast_node_t *h = ast_node("VarDecl", NULL); 
                                                                                            add_child(h,$4); add_child(h,ast_node("Id",&$2)); add_child($$,h); add_as_siblings($$,$3); add_type($$); }
      | VAR ID Type                                                                         {$$ = ast_node("aux", NULL);  ast_node_t *h = ast_node("VarDecl", NULL); add_child($$,h);
                                                                                            add_child(h,$3); add_child(h,ast_node("Id",&$2));}
      | VAR LPAR ID Type SEMICOLON RPAR                                                     {$$ = ast_node("aux", NULL);  ast_node_t *h = ast_node("VarDecl", NULL); 
                                                                                            add_child($$,h);  add_child(h,$4); add_child(h,ast_node("Id",&$3));}
      | VAR LPAR ID Aux Type SEMICOLON RPAR                                                 {$$ = ast_node("aux", NULL);  ast_node_t *h = ast_node("VarDecl", NULL);  
                                                                                            ;add_child(h,$5); add_child(h,ast_node("Id",&$3)); add_child($$,h); add_as_siblings($$,$4); add_type($$); }
      ;
      
FuncDeclaration: FUNC ID LPAR Parameters RPAR Type FuncBody                                 {$$ = ast_node("FuncDecl", NULL); ast_node_t *h = ast_node("FuncHeader", NULL); add_child($$, h); add_child(h, ast_node("Id", &$2)); add_child(h, $6); add_child(h, $4);  add_child($$, $7);}
    | FUNC ID LPAR Parameters RPAR FuncBody                                                 {$$ = ast_node("FuncDecl", NULL); ast_node_t *h = ast_node("FuncHeader", NULL); add_child($$, h); add_child(h, ast_node("Id", &$2));  add_child(h, $4); add_child($$, $6);}
    | FUNC ID LPAR RPAR Type FuncBody                                                       {$$ = ast_node("FuncDecl", NULL); ast_node_t *h = ast_node("FuncHeader", NULL); add_child($$, h); add_child(h, ast_node("Id", &$2)); add_child(h, $5);  add_child(h, ast_node("FuncParams", NULL)); add_child($$, $6);}
    | FUNC ID LPAR RPAR FuncBody                                                            {$$ = ast_node("FuncDecl", NULL); ast_node_t *h = ast_node("FuncHeader", NULL); add_child($$, h); add_child(h, ast_node("Id", &$2)); add_child(h, ast_node("FuncParams", NULL)); add_child($$, $5);}
    ;

Aux: Aux COMMA ID                                                                           {$$ = ast_node("aux", NULL); add_as_siblings($$,$1); ast_node_t *h= ast_node("VarDecl", NULL); add_child($$, h); add_child(h,ast_node("",NULL)); add_child(h,ast_node("Id",&$3));  }
    | COMMA ID                                                                              {$$ = ast_node("aux", NULL);  ast_node_t *h= ast_node("VarDecl", NULL); add_child($$, h); add_child(h,ast_node("",NULL)); add_child(h,ast_node("Id",&$2));}
    ;

Type: INT                                                                                   {$$= ast_node("Int",NULL);var_type = 0; }
    | FLOAT32                                                                               {$$=ast_node("Float32",NULL); var_type = 1;}
    | BOOL                                                                                  {$$=ast_node("Bool",NULL);var_type = 2;}
    | STRING                                                                                {$$=ast_node("String",NULL);var_type = 3;}
    ;
Parameters: ID Type Aux2                                                                    {$$ = ast_node("FuncParams", NULL); ast_node_t *pd = ast_node("ParamDecl", NULL); add_child($$, pd);add_child(pd, $2);  add_child(pd, ast_node("Id", &$1)); add_as_siblings($$, $3);}
    | ID Type                                                                               {$$ = ast_node("FuncParams", NULL); ast_node_t *pd = ast_node("ParamDecl", NULL); add_child($$, pd);add_child(pd, $2);  add_child(pd, ast_node("Id", &$1));}
    ;
 
Aux2: Aux2 COMMA ID Type                                                                    {$$=ast_node("aux",NULL); add_as_siblings($$, $1); ast_node_t *pd = ast_node("ParamDecl", NULL);add_child($$,pd); add_child(pd,$4); add_child(pd,ast_node("Id", &$3));}
    | COMMA ID Type                                                                         {$$=ast_node("aux",NULL);ast_node_t *pd = ast_node("ParamDecl", NULL);add_child($$,pd); add_child(pd,$3); add_child(pd,ast_node("Id", &$2));};
    ; 

FuncBody: LBRACE VarsAndStatements RBRACE                                                   {$$=ast_node("FuncBody", NULL);add_as_siblings($$,$2) ;}
    ; 
    
VarsAndStatements: /* epsilon */                                                            {$$=NULL;}
    | VarsAndStatements VarDeclaration SEMICOLON                                            {if(!$$) {$$=ast_node("VarsAndStatements", NULL);}  add_as_siblings($$, $2);}
    | VarsAndStatements Statement SEMICOLON                                                 {if(!$$) {$$=ast_node("VarsAndStatements", NULL);}  add_child($$, $2);}
    | VarsAndStatements error SEMICOLON                                                     {$$=$1;}
    | VarsAndStatements SEMICOLON                                                           {$$=$1;}
    ; 

Statement: ID ASSIGN Expr                                                                   {$$=ast_node("Assign",&$2); add_child($$,ast_node("Id",&$1));add_child($$,$3);}    
    | LBRACE  Aux4 RBRACE                                                                   {$$=ast_node("Block",NULL); add_as_siblings($$,$2); if (get_size($$)<2)                                             {$$=(ast_node_t *)$$->children->data[0];}    }
    | LBRACE RBRACE                                                                         {$$=NULL;} 
    | IF Expr LBRACE Aux4 RBRACE   %prec NOELSE                                             {$$=ast_node("If",NULL); add_child($$,$2); ast_node_t * b = ast_node("Block",NULL);add_child($$,b); add_as_siblings(b,$4); add_child($$,ast_node("Block",NULL));}
    | IF Expr LBRACE RBRACE  %prec NOELSE                                                   {$$=ast_node("If",NULL); add_child($$,$2);add_child($$,ast_node("Block",NULL));add_child($$,ast_node("Block",NULL));}
    | IF Expr LBRACE Aux4 RBRACE ELSE LBRACE Aux4 RBRACE                                    {$$=ast_node("If",NULL); add_child($$,$2);ast_node_t * b = ast_node("Block",NULL); add_child($$,b); add_as_siblings(b,$4); ast_node_t * b2 = ast_node("Block",NULL); add_child($$,b2); add_as_siblings(b2,$8);}
    | IF Expr LBRACE Aux4 RBRACE ELSE LBRACE  RBRACE                                        {$$=ast_node("If",NULL); add_child($$,$2);ast_node_t * b = ast_node("Block",NULL); add_child($$,b); add_as_siblings(b,$4); add_child($$,ast_node("Block",NULL));}
    | IF Expr LBRACE RBRACE ELSE LBRACE Aux4 RBRACE                                         {$$=ast_node("If",NULL); add_child($$,$2); add_child($$,ast_node("Block",NULL)); ast_node_t * b = ast_node("Block",NULL); add_child($$,b); add_as_siblings(b,$7);}
    | IF Expr LBRACE RBRACE ELSE LBRACE RBRACE                                              {$$=ast_node("If",NULL); add_child($$,$2); add_child($$,ast_node("Block",NULL));add_child($$,ast_node("Block",NULL));}
    | FOR LBRACE Aux4 RBRACE                                                                {$$=ast_node("For",NULL); ast_node_t *b = ast_node("Block",NULL); add_child($$,b); add_as_siblings(b,$3);}
    | FOR LBRACE RBRACE                                                                     {$$=ast_node("For",NULL); add_child($$,ast_node("Block",NULL));}
    | FOR Expr LBRACE Aux4 RBRACE                                                           {$$=ast_node("For",NULL); add_child($$,$2); ast_node_t* b = ast_node("Block",NULL); add_child($$,b); add_as_siblings(b,$4); }
    | FOR Expr LBRACE RBRACE                                                                {$$=ast_node("For",NULL); add_child($$,$2);add_child($$,ast_node("Block",NULL));}
    | RETURN Expr                                                                           {$$=ast_node("Return",NULL);add_child($$,$2);}
    | RETURN                                                                                {$$=ast_node("Return",&$1); }
    | FuncInvocation                                                                        {$$=ast_node("Call",NULL); add_as_siblings($$,$1);} 
    | ParseArgs                                                                             {$$=$1;}
    | PRINT LPAR Expr RPAR                                                                  {$$=ast_node("Print",NULL); add_child($$,$3);}
    | PRINT LPAR STRLIT RPAR                                                                {$$=ast_node("Print",NULL); add_child($$,ast_node("StrLit",&$3));}
    | ID LPAR Expr aux3 RPAR                                                                {$$=ast_node("Call",NULL);add_child($$,ast_node("Id",&$1)); add_child($$,$3); add_as_siblings($$,$4);}
    ;

Aux4: Aux4 Statement SEMICOLON                                                              {$$=ast_node("Statement",NULL);  add_as_siblings($$,$1); add_child($$,$2);}
    | Statement SEMICOLON                                                                   {$$= ast_node("Statement",NULL); add_child($$,$1);} 
    | Aux4 error SEMICOLON                                                                  {$$=ast_node("error",NULL);add_as_siblings($$,$1); }
    | error SEMICOLON                                                                       {$$=NULL;}
    ;

ParseArgs: ID COMMA BLANKID ASSIGN PARSEINT LPAR CMDARGS LSQ Expr RSQ RPAR                  {$$=ast_node("ParseArgs",&$5);add_child($$,ast_node("Id",&$1)); add_child($$,$9);}
    | ID COMMA BLANKID ASSIGN PARSEINT LPAR CMDARGS LSQ error RSQ RPAR                      {$$=NULL;}
    ;

FuncInvocation: ID LPAR Expr RPAR                                                           {$$=ast_node("FuncInvocation",NULL);add_child($$,ast_node("Id",&$1)); add_child($$,$3);}
    | ID LPAR RPAR                                                                          {$$=ast_node("FuncInvocation",NULL);add_child($$,ast_node("Id",&$1));}
    | ID LPAR error RPAR                                                                    {$$=NULL;}
    ;

aux3: aux3 COMMA Expr                                                                       {$$=ast_node("aux",NULL);add_as_siblings($$,$1); add_child($$,$3); }
    | COMMA Expr                                                                            {$$=ast_node("aux",NULL); add_child($$,$2); }
    ;

Expr: Expr OR Expr                                                                          {$$=ast_node("Or",&$2);add_child($$,$1);add_child($$,$3);}
    | Expr AND Expr                                                                         {$$=ast_node("And",&$2);add_child($$,$1);add_child($$,$3);}
    | Expr LT Expr                                                                          {$$=ast_node("Lt",&$2);add_child($$,$1);add_child($$,$3);}
    | Expr GT Expr                                                                          {$$=ast_node("Gt",&$2);add_child($$,$1);add_child($$,$3);}
    | Expr EQ Expr                                                                          {$$=ast_node("Eq",&$2);add_child($$,$1);add_child($$,$3);}
    | Expr NE Expr                                                                          {$$=ast_node("Ne",&$2);add_child($$,$1);add_child($$,$3);}
    | Expr LE Expr                                                                          {$$=ast_node("Le",&$2);add_child($$,$1);add_child($$,$3);}
    | Expr GE Expr                                                                          {$$=ast_node("Ge",&$2);add_child($$,$1);add_child($$,$3);}
    | Expr PLUS Expr                                                                        {$$=ast_node("Add",&$2);add_child($$,$1);add_child($$,$3);}
    | Expr MINUS Expr                                                                       {$$=ast_node("Sub",&$2);add_child($$,$1);add_child($$,$3);}
    | Expr STAR  Expr                                                                       {$$=ast_node("Mul",&$2);add_child($$,$1);add_child($$,$3);}
    | Expr DIV Expr                                                                         {$$=ast_node("Div",&$2);add_child($$,$1);add_child($$,$3);}
    | Expr MOD Expr                                                                         {$$=ast_node("Mod",&$2);add_child($$,$1);add_child($$,$3);}
    | NOT Expr  %prec NOT                                                                   {$$=ast_node("Not",&$1);add_child($$,$2);}
    | MINUS Expr %prec NOT                                                                  {$$=ast_node("Minus",&$1);add_child($$,$2);}
    | PLUS Expr %prec NOT                                                                   {$$=ast_node("Plus",&$1);add_child($$,$2);}
    | INTLIT                                                                                {$$=ast_node("IntLit",&$1);}
    | REALLIT                                                                               {$$=ast_node("RealLit",&$1);}
    | ID                                                                                    {$$=ast_node("Id",&$1);}
    | FuncInvocation                                                                        {$$=ast_node("Call",NULL); add_as_siblings($$,$1);} 
    | ID LPAR Expr aux3 RPAR                                                                {$$=ast_node("Call",NULL);add_child($$,ast_node("Id",&$1)); add_child($$,$3); add_as_siblings($$,$4);}
    | LPAR Expr RPAR                                                                        {$$=$2;}
    | LPAR error RPAR                                                                       {$$=NULL;}
    ;

%%

int main(int argc, char *argv[]) {
    errors_ = 0;
    int i = 1;
    while (i < argc){
        if (strcmp(argv[i],"-l")==0 ){
            flag_l();
            yylex();
            yylex_destroy();
            return 0;
        }else{ if (strcmp(argv[i],"-t")==0 ){
            ast_flag = 1;   
    
  
          
        } else {if (strcmp(argv[i],"-s")==0 ){
            note_ast_flag = 1;
        }}
        i++;   
    }}       

    yyparse();
    if (ast_flag){
                print_ast(program, 0);
                free_ast(program);
            return 0;
    }
        symbol_table = vector();
            check_semantics(program);
        if(note_ast_flag && !syntax_errors){    
            print_symbol_table_vector(symbol_table);
            print_ast(program, 0);
        }
        if (errors_ == 0){
            generate_llvm(program);
        }

        free_ast(program);
        yylex_destroy();

    return 0;
}

void disable_ast_flag(){
    ast_flag = 0;
    syntax_errors = 1;
};

