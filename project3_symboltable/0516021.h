
#include <string.h>

extern int linenum;             /* declared in lex.l */
extern char *yytext;            /* declared by lex */
extern int Opt_Symbol;
extern void print_id();


typedef struct Table Table;
typedef struct Entry Entry;
typedef struct Type Type;
typedef struct TypeDim TypeDim;
typedef struct Attribute Attribute;
typedef struct AttrParam AttrParam;
typedef struct FuncParamidList FuncParamidList;


typedef struct Value Value;
typedef struct idList idList;
typedef struct id_struct id_struct;

extern idList *idlist;
extern FuncParamidList *fpidlist;

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
};
struct Type{
	char name[13];  // int float double bool string scientific
	TypeDim* arr;
};
struct TypeDim{
	int num;
	TypeDim *next;	// array many dimention
};
struct Attribute{
	Value  *value; // constant
	AttrParam *attrparam; // function parameter
	int dim;
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
	TypeDim *dim;
	Attribute *attr;
};


Table* CreateTable();
void PrintTable(Table*);
char* PrintType(Type*);
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
TypeDim* AddToTypeDim(TypeDim*, int);
AttrParam* AddToParam(AttrParam*, Type*);

Entry* FindEntryInCurrentScope(Table*, const char*);
Entry* FindEntryInGlobal(Table*, const char*);