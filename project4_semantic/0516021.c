#include "0516021.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

Table* CreateTable(){
	Table* ret = (Table*) malloc(sizeof(Table));
	ret->level = 0;
	ret->size = 0;
	ret->p = NULL;
	return ret;
}

void PrintTable(Table* table){
    if (Opt_Symbol == 0){
        return;
    }
    Entry *ptr;
    printf("=======================================================================================\n");
    printf("%-33s%-11s%-12s%-19s%-24s\n", "Name", "Kind", "Level", "Type", "Attribute");
    printf("---------------------------------------------------------------------------------------\n");
    if(table->size > 0){
    	ptr = table->p;
    }
    else{
    	ptr = NULL;
    }
    while(ptr != NULL){
        if (ptr->level == table->level) {
            printf("%-33s%-11s", ptr->name, ptr->kind);
            if(ptr->level == 0){
            	printf("0%-11s", "(global)" );
            }
            else if(ptr->level > 0){
            	if(ptr->level < 10)
            		printf("%d%-11s", ptr->level, "(local)" );
            	else
            		printf("%d%-10s", ptr->level, "(local)" );
            }
            printf("%-19s", PrintType(ptr->type, 0));
            if(strcmp(ptr->kind, "constant") == 0 || strcmp(ptr->kind, "function") == 0)
            	PrintAttribute(ptr->attr);
            printf("\n");
        }
        ptr = ptr->next;
    }
    printf("=======================================================================================\n");
}

char *PrintType(Type *type, int dim){
	if(type == NULL){
		return NULL;
	}
	char* ret = (char*) calloc(80, sizeof(char));  // maybe int a[1][234][5678][999]..

	char arr_buf[12];
	snprintf(ret, strlen(type->name)+1, "%s", type->name);
	TypeDim *tmp = type->arr;

	while(tmp != NULL){
		if(dim == 0){
			snprintf(arr_buf, 11, "[%d]", tmp->num);
			strcat(ret, arr_buf);
		}
		else if(dim > 0){
			dim--;
		}
		tmp = tmp->next;
	}

	return ret;
}

void PrintAttribute(Attribute* attr){
	if(attr == NULL){
		return;
	}
	else if(attr->attrparam != NULL){ //function param
		AttrParam *tmp = attr->attrparam;

		while(tmp != NULL){
			if(tmp->next == NULL){
				printf("%s", PrintType(tmp->type, 0));
			}
			else{
				printf("%s,", PrintType(tmp->type, 0));
			}
			tmp = tmp->next;
		}
	}
	else if(attr->value != NULL){ //constant
		if(strcmp(attr->value->type->name, "int") == 0){
			printf("%-24d", attr->value->i);
		}
		else if(strcmp(attr->value->type->name, "float") == 0){
			printf("%-24f", attr->value->d);
		}
		else if(strcmp(attr->value->type->name, "double") == 0){
			printf("%-24f", attr->value->d);
		}
		else if(strcmp(attr->value->type->name, "bool") == 0){
			printf("%-24s", attr->value->str);
		}
		else if(strcmp(attr->value->type->name, "string") == 0){
			printf("%-24s", attr->value->str);
		}
		else if(strcmp(attr->value->type->name, "scientific") == 0){
			printf("%-24f", attr->value->d); // setprecision(6)
		}
	}

}

void DeleteEntrys(Table* table){
    Entry *cur, *prev;
    if(table->size == 1){
    	if((table->p)->level == table->level){
    		free (table->p);
    		table->p = NULL;
    		table->size --;
    	}
    	return;
    }
    else if(table->size > 1){
    	cur = table->p->next;
    	prev = table->p;
    }
    else{
    	return;
    }
    while(cur != NULL){
        if(cur->level == table->level){
        	free(prev->next);
        	prev->next = cur->next;
        	if(cur->next != NULL)
        		cur = cur->next;
        	else
        		cur = NULL;
        	(table->size) --;
        }
        else{
        	prev = cur;
        	if(cur->next != NULL)
        		cur = cur->next;
        	else
        		cur = NULL;
        }
    }
    if(table->p->level == table->level){// if the head is
    	free (table->p);
    	table->p = NULL;
    	table->size --;
    }
}

void InsertTableEntry(Table *t, Entry* e){
	if(FindEntryInCurrentScope(t,e->name) != NULL){
		printf("##########Error at Line #%d: %s redeclared.##########\n", linenum, e->name);
		error = true;
		return;
	}
	if(t->size == 0){
		t->p = e;
		t->size ++;
	}
	else{
		Entry *p = t->p;
		while(p->next != NULL){
			p = p->next;
		}
		p->next = e;
		t->size ++;
	}
	return;
}

void InsertTableEntryFromidList(Table *t, idList* l, const char* kind, Type* type){ // int a,b[3][2],c,d[5];
	idList *p = l;
	// argument 4 : LHS OF ASSIGH : int
	while(p != NULL){
		Type *tmp2 = (Type*)malloc(sizeof(Type));
		strncpy(tmp2->name, type->name, 12);

		tmp2->arr = p->ptr->dim;
		Entry *tmp;
		if(p->ptr->a == NULL){// didnt assign
			tmp = BuildEntry(p->ptr->id, kind, t->level, tmp2, NULL);
		}
		else if(p->ptr->a->value != NULL){ //isnt array have assign
			tmp = BuildEntry(p->ptr->id, kind, t->level, tmp2, BuildConstAttr(p->ptr->a->value));
		}
		else if(p->ptr->a->attrparam != NULL){// is array have assign

			tmp = BuildEntry(p->ptr->id, kind, t->level, tmp2, NULL);
		}
		InsertTableEntry(t, tmp);

		if(p->ptr->dim == NULL && p->ptr->a != NULL){ // LHS is not array and has assigned something
			/*
			int RHS_dim = 0;
			TypeDim *tmp3 = p->ptr->a->value->type->arr;

			while(tmp3 != NULL){
				RHS_dim ++;
				tmp3 = tmp3->next;
			}*/
			if(strcmp(type->name, "double") == 0){ // LHS
				if(p->ptr->a->value != NULL){
					if(strcmp(PrintType(p->ptr->a->value->type, 0), "int") != 0 && strcmp(PrintType(p->ptr->a->value->type, 0), "float") != 0 && strcmp(PrintType(p->ptr->a->value->type, 0), "double") != 0 && strcmp(PrintType(p->ptr->a->value->type, 0), "scientific") != 0){
						printf("##########Error at Line #%d: %s type initialization mismatch.##########\n", linenum, p->ptr->id);
						error = true;
					}
				}
				else{
						printf("##########Error at Line #%d: %s type initialization mismatch.##########\n", linenum, p->ptr->id);
						error = true;
				}
			}
			else if(strcmp(type->name, "float") == 0){
				if(p->ptr->a->value != NULL){
					if(strcmp(PrintType(p->ptr->a->value->type, 0), "int") != 0 && strcmp(PrintType(p->ptr->a->value->type, 0), "float") != 0 && strcmp(PrintType(p->ptr->a->value->type, 0), "scientific") != 0){
						printf("##########Error at Line #%d: %s type initialization mismatch.##########\n", linenum, p->ptr->id);
						error = true;
					}
				}
				else{
						printf("##########Error at Line #%d: %s type initialization mismatch.##########\n", linenum, p->ptr->id);
						error = true;
				}
			}
			else if(strcmp(type->name, "int") == 0){
				if(p->ptr->a->value != NULL){
					if(strcmp(PrintType(p->ptr->a->value->type, 0), "int") != 0 && strcmp(PrintType(p->ptr->a->value->type, 0), "scientific") != 0){
						printf("##########Error at Line #%d: %s type initialization mismatch.##########\n", linenum, p->ptr->id);
						error = true;
					}
				}
				else{
						printf("##########Error at Line #%d: %s type initialization mismatch.##########\n", linenum, p->ptr->id);
						error = true;
				}
			}
			else if(strcmp(type->name, "bool") == 0){
				if(p->ptr->a->value != NULL){
					if(strcmp(PrintType(p->ptr->a->value->type, 0), "bool") != 0){
						printf("##########Error at Line #%d: %s type initialization mismatch.##########\n", linenum, p->ptr->id);
						error = true;
					}
				}
				else{
					printf("##########Error at Line #%d: %s type initialization mismatch.##########\n", linenum, p->ptr->id);
					error = true;
				}
			}
			else if(strcmp(type->name, "string") == 0){
				if(p->ptr->a->value != NULL){
					if(strcmp(PrintType(p->ptr->a->value->type, 0), "string") != 0){
						printf("##########Error at Line #%d: %s type initialization mismatch.##########\n", linenum, p->ptr->id);
						error = true;
					}
				}
				else{
					printf("##########Error at Line #%d: %s type initialization mismatch.##########\n", linenum, p->ptr->id);
					error = true;
				}
			}
			else{
				//unknown condition?
			}
		}
		else if(p->ptr->dim != NULL && p->ptr->a != NULL){ // LHS is array and has assign something

			if(p->ptr->a->attrparam != NULL){
				// array initialization
				bool flag_error = false;
				int cnt_array_initial_number = 0;
				int true_max_number = 1;
				TypeDim *p2 = p->ptr->dim;
				while(p2 != NULL){
					if(p2->num <= 0){
						flag_error = true;
						break;
					}
					true_max_number *= p2->num;
					p2 = p2->next;
				}
				
				AttrParam *p3 = p->ptr->a->attrparam;
				while(p3 != NULL){
					if(strcmp(PrintType(p3->type, 0), type->name) != 0){
						if(strcmp(type->name, "double") == 0 && strcmp(PrintType(p3->type,0), "int") == 0){
							// Type Coercion
						}
						else if(strcmp(type->name, "double") == 0 && strcmp(PrintType(p3->type, 0), "float") == 0){
							// Type Coercion
						}
						else if(strcmp(type->name, "float") == 0 && strcmp(PrintType(p3->type, 0), "int") == 0){
							// Type Coercion
						}
						else{
							printf("##########Error at Line #%d: the initial element assigned to array %s must be type %s.##########\n", linenum, p->ptr->id, type->name);
							error = true;
						}
					}
					cnt_array_initial_number ++;
					p3 = p3->next;
				}
				if(!flag_error){
					if(cnt_array_initial_number > true_max_number){
						printf("##########Error at Line #%d: the number of initializers should be equal or less than the array size.##########\n", linenum);
						error = true;
					}
				}

			}
			else{
				printf("##########Error at Line #%d: %s type initialization mismatch.##########\n", linenum, p->ptr->id);
				error = true;
			}
		}
		p = p->next;
	}

}
void InsertTableEntryFromFuncParamidList(Table *t, FuncParamidList* l){ // bool func(int a, float b[5]){}
	if(l == NULL){
		return;
	}
	FuncParamidList *p = l;
	while(p != NULL){
		Entry *tmp = BuildEntry(p->id, "parameter", t->level, p->type, NULL);
		InsertTableEntry(t, tmp);
		p = p->next;
	}

}

Entry* BuildEntry(const char* name, const char* kind, int level, Type* type, Attribute* attr){
	Entry* ret = (Entry*) malloc(sizeof(Entry));

	strcpy(ret->name, name);
	strcpy(ret->kind, kind);
	ret->level = level;
	ret->type = type;

	ret->attr = attr;
	ret->next = NULL;
	ret->def = false;

	return ret;
}

Type* BuildType(const char* name, TypeDim *t){
	Type *ret = (Type*)malloc(sizeof(Type));
	ret->name[0] = '\0';
	if(name != NULL){
		strcpy(ret->name, name);
	}
	if(t != NULL){
		ret->arr = t;
	}
	else{
		ret->arr = NULL;
	}

	return ret;
}

idList* AddToidList(idList* ret, const char* name, TypeDim *dim, Attribute *a){
	// a : RHS
	// dim : LHS
	if(ret == NULL){

		ret = (idList*)malloc(sizeof(idList));
		ret->ptr = (id_struct*)malloc(sizeof(id_struct));
		strncpy(ret->ptr->id, name, 32);
		ret->ptr->dim = dim;
		ret->ptr->a = a;
		ret->next = NULL;
	}
	else{
		idList *p = ret;
		while(p->next != NULL){
			p = p->next;
		}
		p->next = (idList*)malloc(sizeof(idList));
		p->next->ptr = (id_struct*)malloc(sizeof(id_struct));
		strncpy(p->next->ptr->id, name, 32);
		p->next->ptr->dim = dim;
		p->next->ptr->a = a;
		p->next->next = NULL;
	}

	return ret;
}

void ClearidList(){
	idList *p = idlist;
	while(p != NULL){
		idlist = idlist->next;
		free(p);
		p = idlist;
	}
	idlist = NULL;
}

FuncParamidList* AddToFuncParamidList(FuncParamidList* ret, const char* name, Type *type){
	if(ret == NULL){
		ret = (FuncParamidList*)malloc(sizeof(FuncParamidList));
		strncpy(ret->id, name, 32);
		ret->type = type;
		ret->next = NULL;
	}
	else{
		FuncParamidList *p = ret;
		while(p->next != NULL){
			p = p->next;
		}
		p->next = (FuncParamidList*)malloc(sizeof(FuncParamidList));
		strncpy(p->next->id, name, 32);
		p->next->type = type;
		p->next->next = NULL;
	}
	return ret;
}

void ClearFuncParamidList(){
	FuncParamidList *p = fpidlist;
	while(p != NULL){
		fpidlist = fpidlist->next;
		free(p);
		p = fpidlist;
	}
	fpidlist = NULL;
}
Value* BuildValue(const char* typename, char* val){
	Type* tmp = BuildType(typename, NULL);
	Value* ret = (Value*) malloc(sizeof(Value));

	ret->type = tmp;
	ret->str = NULL;

	if(strcmp(typename, "int") == 0){
		ret->i = atoi(val);
	}
	else if(strcmp(typename, "float") == 0){
		ret->d = atof(val);
		ret->str = strdup(val); // scientific
	}
	/*
	else if(strcmp(typename, "double") == 0){ // impossible literal_const no double
		ret->d = atof(val);
		ret->str = strdup(val); // scientific
	}
	*/
	else if(strcmp(typename, "scientific") == 0){
		char* tmp = strdup(val);
		char* fpart = strtok(tmp, "e");
		char* num = strdup(fpart);
		double f = atof(num);
		fpart = strtok(NULL, "e");
		char *powpart = strdup(fpart); // -1 -2 +1 +2...
		int i = atoi(powpart);
		int precision = 6;
		if(i < 0){
			precision = -i;
		}
		while(i != 0){
			if(i > 0){
				f *= 10;
				i--;
			}
			else if(i < 0){
				f /= 10;
				i++;
			}
		}

		ret->d = f;
		ret->str = strdup(val);  //scientifics

	}
	else if(strcmp(typename, "bool") == 0){
		ret->str = strdup(val);
	}
	else if(strcmp(typename, "string") == 0){
		ret->str = strdup(val);
	}
	return ret;
}

Attribute* BuildConstAttr(Value* v){
	Attribute* ret = (Attribute*)malloc(sizeof(Attribute));

	ret->value = v;
	ret->attrparam = NULL;

	return ret;
}

Attribute* BuildFuncAttr(AttrParam *a){
	Attribute* ret = (Attribute*) malloc(sizeof(Attribute));
	ret->value = NULL;
	ret->attrparam = a;

	return ret;
}
Attribute* BuildExprAttribute(Expr *e){
	if(e == NULL){
		return NULL;
	}
	Attribute *ret = (Attribute *)malloc(sizeof(Attribute));
	ret->value = (Value *)malloc(sizeof(Value));
	ret->value->type = (Type *)malloc(sizeof(Type));
	ret->attrparam = NULL;
	ret->value->type->arr = NULL;
	strcpy(ret->value->type->name, e->type->name);
	TypeDim *p = e->type->arr;
	int i = e->dim;
	while(i--){
		if(p == NULL){
			// size mismatch
			//printf("##########Error at Line #%d: too many dimension for array %s.##########\n", linenum, e->id); //error has been printed
			error = true;
			break;
		}
		p = p->next;
	}
	ret->value->type->arr = CloneTypeDim(p); // wrong ? right !
	return ret;
}
Attribute* BuildExprListAttribute(ExprList *e){
	Attribute *ret = (Attribute *)malloc(sizeof(Attribute));
	//ret->attrparam = (attrparam *)malloc(sizeof(attrparam));
	//ret->attrparam->type = (Type *)malloc(sizeof(Type));
	ret->value = NULL;
	ret->attrparam = CloneExprList(e);
	// debug Mistake
	return ret;
}
Attribute* BuildValueAttribute(Value *v){
	Attribute *ret = (Attribute *)malloc(sizeof(Attribute));
	//ret->value = (Value *)malloc(sizeof(Value));
	ret->attrparam = NULL;
	ret->value = v;
	return ret;
}
TypeDim* AddToTypeDim(TypeDim *ret, int num){ // when declaring or defining
	if(num <= 0){
		printf("##########Error at Line #%d: the index of array declaration must be greater than zero.##########\n", linenum);
		error = true;
	}
	if(ret == NULL){
		ret = (TypeDim*)malloc(sizeof(TypeDim));
		ret->num = num;
		ret->next = NULL;
	}
	else{
		TypeDim *p = ret;
		while(p->next != NULL){
			p = p->next;
		}
		p->next = (TypeDim*)malloc(sizeof(TypeDim));
		p->next->num = num;
		p->next->next = NULL;
	}
	return ret;
}
AttrParam* AddToParam(AttrParam *ret, Type *t){
	if(ret == NULL){
		ret = (AttrParam*)malloc(sizeof(AttrParam));
		ret->type = t;
		ret->next = NULL;
	}
	else{
		AttrParam *p = ret;
		while(p->next != NULL){
			p = p->next;
		}
		p->next = (AttrParam*)malloc(sizeof(AttrParam));
		p->next->type = t;
		p->next->next = NULL;
	}
	return ret;
}

Entry* FindEntryInCurrentScope(Table* t, const char* name){

   	Entry *p;
    if(t->size > 0){
    	p = t->p;
    }
    else{
    	p = NULL;
    }
    while(p != NULL){
        if (p->level == t->level && strcmp(p->name, name) == 0) {
        	return p;
        }
        p = p->next;
    }

	return NULL;
}

Entry* FindEntryInGlobal(Table *t, const char* name){
	Entry *p = t->p;
	while(p != NULL){
		if(strcmp(p->name, name) == 0 && p->level == 0){
			return p;
		}
		p = p->next;
	}
    return NULL;
}
Entry* FindEntryInLocal(Table *t, const char* name){
	Entry *p = t->p;
	while(p != NULL){
		if(strcmp(p->name, name) == 0 && p->level != 0){
			return p;
		}
		p = p->next;
	}
    return NULL;
}
Entry* FindEntryInAllClosest(Table *t, const char* name){
	int current_level;
	for(current_level = t->level;current_level >= 0;current_level--){
		Entry *p = t->p;
		while(p != NULL){
			if(strcmp(p->name, name) == 0 && p->level == current_level){
				return p;
			}
			p = p->next;
		}
	}
    return NULL;
}
void CheckFuncReturn(Type *correct, Expr *e){
	if(e == NULL){
		// the return is error(NULL)
		return;
	}
	if(strcmp(PrintType(correct, 0), "void") == 0){
		printf("##########Error at Line #%d: void function can't return.##########\n", linenum);
		error = true;
		return;
	}
	if(correct == NULL){
		// will not happen
		return;
	}
			if(strcmp(PrintType(correct, 0), "double") == 0){
				
					if(strcmp(PrintType(e->type, e->dim), "double") != 0 && strcmp(PrintType(e->type, e->dim), "float") != 0 && strcmp(PrintType(e->type, e->dim), "int") != 0 && strcmp(PrintType(e->type, e->dim), "scientific") != 0){
						printf("##########Error at Line #%d: return type mismatch, should return %s rather than %s.##########\n", linenum, PrintType(correct, 0), PrintType(e->type, e->dim));
						error = true;
					
				}

			}
			else if(strcmp(PrintType(correct, 0), "float") == 0){
				
					if(strcmp(PrintType(e->type, e->dim), "float") != 0 && strcmp(PrintType(e->type, e->dim), "int") != 0 && strcmp(PrintType(e->type, e->dim), "scientific") != 0){
						printf("##########Error at Line #%d: return type mismatch, should return %s rather than %s.##########\n", linenum, PrintType(correct, 0), PrintType(e->type, e->dim));
						error = true;
					}
				

			}
			else if(strcmp(PrintType(correct, 0), "int") == 0){
				
					if(strcmp(PrintType(e->type, e->dim), "int") != 0){
						printf("##########Error at Line #%d: return type mismatch, should return %s rather than %s.##########\n", linenum, PrintType(correct, 0), PrintType(e->type, e->dim));
						error = true;
					}
				

			}
			else if(strcmp(PrintType(correct, 0), "bool") == 0){
				
					if(strcmp(PrintType(e->type, e->dim), "bool") != 0){
						printf("##########Error at Line #%d: return type mismatch, should return %s rather than %s.##########\n", linenum, PrintType(correct, 0), PrintType(e->type, e->dim));
						error = true;
					}
				
	
			}
			else if(strcmp(PrintType(correct, 0), "string") == 0){
	
					if(strcmp(PrintType(e->type, e->dim), "string") != 0){
						printf("##########Error at Line #%d: return type mismatch, should return %s rather than %s.##########\n", linenum, PrintType(correct, 0), PrintType(e->type, e->dim));
						error = true;
					}
			}
		
		/*
	if(strcmp(PrintType(correct, 0), PrintType(e->type, e->dim)) != 0){
		printf("##########Error at Line #%d: return type mismatch, should return %s rather than %s.##########\n", linenum, PrintType(correct, 0), PrintType(e->type, e->dim));
		error = true;
	}*/
	return;
}

Expr *Arith(Expr *L, Expr *R, char *op){ // + - * /
	if(L == NULL || R == NULL){
		return NULL;
	}
	/*
	if(strcmp(L->kind, "error") == 0 || strcmp(R->kind, "error") == 0){
		return NULL;
	}*/
	Expr *ret = (Expr *)malloc(sizeof(Expr));
	ret->dim = 0;
	ret->e = NULL;
	//strcpy(ret->kind, "variable");
	if(strcmp(PrintType(L->type, L->dim), "int") == 0 && strcmp(PrintType(R->type, R->dim), "int") == 0){
		ret->type = BuildType("int", NULL);
		return ret;
	}
	if(strcmp(PrintType(L->type, L->dim), "int") == 0 && strcmp(PrintType(R->type, R->dim), "float") == 0){
		ret->type = BuildType("float", NULL);
		return ret;
	}
	if(strcmp(PrintType(L->type, L->dim), "float") == 0 && strcmp(PrintType(R->type, R->dim), "int") == 0){
		ret->type = BuildType("float", NULL);
		return ret;
	}
	if(strcmp(PrintType(L->type, L->dim), "int") == 0 && strcmp(PrintType(R->type, R->dim), "double") == 0){
		ret->type = BuildType("double", NULL);
		return ret;
	}
	if(strcmp(PrintType(L->type, L->dim), "double") == 0 && strcmp(PrintType(R->type, R->dim), "int") == 0){
		ret->type = BuildType("double", NULL);
		return ret;
	}
	if(strcmp(PrintType(L->type, L->dim), "float") == 0 && strcmp(PrintType(R->type, R->dim), "float") == 0){
		ret->type = BuildType("float", NULL);
		return ret;
	}
	if(strcmp(PrintType(L->type, L->dim), "double") == 0 && strcmp(PrintType(R->type, R->dim), "double") == 0){
		ret->type = BuildType("double", NULL);
		return ret;
	}
	if(strcmp(PrintType(L->type, L->dim), "float") == 0 && strcmp(PrintType(R->type, R->dim), "double") == 0){
		ret->type = BuildType("double", NULL);
		return ret;
	}
	if(strcmp(PrintType(L->type, L->dim), "double") == 0 && strcmp(PrintType(R->type, R->dim), "float") == 0){
		ret->type = BuildType("double", NULL);
		return ret;
	}
	printf("##########Error at Line #%d:  LHS and RHS of %s should be int,float,double.##########\n", linenum, op);
	error = true;
	return NULL;
	ret->type = BuildType(L->type->name, NULL); // unnecessary
	return ret;
}

Expr *Mod(Expr *L, Expr *R, char *op){
	if(L == NULL || R == NULL){
		return NULL;
	}
	Expr *ret = (Expr *)malloc(sizeof(Expr));
	ret->dim = 0;
	ret->e = NULL;
	//strcpy(ret->kind, "variable");
	if(strcmp(PrintType(L->type, L->dim), "int") == 0 && strcmp(PrintType(R->type, R->dim), "int") == 0){
		ret->type = BuildType("int", NULL);
		return ret;
	}
	printf("##########Error at Line #%d:  LHS and RHS of %s should be int.##########\n", linenum, op);
	error = true;
	return NULL;
	//strcpy(ret->kind, "error");
	ret->type = BuildType(L->type->name, NULL); // unnecessary
	return ret;
}

Expr *Logic(Expr *L, Expr *R, char *op){ // OR AND
	if(L == NULL || R == NULL){
		return NULL;
	}
	/*
	if(strcmp(L->kind, "error") == 0 || strcmp(R->kind, "error") == 0){
		return NULL;
	}*/
	Expr *ret = (Expr *)malloc(sizeof(Expr));
	ret->dim = 0;
	ret->e = NULL;
	//strcpy(ret->kind, "variable");
	if(strcmp(PrintType(L->type, L->dim), "bool") == 0 && strcmp(PrintType(R->type, R->dim), "bool") == 0){
		ret->type = BuildType("bool", NULL);
		return ret;
	}
	printf("##########Error at Line #%d:  LHS and RHS of %s should be boolean.##########\n", linenum, op);
	error = true;
	return NULL;
}
Expr *Not(Expr *R, char *op){ // !
	if(R == NULL){
		return NULL;
	}
	Expr *ret = (Expr *)malloc(sizeof(Expr));
	ret->dim = 0;
	ret->e = NULL;
	//strcpy(ret->kind, "variable");
	if(strcmp(PrintType(R->type, R->dim), "bool") == 0){
		ret->type = BuildType("bool", NULL);
		return ret;
	}
	printf("##########Error at Line #%d:  RHS of %s should be boolean.##########\n", linenum, op);
	error = true;
	return NULL;
}
Expr *Negative(Expr *R){ // -(unary)
	if(R == NULL){
		return NULL;
	}
	Expr *ret = (Expr *)malloc(sizeof(Expr));
	ret->dim = 0;
	ret->e = NULL;
	if(strcmp(PrintType(R->type, R->dim), "int") == 0){
		ret->type = BuildType("int", NULL);
		return ret;
	}
	if(strcmp(PrintType(R->type, R->dim), "float") == 0){
		ret->type = BuildType("float", NULL);
		return ret;
	}
	if(strcmp(PrintType(R->type, R->dim), "double") == 0){
		ret->type = BuildType("double", NULL);
		return ret;
	}
	printf("##########Error at Line #%d:  RHS of -(unary) should be int,float,double.##########\n", linenum);
	error = true;
	return NULL;
	ret->type = BuildType(R->type->name, NULL); // unnecessary
	return ret;
}
Expr *Relation(Expr *L, Expr *R, char *op){ // < > <= >= == !=
	if(L == NULL || R == NULL){
		return NULL;
	}

	Expr *ret = (Expr *)malloc(sizeof(Expr));
	ret->dim = 0;
	ret->e = NULL;

	if(strcmp(PrintType(L->type, L->dim), "int") == 0 && strcmp(PrintType(R->type, R->dim), "int") == 0){
		ret->type = BuildType("bool", NULL);
		return ret;
	}
	if(strcmp(PrintType(L->type, L->dim), "int") == 0 && strcmp(PrintType(R->type, R->dim), "float") == 0){
		ret->type = BuildType("bool", NULL);
		return ret;
	}
	if(strcmp(PrintType(L->type, L->dim), "float") == 0 && strcmp(PrintType(R->type, R->dim), "int") == 0){
		ret->type = BuildType("bool", NULL);
		return ret;
	}
	if(strcmp(PrintType(L->type, L->dim), "int") == 0 && strcmp(PrintType(R->type, R->dim), "double") == 0){
		ret->type = BuildType("bool", NULL);
		return ret;
	}
	if(strcmp(PrintType(L->type, L->dim), "double") == 0 && strcmp(PrintType(R->type, R->dim), "int") == 0){
		ret->type = BuildType("bool", NULL);
		return ret;
	}
	if(strcmp(PrintType(L->type, L->dim), "float") == 0 && strcmp(PrintType(R->type, R->dim), "float") == 0){
		ret->type = BuildType("bool", NULL);
		return ret;
	}
	if(strcmp(PrintType(L->type, L->dim), "double") == 0 && strcmp(PrintType(R->type, R->dim), "double") == 0){
		ret->type = BuildType("bool", NULL);
		return ret;
	}
	if(strcmp(PrintType(L->type, L->dim), "float") == 0 && strcmp(PrintType(R->type, R->dim), "double") == 0){
		ret->type = BuildType("bool", NULL);
		return ret;
	}
	if(strcmp(PrintType(L->type, L->dim), "double") == 0 && strcmp(PrintType(R->type, R->dim), "float") == 0){
		ret->type = BuildType("bool", NULL);
		return ret;
	}
	if(strcmp(op, "==") == 0 || strcmp(op, "!=") == 0){
		if(strcmp(PrintType(L->type, L->dim), "bool") == 0 && strcmp(PrintType(R->type, R->dim), "bool") == 0){
			ret->type = BuildType("bool", NULL);
			return ret;
		}
	}

	printf("##########Error at Line #%d:  LHS and RHS of %s should be int,float,double. LHS and RHS of ==,!= can be both boolean.##########\n", linenum, op);
	error = true;
	return NULL;

}

Expr *FindVar(Table *t, const char *name){
	Expr *ret = (Expr *)malloc(sizeof(Expr));
	Entry *find = FindEntryInAllClosest(t, name);
	if(find == NULL){
				strcpy(ret->id, name);
				ret->dim = 0;
				ret->e = NULL;
				ret->param = NULL;
				printf("##########Error at Line #%d: %s is not declared.##########\n", linenum, name);
				error = true;
				return NULL;
	}
	else if(strcmp(find->kind, "function") == 0){
		printf("##########Error at Line #%d: function name %s can't be variable (need calling function i.e.L_PAREN R_PAREN ).##########\n", linenum, find->name);
		error = true;
		return NULL;
	}

	strcpy(ret->id, name);
	ret->dim = 0;
	ret->e = find;
	ret->type = find->type;
	return ret;
}

Expr *ConstToExpr(Value *v){
	Expr *ret = (Expr*)malloc(sizeof(Expr));
	//strcpy(ret->kind, "constant");
	ret->dim = 0;
	ret->e = NULL;
	ret->type = v->type;
	ret->id[0] = '\0';
	if(strcmp(ret->type->name, "scientific") == 0){
		strcpy(ret->type->name, "float"); 
	}
	return ret;
}

Expr *Function_invoke(const char *name, ExprList *head, Table *t){
	Expr *ret = (Expr *)malloc(sizeof(Expr));

	strcpy(ret->id, name);
	ret->dim = 0;
	ret->e = FindEntryInGlobal(t, name);
	if(ret->e == NULL){
		printf("##########Error at Line #%d: function %s is not declared or defined.##########\n", linenum, name);
		error = true;
		return NULL;

	}
	ExprList *p = head;
	while(p != NULL){
		if(p->expression == NULL){
			return NULL; // parameter crash
		}
		p = p->next;
	}
	ret->type = ret->e->type;
	ret->param = CloneExprList(head); // function parameter

	CheckFuncParam(ret->e->attr->attrparam, ret->param);
	/*
	if(!CheckFuncParamNum(ret)){ // no problem in func parameter number
		if(ret->param == NULL){
			return ret;
		}
		CheckFuncParamType(ret->e->attr->attrparam, ret->param);
	}*/
	return ret;
}

AttrParam *CloneExprList(ExprList *list){

	if(list == NULL){
		return NULL;
	}

	AttrParam *result = (AttrParam *)malloc(sizeof(AttrParam));
	result->type = (Type *)malloc(sizeof(Type));
	if(list->expression == NULL){

	}
	else{
		strcpy(result->type->name, list->expression->type->name);

		TypeDim *p = list->expression->type->arr;

		int i = list->expression->dim;

		while(i--){
			if(p == NULL){
				// size mismatch
				//printf("##########Error at Line #%d: too many dimension for array %s.##########\n", linenum, list->expression->id);// error has been printed
				error = true;
				break;
			}
			p = p->next;
		}

		result->type->arr = CloneTypeDim(p);
	}
	result->next = CloneExprList(list->next);
	return result;
}
TypeDim *CloneTypeDim(TypeDim *list){
	if(list == NULL){
		return NULL;
	}
	TypeDim *result = (TypeDim *)malloc(sizeof(TypeDim));
	result->num = list->num;
	result->next = CloneTypeDim(list->next);
	return result;
}

bool CheckFuncParam(AttrParam *p1, AttrParam *p2){ // check if the func param is correct

	// p1 is true
	// p2 is input test
	bool mismatch_num = false;
	bool mismatch_type = false;
	if(p1 == NULL && p2 != NULL){
		printf("##########Error at Line #%d: function parameter mismatch .##########\n", linenum);
		error = true;
		mismatch_num = true;
		return true;
	}
	else if (p1 != NULL && p2 == NULL){
		printf("##########Error at Line #%d: function parameter mismatch .##########\n", linenum);
		error = true;
		mismatch_num = true;
		return true;	
	}
	else if(p1 == NULL && p2 == NULL){
		return false;
	}
	int correct_param_num = 0;
	int input_param_num = 0;
	AttrParam *p1tmp = p1;
	while(p1tmp != NULL){
		correct_param_num ++;
		p1tmp = p1tmp->next;
	}
	AttrParam *p2tmp = p2;
	while(p2tmp != NULL){
		input_param_num ++;
		p2tmp = p2tmp->next;
	}
	if(correct_param_num > input_param_num){
		printf("##########Error at Line #%d: function parameter mismatch .##########\n", linenum);
		error = true;
		mismatch_num = true;
		return true;
	}
	else if(correct_param_num < input_param_num){
		printf("##########Error at Line #%d: function parameter mismatch .##########\n", linenum);
		error = true;
		mismatch_num = true;
		return true;
	}

	while(p2 != NULL){
		if(strcmp(p1->type->name, p2->type->name) != 0){
			// Type Coercion
			if(strcmp(p1->type->name, "double") == 0 && strcmp(p2->type->name, "int") == 0){
				// Type Coercion
			}
			else if(strcmp(p1->type->name, "double") == 0 && strcmp(p2->type->name, "float") == 0){
				// Type Coercion
			}
			else if(strcmp(p1->type->name, "float") == 0 && strcmp(p2->type->name, "int") == 0){
				// Type Coercion
			}
			else{
				mismatch_type = true;
				break;
			}
		}
		TypeDim *tmp1 = p1->type->arr;
		TypeDim *tmp2 = p2->type->arr;
		while(tmp2 != NULL){
			if(tmp1 == NULL){

				mismatch_type = true;
				break;
			}
			if(tmp2->num != tmp1->num){
				mismatch_type = true;
				break;
			}
			tmp1 = tmp1->next;
			tmp2 = tmp2->next;
		}
		if(mismatch_type){
			break;
		}
		if(tmp1 != NULL){
			mismatch_type = true;
			break;
		}
		p1 = p1->next;
		p2 = p2->next;
	}
	if(mismatch_type){
		printf("##########Error at Line #%d: function parameter mismatch.##########\n", linenum);
		error = true;
		return true;
	}
	return false;
}

void CheckPtRd(Expr* R){
	// PRINT READ can only use scalar type

	if(R == NULL){ // R not declared
		return;
	}
		if(strcmp(PrintType(R->type, R->dim), "int") != 0 &&
			strcmp(PrintType(R->type, R->dim), "float") != 0 &&
			strcmp(PrintType(R->type, R->dim), "double") != 0 &&
			strcmp(PrintType(R->type, R->dim), "string") != 0 &&
			strcmp(PrintType(R->type, R->dim), "bool") != 0 &&
			strcmp(PrintType(R->type, R->dim), "scientific") != 0
		){
			printf("##########Error at Line #%d: Print/Read statements must be scalar type.##########\n", linenum);
			error = true;
		}

}

void CheckContinueBreak(bool in_forwhile){
	if(!in_forwhile){
		printf("##########Error at Line #%d: break and continue can only appear in loop statements.##########\n", linenum);
		error = true;
	}
}
void CheckType(Expr *L, Expr *R){ // for assign
	if(L == NULL || R == NULL){ // didn't declare
		return;
	}

	if(strcmp(FindEntryInAllClosest(symbol_table, L->id)->kind, "constant") == 0){
		printf("##########Error at Line #%d: Constants can't be re-assigned.##########\n", linenum);
		error = true;
		return;		
	}

	int cnt_L_typearr = 0, cnt_R_typearr = 0;
	TypeDim *pL = L->type->arr, *pR = R->type->arr;
	while(pL != NULL){
		cnt_L_typearr++;
		pL = pL->next;
	}
	while(pR != NULL){
		cnt_R_typearr++;
		pR = pR->next;
	}
	if(L->dim != cnt_L_typearr || R->dim != cnt_R_typearr){// L or R is array-type
		printf("##########Error at Line #%d: array arithmetic and assignment are not allowed.##########\n", linenum);
		error = true;
		return;
	}
	else if(strcmp(PrintType(L->type, L->dim), "double") == 0){
		if(strcmp(PrintType(R->type, R->dim), "int") != 0 && strcmp(PrintType(R->type, R->dim), "float") != 0 && strcmp(PrintType(R->type, R->dim), "double") != 0 && strcmp(PrintType(R->type, R->dim), "scientific") != 0){
			printf("##########Error at Line #%d: type mismatch.##########\n", linenum);
			error = true;
		}
		return;
	}
	else if(strcmp(PrintType(L->type, L->dim), "float") == 0){
		if(strcmp(PrintType(R->type, R->dim), "int") != 0 && strcmp(PrintType(R->type, R->dim), "float") != 0 && strcmp(PrintType(R->type, R->dim), "scientific") != 0){
			printf("##########Error at Line #%d: type mismatch.##########\n", linenum);
			error = true;
		}
		return;
	}
	else if(strcmp(PrintType(L->type, L->dim), "int") == 0){
		if(strcmp(PrintType(R->type, R->dim), "int") != 0){
			printf("##########Error at Line #%d: type mismatch.##########\n", linenum);
			error = true;
		}
		return;
	}
	else if(strcmp(PrintType(L->type, L->dim), "bool") == 0){
		if(strcmp(PrintType(R->type, R->dim), "bool") != 0){
			printf("##########Error at Line #%d: type mismatch.##########\n", linenum);
			error = true;
		}
		return;
	}
	else if(strcmp(PrintType(L->type, L->dim), "string") == 0){
		if(strcmp(PrintType(R->type, R->dim), "string") != 0){
			printf("##########Error at Line #%d: type mismatch.##########\n", linenum);
			error = true;
		}
		return;
	}
	else{// unknown condition?
		printf("##########Error at Line #%d: type mismatch.##########\n", linenum);
		error = true;
		return;
	}
}
void CheckBool(Expr *e){
	if(e == NULL){
		return;
	}
	if(strcmp(PrintType(e->type, e->dim), "bool") != 0){
		printf("##########Error at Line #%d: conditional expression should be boolean type.##########\n", linenum);
		error = true;
		return ;
	}
}
/*
bool CheckFuncParamType(AttrParam *p1, AttrParam *p2){
		// p1 is the true one
		// p2 is the input one
		// compare that two link should be exactly same
		bool mismatch = false;
		while(p2 != NULL){
			if(p1 == NULL){
				mismatch = true;
				break;
			}
			if(strcmp(p1->type->name, p2->type->name) != 0){
				mismatch = true;
				break;
			}
			TypeDim *tmp1 = p1->type->arr;
			TypeDIm *tmp2 = p2->type->arr;
			while(tmp2 != NULL){
				if(tmp1 == NULL){
					mismatch = true;
					break;
				}
				if(tmp2->num != tmp1->num){
					mismatch = true;
					break;
				}
			}
			if(mismatch){
				break;
			}
			if(tmp1 != NULL){
				mismatch = true;
				break;
			}

		}
		if(p1 != NULL){
			mismatch = true;
		}
		if(mismatch){
			printf("##########Error at Line #%d: parameter type mismatch.##########\n", linenum);
			error = true;
		}
		return mismatch;
}*/

ExprList *AddToExprList(ExprList *head, Expr *new){
	if(head == NULL){
		head = (ExprList *)malloc(sizeof(Expr));
		head->expression = (Expr *)malloc(sizeof(Expr));
		head->expression = new;
		head->next = NULL;
	}
	else{
		ExprList *p = head;
		while(p->next != NULL){
			p = p->next;
		}
		p->next = (ExprList *)malloc(sizeof(Expr));
		p->next->expression = (Expr *)malloc(sizeof(Expr));
		p->next->expression = new;
		p->next->next = NULL;
	}
	return head;
}
void FuncShouldDefine(Table *t){
	Entry *p = t->p;
	while(p != NULL){
		if(strcmp(p->kind, "function") == 0 && p->def == false){
			printf("##########Error at Line #%d: declared Function %s must be defined.##########\n", linenum, p->name);
			error = true;
		}
		p = p->next;
	}
    return;
}
