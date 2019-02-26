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
            printf("%-19s", PrintType(ptr->type));
            if(strcmp(ptr->kind, "constant") == 0 || strcmp(ptr->kind, "function") == 0)
            	PrintAttribute(ptr->attr);
            printf("\n");
        }
        ptr = ptr->next;
    }
    printf("=======================================================================================\n");
}

char *PrintType(Type *type){
	if(type == NULL){
		return NULL;
	}
	char* ret = (char*) calloc(80, sizeof(char));  // maybe int a[1][234][5678][999]..
	//return ret;//debug
	//memset(ret, '\0' ,80);
	char arr_buf[12];
	snprintf(ret, strlen(type->name)+1, "%s", type->name);
	TypeDim *tmp = type->arr;

	while(tmp != NULL){
		snprintf(arr_buf, 11, "[%d]", tmp->num);
		strcat(ret, arr_buf);
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
				printf("%s", PrintType(tmp->type));
			}
			else{
				printf("%s,", PrintType(tmp->type));
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

	while(p != NULL){
		Type *tmp2 = (Type*)malloc(sizeof(Type));
		strncpy(tmp2->name, type->name, 12);
		tmp2->arr = p->ptr->dim;
		Entry *tmp = BuildEntry(p->ptr->id, kind, t->level, tmp2, p->ptr->attr);
		InsertTableEntry(t, tmp);
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

idList* AddToidList(idList* ret, const char* name, TypeDim *dim, Attribute *attr){
	if(ret == NULL){
		ret = (idList*)malloc(sizeof(idList));
		ret->ptr = (id_struct*)malloc(sizeof(id_struct));
		strncpy(ret->ptr->id, name, 32);
		ret->ptr->dim = dim;
		ret->ptr->attr = attr;
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
		p->next->ptr->attr = attr;
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

TypeDim* AddToTypeDim(TypeDim *ret, int num){
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
