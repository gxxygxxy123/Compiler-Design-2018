%{
#include <stdio.h>
#include <stdlib.h>
int yylex();
extern int linenum;             /* declared in lex.l */
extern FILE *yyin;              /* declared by lex */
extern char *yytext;            /* declared by lex */
extern char buf[256];           /* declared in lex.l */
int yyerror(char* msg);
%}

%token SEMICOLON    /* ; */
%token ID           /* identifier */


%token COMMA                       /* , */
%token LEFT_PARENTHESES            /* ( */
%token RIGHT_PARENTHESES           /* ) */
%token LEFT_SQUARE_BRACKETS        /* [ */
%token RIGHT_SQUARE_BRACKETS       /* ] */
%token LEFT_CURLY_BRACKETS         /* { */
%token RIGHT_CURLY_BRACKETS        /* } */

%token READ                 /* keyword */
%token BOOL                 /* keyword */
%token WHILE                /* keyword */
%token DO                   /* keyword */
%token IF                   /* keyword */

%nonassoc ELSE              /* keyword */

%token TRUE                 /* keyword */
%token FALSE                /* keyword */
%token FOR                  /* keyword */
%token CONST                /* keyword */
%token INT                  /* keyword */
%token PRINT                /* keyword */

%token VOID                 /* keyword */
%token FLOAT                /* keyword */
%token DOUBLE               /* keyword */
%token STRING               /* keyword */
%token CONTINUE             /* keyword */
%token BREAK                /* keyword */
%token RETURN               /* keyword */



%token STRING_FORM          /* string */
%token INT_FORM             /* integer */
%token FLOAT_FORM           /* float */
%token SCIENTIFIC

%left OR                    /* || */
%left AND                   /* && */
%left LESS                  /* < */
%left LESS_EQUAL            /* <= */
%left GREATER               /* > */
%left GREATER_EQUAL         /* >= */
%left EQUAL                 /* == */
%left NOT_EQUAL             /* != */
%left PLUS                  /* + */
%left MINUS                 /* - */
%left MULTIPLY              /* * */
%left DIVIDE                /* / */
%left MOD                   /* % */

%right ASSIGN               /* = */
%right NOT                  /* ! */

%%

program : declaration_list function_def decl_and_def_list
	;

decl_and_def_list	: decl_and_def_list function_decl
      | decl_and_def_list const_decl
		  | decl_and_def_list var_decl
			| decl_and_def_list function_def
      |
			;

declaration_list : declaration_list const_decl
                 | declaration_list var_decl
                 | declaration_list function_decl
                 |
                 ;

data_type : INT
          | DOUBLE
          | FLOAT
          | STRING
          | BOOL
          ; 

function_def : data_type ID LEFT_PARENTHESES some_argument RIGHT_PARENTHESES compound            /* data_t f(arg){...} */
             | procedure_def
             ;

procedure_def : VOID ID LEFT_PARENTHESES some_argument RIGHT_PARENTHESES compound         /* void f(arg){...} */
                     ;

statement : compound 
          | simple
          | conditional
          | while
          | for
          | jump
          ;

compound : LEFT_CURLY_BRACKETS compound_inside RIGHT_CURLY_BRACKETS             /* {...} */
         ;

simple : simple_statement SEMICOLON                      /* simple statement */
       ;


simple_statement : variable_reference ASSIGN expression  /* var = expression */
                 | PRINT expression                      /* print expression */
                 | READ variable_reference               /* read var */
                 ;

expression : MINUS expression %prec MULTIPLY             /* (1) */
           | expression MULTIPLY expression              /* (2) */
           | expression DIVIDE expression
           | expression MOD expression
           | expression PLUS expression                  /* (3) */
           | expression MINUS expression
           | expression LESS expression                  /* (4) */
           | expression LESS_EQUAL expression
           | expression EQUAL expression
           | expression GREATER_EQUAL expression
           | expression GREATER expression
           | expression NOT_EQUAL expression
           | NOT expression                              /* (5) */
           | expression AND expression                   /* (6) */
           | expression OR expression                    /* (7) */
           | LEFT_PARENTHESES expression RIGHT_PARENTHESES %prec MULTIPLY
           | literal_constant
           | variable_reference
           | function_invocation
           ;

while : WHILE LEFT_PARENTHESES boolean_expression RIGHT_PARENTHESES compound
      | DO compound WHILE LEFT_PARENTHESES boolean_expression RIGHT_PARENTHESES SEMICOLON
      ;

for : FOR LEFT_PARENTHESES initial_expression SEMICOLON control_expression SEMICOLON increment_expression RIGHT_PARENTHESES compound
    ;

jump : RETURN expression SEMICOLON
     | BREAK SEMICOLON
     | CONTINUE SEMICOLON
     ;

function_invocation : ID LEFT_PARENTHESES some_expression RIGHT_PARENTHESES
                    ;

some_expression : more_than_one_expression
                |
                ;

more_than_one_expression : more_than_one_expression COMMA expression    /* expressions seperated by comma */
                         | expression
                         ;

variable_reference : ID                                                 /* ID [exp][exp][...] */
                   | arr_reference
                   ;

arr_reference : ID more_arr_reference
              ;

more_arr_reference : more_arr_reference LEFT_SQUARE_BRACKETS expression RIGHT_SQUARE_BRACKETS
                     | LEFT_SQUARE_BRACKETS expression RIGHT_SQUARE_BRACKETS
                     ;

conditional : IF LEFT_PARENTHESES boolean_expression RIGHT_PARENTHESES compound ELSE compound 
            | IF LEFT_PARENTHESES boolean_expression RIGHT_PARENTHESES compound
            ;

boolean_expression : expression
                   ;

initial_expression : ID ASSIGN expression
                   | expression
                   |
                   ;

control_expression : ID ASSIGN expression
                   | expression
		               |
                   ;

increment_expression : ID ASSIGN expression
                     | expression
                     |
                     ;

compound_inside : compound_inside const_decl
                | compound_inside var_decl
                | compound_inside statement
                | 
                ;

const_decl : CONST data_type const_list SEMICOLON
           ;

const_list : const_list COMMA const
           | const
           ;

const : ID ASSIGN literal_constant
      ;

literal_constant : INT_FORM
                 | STRING_FORM
                 | FLOAT_FORM
                 | SCIENTIFIC
                 | TRUE
                 | FALSE
                 ;

var_decl : data_type one_or_more_identifier SEMICOLON
         ;

one_or_more_identifier : one_or_more_identifier COMMA identifier
                       | identifier
                       ;

identifier : non_initial_identifier
           | initial_identifier
           ;

non_initial_identifier : ID
                       | ID array
                       ;
initial_identifier : ID ASSIGN expression
                   | ID array ASSIGN initial_arr
                   ;

initial_arr : LEFT_CURLY_BRACKETS some_expression RIGHT_CURLY_BRACKETS
            ;

array : array LEFT_SQUARE_BRACKETS INT_FORM RIGHT_SQUARE_BRACKETS
      | LEFT_SQUARE_BRACKETS INT_FORM RIGHT_SQUARE_BRACKETS
      ;

procedure_decl : VOID ID LEFT_PARENTHESES some_argument RIGHT_PARENTHESES SEMICOLON
               ;

function_decl : data_type ID LEFT_PARENTHESES some_argument RIGHT_PARENTHESES SEMICOLON
              | procedure_decl
              ;

some_argument : one_or_more_argument
              |
              ;

one_or_more_argument : one_or_more_argument COMMA argument              /* seperated by comma */
                     | argument
                     ;
argument : data_type non_initial_identifier
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
}
int  main( int argc, char **argv )
{
	if( argc != 2 ) {
		fprintf(  stdout,  "Usage:  ./parser  [filename]\n"  );
		exit(0);
	}

	FILE *fp = fopen( argv[1], "r" );
	
	if( fp == NULL )  {
		fprintf( stdout, "Open  file  error\n" );
		exit(-1);
	}

	yyin = fp;
	yyparse();

	fprintf( stdout, "\n" );
	fprintf( stdout, "|--------------------------------|\n" );
	fprintf( stdout, "|  There is no syntactic error!  |\n" );
	fprintf( stdout, "|--------------------------------|\n" );
	exit(0);
}
