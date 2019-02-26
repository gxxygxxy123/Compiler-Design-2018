#include "0516021.h"
#include "0516021_generate.h"
#include <stdlib.h>
#include <string.h>

void Initialization(){

	fprintf(fout, ".class public output\n");
	fprintf(fout, ".super java/lang/Object\n");
	fprintf(fout, ".field public static _sc Ljava/util/Scanner;\n");
	label_index = 0;
	loop_index = 0;
	l_stack.top = -1;
}


// variable(local or global) or constant variable
void GenVariable(Expr* e){
	if(e == NULL){
		return;
	}
	else if(strcmp(e->e->kind, "variable") == 0 || strcmp(e->e->kind, "parameter") == 0){
		if(e->e->level == 0){
  			if(strcmp(PrintType(e->e->type, 0), "int") == 0){
  				fprintf(fout, "\tgetstatic output/%s I\n", e->e->name);
  				/*if(now_param != NULL){
  					if(strcmp(PrintType(now_param->type, 0), "float") == 0 || strcmp(PrintType(now_param->type, 0), "double") == 0){
  						fprintf(fout, "\ti2f\n");
  					}
  				}*/
  			}
   			else if(strcmp(PrintType(e->e->type, 0), "bool") == 0){
  				fprintf(fout, "\tgetstatic output/%s Z\n", e->e->name);
  			}
  			else if(strcmp(PrintType(e->e->type, 0), "float") == 0 || strcmp(PrintType(e->e->type, 0), "double") == 0){
  				fprintf(fout, "\tgetstatic output/%s F\n", e->e->name);
  			}
		}
		else{
			if(strcmp(PrintType(e->e->type, 0), "int") == 0){
				fprintf(fout, "\tiload %d\n", e->e->local_index);
  				/*if(now_param != NULL){
  					if(strcmp(PrintType(now_param->type, 0), "float") == 0 || strcmp(PrintType(now_param->type, 0), "double") == 0){
  						fprintf(fout, "\ti2f\n");
  					}
  				}*/
			}
			else if(strcmp(PrintType(e->e->type, 0), "bool") == 0){
				fprintf(fout, "\tiload %d\n", e->e->local_index);
			}
			else if(strcmp(PrintType(e->e->type, 0), "float") == 0 || strcmp(PrintType(e->e->type, 0), "double") == 0){
				fprintf(fout, "\tfload %d\n", e->e->local_index);
			}
		}
	}
	else if(strcmp(e->e->kind, "constant") == 0){
		if(strcmp(PrintType(e->e->type, 0), "int") == 0){
			fprintf(fout, "\tldc %d\n", e->e->attr->value->i);
		}
		else if(strcmp(PrintType(e->e->type, 0), "bool") == 0 && strcmp(e->e->attr->value->str, "true") == 0){
			fprintf(fout, "\ticonst_1\n");
		}
		else if(strcmp(PrintType(e->e->type, 0), "bool") == 0 && strcmp(e->e->attr->value->str, "false") == 0){
			fprintf(fout, "\ticonst_0\n");
		}
		else if(strcmp(PrintType(e->e->type, 0), "float") == 0){
			fprintf(fout, "\tldc %f\n", e->e->attr->value->d);
		}
		else if(strcmp(PrintType(e->e->type, 0), "double") == 0){
			fprintf(fout, "\tldc %f\n", e->e->attr->value->d);
		}
		else if(strcmp(PrintType(e->e->type, 0), "string") == 0){
			fprintf(fout, "\tldc \"%s\"\n", e->e->attr->value->str);
		}
	}
}

void GenLiteralConst(Value *v){
	if(v == NULL){
		return;
	}
	else{
		if(strcmp(PrintType(v->type, 0), "int") == 0){
			fprintf(fout, "\tldc %d\n", v->i);   // sipush ? both ok ?
		}
		else if(strcmp(PrintType(v->type, 0), "bool") == 0 && strcmp(v->str, "true") == 0){
			fprintf(fout, "\ticonst_1\n");
		}
		else if(strcmp(PrintType(v->type, 0), "bool") == 0 && strcmp(v->str, "false") == 0){
			fprintf(fout, "\ticonst_0\n");
		}
		else if(strcmp(PrintType(v->type, 0), "float") == 0){
			fprintf(fout, "\tldc %f\n", v->d);
		}
		else if(strcmp(PrintType(v->type, 0), "double") == 0){
			fprintf(fout, "\tldc %f\n", v->d);
		}

		else if(strcmp(PrintType(v->type, 0), "string") == 0){
			fprintf(fout, "\tldc \"%s\"\n", v->str);
		}
	}
}

void GenArith(Expr* L, Expr* R, const char op){
	if(L == NULL || R == NULL)
		return;
	else if(op == '+'){
		if(strcmp(PrintType(L->type, L->dim), "int") == 0 && strcmp(PrintType(R->type, R->dim), "int") == 0){
	    	fprintf(fout, "\tiadd\n");
	    }
	    else if((strcmp(PrintType(L->type, L->dim), "float") == 0 || strcmp(PrintType(L->type, L->dim), "double") == 0 )&& (strcmp(PrintType(R->type, R->dim), "float") == 0 || strcmp(PrintType(R->type, R->dim), "double") == 0)){
	    	fprintf(fout, "\tfadd\n");
	    }
	    else if(strcmp(PrintType(L->type, L->dim), "int") == 0 && (strcmp(PrintType(R->type, R->dim), "float") == 0 || strcmp(PrintType(R->type, R->dim), "double") == 0)){
	    	fprintf(fout, "\tfstore 99\n");
	    	fprintf(fout, "\ti2f\n");
	    	fprintf(fout, "\tfload 99\n");
	    	fprintf(fout, "\tfadd\n");
	    }
	    else if((strcmp(PrintType(L->type, L->dim), "float") == 0 || strcmp(PrintType(L->type, L->dim), "double") == 0)&& strcmp(PrintType(R->type, R->dim), "int") == 0){
	    	fprintf(fout, "\ti2f\n");
	    	fprintf(fout, "\tfadd\n");
	    }
	}
	else if(op == '-'){
		if(strcmp(PrintType(L->type, L->dim), "int") == 0 && strcmp(PrintType(R->type, R->dim), "int") == 0){
	    	fprintf(fout, "\tisub\n");
	    }
	    else if((strcmp(PrintType(L->type, L->dim), "float") == 0 || strcmp(PrintType(L->type, L->dim), "double") == 0 )&& (strcmp(PrintType(R->type, R->dim), "float") == 0 || strcmp(PrintType(R->type, R->dim), "double") == 0)){
	    	fprintf(fout, "\tfsub\n");
	    }
	    else if(strcmp(PrintType(L->type, L->dim), "int") == 0 && (strcmp(PrintType(R->type, R->dim), "float") == 0 || strcmp(PrintType(R->type, R->dim), "double") == 0)){
	    	fprintf(fout, "\tfstore 99\n");
	    	fprintf(fout, "\ti2f\n");
	    	fprintf(fout, "\tfload 99\n");
	    	fprintf(fout, "\tfsub\n");
	    }
	    else if((strcmp(PrintType(L->type, L->dim), "float") == 0 || strcmp(PrintType(L->type, L->dim), "double") == 0)&& strcmp(PrintType(R->type, R->dim), "int") == 0){
	    	fprintf(fout, "\ti2f\n");
	    	fprintf(fout, "\tfsub\n");
	    }
	}
	else if(op == '*'){
		if(strcmp(PrintType(L->type, L->dim), "int") == 0 && strcmp(PrintType(R->type, R->dim), "int") == 0){
	    	fprintf(fout, "\timul\n");
	    }
	    else if((strcmp(PrintType(L->type, L->dim), "float") == 0 || strcmp(PrintType(L->type, L->dim), "double") == 0 )&& (strcmp(PrintType(R->type, R->dim), "float") == 0 || strcmp(PrintType(R->type, R->dim), "double") == 0)){
	    	fprintf(fout, "\tfmul\n");
	    }
	    else if(strcmp(PrintType(L->type, L->dim), "int") == 0 && (strcmp(PrintType(R->type, R->dim), "float") == 0 || strcmp(PrintType(R->type, R->dim), "double") == 0)){
	    	fprintf(fout, "\tfstore 99\n");
	    	fprintf(fout, "\ti2f\n");
	    	fprintf(fout, "\tfload 99\n");
	    	fprintf(fout, "\tfmul\n");
	    }
	    else if((strcmp(PrintType(L->type, L->dim), "float") == 0 || strcmp(PrintType(L->type, L->dim), "double") == 0)&& strcmp(PrintType(R->type, R->dim), "int") == 0){
	    	fprintf(fout, "\ti2f\n");
	    	fprintf(fout, "\tfmul\n");
	    }
	}
	else if(op == '/'){
		if(strcmp(PrintType(L->type, L->dim), "int") == 0 && strcmp(PrintType(R->type, R->dim), "int") == 0){
	    	fprintf(fout, "\tidiv\n");
	    }
	    else if((strcmp(PrintType(L->type, L->dim), "float") == 0 || strcmp(PrintType(L->type, L->dim), "double") == 0 )&& (strcmp(PrintType(R->type, R->dim), "float") == 0 || strcmp(PrintType(R->type, R->dim), "double") == 0)){
	    	fprintf(fout, "\tfdiv\n");
	    }
	    else if(strcmp(PrintType(L->type, L->dim), "int") == 0 && (strcmp(PrintType(R->type, R->dim), "float") == 0 || strcmp(PrintType(R->type, R->dim), "double") == 0)){
	    	fprintf(fout, "\tfstore 99\n");
	    	fprintf(fout, "\ti2f\n");
	    	fprintf(fout, "\tfload 99\n");
	    	fprintf(fout, "\tfdiv\n");
	    }
	    else if((strcmp(PrintType(L->type, L->dim), "float") == 0 || strcmp(PrintType(L->type, L->dim), "double") == 0)&& strcmp(PrintType(R->type, R->dim), "int") == 0){
	    	fprintf(fout, "\ti2f\n");
	    	fprintf(fout, "\tfdiv\n");
	    }
	}
	
}

void GenMod(){
	fprintf(fout, "\tirem\n");
}
void GenNeg(Expr* R){
	if(R == NULL){
		return;
	}
	else if(strcmp(PrintType(R->type, R->dim), "int") == 0){
		fprintf(fout, "\tineg\n");
	}
	else if(strcmp(PrintType(R->type, R->dim), "float") == 0 || strcmp(PrintType(R->type, R->dim), "double") == 0){
		fprintf(fout, "\tfneg\n");
	}
}
void GenOr(){	
	fprintf(fout, "\tior\n");
}
void GenAnd(){
	fprintf(fout, "\tiand\n");
}
void GenNot(){
	fprintf(fout, "\tixor\n");
}


void GenRelation(Expr* L, Expr* R, const char* op){
	if(L == NULL || R == NULL){
		return;
	}
	if(strcmp(PrintType(L->type, L->dim), "int") == 0&&strcmp(PrintType(R->type, R->dim), "int") == 0){
		fprintf(fout, "\tisub\n");
	}
	else{
		if(strcmp(PrintType(L->type, L->dim), "int") == 0 && (strcmp(PrintType(R->type, R->dim), "float") == 0 || strcmp(PrintType(R->type, R->dim), "double") == 0) ){
			fprintf(fout, "\tfstore 99\n");
	    	fprintf(fout, "\ti2f\n");
	    	fprintf(fout, "\tfload 99\n");
		}
		else if((strcmp(PrintType(L->type, L->dim), "float") == 0 || strcmp(PrintType(L->type, L->dim), "double") == 0) && strcmp(PrintType(R->type, R->dim), "int") == 0 ){
	    	fprintf(fout, "\ti2f\n");
		}
		fprintf(fout, "\tfcmpl\n");
	}

	if(strcmp(op, "<") == 0){
		fprintf(fout, "\tiflt ");
	}
	else if(strcmp(op, "<=") == 0){
		fprintf(fout, "\tifle ");
	}
	else if(strcmp(op, "!=") == 0){
		fprintf(fout, "\tifne ");
	}
	else if(strcmp(op, ">=") == 0){
		fprintf(fout, "\tifge ");
	}
	else if(strcmp(op, ">") == 0){
		fprintf(fout, "\tifgt ");
	}
	else if(strcmp(op, "==") == 0){
		fprintf(fout, "\tifeq ");
	}
	fprintf(fout, "\tL_true_%d\n", label_index);
	fprintf(fout, "\ticonst_0\n");
	fprintf(fout, "\tgoto L_false_%d\n", label_index);
	fprintf(fout, "L_true_%d:\n", label_index);
	fprintf(fout, "\ticonst_1\n");
	fprintf(fout, "L_false_%d:\n", label_index);
	label_index ++;
}


void GenIdListInGlobalVar(idList *l){
	idList *p = l;
	while(p != NULL){
		Expr *tmp = FindVar(symbol_table, p->ptr->id);
		if(tmp == NULL){ // not declared
			continue;
		}
		if(tmp->e->level == 0){
			if(strcmp(PrintType(tmp->e->type, 0), "int") == 0){
				fprintf(fout, ".field public static %s I\n", tmp->e->name);
			}
			else if(strcmp(PrintType(tmp->e->type, 0), "bool") == 0){
				fprintf(fout, ".field public static %s Z\n", tmp->e->name);
			}
			else if(strcmp(PrintType(tmp->e->type, 0), "float") == 0 || strcmp(PrintType(tmp->e->type, 0), "double") == 0){
				fprintf(fout, ".field public static %s F\n", tmp->e->name);
			}
		}
		p = p->next;
	}
}

void GenVarAssign(Expr* var, Expr* val){ // variable = value
	if(var == NULL || val == NULL){
		return;
	}
	if(var->e->level == 0){
		if(strcmp(PrintType(var->type, var->dim), "int") == 0 && strcmp(PrintType(val->type, val->dim), "int") == 0){
			fprintf(fout, "\tputstatic output/%s I\n", var->id);
		}
		else if((strcmp(PrintType(var->type, var->dim), "float") == 0 || strcmp(PrintType(var->type,var->dim), "double") == 0) && strcmp(PrintType(val->type, val->dim), "int") == 0){
			fprintf(fout, "\ti2f\n");
			fprintf(fout, "\tputstatic output/%s F\n", var->id);
		}
		else if((strcmp(PrintType(var->type, var->dim), "float") == 0 && strcmp(PrintType(val->type, val->dim), "float") == 0)||(strcmp(PrintType(var->type, var->dim), "double") == 0 && strcmp(PrintType(val->type, val->dim), "double") == 0)||(strcmp(PrintType(var->type, var->dim), "double") == 0 && strcmp(PrintType(val->type, val->dim), "float") == 0)){
			fprintf(fout, "\tputstatic output/%s F\n", var->id);
		}
		else if(strcmp(PrintType(var->type, var->dim), "bool") == 0){
			fprintf(fout, "\tputstatic output/%s Z\n", var->id);
		}
	}
	else{
		if((strcmp(PrintType(var->e->type,0), "int") == 0||strcmp(PrintType(var->e->type,0), "bool") == 0) && (strcmp(PrintType(val->type,0), "int") == 0 || strcmp(PrintType(val->type,0), "bool") == 0) ){
			fprintf(fout, "\tistore %d\n", var->e->local_index);
		}
		else if((strcmp(PrintType(var->e->type,0), "float") == 0||strcmp(PrintType(var->e->type,0), "double") == 0) && strcmp(PrintType(val->type,0), "int") == 0){ // type coertion
			fprintf(fout, "\ti2f\n");
			fprintf(fout, "\tfstore %d\n", var->e->local_index);
		}
		else if((strcmp(PrintType(var->e->type,0), "float") == 0 && strcmp(PrintType(val->type,0), "float") == 0) ||(strcmp(PrintType(var->e->type,0), "double") == 0&&strcmp(PrintType(val->type,0), "double") == 0)||(strcmp(PrintType(var->e->type,0), "double") == 0 &&strcmp(PrintType(val->type,0), "float") == 0)){
			fprintf(fout, "\tfstore %d\n", var->e->local_index);
		}
	}
}

void GenInitialAssign(Table *table, idList *l, const char *L_name, Type *t, Expr *R){ // int "a = 2", b, "c = d";
	if(table->level == 0){
		if(strcmp(PrintType(t,0), "int") == 0){
			fprintf(fout, "\tputstatic output/%s I\n", L_name);
		}
		else if(strcmp(PrintType(t,0), "float") == 0 || strcmp(PrintType(t,0), "double") == 0){
			fprintf(fout, "\tputstatic output/%s F\n", L_name);
		}
		else if(strcmp(PrintType(t,0), "bool") == 0){
			fprintf(fout, "\tputstatic output/%s Z\n", L_name);
		}
	}
	else{
		if(strcmp(PrintType(t,0), "int") == 0 && strcmp(PrintType(R->type,0), "int") == 0){
			fprintf(fout, "\tistore %d\n", table->next_local_index-1);
		}
		else if((strcmp(PrintType(t,0), "float") == 0 || strcmp(PrintType(t,0), "double") == 0)&& strcmp(PrintType(R->type,0), "int") == 0){
			fprintf(fout, "\ti2f\n");
			fprintf(fout, "\tfstore %d\n", table->next_local_index-1);
		}

		else if((strcmp(PrintType(t,0), "float") == 0  && strcmp(PrintType(R->type,0), "float") == 0)||(strcmp(PrintType(t,0), "double") == 0  && strcmp(PrintType(R->type,0), "double") == 0)||(strcmp(PrintType(t,0), "double") == 0  && strcmp(PrintType(R->type,0), "float") == 0)){
			fprintf(fout, "\tfstore %d\n", table->next_local_index-1);
		}
		else if(strcmp(PrintType(t,0), "bool") == 0){
			fprintf(fout, "\tistore %d\n", table->next_local_index-1);
		}
	}

}

void GenPrintInitialization(){
	fprintf(fout, "\tgetstatic java/lang/System/out Ljava/io/PrintStream;\n");
}
void GenReadInitialization(){
	fprintf(fout, "\tgetstatic output/_sc Ljava/util/Scanner;\n");
}

void GenPrint(Expr* e){
	if(e == NULL){
		return;
	}

	if(strcmp(PrintType(e->type,e->dim), "int") == 0)
		fprintf(fout, "\tinvokevirtual java/io/PrintStream/print(I)V\n");
	else if(strcmp(PrintType(e->type,e->dim), "float") == 0 || strcmp(PrintType(e->type,e->dim), "double") == 0)
		fprintf(fout, "\tinvokevirtual java/io/PrintStream/print(F)V\n");
	else if(strcmp(PrintType(e->type,e->dim), "bool") == 0)
		fprintf(fout, "\tinvokevirtual java/io/PrintStream/print(Z)V\n");
	else if(strcmp(PrintType(e->type,e->dim), "string") == 0)
		fprintf(fout, "\tinvokevirtual java/io/PrintStream/print(Ljava/lang/String;)V\n");
}

void GenRead(Expr* e){
	if(e == NULL){
		return;
	}
	GenReadInitialization();

	if(strcmp(PrintType(e->type, e->dim), "int") == 0){
		fprintf(fout, "\tinvokevirtual java/util/Scanner/nextInt()I\n");
	}
	else if(strcmp(PrintType(e->type, e->dim), "float") == 0 || strcmp(PrintType(e->type, e->dim), "double") == 0){
		fprintf(fout, "\tinvokevirtual java/util/Scanner/nextFloat()F\n");
	}
	else if(strcmp(PrintType(e->type, e->dim), "bool") == 0){
		fprintf(fout, "\tinvokevirtual java/util/Scanner/nextBoolean()Z\n");
	}
	if(e->e->level != 0){
		if(strcmp(PrintType(e->type,e->dim), "int") == 0|| strcmp(PrintType(e->type,e->dim), "bool") == 0){
			fprintf(fout, "\tistore %d\n", e->e->local_index);
		}
		else if(strcmp(PrintType(e->type,e->dim), "float") == 0||strcmp(PrintType(e->type,e->dim), "double") == 0){
			fprintf(fout, "\tfstore %d\n", e->e->local_index);
		}
		else if(strcmp(PrintType(e->type,e->dim), "string") == 0){
			// action not defined
		}
	}
	else{
		if(strcmp(PrintType(e->type, e->dim), "int") == 0){
			fprintf(fout, "\tputstatic output/%s I\n", e->id);
		}
		else if(strcmp(PrintType(e->type, e->dim), "float") == 0 || strcmp(PrintType(e->type, e->dim), "double") == 0){
			fprintf(fout, "\tputstatic output/%s F\n", e->id);
		}
		else if(strcmp(PrintType(e->type, e->dim), "bool") == 0){
			fprintf(fout, "\tputstatic output/%s Z\n", e->id);
		}
	}
}

void GenFuncInitialization(Table *table, const char* name){
	Expr* func = FindVar(symbol_table, name);
	fprintf(fout, ".method public static %s(", name);
	if(func->e->attr->attrparam != NULL){
		AttrParam *p = func->e->attr->attrparam;
		while(p != NULL){
			if(strcmp(PrintType(p->type, 0), "int") == 0){
				fprintf(fout, "I");
			}
			else if(strcmp(PrintType(p->type, 0), "float") == 0 || strcmp(PrintType(p->type, 0), "double") == 0){
				fprintf(fout, "F");
			}
			else if(strcmp(PrintType(p->type, 0), "bool") == 0){
				fprintf(fout, "Z");
			}
			else if(strcmp(PrintType(p->type, 0), "string") == 0){
				// action not defined
			}
			p = p->next;
		}
	}
	
	else if(strcmp(name, "main") == 0){
		fprintf(fout, "[Ljava/lang/String;");
		in_main = true;
		table->next_local_index = 1;
	}
	fprintf(fout, ")");
	// return type
	if(in_main){
		fprintf(fout, "V\n");
	}
	else if(strcmp(PrintType(func->e->type,0), "int") == 0){
		fprintf(fout, "I\n");
	}
	else if(strcmp(PrintType(func->e->type,0), "float") == 0 || strcmp(PrintType(func->e->type,0), "double") == 0){
		fprintf(fout, "F\n");
	}
	else if(strcmp(PrintType(func->e->type,0), "bool") == 0){
		fprintf(fout, "Z\n");
	}
	else if(strcmp(PrintType(func->e->type,0), "void") == 0){
		fprintf(fout, "V\n");
	}

	fprintf(fout, ".limit stack 100\n");
	fprintf(fout, ".limit locals 100\n");
	if(in_main){
		fprintf(fout, "\tnew java/util/Scanner\n");
		fprintf(fout, "\tdup\n");
		fprintf(fout, "\tgetstatic java/lang/System/in Ljava/io/InputStream;\n");
		fprintf(fout, "\tinvokespecial java/util/Scanner/<init>(Ljava/io/InputStream;)V\n");
		fprintf(fout, "\tputstatic output/_sc Ljava/util/Scanner;\n");
	}


}

void GenFuncEnd(){
	fprintf(fout, ".end method\n");
}
void GenProcedureEnd(){
	fprintf(fout, "\treturn\n");
	fprintf(fout, ".end method\n");
}
void GenReturn(Type *true_type, Expr* e){
	if(e == NULL){
		return;
	}
	if(in_main){
		fprintf(fout, "\treturn\n");
	}
	else if(strcmp(PrintType(true_type, 0), "int") == 0 || strcmp(PrintType(true_type, 0), "bool") == 0){
		fprintf(fout, "\tireturn\n");
	}
	else if(strcmp(PrintType(true_type, 0), "float") == 0 || strcmp(PrintType(true_type, 0), "double") == 0){
		if(strcmp(PrintType(e->type, 0), "int") == 0 || strcmp(PrintType(e->type, 0), "bool") == 0){
			fprintf(fout, "\ti2f\n");
		}
		fprintf(fout, "\tfreturn\n");
	}
}

void GenFunction(const char* name){
	Entry* t = FindEntryInGlobal(symbol_table, name);
	if(t == NULL){
		return;
	}
	fprintf(fout, "\tinvokestatic output/%s(", t->name);
	if(t->attr->attrparam != NULL){
		AttrParam *p = t->attr->attrparam;
		while(p != NULL){
			if(strcmp(PrintType(p->type, 0), "int") == 0){
				fprintf(fout, "I");
			}
			else if(strcmp(PrintType(p->type, 0), "float") == 0 || strcmp(PrintType(p->type, 0), "double") == 0){
				fprintf(fout, "F");
			}
			else if(strcmp(PrintType(p->type, 0), "bool") == 0){
				fprintf(fout, "Z");
			}
			p = p->next;
		}
	}
	fprintf(fout, ")");
	if(strcmp(PrintType(t->type,0), "int") == 0){
		fprintf(fout, "I\n");
	}
	else if(strcmp(PrintType(t->type,0), "float") == 0 || strcmp(PrintType(t->type,0), "double") == 0){
		fprintf(fout, "F\n");
	}
	else if(strcmp(PrintType(t->type,0), "bool") == 0){
		fprintf(fout, "Z\n");
	}
	else if(strcmp(PrintType(t->type,0), "void") == 0){
		fprintf(fout, "V\n");
	}
}
void push_stack(int value){
	l_stack.stack[++(l_stack.top)] = value;
}

void pop_stack(){
	l_stack.top --;
}
void GenIfBegin(){
	fprintf(fout, "\tifeq L_else_%d\n", l_stack.stack[l_stack.top]);

}

void GenElse(){
	fprintf(fout, "\tgoto L_exit_%d\n", l_stack.stack[l_stack.top]);
	fprintf(fout, "L_else_%d:\n", l_stack.stack[l_stack.top]);
}

void GenIfEnd(){
	fprintf(fout, "L_exit_%d:\n", l_stack.stack[l_stack.top]);
}

void GenIfNoElse(){
	fprintf(fout, "\tgoto L_exit_%d\n", l_stack.stack[l_stack.top]);
	fprintf(fout, "L_else_%d:\n", l_stack.stack[l_stack.top]);
	fprintf(fout, "L_exit_%d:\n", l_stack.stack[l_stack.top]);
}

void GenControlBegin(){
	fprintf(fout, "L_begin_%d:\n", l_stack.stack[l_stack.top]);
}

void GenForControl(){
	fprintf(fout, "\tifeq L_exit_%d\n", l_stack.stack[l_stack.top]);
	fprintf(fout, "\tgoto L_inside_%d\n", l_stack.stack[l_stack.top]);
	fprintf(fout, "L_inc_%d:\n", l_stack.stack[l_stack.top]);
}

void GenForEnd(){
	fprintf(fout, "\tgoto L_inc_%d\n", l_stack.stack[l_stack.top]);
	fprintf(fout, "L_exit_%d:\n", l_stack.stack[l_stack.top]);
}

void GenForInside(){
	fprintf(fout, "L_inside_%d:\n", l_stack.stack[l_stack.top]);
}

void GenForIncEnd(){
	fprintf(fout, "\tgoto L_begin_%d\n", l_stack.stack[l_stack.top]);
}

void GenWhileControl(){
	fprintf(fout, "\tifeq L_exit_%d\n", l_stack.stack[l_stack.top]);
}

void GenWhileEnd(){
	fprintf(fout, "\tgoto L_begin_%d\n", l_stack.stack[l_stack.top]);
	fprintf(fout, "L_exit_%d:\n", l_stack.stack[l_stack.top]);
}

