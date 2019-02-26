%{
#include "0516021.h"
#include "0516021_generate.h"
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

Type *return_type;

bool last_return = false;

bool error = false;

int cnt_array_init = 0;
bool in_forwhile = false;

bool in_main = false;
int loop_index = 0;
FILE *fout;
loop_stack l_stack;
Type *idlist_type;
int label_index = 0;


bool calling_func = false;
AttrParam *now_param = NULL;
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
%type <i_num> dimension
%type <expr> logical_expression
%type <expr> logical_term
%type <expr> logical_factor
%type <expr> arithmetic_expression
%type <expr> relation_expression
%type <expr> term
%type <expr> factor
%type <str> relation_operator

%type <exprlist> initial_array
%type <exprlist> literal_list /* ? */
%type <exprlist> logical_expression_list
%type <expr> variable_reference
%type <expr> array_list
%union  {
  int i_num;
  double d_num;
  char* str;
  Type* type;
  Value* value;
  Attribute* attribute;
  AttrParam* attrparam;
  TypeDim* typedim;
  Expr* expr;
  ExprList* exprlist;
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

program : decl_list funct_def decl_and_def_list {FuncShouldDefine(symbol_table); PrintTable(symbol_table);}
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

funct_def : scalar_type ID L_PAREN R_PAREN L_BRACE
              {
                return_type = $1;
                if(FindEntryInGlobal(symbol_table, $2) == NULL){
                   Entry *new = BuildEntry($2, "function", symbol_table->level, $1, BuildFuncAttr(NULL));
                   new->def = true;
                   InsertTableEntry(symbol_table, new);
                   
                }
                else if(FindEntryInGlobal(symbol_table, $2)->def){
                  printf("##########Error at Line #%d: function has been defined.##########\n", linenum);
                  error = true;
                }
                else{ // has been declared  but not defined
                  FindEntryInGlobal(symbol_table, $2)->def = true;
                  CheckFuncParam(FindEntryInGlobal(symbol_table, $2)->attr->attrparam, NULL);
                }
                symbol_table->level++;
                InsertTableEntryFromFuncParamidList(symbol_table, fpidlist);
                symbol_table->level--;
                GenFuncInitialization(symbol_table, $2);
                ClearFuncParamidList();
                symbol_table->level ++; // #001
              }
            var_const_stmt_list R_BRACE 
              {
                if(last_return == false){
                    printf("##########Error at Line #%d: Last statement must be a return statement.##########\n", linenum);
                    error = true;
                }
                last_return = false;
                return_type = NULL;
                PrintTable(symbol_table);
                DeleteEntrys(symbol_table);
                symbol_table->level --; // #001
                GenFuncEnd();
                in_main = false;
              }
          | scalar_type ID L_PAREN parameter_list R_PAREN L_BRACE 
              {
                return_type = $1;
                if(FindEntryInGlobal(symbol_table, $2) == NULL){
                  Entry *new = BuildEntry($2, "function", symbol_table->level, $1, BuildFuncAttr($4));
                  new->def = true;
                  InsertTableEntry(symbol_table, new);

                }
                else if(FindEntryInGlobal(symbol_table, $2)->def){

                  printf("##########Error at Line #%d: function has been defined.##########\n", linenum);
                  error = true;

                }
                else{ // has been declared  but not defined

                  FindEntryInGlobal(symbol_table, $2)->def = true;

                  if(FindEntryInGlobal(symbol_table, $2)->attr == NULL){

                  }
                  CheckFuncParam(FindEntryInGlobal(symbol_table, $2)->attr->attrparam, $4);

                }
                symbol_table->level++;
                InsertTableEntryFromFuncParamidList(symbol_table, fpidlist);

                symbol_table->level--;
                GenFuncInitialization(symbol_table, $2);
                ClearFuncParamidList();
                symbol_table->level ++; // #002
              }
            var_const_stmt_list R_BRACE
              {
                if(last_return == false){
                    printf("##########Error at Line #%d: Last statement must be a return statement.##########\n", linenum);
                    error = true;
                }
                last_return = false;
                return_type = NULL;
                PrintTable(symbol_table);
                DeleteEntrys(symbol_table);
                symbol_table->level --; // #002
                GenFuncEnd();
                in_main = false;
              }
          | VOID ID L_PAREN R_PAREN L_BRACE
              {
                return_type = BuildType($1, NULL);
                if(FindEntryInGlobal(symbol_table, $2) == NULL){
                  Entry *new = BuildEntry($2, "function", symbol_table->level, BuildType($1, NULL), BuildFuncAttr(NULL));
                  new->def = true;
                  InsertTableEntry(symbol_table, new);         
                }
                else if(FindEntryInGlobal(symbol_table, $2)->def){
                  printf("##########Error at Line #%d: function has been defined.##########\n", linenum);
                  error = true;
                }
                else{ // has been declared  but not defined
                  FindEntryInGlobal(symbol_table, $2)->def = true;
                  CheckFuncParam(FindEntryInGlobal(symbol_table, $2)->attr->attrparam, NULL);
                }
                GenFuncInitialization(symbol_table, $2);
                symbol_table->level ++; // #003
              } 
            var_const_stmt_list R_BRACE
              {
                last_return = false;
                return_type = NULL; // useless(?) I think useful
                PrintTable(symbol_table);
                DeleteEntrys(symbol_table);
                symbol_table->level --; // #003
                GenProcedureEnd();
                in_main = false;
              }
          | VOID ID L_PAREN parameter_list R_PAREN L_BRACE
              {
                return_type = BuildType($1, NULL);
                if(FindEntryInGlobal(symbol_table, $2) == NULL){
                  Entry *new = BuildEntry($2, "function", symbol_table->level, BuildType($1, NULL), BuildFuncAttr($4));
                  new->def = true;
                  InsertTableEntry(symbol_table, new);                 
                }
                else if(FindEntryInGlobal(symbol_table, $2)->def){
                  printf("##########Error at Line #%d: function has been defined.##########\n", linenum);
                  error = true;
                }
                else{ // has been declared  but not defined
                  FindEntryInGlobal(symbol_table, $2)->def = true;
                  CheckFuncParam(FindEntryInGlobal(symbol_table, $2)->attr->attrparam, $4);
                }
                symbol_table->level++;
                InsertTableEntryFromFuncParamidList(symbol_table, fpidlist);
                symbol_table->level--;
                GenFuncInitialization(symbol_table, $2);
                ClearFuncParamidList();
                symbol_table->level ++; // #004
              } 
            var_const_stmt_list R_BRACE
              {
                last_return = false;
                return_type = NULL; // useful ?
                PrintTable(symbol_table);
                DeleteEntrys(symbol_table);
                symbol_table->level --; // #004
                GenProcedureEnd();
                in_main = false;
              }

          ;

funct_decl : scalar_type ID L_PAREN R_PAREN SEMICOLON
              {
                Entry *new = BuildEntry($2, "function", symbol_table->level, $1, BuildFuncAttr(NULL));
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
                Entry *new = BuildEntry($2, "function", symbol_table->level, BuildType($1, NULL), BuildFuncAttr(NULL));
                InsertTableEntry(symbol_table, new);

              }
           | VOID ID L_PAREN parameter_list R_PAREN SEMICOLON
              {
                Entry *new = BuildEntry($2, "function", symbol_table->level, BuildType($1, NULL), BuildFuncAttr($4));
                InsertTableEntry(symbol_table, new);

                ClearFuncParamidList(); // Inside parameter will AddTo
              }
           ;

parameter_list : parameter_list COMMA scalar_type ID  {$$ = AddToParam($1, BuildType(($3)->name, NULL)); fpidlist = AddToFuncParamidList(symbol_table, fpidlist, $4, $3);}
               | parameter_list COMMA scalar_type ID dim {$$ = AddToParam($1, BuildType(($3)->name, $5) ); fpidlist = AddToFuncParamidList(symbol_table, fpidlist, $4, BuildType(($3)->name, $5));}
               | scalar_type ID dim {$$ = AddToParam(NULL, BuildType(($1)->name, $3) ); fpidlist = AddToFuncParamidList(symbol_table, fpidlist, $2, BuildType(($1)->name, $3));}
               | scalar_type ID {$$ = AddToParam(NULL, BuildType(($1)->name, NULL)); fpidlist = AddToFuncParamidList(symbol_table, fpidlist, $2, $1);}
               ;

var_decl : scalar_type identifier_list SEMICOLON
            {

              InsertTableEntryFromidList(symbol_table, idlist, "variable", $1);
              GenIdListInGlobalVar(idlist);
              ClearidList();
            }
         ;

identifier_list : identifier_list COMMA ID {idlist = AddToidList(symbol_table, idlist, $3, NULL, NULL);}
                | identifier_list COMMA ID ASSIGN_OP logical_expression {idlist = AddToidList(symbol_table, idlist, $3, NULL, BuildExprAttribute($5)); GenInitialAssign(symbol_table, idlist,$3, idlist_type, $5);}
                | identifier_list COMMA ID dim ASSIGN_OP initial_array {idlist = AddToidList(symbol_table, idlist, $3, $4, BuildExprListAttribute($6));}
                | identifier_list COMMA ID dim {idlist = AddToidList(symbol_table, idlist, $3, $4, NULL);}
                | ID dim ASSIGN_OP initial_array {idlist = AddToidList(symbol_table, idlist, $1, $2, BuildExprListAttribute($4));}
                | ID dim {idlist = AddToidList(symbol_table, idlist, $1, $2, NULL);}
                | ID ASSIGN_OP logical_expression {idlist = AddToidList(symbol_table, idlist, $1, NULL, BuildExprAttribute($3)); GenInitialAssign(symbol_table, idlist, $1, idlist_type, $3);}
                | ID {idlist = AddToidList(symbol_table, idlist, $1, NULL, NULL);}
                ;

initial_array : L_BRACE literal_list R_BRACE { $$ = $2;}
              ;

literal_list : literal_list COMMA logical_expression { $$ = AddToExprList($1, $3);}
             | logical_expression { $$ = AddToExprList(NULL, $1);}
             | {$$ = NULL;}
             ;

const_decl : CONST scalar_type const_list SEMICOLON
              {
                InsertTableEntryFromidList(symbol_table, idlist, "constant", $2);
                ClearidList();
              }

const_list : const_list COMMA ID ASSIGN_OP literal_const {idlist = AddToidList(symbol_table, idlist, $3, NULL, BuildValueAttribute($5));}
           | ID ASSIGN_OP literal_const 
            {
              idlist = AddToidList(symbol_table, idlist, $1, NULL, BuildValueAttribute($3));
            }
           ;


dim : dim ML_BRACE INT_CONST MR_BRACE {$$ = AddToTypeDim($1, $3); /* declare or define*/}
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
                    | var_const_stmt_list var_decl {last_return = false;}
                    | var_const_stmt_list const_decl {last_return = false;}
                    |
                    ;

statement : compound_statement {last_return = false;}
          | simple_statement {last_return = false;}
          | conditional_statement {last_return = false;}
          | while_statement {last_return = false;}
          | for_statement {last_return = false;}
          | function_invoke_statement {last_return = false;}
          | jump_statement
          ;     

simple_statement : variable_reference ASSIGN_OP logical_expression SEMICOLON { CheckType($1, $3); GenVarAssign($1, $3);}
                 | PRINT {GenPrintInitialization();} logical_expression SEMICOLON {CheckPtRd($3); GenPrint($3);}
                 | READ variable_reference SEMICOLON {CheckPtRd($2); GenRead($2);}
                 ;
conditional_statement : IF L_PAREN logical_expression R_PAREN {push_stack(loop_index); loop_index++; GenIfBegin();} L_BRACE { CheckBool($3); symbol_table->level ++;} var_const_stmt_list R_BRACE {PrintTable(symbol_table);DeleteEntrys(symbol_table);symbol_table->level--;} conditional_else /* new statement */

conditional_else : ELSE {GenElse();} L_BRACE {symbol_table->level++;} var_const_stmt_list R_BRACE {GenIfEnd(); pop_stack();  PrintTable(symbol_table);DeleteEntrys(symbol_table);symbol_table->level--; }
                 | {GenIfNoElse();pop_stack();}
                 ;
while_statement : WHILE L_PAREN {push_stack(loop_index); loop_index++; GenControlBegin();} logical_expression R_PAREN
                    L_BRACE {CheckBool($4); symbol_table->level++; in_forwhile = true; GenWhileControl();} var_const_stmt_list R_BRACE {PrintTable(symbol_table);DeleteEntrys(symbol_table);symbol_table->level--; in_forwhile = false; GenWhileEnd(); pop_stack();}
                | DO L_BRACE {symbol_table->level++;  push_stack(loop_index); loop_index++; GenControlBegin(); in_forwhile = true;}
                    var_const_stmt_list
                  R_BRACE {PrintTable(symbol_table);DeleteEntrys(symbol_table);symbol_table->level--;} WHILE L_PAREN logical_expression R_PAREN SEMICOLON { CheckBool($9); in_forwhile = false; GenWhileControl(); GenWhileEnd(); pop_stack();}
                ;

for_statement : FOR {push_stack(loop_index); loop_index++;} L_PAREN initial_expression_list SEMICOLON {GenControlBegin();} control_expression_list SEMICOLON {GenForControl();} increment_expression_list {GenForIncEnd();} R_PAREN 
                    L_BRACE {symbol_table->level++; in_forwhile = true; GenForInside();} var_const_stmt_list R_BRACE {PrintTable(symbol_table);DeleteEntrys(symbol_table);symbol_table->level--; GenForEnd(); pop_stack(); in_forwhile = false;}
              ;

initial_expression_list : initial_expression
                        |
                        ;

initial_expression : initial_expression COMMA variable_reference ASSIGN_OP logical_expression { CheckType($3, $5); GenVarAssign($3,$5);}
                   | initial_expression COMMA logical_expression
                   | logical_expression
                   | variable_reference ASSIGN_OP logical_expression {CheckType($1, $3); GenVarAssign($1,$3);}

control_expression_list : control_expression
                        |
                        ;

control_expression : control_expression COMMA variable_reference ASSIGN_OP logical_expression {CheckType($3, $5); GenVarAssign($3,$5);}
                   | control_expression COMMA logical_expression {CheckBool($3);}
                   | logical_expression {CheckBool($1);}
                   | variable_reference ASSIGN_OP logical_expression {CheckType($1, $3); GenVarAssign($1,$3);}
                   ;

increment_expression_list : increment_expression 
                          |
                          ;

increment_expression : increment_expression COMMA variable_reference ASSIGN_OP logical_expression {CheckType($3, $5); GenVarAssign($3,$5);}
                     | increment_expression COMMA logical_expression
                     | logical_expression
                     | variable_reference ASSIGN_OP logical_expression {CheckType($1, $3);GenVarAssign($1,$3); }
                     ;

function_invoke_statement : ID L_PAREN {now_param = (FindVar(symbol_table, $1)->e->attr->attrparam);} logical_expression_list R_PAREN SEMICOLON 
                            { 
                              now_param = NULL;
                              if(FindEntryInGlobal(symbol_table, $1) == NULL){
                                printf("##########Error at Line #%d: function %s should be declared or defined.##########\n", linenum, $1);
                                error = true;
                              }
                              else{
                                  bool flag = true;
                                  ExprList *p = $4;
                                  while(p != NULL){
                                    if(p->expression == NULL){
                                      flag = false; // parameter crash
                                    }
                                    p = p->next;
                                  }
                                  if(flag){
                                    CheckFuncParam(FindEntryInGlobal(symbol_table, $1)->attr->attrparam, BuildExprListAttribute($4)->attrparam);
                                  }
                                GenFunction($1);
                              } 
                            }
                          | ID L_PAREN R_PAREN SEMICOLON 
                            { 
                              if(FindEntryInGlobal(symbol_table, $1) == NULL){
                                printf("##########Error at Line #%d: function %s should be declared or defined.##########\n", linenum, $1);
                                error = true;
                              }
                              else{
                                CheckFuncParam(FindEntryInGlobal(symbol_table, $1)->attr->attrparam, NULL);
                                GenFunction($1);
                              }
                            }
                          ;

jump_statement : CONTINUE SEMICOLON { CheckContinueBreak(in_forwhile); last_return = false;}
               | BREAK SEMICOLON { CheckContinueBreak(in_forwhile); last_return = false;}
               | RETURN logical_expression SEMICOLON { CheckFuncReturn(return_type, $2); last_return = true; GenReturn(return_type, $2);}
               ;

variable_reference : array_list {$$ = $1;}
                   | ID { $$ = FindVar(symbol_table, $1);}
                   ;


logical_expression : logical_expression OR_OP logical_term { $$ = Logic($1, $3, $2); GenOr();}
                   | logical_term {$$ = $1;}
                   ;

logical_term : logical_term AND_OP logical_factor { $$ = Logic($1, $3, $2); GenAnd();}
             | logical_factor {$$ = $1;}
             ;

logical_factor : NOT_OP logical_factor { $$ = Not($2, $1); GenNot();}
               | relation_expression {$$ = $1;}
               ;

relation_expression : relation_expression relation_operator arithmetic_expression {$$ = Relation($1,$3,$2); GenRelation($1,$3,$2);}
                    | arithmetic_expression {$$ = $1;}
                    ;

relation_operator : LT_OP {$$ = $1;}
                  | LE_OP {$$ = $1;}
                  | EQ_OP {$$ = $1;}
                  | GE_OP {$$ = $1;}
                  | GT_OP {$$ = $1;}
                  | NE_OP {$$ = $1;}
                  ;

arithmetic_expression : arithmetic_expression ADD_OP term { $$ = Arith($1, $3, $2); GenArith($1,$3,'+');}
                      | arithmetic_expression SUB_OP term { $$ = Arith($1, $3, $2); GenArith($1,$3,'-');}
                      | term {$$ = $1;}
                      ;

term : term MUL_OP factor { $$ = Arith($1, $3, $2); GenArith($1,$3,'*');}
     | term DIV_OP factor { $$ = Arith($1, $3, $2); GenArith($1,$3,'/');}
     | term MOD_OP factor { $$ = Mod($1, $3, $2); GenMod();}
     | factor {$$ = $1;}
     ;

factor : SUB_OP factor { $$ = Negative($2); GenNeg($2);}
       | literal_const { $$ = ConstToExpr($1); GenLiteralConst($1);}
       | variable_reference { $$ = $1; GenVariable($1);}
       | L_PAREN logical_expression R_PAREN { $$ = $2;}
       | ID L_PAREN {now_param = (FindVar(symbol_table, $1)->e->attr->attrparam);}  logical_expression_list R_PAREN 
          {
            now_param = NULL;
            if($4 == NULL){ // parameter is wrong not declared
              $$ = NULL;
            }
            else{
              Expr *e;
              if(FindEntryInGlobal(symbol_table, $1) == NULL){
                printf("##########Error at Line #%d: function %s should be declared or defined.##########\n", linenum, $1);
                error = true;
                e = NULL;
              }
              else{
                e = Function_invoke($1, $4, symbol_table);
                GenFunction($1);
              }
              $$ = e;
            }

          }
       | ID L_PAREN R_PAREN
          {
            Expr *e;
            if(FindEntryInGlobal(symbol_table, $1) == NULL){
              printf("##########Error at Line #%d: function %s should be declared or defined.##########\n", linenum, $1);
              error = true;
              e = NULL;
            }
            else{
              e = Function_invoke($1, NULL, symbol_table);
              GenFunction($1);
            }
            $$ = e;
          }
       ;

logical_expression_list : logical_expression_list COMMA logical_expression {$$ = AddToExprList($1, $3);
                              if(now_param != NULL){
                                if((strcmp(PrintType($3->type, 0), "int") == 0) && (strcmp(PrintType(now_param->type, 0), "float") == 0 || strcmp(PrintType(now_param->type, 0), "double") == 0)){
                                  fprintf(fout, "\ti2f\n");
                                }
                                now_param = now_param->next;
                              }
                            }
                        | logical_expression {$$ = AddToExprList(NULL, $1);

                              if(now_param != NULL){
                                if((strcmp(PrintType($1->type, 0), "int") == 0) && (strcmp(PrintType(now_param->type, 0), "float") == 0 || strcmp(PrintType(now_param->type, 0), "double") == 0)){
                                  fprintf(fout, "\ti2f\n");
                                }
                                now_param = now_param->next;
                              }
                            }
                        ;

array_list : ID dimension { $$ = FindVar(symbol_table, $1); 
                              if($$ == NULL){
                                  // ID doesn't declare   
                              }
                              else{
                                $$->dim = $2;
                                int cnt = 0;
                                TypeDim *p = $$->type->arr; // real
                                while(p != NULL){
                                  cnt ++;
                                  p = p->next;
                                }
                                if($$->dim > cnt){
                                  printf("##########Error at Line #%d: too many dimension for array %s.##########\n", linenum, $1);
                                  error = true;
                                  $$ = NULL;
                                } 
                              }
                            }
           ;

dimension : dimension ML_BRACE logical_expression MR_BRACE {
              if($3 != NULL && strcmp(PrintType($3->type, $3->dim), "int") != 0){
                printf("##########Error at Line #%d: array index should be int.##########\n", linenum);
                error = true;
              }
              $$ = $1+1;
            }
          | ML_BRACE logical_expression MR_BRACE {
              if($2 != NULL && strcmp(PrintType($2->type, $2->dim), "int") != 0){
                printf("##########Error at Line #%d: array index should be int.##########\n", linenum);
                error = true;
              }
              $$ = 1;
            }
          ;



scalar_type : INT {$$ = BuildType("int", NULL); idlist_type = BuildType("int", NULL);}
            | DOUBLE {$$ = BuildType("double", NULL); idlist_type = BuildType("double", NULL);}
            | STRING {$$ = BuildType("string", NULL); idlist_type = BuildType("string", NULL);}
            | BOOL {$$ = BuildType("bool", NULL); idlist_type = BuildType("bool", NULL);}
            | FLOAT {$$ = BuildType("float", NULL); idlist_type = BuildType("float", NULL);}
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
  FILE *fp;
  if(argc == 1){
    yyin = stdin;
  }
  else if(argc == 2){
    fp = fopen(argv[1], "r");
    if(fp == NULL){
      fprintf(stderr, "Open file error\n");
      exit(-1);
    }
    yyin = fp;
  }
  else{
    fprintf(stderr, "Usage: ./compiler [filename]\n");
  }

  fout = fopen("output.j", "w");
  if(fout == NULL){
    fprintf(stderr, "Open file error\n");
    exit(-1);
  }

  symbol_table = CreateTable();
  idlist = NULL;

  yyin = fp;
  Initialization();


  yyparse();   /* primary procedure of parser */

  if (Opt_Statistic){
    printf("\nfrequencies of identifiers:\n");
    print_id();
  }
  if(!error){
    fprintf( stdout, "\n|-------------------------------------------|\n" );
    fprintf( stdout, "| There is no syntactic and semantic error! |\n" );
    fprintf( stdout, "|-------------------------------------------|\n" );
  }


  exit(0);
}
