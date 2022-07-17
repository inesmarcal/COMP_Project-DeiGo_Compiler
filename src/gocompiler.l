%{
    //Inês Martins Marçal 2019215917
    //Noémia Quintano Mora Gonçalves 2019219433
    
    #include <string.h> 
    #include <stdio.h>
    #include "llvm.h"
    #include "ast.h"
    #include "y.tab.h"
   
    
    int flag = 0 ;
    int col = 1;

    void disable_ast_flag();
    int start_col = 1;
    int start_line = 1;
    int valid = 0;
    int semicolon_flag = 0;
    int error_flag = 0;
    
    void name_value(char * name, char* value){
        if (flag){
                printf("%s(%s)\n",name,value);
            
    }
    }
    void name(char * name){
        if (flag){
            printf("%s\n",name);
        }
       
    }
   void octal_error(char * value){
       printf("Line %d, column %d: invalid octal constant (%s)\n",yylineno,col,value);
   }

    void ilegal_char(char * value){
       printf("Line %d, column %d: illegal character (%s)\n",yylineno,col,value);
   }

   void unterminated_comment(){
       printf("Line %d, column %d: unterminated comment\n",start_line,start_col);}

   void invalid_escape_char(char * value){
        printf("Line %d, column %d: invalid escape sequence (%s)\n",start_line,col,value);
   }

   void unterminated_string(){
       printf("Line %d, column %d: unterminated string literal\n",start_line,start_col);} 
   
   int check_semicolon(){
       if (semicolon_flag) {
           semicolon_flag = 0;
           name("SEMICOLON");
           return 1;
        }
        return  0;
    }    
 
%}
%option yylineno
%X COMMENT STR COMMENT_SINGLE
digit				[0-9]
letter				[a-zA-Z]
BLANKID             		"_"
ID                  ({letter}|{BLANKID})({digit}|{letter}|{BLANKID})*
SEMICOLON           		;
COMMA               		,
ASSIGN              		"="
STAR                		"*"
DIV                 		"/"
MINUS               		"-"
PLUS                		"+"
EQ                  		"=="
GE                  		">="
GT                  		">"
LBRACE              		"{"
LE                  		"<="
LPAR                		"("
LSQ                 		"["
LT                   		"<"
MOD                  		"%"
NE                  		"!="
NOT                 		"!"
AND                 		"&&"
OR                             "||"
RBRACE    			            "}"
RPAR				            ")"
RSQ                            "]"
PACKAGE                        "package"
RETURN                         "return"
ELSE				           "else"
FOR                            "for"
IF                             "if"
VAR                            "var"
INT                            "int"
FLOAT32                        "float32"
BOOL                           "bool"
STRING                         "string"
PRINT                          "fmt.Println"
PARSEINT                       "strconv.Atoi"
FUNC                           "func"
CMDARGS                        "os.Args"
RESERVED			"range"|"break"|"case"|"chan"|"const"|"continue"|"default"|"defer"|"fallthrough"|"go"|"goto"|"import"|"interface"|"map"|"select"|"struct"|"switch"|"type"|"++"|"--"
INTLIT               "0"("x"|"X")([a-fA-F]|{digit})+|"0"[0-7]*|[1-9]{digit}* 
REALLIT               {digit}+"."{digit}*(("e"|"E")("+"|"-")?{digit}+)?|{digit}+("e"|"E")("+"|"-")?{digit}+|"."{digit}+(("e"|"E")("+"|"-")?{digit}+)?
INVOCT                  "0"([0-7]|[8-9])+ 


%%

"/*"    { start_col = col; start_line = yylineno; col+=yyleng; BEGIN COMMENT;}
<COMMENT>"*/"      {col+=yyleng; BEGIN 0;}
<COMMENT><<EOF>>    {unterminated_comment(); yyterminate();}
<COMMENT>.         {col+=yyleng;}
<COMMENT>\n         {col = 1;}

"//"                 {col+=yyleng; BEGIN COMMENT_SINGLE;}
<COMMENT_SINGLE><<EOF>> {yyterminate();}
<COMMENT_SINGLE>.*  {col+=yyleng;}
<COMMENT_SINGLE>\n  {col=1; BEGIN 0;}


"//".*$  {col+=yyleng;}     

{RESERVED}      { name_value("RESERVED",yytext);col+=yyleng;error_flag = 0; semicolon_flag = 0; if (!flag) return RESERVED; } 
{SEMICOLON}     { name("SEMICOLON");col+=yyleng;error_flag = 0; semicolon_flag = 0; if (!flag) return SEMICOLON; }
{COMMA}         { name("COMMA");col+=yyleng;error_flag = 0; semicolon_flag = 0; if (!flag) return COMMA; }
{BLANKID}       { name("BLANKID");col+=yyleng;error_flag = 0; semicolon_flag = 0; if (!flag) return BLANKID; }
{ASSIGN}        { yylval.token.column=col; yylval.token.line=yylineno; yylval.token.value = NULL; name("ASSIGN");col+=yyleng;error_flag = 0; semicolon_flag = 0; if (!flag) return ASSIGN; }
{STAR}          { yylval.token.column=col; yylval.token.line=yylineno; yylval.token.value = NULL;name("STAR");col+=yyleng;error_flag = 0; semicolon_flag = 0; if (!flag) return STAR; }
{DIV}           { yylval.token.column=col; yylval.token.line=yylineno; yylval.token.value = NULL;name("DIV");col+=yyleng;error_flag = 0; semicolon_flag = 0; if (!flag) return DIV; }   		
{MINUS}         { yylval.token.column=col; yylval.token.line=yylineno; yylval.token.value = NULL;name("MINUS");col+=yyleng;error_flag = 0; semicolon_flag = 0; if (!flag) return MINUS; }      	
{PLUS}          { yylval.token.column=col; yylval.token.line=yylineno; yylval.token.value = NULL;name("PLUS");col+=yyleng;error_flag = 0; semicolon_flag = 0; if (!flag) return PLUS;  }
{EQ}            { yylval.token.column=col; yylval.token.line=yylineno; yylval.token.value = NULL;name("EQ");col+=yyleng;error_flag = 0; semicolon_flag = 0; if (!flag) return EQ; }
{GE}            { yylval.token.column=col; yylval.token.line=yylineno; yylval.token.value = NULL;name("GE");col+=yyleng;error_flag = 0; semicolon_flag = 0;  if (!flag) return GE; }
{GT}            { yylval.token.column=col; yylval.token.line=yylineno; yylval.token.value = NULL;name("GT");col+=yyleng;error_flag = 0; semicolon_flag = 0; if (!flag) return GT; }
{LBRACE}        { name("LBRACE");col+=yyleng;error_flag = 0; semicolon_flag = 0; if (!flag) return LBRACE;  }
{LE}            {  yylval.token.column=col; yylval.token.line=yylineno; yylval.token.value = NULL; name("LE");col+=yyleng;error_flag = 0; semicolon_flag = 0; if (!flag) return LE; } 
{LPAR}          { name("LPAR");col+=yyleng;error_flag = 0; semicolon_flag = 0;if (!flag) return LPAR;  }   		
{LSQ}           { name("LSQ");col+=yyleng;error_flag = 0; semicolon_flag = 0;if (!flag) return LSQ;  }      	
{LT}            {  yylval.token.column=col; yylval.token.line=yylineno; yylval.token.value = NULL; name("LT");col+=yyleng;error_flag = 0; semicolon_flag = 0; if (!flag) return LT; }
{MOD}            {  yylval.token.column=col; yylval.token.line=yylineno; yylval.token.value = NULL; name("MOD");col+=yyleng;error_flag = 0; semicolon_flag = 0; if (!flag) return MOD; }
{NE}            {  yylval.token.column=col; yylval.token.line=yylineno; yylval.token.value = NULL; name("NE");col+=yyleng;error_flag = 0; semicolon_flag = 0; if (!flag) return NE;  }
{NOT}            {  yylval.token.column=col; yylval.token.line=yylineno; yylval.token.value = NULL; name("NOT");col+=yyleng;error_flag = 0; semicolon_flag = 0; if (!flag) return NOT; }
{AND}            {  yylval.token.column=col; yylval.token.line=yylineno; yylval.token.value = NULL; name("AND");col+=yyleng;error_flag = 0; semicolon_flag = 0; if (!flag) return AND; }
{OR}            {  yylval.token.column=col; yylval.token.line=yylineno; yylval.token.value = NULL; name("OR");col+=yyleng;error_flag = 0; semicolon_flag = 0; if (!flag) return OR; }
{RBRACE}         { name("RBRACE");col+=yyleng;error_flag = 0; semicolon_flag = 1; if (!flag) return RBRACE; }
{RPAR}           { name("RPAR");col+=yyleng;error_flag = 0; semicolon_flag = 1; if (!flag) return RPAR; } 
{RSQ}            { name("RSQ");col+=yyleng;error_flag = 0; semicolon_flag = 1; if (!flag) return RSQ; } 
{PACKAGE}        { name("PACKAGE");col+=yyleng;error_flag = 0; semicolon_flag = 0; if (!flag) return PACKAGE;}
{RETURN}         { yylval.token.column=col; yylval.token.line=yylineno; yylval.token.value = NULL; name("RETURN");col+=yyleng;error_flag = 0; semicolon_flag = 1; if (!flag) return RETURN;  }
{ELSE}			 { name("ELSE");col+=yyleng;error_flag = 0; semicolon_flag = 0; if (!flag) return ELSE;  }
{FOR}            { name("FOR");col+=yyleng;error_flag = 0; semicolon_flag = 0; if (!flag) return FOR;  }
{IF}             { name("IF");col+=yyleng;error_flag = 0; semicolon_flag = 0; if (!flag) return IF; }
{VAR}            { name("VAR");col+=yyleng;error_flag = 0; semicolon_flag = 0; if (!flag) return VAR; }
{INT}            { name("INT");col+=yyleng;error_flag = 0; semicolon_flag = 0; if (!flag) return INT; }
{FLOAT32}        { name("FLOAT32");col+=yyleng;error_flag = 0; semicolon_flag = 0; if (!flag) return FLOAT32; }
{BOOL}           { name("BOOL");col+=yyleng;error_flag = 0; semicolon_flag = 0; if (!flag) return BOOL; }
{STRING}         { name("STRING");col+=yyleng;error_flag = 0; semicolon_flag = 0; if (!flag) return STRING; }
{PRINT}          { name("PRINT");col+=yyleng;error_flag = 0; semicolon_flag = 0; if (!flag) return PRINT; }
{PARSEINT}       { yylval.token.column=col; yylval.token.line=yylineno; yylval.token.value = NULL; name("PARSEINT");col+=yyleng;error_flag = 0; semicolon_flag = 0; if (!flag) return PARSEINT; }
{FUNC}           { name("FUNC");col+=yyleng;error_flag = 0; semicolon_flag = 0; if (!flag) return FUNC; }
{CMDARGS}        { name("CMDARGS");col+=yyleng;error_flag = 0; semicolon_flag = 0; if (!flag) return CMDARGS; }
{INTLIT}         { yylval.token.column=col; yylval.token.line=yylineno; yylval.token.value = strdup(yytext); name_value("INTLIT",yytext);col+=yyleng;error_flag = 0; semicolon_flag = 1;  if (!flag) return INTLIT; }
{REALLIT}        { yylval.token.column=col; yylval.token.line=yylineno; yylval.token.value = strdup(yytext); name_value("REALLIT",yytext);col+=yyleng;error_flag = 0; semicolon_flag = 1; if (!flag) return REALLIT; }
{ID}             { yylval.token.column=col; yylval.token.line=yylineno; yylval.token.value = strdup(yytext); name_value("ID",yytext);col+=yyleng;error_flag = 0; semicolon_flag = 1; if (!flag) return ID; }
{INVOCT}         { octal_error(yytext);col+=yyleng; error_flag = 0; semicolon_flag = 0; }

\"  {yymore(); valid = 1; start_line = yylineno; error_flag = 0;  start_col = col; col++; BEGIN STR;}
<STR>\"   {if (valid) {  yylval.token.column=start_col; yylval.token.line=start_line; yylval.token.value = strdup(yytext); name_value("STRLIT",yytext); semicolon_flag = 1;} col+= 1; BEGIN 0; if (!flag && valid) { return STRLIT;}  } 
<STR>\\$  {invalid_escape_char(&yytext[yyleng-1]); unterminated_string();col=1; BEGIN 0;}
<STR>\n   {unterminated_string(); col = 1; BEGIN 0;}
<STR>\\   {valid = 0;invalid_escape_char(&yytext[yyleng-1]);col+=1; }
<STR>\\[^nftr\\\"] {valid = 0;invalid_escape_char(&yytext[yyleng-2]);col+=2; }
<STR>\\(n|f|t|r|\\|\")    {col+= 2; yymore(); }
<STR>.        {col+= 1; yymore(); }
<STR><<EOF>>  {unterminated_string(); yyterminate();}

"\n"    { start_line = yylineno - 1; start_col = col; error_flag=1; col = 1; if (check_semicolon()){if (!flag) {return SEMICOLON; }};  }

(" "|"\t")    {error_flag = 0;col+=yyleng;} 
<<EOF>>       {error_flag = 0;if (check_semicolon()){if (!flag) {return SEMICOLON; }}; yyterminate();}
.       { error_flag = 0;ilegal_char(yytext);col++;}   

%%

int yywrap()
{
return 1;
}
void yyerror (const char *s) { 
    if (error_flag){
       printf ("Line %d, column %d: %s: %s\n" , start_line, start_col , s, yytext );
    }else{


        if (col == 1){
            printf ("Line %d, column %d: %s: %s\n" , yylineno, col , s, yytext );
        }else{ 
     
               printf ("Line %d, column %d: %s: %s\n" , yylineno, col - yyleng, s, yytext );     
        }
    }
    disable_ast_flag();
}

void flag_l(){
    flag = 1;
}
