#include <stdio.h>
//#define limit_stack 100
typedef struct loop_stack loop_stack;
extern FILE *fout;
extern idList *idlist;
extern FuncParamidList *fpidlist;
extern Table *symbol_table;
extern bool calling_func;
extern AttrParam *now_param;
extern Type *return_type;
extern bool error;
extern int loop_index;
extern int label_index;
extern loop_stack l_stack;


struct loop_stack{
	int top;
	int stack[50];
};

void Initialization();



// variable(local or global) or constant variable
void GenVariable(Expr* e);

void GenLiteralConst(Value *v);

void GenArith(Expr* L, Expr* R, const char op);

void GenMod();
void GenNeg(Expr* R);
void GenOr();
void GenAnd();
void GenNot();

void GenRelation(Expr* L, Expr* R, const char* op);
void GenIdListInGlobalVar(idList *l);
void GenVarAssign(Expr* var, Expr* val); // variable = value
void GenInitialAssign(Table *table, idList *l, const char *L_name, Type *t, Expr *R); // int "a = 2", b, "c = d";

void GenPrintInitialization();
void GenReadInitialization();
void GenPrint(Expr* e);
void GenRead(Expr* e);
void GenFuncInitialization(Table *table, const char* name);
void GenFuncEnd();
void GenProcedureEnd();
void GenReturn(Type *, Expr* e);

void GenFunction(const char* name);
void push_stack(int value);
void pop_stack();
void GenIfBegin();
void GenElse();
void GenIfEnd();
void GenIfNoElse();
void GenControlBegin();
void GenForControl();
void GenForEnd();
void GenForInside();
void GenForIncEnd();
void GenWhileControl();
void GenWhileEnd();
