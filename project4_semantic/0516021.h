#include <string.h>
#include <stdbool.h>
extern int linenum;             /* declared in lex.l */
extern char *yytext;            /* declared by lex */
extern int Opt_Symbol;
extern bool error;
extern bool last_return;
extern void print_id();


typedef struct Table Table;
typedef struct Entry Entry;
typedef struct Type Type;
typedef struct TypeDim TypeDim;
typedef struct Attribute Attribute;
typedef struct AttrParam AttrParam;
typedef struct FuncParamidList FuncParamidList;
typedef struct ExprList ExprList;
typedef struct ExprList_head ExprList_head;
typedef struct Value Value;
typedef struct idList idList;
typedef struct id_struct id_struct;

typedef struct Expr Expr;
extern idList *idlist;
extern FuncParamidList *fpidlist;
extern Table *symbol_table;
typedef struct Table{
	int level;
	int size;
	Entry *p;
}Table;
struct Entry {
	char name[33];  // 1~32 char
	char kind[10];	// variable function constant parameter
	int level;
	Type* type;
	Attribute* attr;
	Entry *next;
	bool def;
};
struct Type{
	char name[13];  // int float double bool string scientific void??
	TypeDim* arr;
};
struct TypeDim{
	int num;
	TypeDim *next;	// array many dimention
};
struct Attribute{
	Value  *value; // constant
	AttrParam *attrparam; // function parameter
	//int dim;
};
struct AttrParam{
	Type *type;	//	function-kind have multiple parameter i.e. attribute
	AttrParam *next;
};
struct FuncParamidList{ // parameter of function linked list
	char id[33];
	Type *type;
	FuncParamidList *next;
};
struct Value{	// attribute of constant
	Type* type;
	int i;
	double d;
	char* str;
};
struct idList{ // id linked list
	id_struct *ptr;
	idList *next;
};
struct id_struct{
	char id[33];
	TypeDim *dim; // LHS of Array
	Attribute *a; // RHS of ASSIGN
};
struct Expr{
	Type *type;
	Entry *e;
	char id[33];
	int dim; // current dimension
	AttrParam *param; // for function parameter list
};
struct ExprList{
	Expr *expression;
	ExprList *next;
	bool error;
};
struct ExprList_head{
	Expr *expression;
	ExprList *next;
	bool error;
};
Table* CreateTable();
void PrintTable(Table*);
char* PrintType(Type*, int);
void PrintAttribute(Attribute*);
void DeleteEntrys(Table*);

void InsertTableEntry(Table*, Entry*);
void InsertTableEntryFromidList(Table*, idList*, const char*, Type*);
void InsertTableEntryFromFuncParamidList(Table*, FuncParamidList*);

Entry* BuildEntry(const char*, const char*, int , Type*, Attribute*);

Type* BuildType(const char*, TypeDim*);

idList* AddToidList(idList*, const char*, TypeDim*, Attribute*);
FuncParamidList* AddToFuncParamidList(FuncParamidList*, const char*, Type*);

void ClearidList();
void ClearFuncParamidList();

Value* BuildValue(const char*, char*);
Attribute* BuildConstAttr(Value*);
Attribute* BuildFuncAttr(AttrParam*);
Value* BuildExprValue(Expr*);
Value* BuildExprListValue(ExprList*);
Attribute* BuildExprListAttribute(ExprList*);
Attribute* BuildExprAttribute(Expr*);
Attribute* BuildValueAttribute(Value*);
TypeDim* AddToTypeDim(TypeDim*, int);
AttrParam* AddToParam(AttrParam*, Type*);

Entry* FindEntryInCurrentScope(Table*, const char*);
Entry* FindEntryInGlobal(Table*, const char*);
Entry* FindEntryInLocal(Table*, const char*);
Entry* FindEntryInAllClosest(Table*, const char*);
void CheckFuncReturn(Type*, Expr*);
Expr *Arith(Expr*, Expr*, char*);
Expr *Mod(Expr*, Expr*, char*);
Expr *Logic(Expr*, Expr*, char*);
Expr *Not(Expr*, char*);
Expr *Negative(Expr*);
Expr *Relation(Expr*, Expr*, char*);
Expr *FindVar(Table*, const char*);
Expr *ConstToExpr(Value*);
Expr *Function_invoke(const char*, ExprList*, Table*);
AttrParam *CloneExprList(ExprList*);
TypeDim *CloneTypeDim(TypeDim*);
bool CheckFuncParam(AttrParam*, AttrParam*);
void CheckPtRd(Expr*);
void CheckContinueBreak(bool);
void CheckType(Expr*, Expr*);
void CheckBool(Expr*);
ExprList *AddToExprList(ExprList*, Expr*);
void FuncShouldDefine(Table*);