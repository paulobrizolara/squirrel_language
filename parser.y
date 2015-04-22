%{
#include <stdio.h>

#include <string.h> //strlen, strcat, ...
#include <stdlib.h> //malloc

    char  * concat(const char * str1, const char * str2){
        char * strConcat = malloc(strlen(str1) + strlen(str2) + 1);
        
        if(strConcat == NULL){
            return "";
        }
        
        strcpy(strConcat, str1);
        strcat(strConcat, str2);
        return strConcat;
    }
    char  * concat_n(int size, const char ** values){
        if(size <= 0){
            return "";
        }
        
        char * strConcat = strdup(values[0]);
        int i;
        for(i=1; i < size; ++i){
            char * new_concat = concat(strConcat, values[i]);
            free(strConcat);
            strConcat = new_concat;
        }
        
        return strConcat;
    }
    char * intToString(int value){
        char tmp[30];
        sprintf(tmp, "%d", value);
        return strdup(tmp);
    }

%}

%union {
	int    iValue; 	/* integer value */
	char   cValue; 	/* char value */
	char * sValue;  /* string value */
	};

%token <sValue> ID
%token <iValue> NUMBER
%token INT
%token LPAREN RPAREN LBRACE RBRACE
%token PRINT RETURN
%token <sValue> STRING 
%token SEMICOLON

%type <sValue> expr expr_list
%type <sValue> type
%type <sValue> inline_statement function_call io_command return_statement statement_list
%type <sValue> block_body block_stmt_list
%type <sValue> func_params function function_list

%start program

%%
program         : function_list                     { printf("%s\n", $1); } ;
	
/* *********************************** FUNCTIONS ********************************************** */
function_list   : function                          {   $$ = $1; }
		            | function_list	function        {   const char * values[] = {$1, "\n", $2};
                                                        $$ = concat_n(3, values); };

function        : type ID func_params block_body    {   
                                                        const char * values[] = {$1, " ", $2, " ", $3, $4};
                                                        $$ = concat_n(6, values); };

func_params     : LPAREN RPAREN                     { $$ = strdup("()"); };

/* ************************************* TYPES *********************************************** */
type : INT  { $$ = strdup("INT"); };

/* *********************************** STATEMENTS ******************************************** */
block_body       : LBRACE block_stmt_list RBRACE    {   char * begin_block = concat("{\n", $2);
                                                        $$ = concat(begin_block, "}\n"); };

block_stmt_list  : /* Vazio */                      {  $$ = ""; }
                    | statement_list                {  $$ = $1; };

statement_list   : inline_statement SEMICOLON       { $$ = concat($1, ";");}
                    | statement_list inline_statement SEMICOLON 
                                                    {   char * line1 = concat($1, "\n");
                                                        char * line2 = concat($2, ";\n");
                                                        $$ = concat(line1,line2); };

inline_statement : function_call                    {   $$ = $1; }
                    | return_statement              {   $$ = $1; };

function_call    : ID LPAREN expr_list RPAREN       {   const char * values[] = {$1, "(", $3, ")"};
                                                        $$ = concat_n(4, values); }
                    | io_command                    {   $$ = $1;};

io_command       : PRINT LPAREN expr RPAREN         {   $$ = concat(concat("print(", $3), ")"); }
                    | "read" LPAREN expr RPAREN     {   $$ = concat(concat("read(", $3), ")"); }
                    | "readchar" LPAREN RPAREN      {   $$ = strdup("readchar()"); }
                    | "readline" LPAREN RPAREN      {   $$ = strdup("readline()"); };


return_statement : RETURN expr                      {   $$ = concat("return ", $2); };

/* ********************************* EXPRESSIONS ********************************************* */
expr_list        : /* Vazio */                      { $$ = "";}
                    | expr                          { $$ = $1;};

expr             : NUMBER                           { $$ = intToString(yylval.iValue);} 
                    | STRING                        { $$ = strdup(yylval.sValue); }
                    | function_call                 { $$ = $1;};

%%

int main (void) {return yyparse ( );}

int yyerror (char *s) {fprintf (stderr, "%s\n", s);}
