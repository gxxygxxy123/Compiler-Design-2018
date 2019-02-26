%{
#include "0516021.h"
#include <stdio.h>
#include <stdlib.h>

extern int linenum;
extern FILE *yyin;
extern char *yytext;
extern char buf[256];

int yylex();
int yyerror( char *msg );

extern int Opt_Statistic;


Table* symbol_table;
idList *idlist;
FuncParamidList *fpidlist;
%}

%token  <str> ID
%token  <i_num> INT_CONST
%token  <d_num> FLOAT_CONST
%token  <str> SCIENTIFIC
%token  <str> STR_CONST

%left  <str> LE_OP
%left  <str> NE_OP
%left  <str> GE_OP
%left  <str> EQ_OP
%left  <str> AND_OP
%left  <str> OR_OP

%token  <str> READ
%token  <str> BOOLEAN
%token  <str> WHILE
%token  <str> DO
%token  <str> IF
%token  <str> ELSE
%token  <str> TRUE
%token  <str> FALSE
%token  <str> FOR
%token  <str> INT
%token  <str> PRINT
%token  <str> BOOL
%token  <str> VOID
%token  <str> FLOAT
%token  <str> DOUBLE
%token  <str> STRING
%token  <str> CONTINUE
%token  <str> BREAK
%token  <str> RETURN
%token  <str> CONST

%token  <str> L_PAREN
%token  <str> R_PAREN
%token  <str> COMMA
%token  <str> SEMICOLON
%token  <str> ML_BRACE
%token  <str> MR_BRACE
%token  <str> L_BRACE
%token  <str> R_BRACE
%left  <str> ADD_OP
%left  <str> SUB_OP
%left  <str> MUL_OP
%left  <str> DIV_OP
%left  <str> MOD_OP
%right  <str> ASSIGN_OP
%left  <str> LT_OP
%left  <str> GT_OP
%right  <str> NOT_OP

%type <type> scalar_type
%type <value> literal_const
%type <attrparam> parameter_list
%type <typedim> dim


%union  {
  int i_num;
  double d_num;
  char* str;
  Type* type;
  Value* value;
  Attribute* attribute;
  AttrParam* attrparam;
  TypeDim* typedim;
}
/*  Program 
    Function 
    Array 
    Const 
    IF 
    ELSE 
    RETURN 
    FOR 
    WHILE
*/
%start program
%%

program : decl_list funct_def decl_and_def_list {PrintTable(symbol_table);}
        ;

decl_list : decl_list var_decl
          | decl_list const_decl
          | decl_list funct_decl
          |
          ;


decl_and_def_list : decl_and_def_list var_decl
                  | decl_and_def_list const_decl
                  | decl_and_def_list funct_decl
                  | decl_and_def_list funct_def
                  | 
                  ;

funct_def : scalar_type ID L_PAREN R_PAREN
              {
                if(FindEntryInGlobal(symbol_table, $2) == NULL){
                   Entry *new = BuildEntry($2, "function", symbol_table->level, $1, NULL);
                   InsertTableEntry(symbol_table, new);               
                }
              }
            compound_statement
          | scalar_type ID L_PAREN parameter_list R_PAREN  
              {
                if(FindEntryInGlobal(symbol_table, $2) == NULL){
                  Entry *new = BuildEntry($2, "function", symbol_table->level, $1, BuildFuncAttr($4));
                  InsertTableEntry(symbol_table, new);           
                }
                symbol_table->level++;
                InsertTableEntryFromFuncParamidList(symbol_table, fpidlist);
                symbol_table->level--;
                ClearFuncParamidList();

              }
            compound_statement
          | VOID ID L_PAREN R_PAREN
              {
                if(FindEntryInGlobal(symbol_table, $2) == NULL){
                  Entry *new = BuildEntry($2, "function", symbol_table->level, BuildType($1, NULL), NULL);
                  InsertTableEntry(symbol_table, new);                  
                }
              } 
            compound_statement
          | VOID ID L_PAREN parameter_list R_PAREN
              {
                if(FindEntryInGlobal(symbol_table, $2) == NULL){
                  Entry *new = BuildEntry($2, "function", symbol_table->level, BuildType($1, NULL), BuildFuncAttr($4));
                  InsertTableEntry(symbol_table, new);                 
                }
                symbol_table->level++;
                InsertTableEntryFromFuncParamidList(symbol_table, fpidlist);
                symbol_table->level--;
                ClearFuncParamidList();
              } 
            compound_statement

          ;

funct_decl : scalar_type ID L_PAREN R_PAREN SEMICOLON
              {
                Entry *new = BuildEntry($2, "function", symbol_table->level, $1, NULL);
                InsertTableEntry(symbol_table, new);
              }
           | scalar_type ID L_PAREN parameter_list R_PAREN SEMICOLON
              {
                Entry *new = BuildEntry($2, "function", symbol_table->level, $1, BuildFuncAttr($4));
                InsertTableEntry(symbol_table, new);

                ClearFuncParamidList(); // Inside parameter will AddT
              }
           | VOID ID L_PAREN R_PAREN SEMICOLON
              {
                Entry *new = BuildEntry($2, "function", symbol_table->level, BuildType($1, NULL), NULL);
                InsertTableEntry(symbol_table, new);

              }
           | VOID ID L_PAREN parameter_list R_PAREN SEMICOLON
              {
                Entry *new = BuildEntry($2, "function", symbol_table->level, BuildType($1, NULL), BuildFuncAttr($4));
                InsertTableEntry(symbol_table, new);

                ClearFuncParamidList(); // Inside parameter will AddTo
              }
           ;

parameter_list : parameter_list COMMA scalar_type ID  {$$ = AddToParam($1, BuildType(($3)->name, NULL)); fpidlist = AddToFuncParamidList(fpidlist, $4, $3);}
               | parameter_list COMMA scalar_type ID dim {$$ = AddToParam($1, BuildType(($3)->name, $5) ); fpidlist = AddToFuncParamidList(fpidlist, $4, BuildType(($3)->name, $5));}
               | scalar_type ID dim {$$ = AddToParam(NULL, BuildType(($1)->name, $3) ); fpidlist = AddToFuncParamidList(fpidlist, $2, BuildType(($1)->name, $3));}
               | scalar_type ID {$$ = AddToParam(NULL, BuildType(($1)->name, NULL)); fpidlist = AddToFuncParamidList(fpidlist, $2, $1);}
               ;

var_decl : scalar_type identifier_list SEMICOLON
            {
              InsertTableEntryFromidList(symbol_table, idlist, "variable", $1);
              ClearidList();
            }
         ;

identifier_list : identifier_list COMMA ID {idlist = AddToidList(idlist, $3, NULL, NULL);}
                | identifier_list COMMA ID ASSIGN_OP logical_expression {idlist = AddToidList(idlist, $3, NULL, NULL);}
                | identifier_list COMMA ID dim ASSIGN_OP initial_array {idlist = AddToidList(idlist, $3, $4, NULL);}
                | identifier_list COMMA ID dim {idlist = AddToidList(idlist, $3, $4, NULL);}
                | ID dim ASSIGN_OP initial_array {idlist = AddToidList(idlist, $1, $2, NULL);}
                | ID dim {idlist = AddToidList(idlist, $1, $2, NULL);}
                | ID ASSIGN_OP logical_expression {idlist = AddToidList(idlist, $1, NULL, NULL);}
                | ID {idlist = AddToidList(idlist, $1, NULL, NULL);}
                ;

initial_array : L_BRACE literal_list R_BRACE
              ;

literal_list : literal_list COMMA logical_expression
             | logical_expression
             | 
             ;

const_decl : CONST scalar_type const_list SEMICOLON
              {
                InsertTableEntryFromidList(symbol_table, idlist, "constant", $2);
                ClearidList();
              }

const_list : const_list COMMA ID ASSIGN_OP literal_const {idlist = AddToidList(idlist, $3, NULL, BuildConstAttr($5));}
           | ID ASSIGN_OP literal_const 
            {
              idlist = AddToidList(idlist, $1, NULL, BuildConstAttr($3));
            }
           ;


dim : dim ML_BRACE INT_CONST MR_BRACE {$$ = AddToTypeDim($1, $3);}
    | ML_BRACE INT_CONST MR_BRACE {$$ = AddToTypeDim(NULL, $2);}
    ;

compound_statement : L_BRACE {symbol_table->level ++;} var_const_stmt_list R_BRACE 
                      {
                        PrintTable(symbol_table);
                        DeleteEntrys(symbol_table);
                        symbol_table->level --;
                      }
                   ;

var_const_stmt_list : var_const_stmt_list statement 
                    | var_const_stmt_list var_decl
                    | var_const_stmt_list const_decl
                    |
                    ;

statement : compound_statement
          | simple_statement
          | conditional_statement
          | while_statement
          | for_statement
          | function_invoke_statement
          | jump_statement
          ;     

simple_statement : variable_reference ASSIGN_OP logical_expression SEMICOLON
                 | PRINT logical_expression SEMICOLON
                 | READ variable_reference SEMICOLON
                 ;
conditional_statement : IF L_PAREN logical_expression R_PAREN L_BRACE {symbol_table->level ++;} var_const_stmt_list R_BRACE {PrintTable(symbol_table);DeleteEntrys(symbol_table);symbol_table->level--;} conditional_else /* new statement */

conditional_else : ELSE L_BRACE {symbol_table->level++;} var_const_stmt_list R_BRACE {PrintTable(symbol_table);DeleteEntrys(symbol_table);symbol_table->level--;}
                 | 
                 ;
while_statement : WHILE L_PAREN logical_expression R_PAREN
                    L_BRACE {symbol_table->level++;} var_const_stmt_list R_BRACE {PrintTable(symbol_table);DeleteEntrys(symbol_table);symbol_table->level--;}
                | DO L_BRACE {symbol_table->level++;}
                    var_const_stmt_list
                  R_BRACE {PrintTable(symbol_table);DeleteEntrys(symbol_table);symbol_table->level--;} WHILE L_PAREN logical_expression R_PAREN SEMICOLON
                ;

for_statement : FOR L_PAREN initial_expression_list SEMICOLON control_expression_list SEMICOLON increment_expression_list R_PAREN 
                    L_BRACE {symbol_table->level++;} var_const_stmt_list R_BRACE {PrintTable(symbol_table);DeleteEntrys(symbol_table);symbol_table->level--;}
              ;

initial_expression_list : initial_expression
                        |
                        ;

initial_expression : initial_expression COMMA variable_reference ASSIGN_OP logical_expression
                   | initial_expression COMMA logical_expression
                   | logical_expression
                   | variable_reference ASSIGN_OP logical_expression

control_expression_list : control_expression
                        |
                        ;

control_expression : control_expression COMMA variable_reference ASSIGN_OP logical_expression
                   | control_expression COMMA logical_expression
                   | logical_expression
                   | variable_reference ASSIGN_OP logical_expression
                   ;

increment_expression_list : increment_expression 
                          |
                          ;

increment_expression : increment_expression COMMA variable_reference ASSIGN_OP logical_expression
                     | increment_expression COMMA logical_expression
                     | logical_expression
                     | variable_reference ASSIGN_OP logical_expression
                     ;

function_invoke_statement : ID L_PAREN logical_expression_list R_PAREN SEMICOLON
                          | ID L_PAREN R_PAREN SEMICOLON
                          ;

jump_statement : CONTINUE SEMICOLON
               | BREAK SEMICOLON
               | RETURN logical_expression SEMICOLON
               ;

variable_reference : array_list
                   | ID
                   ;


logical_expression : logical_expression OR_OP logical_term
                   | logical_term
                   ;

logical_term : logical_term AND_OP logical_factor
             | logical_factor
             ;

logical_factor : NOT_OP logical_factor
               | relation_expression
               ;

relation_expression : relation_expression relation_operator arithmetic_expression
                    | arithmetic_expression
                    ;

relation_operator : LT_OP
                  | LE_OP
                  | EQ_OP
                  | GE_OP
                  | GT_OP
                  | NE_OP
                  ;

arithmetic_expression : arithmetic_expression ADD_OP term
                      | arithmetic_expression SUB_OP term
                      | term
                      ;

term : term MUL_OP factor
     | term DIV_OP factor
     | term MOD_OP factor
     | factor
     ;

factor : SUB_OP factor
       | literal_const
       | variable_reference
       | L_PAREN logical_expression R_PAREN
       | ID L_PAREN logical_expression_list R_PAREN
       | ID L_PAREN R_PAREN
       ;

logical_expression_list : logical_expression_list COMMA logical_expression
                        | logical_expression
                        ;

array_list : ID dimension
           ;

dimension : dimension ML_BRACE logical_expression MR_BRACE         
          | ML_BRACE logical_expression MR_BRACE
          ;



scalar_type : INT {$$ = BuildType("int", NULL);}
            | DOUBLE {$$ = BuildType("double", NULL);}
            | STRING {$$ = BuildType("string", NULL);}
            | BOOL {$$ = BuildType("bool", NULL);}
            | FLOAT {$$ = BuildType("float", NULL);}
            ;
 
literal_const : INT_CONST {$$ = BuildValue("int", yytext);}
              | FLOAT_CONST {$$ = BuildValue("float", yytext);}
              | SCIENTIFIC {$$ = BuildValue("scientific", yytext);}
              | STR_CONST {$$ = BuildValue("string", $1);}
              | TRUE {$$ = BuildValue("bool", yytext);}
              | FALSE {$$ = BuildValue("bool", yytext);}
              ;


%%

int yyerror( char *msg )
{

    fprintf( stderr, "\n|--------------------------------------------------------------------------\n" );
    fprintf( stderr, "| Error found in Line #%d: %s\n", linenum, buf );
    fprintf( stderr, "|\n" );
    fprintf( stderr, "| Unmatched token: %s\n", yytext );
    fprintf( stderr, "|--------------------------------------------------------------------------\n" );
    exit(-1);
    //  fprintf( stderr, "%s\t%d\t%s\t%s\n", "Error found in Line ", linenum, "next token: ", yytext );
}

int  main(int argc, char **argv){
  if(argc == 1){
    yyin = stdin;
  }
  else if(argc == 2){
    FILE *fp = fopen(argv[1], "r");
    if(fp == NULL){
      fprintf(stderr, "Open file error\n");
      exit(-1);
    }
    yyin = fp;
  }
  else{
    fprintf(stderr, "Usage: ./parser [filename]\n");
  }

  symbol_table = CreateTable();
  idlist = NULL;

  yyparse();   /* primary procedure of parser */

  if (Opt_Statistic){
    printf("\nfrequencies of identifiers:\n");
    print_id();
  }

  fprintf( stdout, "\n|--------------------------------|\n" );
  fprintf( stdout, "|  There is no syntactic error!  |\n" );
  fprintf( stdout, "|--------------------------------|\n" );

  exit(0);
}
