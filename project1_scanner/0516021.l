%{
#define LIST                { strncat(buf, yytext, sizeof(buf) - strlen(buf) - 1); }
#define token(t)            { LIST; if (Opt_Token) printf("<%s>\n", #t); }
#define tokenString(t, s)   { LIST; if (Opt_Token) printf("<%s:%s>\n", t, s); }
#define MAX_LINE_LENGTH 257
#define MAX_ID_LENGTH 257
int Opt_Source = 1;
int Opt_Token = 1;
int Opt_Statistic = 1;
int linenum = 1;
char buf[MAX_LINE_LENGTH];

char string_buf[MAX_LINE_LENGTH];
typedef struct statistic{
    char id[MAX_ID_LENGTH];
    int times;
    struct statistic *next;
}ID;

ID *head = NULL;

void add_id(char *);
void print_id(void);
%}

digit [0-9]
float {digit}+\.{digit}+
integer (0|([1-9]{digit}*))
scientific ({float}|0*{integer})[eE][\-\+]?{digit}+
letter [a-zA-Z_]
space [ \t]+


delim [,;\(\)\[\]\{\}]
keyword ("while"|"do"|"if"|"else"|"true"|"false"|"for"|"int"|"print"|"const"|"read"|"boolean"|"bool"|"void"|"float"|"double"|"string"|"continue"|"break"|"return")
id {letter}({letter}|{digit})*

%x PRAGMA
%x PRAGMA_COMMENT
%x STRING
%x COMMENT_ONELINE
%x COMMENT_STAR
%x SOURCE
%x STATISTIC
%x TOKEN

%%

"+" {
    token("+");
}
"-" {
    token("-");
}
"*" {
    token("*");
}
"/" {
    token("/");
}
"%" {
    token("%");
}
"=" {
    token("=");
}
"<" {
    token("<");
}
"<=" {
    token("<=");
}
"!=" {
    token("!=");
}
">=" {
    token(">=");
}
">" {
    token(">");
}
"==" {
    token("==");
}
"&&" {
    token("&&");
}
"||" {
    token("||");
}
"!" {
    token("!");
}

{keyword} {
    tokenString("KW", yytext);
}
{id} {
    tokenString("id", yytext);
    add_id(yytext);
}
{float} {
    tokenString("float", yytext);
}
{integer} {
    tokenString("integer",yytext);
}
{delim} {
    tokenString("delim",yytext);
}
{scientific} {
    tokenString("scientific",yytext);
}

"/*" {
    LIST;
    BEGIN COMMENT_STAR;
}
<COMMENT_STAR>"*/" {
    LIST;
    BEGIN INITIAL;
}
<COMMENT_STAR>. {
    LIST;
}
<COMMENT_STAR>"\n" {
    LIST;
    if(Opt_Source){
        printf("%d:%s\n", linenum, buf);
    }
    linenum ++;
    buf[0] = '\0';
}
"//" {
    LIST;
    BEGIN COMMENT_ONELINE;
}
<COMMENT_ONELINE>"\n" {
    LIST;
    if(Opt_Source){
        printf("%d:%s\n", linenum, buf);
    }
    linenum ++;
    buf[0] = '\0';
    BEGIN INITIAL;
}
<COMMENT_ONELINE>. {
    LIST;
}

\" {
    string_buf[0] = '\0';
    LIST;
    BEGIN STRING;
}

<STRING>\" {    // string end
    tokenString("string", string_buf);
    BEGIN INITIAL;
}
<STRING>\\\" {  // In the string, actually the single double quotes in C/C++
    LIST;
    strncat(string_buf, "\"", sizeof(string_buf)-strlen(buf)-1);
}
<STRING>\\\\ {  // In the string, actually the single back slash in C/C++
    LIST;
    strncat(string_buf, "\\", sizeof(string_buf)-strlen(buf)-1);
}
<STRING>. { // Others
    LIST;
    strncat(string_buf, yytext, sizeof(buf)-strlen(buf)-1);
}



^"#pragma" {
    LIST;
    BEGIN PRAGMA;
}
<PRAGMA>{space}"source" {
    LIST;
    BEGIN SOURCE;
}
<PRAGMA>{space}"token" {
    LIST;
    BEGIN TOKEN;
}
<PRAGMA>{space}"statistic" {
    LIST;
    BEGIN STATISTIC;
}

<SOURCE>{space}"on"({space})*("/*") {
    LIST;
    Opt_Source = 1;
    BEGIN COMMENT_STAR;
}
<SOURCE>{space}"off"({space})*("/*") {
    LIST;
    Opt_Source = 0;
    BEGIN COMMENT_STAR;
}
<SOURCE>{space}"on"({space})*("//") {
    LIST;
    Opt_Source = 1;
    BEGIN COMMENT_ONELINE;
}
<SOURCE>{space}"off"({space})*("//") {
    LIST;
    Opt_Source = 0;
    BEGIN COMMENT_ONELINE;
}
<SOURCE>{space}"on"({space})*"\n" {
    LIST;
    Opt_Source = 1;
    if(Opt_Source){
        printf("%d:%s\n", linenum, buf);
    }
    linenum ++;
    buf[0] = '\0';
    BEGIN INITIAL;
}
<SOURCE>{space}"off"({space})*"\n" {
    LIST;
    Opt_Source = 0;
    if(Opt_Source){
        printf("%d:%s\n", linenum, buf);
    }
    linenum ++;
    buf[0] = '\0';
    BEGIN INITIAL;
}
<SOURCE>. {
    fprintf(stderr, "Error at line %d: %s\n", linenum, yytext);
    exit(1);
}

<TOKEN>{space}"on"({space})*("/*") {
    LIST;
    Opt_Token = 1;
    BEGIN COMMENT_STAR;
}
<TOKEN>{space}"off"({space})*("/*") {
    LIST;
    Opt_Token = 0;
    BEGIN COMMENT_STAR;
}
<TOKEN>{space}"on"({space})*("//") {
    LIST;
    Opt_Token = 1;
    BEGIN COMMENT_ONELINE;
}
<TOKEN>{space}"off"({space})*("//") {
    LIST;
    Opt_Token = 0;
    BEGIN COMMENT_ONELINE;
}
<TOKEN>{space}"on"({space})*"\n" {
    LIST;
    Opt_Token = 1;
    if(Opt_Source){
        printf("%d:%s\n", linenum, buf);
    }
    linenum ++;
    buf[0] = '\0';
    BEGIN INITIAL;
}
<TOKEN>{space}"off"({space})*"\n" {
    LIST;
    Opt_Token = 0;
    if(Opt_Source){
        printf("%d:%s\n", linenum, buf);
    }
    linenum ++;
    buf[0] = '\0';
    BEGIN INITIAL;
}
<TOKEN>. {
    fprintf(stderr, "Error at line %d: %s\n", linenum, yytext);
    exit(1);
}

<STATISTIC>{space}"on"({space})*("/*") {
    LIST;
    Opt_Statistic = 1;
    BEGIN COMMENT_STAR;
}
<STATISTIC>{space}"off"({space})*("/*") {
    LIST;
    Opt_Statistic = 0;
    BEGIN COMMENT_STAR;
}
<STATISTIC>{space}"on"({space})*("//") {
    LIST;
    Opt_Statistic = 1;
    BEGIN COMMENT_ONELINE;
}
<STATISTIC>{space}"off"({space})*("//") {
    LIST;
    Opt_Statistic = 0;
    BEGIN COMMENT_ONELINE;
}
<STATISTIC>{space}"on"({space})*"\n" {
    LIST;
    Opt_Statistic = 1;
    if(Opt_Source){
        printf("%d:%s\n", linenum, buf);
    }
    linenum ++;
    buf[0] = '\0';
    BEGIN INITIAL;
}
<STATISTIC>{space}"off"({space})*"\n" {
    LIST;
    Opt_Statistic = 0;
    if(Opt_Source){
        printf("%d:%s\n", linenum, buf);
    }
    linenum ++;
    buf[0] = '\0';
    BEGIN INITIAL;
}
<STATISTIC>. {
    fprintf(stderr, "Error at line %d: %s\n", linenum, yytext);
    exit(1);
}

[ \t] {
    LIST;
}

\n {
    LIST;
    if (Opt_Source)
        printf("%d:%s\n", linenum, buf);
    ++linenum;
    buf[0] = '\0';
}

. { // Unrecognized character
    fprintf(stderr, "Error at line %d: %s\n", linenum, yytext);
    exit(1);
}

%%


void add_id(char *id){
    ID *p = head;
    if(p == NULL){
        ID *tmp = (ID*)malloc(sizeof(ID));
        strcpy(tmp->id, id);
        tmp->times = 1;
        tmp->next = NULL;
        head = tmp;
        return;
    }
    while(p != NULL){
        if(strcmp(p->id, id) == 0){
            p->times ++;
            return;
        }
        if(p->next == NULL){ // insert at the end
            ID *tmp = (ID*)malloc(sizeof(ID));
            strcpy(tmp->id, id);
            tmp->times = 1;
            tmp->next = NULL;
            p->next = tmp;
            return;
        }
        else{
            p = p->next;
        }

    }
}

void print_id(void){
    ID *p = head;
    while(p != NULL){
        printf("%s\t%d\n", p->id, p->times);
        p = p->next;
    }
}

int main( int argc, char **argv )
{
    if (argc >= 2)
    {
        yyin = fopen( argv[1], "r" );
        if ( NULL == yyin ) {
            fprintf( stderr, "Cannot open: %s\n", argv[1] );
            exit(-1);
        }
    }
    else
    {
        yyin = stdin;
    }

    yylex();

    if (Opt_Statistic)
    {
        // Print frequency table
        printf("frequencies of identifiers:\n");
        print_id();
    }

    exit(0);
}
