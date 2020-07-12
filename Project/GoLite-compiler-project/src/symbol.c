#include "symbol.h"
#include "string.h"
#include <stdlib.h>
#include <stdio.h>

PROGRAM *root;
int class_count = 0;

void prettyTabs(int n)
{
	for(int i = 0; i < n; i++)
	{
		printf("\t");
	}
}

int Hash(char *str){
	unsigned int hash = 0;
	while(*str) hash = (hash << 1) + *str++;
	return hash % HASHSIZE;
}


SYMBOL* makeSymbol(char* id, SymbolKind kind) {
	SYMBOL* sym = malloc(sizeof(SYMBOL));
	sym->name = id;
	sym->kind = kind;
	sym->isBaseType = 0;
	sym->isConstant = 0;
	return sym;
}

//TODO: add print for the top global scope
void addPredefinedtypeSymKindbol(SymbolTable *symbolTable) {
	SYMBOL *sym = NULL;
	TYPE *type = NULL;

	sym = makeSymbol("int", typeSymKind);
	sym->isBaseType = 1;
	putType(sym, symbolTable, 0);

	sym = makeSymbol("float64", typeSymKind);
	sym->isBaseType = 1;
	putType(sym, symbolTable, 0);

	sym = makeSymbol("rune", typeSymKind);
	sym->isBaseType = 1;
	putType(sym, symbolTable, 0);

	sym = makeSymbol("string", typeSymKind);
	sym->isBaseType = 1;
	putType(sym, symbolTable, 0);

	sym = makeSymbol("bool", typeSymKind);
	sym->isBaseType = 1;
	putType(sym, symbolTable, 0);

    // store true and false as two variable in var table, with bool type
    sym = makeSymbol("true", varSymKind); //add isConst to print CONST
    sym->val.varType = getSymbol(symbolTable, "bool", 0);
    sym->isConstant = 1;
    putVar(sym, symbolTable, 0);
    
    sym = makeSymbol("false", varSymKind);
    sym->val.varType = getSymbol(symbolTable, "bool", 0);
    sym->isConstant = 1;
    putVar(sym, symbolTable, 0);

}


SymbolTable* initSymbolTable() {
	SymbolTable *t = malloc(sizeof(SymbolTable));

	for (int i = 0; i < HASHSIZE; i++) {
		t->varTable[i] = malloc(sizeof(SYMBOL));
		t->varTable[i]->kind = nullSymKind;
		t->typeTable[i] = malloc(sizeof(SYMBOL));
		t->typeTable[i]->kind = nullSymKind;
		t->funcTable[i] = malloc(sizeof(SYMBOL));
		t->funcTable[i]->kind = nullSymKind;
	}

	t->parent = NULL;
	return t;
}

SymbolTable *scopeSymbolTable(SymbolTable *parent)
{
	SymbolTable *t = initSymbolTable();
	t->parent = parent;
	return t;
}


SYMBOL* localSymbolTableCheck(SymbolTable *t, char* id)
{
	int cell = Hash(id);

	SYMBOL *tmp = t->varTable[cell];
	while(tmp->kind != nullSymKind)
	{
		if (strcmp(tmp->name, id) == 0) return tmp;
		tmp = tmp->next;
	}

	tmp = t->typeTable[cell];
	while(tmp->kind != nullSymKind)
	{
		if (strcmp(tmp->name, id) == 0) return tmp;
		tmp = tmp->next;
	}

	tmp = t->funcTable[cell];
	while(tmp->kind != nullSymKind)
	{
		if (strcmp(tmp->name, id) == 0) return tmp;
		tmp = tmp->next;
	}

	return NULL;
}

void assignTableDepth(SYMBOL *s, SymbolTable *t){
	int k = 0;

	while(t->parent != NULL){
		k++;
		t = t->parent;
	}
	s->tableDepth = k;
}

void putVar(SYMBOL *s, SymbolTable *t, int lineno)
{
	s->lineno = lineno;
	int cell = Hash(s->name);
	s->next = t->varTable[cell];
	SYMBOL *tmp = s->next;
	t->varTable[cell] = s;
	assignTableDepth(s, t);
}

void putType(SYMBOL *s, SymbolTable *t, int lineno)
{
	s->lineno = lineno;
	int cell = Hash(s->name);
	s->next = t->typeTable[cell];
	SYMBOL *tmp = s->next;
	t->typeTable[cell] = s;
	assignTableDepth(s, t);
	//printf("table: %d" , s->tableDepth);
}

void putFunc(SYMBOL *s, SymbolTable *t, int lineno)
{
	s->lineno = lineno;
	int cell = Hash(s->name);
	s->next = t->funcTable[cell];
	SYMBOL *tmp = s->next;
	t->funcTable[cell] = s;
	assignTableDepth(s, t);
}


//check for redeclaration in local scope
void redeclCheck(int lineno, char *id, SymbolTable *t) {
	int cell = Hash(id);

    //check for var table
	SYMBOL *tmp = t->varTable[cell];
	while(tmp->kind != nullSymKind)
	{
		if (strcmp(tmp->name, id) == 0){
			if(!tmp->isConstant){ //acount for redecl of true/false
				fflush(stdout);
				fprintf(stderr, "Error: (line %d) %s already declared on line %d\n", lineno, id, tmp->lineno);
				exit(1);
			}
		}
		else tmp = tmp->next;
	}

    //check type table
	tmp = t->typeTable[cell];
	while(tmp->kind != nullSymKind)
	{
		if (strcmp(tmp->name, id) == 0){
			fflush(stdout);
			fprintf(stderr, "Error: (line %d) %s already declared on line %d\n", lineno, id, tmp->lineno);
			exit(1);
		}
		else tmp = tmp->next;
	}

    //check func table
	tmp = t->funcTable[cell];
	while(tmp->kind != nullSymKind)
	{
		if (strcmp(tmp->name, id) == 0){
			fflush(stdout);
			fprintf(stderr, "Error: (line %d) %s already declared on line %d\n", lineno, id, tmp->lineno);
			exit(1);
		}
		else tmp = tmp->next;
	}
}

/*returns the symbol named identifier*/
SYMBOL *getSymbol(SymbolTable *t, char* id, int lineno)
{
	int cell = Hash(id);

	SYMBOL *tmp = t->varTable[cell];
	while(tmp->kind != nullSymKind)
	{
		if (strcmp(tmp->name, id) == 0) return tmp;
		tmp = tmp->next;
	}

	tmp = t->typeTable[cell];
	while(tmp->kind != nullSymKind)
	{
		if (strcmp(tmp->name, id) == 0) return tmp;
		tmp = tmp->next;
	}

	tmp = t->funcTable[cell];
	while(tmp->kind != nullSymKind)
	{
		if (strcmp(tmp->name, id) == 0) return tmp;
		tmp = tmp->next;
	}

	if (t->parent != NULL) return getSymbol(t->parent, id, lineno);
	else {
		fprintf(stderr, "Error: (line %d) %s is not declared.\n", lineno, id);
		exit(1);
	}
}

//First will build a symbol table for the program
void symPROGRAM(PROGRAM *prog)
{
	root = prog;
	SymbolTable *symbolTable = initSymbolTable();
	addPredefinedtypeSymKindbol(symbolTable);

	if(print){
		printf("{\n");
		prettyTabs(1);
		printf("int[type] = int\n");
		prettyTabs(1);
		printf("float64[type] = float64\n");
		prettyTabs(1);
		printf("bool[type] = bool\n");
		prettyTabs(1);
		printf("rune [type] = rune\n");
		prettyTabs(1);
		printf("string[type] = string\n");
		prettyTabs(1);
		printf("true[constant] = bool\n");
		prettyTabs(1);
		printf("false[constant] = bool\n");

		prettyTabs(1);
		printf("{\n");
	}
	
    //then feed to the program and used as the main table
	SymbolTable *declTable = scopeSymbolTable(symbolTable);

	symDECLARATION(prog->decl, declTable, 2);

	if(print){
		prettyTabs(1);
		printf("}\n");
		printf("}\n");
	}
}

void symDECLARATION(DECLARATION *decl, SymbolTable *symbolTable, int tabs) {

	if (decl == NULL){
		return;
	}

	if(decl->next != NULL){
		symDECLARATION(decl->next, symbolTable, tabs);
	}

	switch (decl->kind) {
		case k_DeclarationKindFunction:
			symFUNC(decl->val.func_decl, symbolTable, tabs);
			break;
		case k_DeclarationKindVar:
			symVAR(decl->val.var_decl, symbolTable, tabs);
			break;
		case k_DeclarationKindType:
			symTYPE(decl->val.type_decl, symbolTable, tabs);
			break;
	}
}

void symFUNC_param(SYMBOL* sym, PARAM *param, SymbolTable *table, int tabs){
	if(param == NULL) return;
	symFUNC_param(sym, param->next, table, tabs);

	Node* param_list = symFUNC_decl(param->id_list, param->type, table, tabs);
	//link the two lists together
	if(sym->val.func.funcParams == NULL){
			sym->val.func.funcParams = param_list;
	}
	else{
		//find the last element in the param list
		Node *last = param_list;
		while(last->next){
			last = last->next;
		}
		last->next = sym->val.func.funcParams;
		sym->val.func.funcParams = param_list;
	}
}

void symCheckType(SYMBOL *type_sym){
	if(type_sym->kind == varSymKind || type_sym->kind == funcSymKind || type_sym->kind == nullSymKind){
		fprintf(stderr, "Error: (line %d) %s is not a type\n", type_sym->lineno, type_sym->name);
		exit(1);
	}
}

//mode 1: print type
//mode 0: print infer
void printVar(SYMBOL *sym, TYPE *t, int tabs, bool mode){
	prettyTabs(tabs);
	if(mode) {
		printf("%s [variable] = ", sym->name);
		printType(t);
	}
	else printf("%s [variable] = <infer>", sym->name);

	printf("\n");
}

Node* symFUNC_decl(EXP *id, TYPE *t, SymbolTable *table, int tabs) {
	SYMBOL *sym;
	Node *param_list = malloc(sizeof(Node));
	SYMBOL *type_sym;
	char *name = id->val.identifier.name;

	sym = makeSymbol(name, varSymKind);
	param_list->val = sym;	
	param_list->next = NULL;

	//assign type to the symbol 
	type_sym = symMakeParentTypeHelper(t, table);
	sym->val.varType = type_sym;

	if(strcmp("_", name) != 0) {
		//redeclCheck(id->lineno, name, table);
		//put the symbol into table
		//putVar(sym, table, t->lineno);
		if(print) printVar(sym, t, tabs, true);
	    //put into AST
		//id->val.identifier.sym = sym;
	}
	id->val.identifier.sym = sym;
	id = id->next;

	while(id != NULL){
		name = id->val.identifier.name;
		sym = makeSymbol(name, varSymKind);
		sym->val.varType = type_sym;

		if(strcmp("_", name) != 0){
			//redeclCheck(id->lineno, name, table);
			//putVar(sym, table, t->lineno);
			if(print) printVar(sym, t, tabs, true);
			//put into AST
			//id->val.identifier.sym = sym;
		}
		id->val.identifier.sym = sym;
		Node *node = malloc(sizeof(Node));
		node->val = sym;
		node->next = param_list;
		param_list = node;
		id = id->next;
	}

	return param_list;
}

void symFUNC_mainAndInit(FUNCDECL *func){
	char *name = func->name->val.identifier.name;
	if(strcmp(name, "main") == 0 || strcmp(name, "init") == 0){
		if (func->returnType != NULL || func->params != NULL) {
    		fprintf(stderr, "Error: (line %d) %s must have no parameters and no return value.\n", func->lineno, func->name->val.identifier.name);
    		exit(1);
    	}
	}
}


void symFUNC(FUNCDECL *func, SymbolTable *s, int tabNum) {
    //check if main/init then no params nor return values
    symFUNC_mainAndInit(func);
	SYMBOL *tmp = makeSymbol(func->name->val.identifier.name, funcSymKind);
	
	SYMBOL *type;
	SYMBOL *struct_symbol;

	//1. if init or _ function, don't add mapping
	if(strcmp(tmp->name, "init") == 0) {
		if(print){
			prettyTabs(tabNum);
			printf("%s [function] = <unmapped>", func->name->val.identifier.name);
		}
		
	} 
	else{
		if(strcmp(tmp->name, "_") == 0){
			if(print){
				prettyTabs(tabNum);
				printf("%s [function] = <unmapped>", func->name->val.identifier.name);
			}
			if(func->returnType){
				TYPE *t = func->returnType;
				tmp->val.func.returnSymRef = symMakeParentTypeHelper(t, s);
			}
			
		}
		else{
			redeclCheck(func->lineno, func->name->val.identifier.name, s);
			putFunc(tmp, s, func->lineno);
			if(print){
				prettyTabs(tabNum);
				printf("%s [function] = (", func->name->val.identifier.name);

				//2. print the parameters
				printFuncParams(func->params);
				printf(") -> ");
			}
			
			//3. print the return type
			if(func->returnType == NULL){
				if(print) printf("void");
				//if void function, the func symbol's returnSymRef = NULL
			}
			else{
				TYPE *t = func->returnType;
				tmp->val.func.returnSymRef = symMakeParentTypeHelper(t, s);
				if (print) printType(t);
			}
		}
		func->sym = tmp;
	}

	//4. make new function scope
	SymbolTable *subTable = scopeSymbolTable(s);
	func->localScope = subTable;

	//5. print the new scope
	if (print) printf("\n");
	printScopeIn(tabNum);

	//6a. construct param node list
	symFUNC_param(tmp, func->params, subTable, tabNum+1);

	//6b. put parameters inside the new scope
	Node *params = tmp->val.func.funcParams;
	while(params){
		if(strcmp(params->val->name, "_") != 0){
			redeclCheck(func->lineno, params->val->name, subTable);
			putVar(params->val, subTable, func->lineno);
		}
		params = params->next;
	}

	//7. call sym on function body
    symSTMTS(func->body, subTable, tabNum+1);

    printScopeOut(tabNum);
}

void printFuncParams(PARAM *params)
{
	if (params == NULL) return;	
	else if (params->next != NULL){
		printFuncParams(params->next);
		printf(", ");
	}

	EXP *id = params->id_list;
	printType(params->type);
	id = id->next;
 	if(id) printf(", ");
	
	while(id){
		printType(params->type);
		id = id->next;
		if(id) printf(", ");
	}
}

void printType(TYPE *type)
{
	if(type == NULL) return;

	switch(type->kind){
		case k_TypeKindArray:
			printf("[%d]", type->val.tArray.size);
			printType(type->val.tArray.type);
			break;

		case k_TypeKindSlice:
			printf("[]");
			printType(type->val.tSlice.type);
			break;

		case k_TypeKindSingle:
			printf("%s", type->val.identifier->val.identifier.name);
			break;

		case k_TypeKindStruct:
			printf("struct { ");
			STRUCTFIELD *sf = type->val.tStruct.fields;
			printStruct(sf);
			printf("}");
			break;
	}
}

void printStruct(STRUCTFIELD *sf) {
	if(sf == NULL) return;
	printStruct(sf->next);

	EXP *temp = sf->id_list;
	TYPE *type = sf->type;

	while (temp != NULL) {
		printf("%s ", temp->val.identifier.name);
		printType(type);
		printf("; ");
		temp = temp->next;
	}
}

CLASS *createNewClass(SYMBOL *sym){
	CLASS *newClass = malloc(sizeof(CLASS));
	char *str = malloc(100*sizeof(char));
    strcpy(str, "__golitec__class");
    char num[10];
    sprintf(num, "%d", class_count);
    strcat(str, num);
    class_count++;
    newClass->structClass = sym;
    newClass->name = str;
    return newClass;
}

//returns the name of the struct class
char *addStructClass(SYMBOL *sym){
	CLASS *list = root->class_list;
	CLASS *newClass;
	char *str;

	if(list == NULL){
		newClass = createNewClass(sym);
		newClass->next = NULL;
		root->class_list = newClass;
		return newClass->name;
	}
	else{
		while(list){
			//printf("list: %s\n", list->structClass->name);
			if (checkEqualStruct(list->structClass, sym)){
				return list->name;
			}
			list = list->next;
		}
		//new class
		newClass = createNewClass(sym);
		newClass->next = root->class_list;
		root->class_list = newClass;
		return newClass->name;
	}
}

bool checkEqualStruct(SYMBOL *struct1, SYMBOL *struct2){
	SYMBOL *s1 = struct1->val.structFields;
	SYMBOL *s2 = struct2->val.structFields;

	while(s1){
		if(s2 == NULL){
			return false;
		}
		if(strcmp(getTypePrintName(s1), getTypePrintName(s2)) != 0)
			return false;
		if(strcmp(s1->name, s2->name) != 0)
			return false;
		s1 = s1->next;
		s2 = s2->next;
	}
	if(s2 != NULL) return false;
	return true;
}

void symTYPE(TYPEDECL *type_decl, SymbolTable *s, int tabs) {
	if(type_decl == NULL) return;
	else if(type_decl->next != NULL){
		symTYPE(type_decl->next, s, tabs);
	}

	//to deal with recursive types, first put in the symbol table
    //change sym kind later
    char *name = type_decl->id->val.identifier.name;
	SYMBOL *sym = makeSymbol(name, nullSymKind);

    //check for blank id
	if(strcmp("_", name) == 0){
		//check for underlying type
		makeParentType(type_decl->type, sym, s);
		return;
	}

	checkMainorInit(s, name, type_decl->lineno);

	redeclCheck(type_decl->lineno, type_decl->id->val.identifier.name, s);
    //rule out: type int int, type int [9]int, or b struct {x b;}
	checkRecursiveType(name, type_decl->type);

    //add sym to table
	putType(sym, s, type_decl->lineno);

	TypeKind kind = type_decl->type->kind;

	if(print) prettyTabs(tabs);

	if(kind == k_TypeKindSingle){
		//b -> a -> int
        //sym's kind will be changed to a's kind
		SYMBOL *tmp = lookUpType(type_decl->type, s);
		s->typeTable[Hash(name)]->kind = tmp->kind;
		s->typeTable[Hash(name)]->val.parentType = tmp;
		if(tmp->kind == structSymKind){
			s->typeTable[Hash(name)]->structClassName = tmp->structClassName;
		}

		if (print) {
			fprintf(stdout, "%s [type] = %s ", name, name);
			printSingleType(sym);
			printf("\n");
		}
	}
	else{
		switch(kind){
			case k_TypeKindSlice:
				s->typeTable[Hash(name)]->kind = sliceSymKind;
				makeParentType(type_decl->type, sym, s);
				break;
			case k_TypeKindArray:
				s->typeTable[Hash(name)]->kind = arraySymKind;
				makeParentType(type_decl->type, sym, s);
				break;
			case k_TypeKindStruct:
				s->typeTable[Hash(name)]->kind = structSymKind;
				symTYPE_struct(type_decl->type->val.tStruct.fields, sym, s);
				sym->structClassName = addStructClass(sym);
		}
		
		if (print) {
			printf("%s [type] = %s -> ", name, name);
			printType(type_decl->type);
			printf("\n");
		}
	}

	//fprintf(stdout, "%s parent type is %s \n", name, sym->val.parentType->name);
}

 //b -> a -> []int
void printSingleType(SYMBOL *sym){
	SYMBOL *parent_type = sym->val.parentType;

	while(parent_type && strcmp(parent_type->name, "") != 0){
		if(parent_type->kind == structSymKind){
			printf("-> %s ", parent_type->name);
			break;
		}
		printf("-> %s ", parent_type->name);
		parent_type = parent_type->val.parentType;
	}

	if(parent_type){
		printf("-> ");
		printSymType(parent_type);
	}
	
}

void printStructSym(SYMBOL *sym){
	if(sym == NULL) return;
	else if (sym->next){
		printStructSym(sym->next);
	}

	printf("%s ", sym->name);
	printSymType(sym->val.varType);
	printf("; ");
}

void printSymType(SYMBOL *sym){
	while(sym){
		if(strcmp(sym->name, "") != 0 && strcmp(sym->name, "struct") != 0){
			printf("%s", sym->name);
			return;
		}
		switch(sym->kind){
			case arraySymKind:
				printf("[%d]", sym->arraySize);
				break;
			case sliceSymKind:
				printf("[]");
				break;
			case structSymKind:
				printf("struct { ");
				printStructSym(sym->val.structFields);
				printf("}");
				return;
			case typeSymKind:
				printf("%s", sym->name);
				return;
		}
		sym = sym->val.parentType;
	}
}

bool checkIsBaseType(TYPE *t){
	char *name = getTypeName(t);
	if(strcmp(name, "int") == 0 || strcmp(name, "float64") == 0 || strcmp(name, "rune") == 0 ||
		strcmp(name, "bool") == 0 || strcmp(name, "string") == 0){
		return true;
	}
	return false;
}

SYMBOL* symMakeParentTypeHelper(TYPE *t, SymbolTable *table){
	SYMBOL *type_sym;

	switch(t->kind){
		case k_TypeKindSingle:
			type_sym = lookUpType(t, table);
			break;

		case k_TypeKindSlice:
			type_sym = makeSymbol("slice", sliceSymKind);
			type_sym->isBaseType = checkIsBaseType(t);
			makeParentType(t, type_sym, table);
			break;

		case k_TypeKindArray:
			type_sym = makeSymbol("array", arraySymKind);
			type_sym->isBaseType = checkIsBaseType(t);
			makeParentType(t, type_sym, table);
			break;

		case k_TypeKindStruct:
			//untagged struct
			type_sym = makeSymbol("", structSymKind);
			symTYPE_struct(t->val.tStruct.fields, type_sym, table);
			type_sym->structClassName = addStructClass(type_sym);
			break;
	}
	return type_sym;
}

//construct a list of parentType eg. type a [][]int will be a ->[] -> [] -> int
//singleType flag tells if it is all single type types, b -> a -> int  or  c -> []b
//1. constructs the intermediate type nodes
//2. checks if the base type is declared
void makeParentType(TYPE *t, SYMBOL *type_sym, SymbolTable *table){
	if(t == NULL) return;

	SYMBOL *temp;

	switch(t->kind){
		case k_TypeKindSlice:
			temp = makeSymbol("", sliceSymKind);
			type_sym->val.parentType = temp;
			makeParentType(t->val.tSlice.type, temp, table);
			break;

		case k_TypeKindArray:
			temp = makeSymbol("", arraySymKind);
			temp->arraySize = t->val.tArray.size;
			type_sym->val.parentType = temp;
			makeParentType(t->val.tArray.type, temp, table);
			break;

		case k_TypeKindStruct:
			temp = makeSymbol("struct", structSymKind);
			type_sym->val.parentType = temp;
			symTYPE_struct(t->val.tStruct.fields, temp, table);
			type_sym->structClassName = addStructClass(type_sym);
			break;
		
		case k_TypeKindSingle:
			temp = lookUpType(t, table);
			temp->isBaseType = 1;
			type_sym->val.parentType = temp;
			break;
	}
}

//go to the global symbol table and find the predefined types
//invalid recursive types: single and array and struct
void checkRecursiveType(char *name, TYPE *t){
    //slice can be recursive
    //type int [5][]int is valid
    //as long as there is slice in the type, it is valid
	if(t->kind == k_TypeKindSlice){
		return;
	}
	else if (t->kind == k_TypeKindArray){
		checkRecursiveType(name, t->val.tArray.type);
	}
	else if(t->kind == k_TypeKindStruct){
		checkRecursiveStruct(name, t);
		return;
	}
	else if (t->kind == k_TypeKindSingle){
		char *type_name = t->val.identifier->val.identifier.name;
		if(strcmp(name, type_name) == 0){
			fflush(stdout);
			fprintf(stderr, "Error: (line %d) invalid recursive type %s\n", t->lineno, name);
			exit(1);
		}
	}

}

void checkRecursiveStruct(char *name, TYPE* structType) {
	STRUCTFIELD* field = structType->val.tStruct.fields;

	while (field != NULL) {
		if (field->type->kind == k_TypeKindStruct) {
			checkRecursiveStruct(name, field->type);
		}
		else{
			checkRecursiveType(name, field->type);
		}

		field = field->next;
	}
}

//in struct, the field name cannot be the same
//consider list of identifiers
//struct within a struct
SYMBOL* symTYPE_struct(STRUCTFIELD *field_list, SYMBOL *struct_symbol, SymbolTable *table){
    
	struct_symbol->isBaseType = 1;

	//1. iterate through all the fields of the struct
	if(field_list == NULL){
		return struct_symbol;
	}else{
		symTYPE_struct(field_list->next, struct_symbol, table);
	}

	EXP *ids = field_list->id_list;
	char *name = ids->val.identifier.name;
	SYMBOL *type_of_field;

    //3. iterate through the id_list
    //3.a check if id already exist in struct scope
	checkStructId_List(struct_symbol, name, ids->lineno);
    //if new id name, make new var symbol and add to struct fields list
	SYMBOL *sym_id = makeSymbol(name, varSymKind);
	sym_id->lineno = ids->lineno;

	//2. check if the type is defined in the scopes
    //already checks if parent type is defined in the scopes by calling lookUpType in makeParentType
	type_of_field = symMakeParentTypeHelper(field_list->type, table);

    //reassign head pointers of the structFields list
    //in struct, the fields are not added to the symbol table but rather stored in the structFields list
	sym_id->val.varType = type_of_field;
	sym_id->next = struct_symbol->val.structFields;
	struct_symbol->val.structFields = sym_id;

    //iterate throuh id_list
	ids = ids->next;

	while(ids != NULL){
		checkStructId_List(struct_symbol, ids->val.identifier.name, ids->lineno);
		sym_id = makeSymbol(ids->val.identifier.name, varSymKind);
		sym_id->val.varType = type_of_field;
		sym_id->next = struct_symbol->val.structFields;
		struct_symbol->val.structFields = sym_id;

		ids = ids->next;
	}

}

//returns the symbol that the type is associated with
SYMBOL *lookUpType(TYPE *t, SymbolTable *table){

	char *type_name;
	if(t->kind != k_TypeKindStruct){
		type_name = getTypeName(t);
	}
	else{
		fprintf(stderr, "(line %d) you did something wrong. should never look up a struct type!\n", t->lineno);
		exit(1);
	}

	SYMBOL *sym = getSymbol(table, type_name, t->lineno);
	symCheckType(sym);
	return sym;
}

//returns the type name   eg. [][]a returns a
char *getTypeName(TYPE *t){
	if(t == NULL) return "";

	switch(t->kind){
		case k_TypeKindSingle:
			return t->val.identifier->val.identifier.name;
		case k_TypeKindSlice:
			getTypeName(t->val.tSlice.type);
			break;
		case k_TypeKindArray:
			getTypeName(t->val.tArray.type);
			break;
		case k_TypeKindStruct:
			fprintf(stderr, "it is a struct, structs doesn't have name!!!\n");
			exit(1);
	}
}


//function to check for duplicate fields in a struct
void checkStructId_List(SYMBOL *s, char *name, int lineno){
	SYMBOL *fields;
	for(fields = s->val.structFields; fields; fields = fields->next){
		if(strcmp(name, "_") != 0){
			if(strcmp(name, fields->name) == 0){
				fprintf(stderr, "Error: (line %d) %s already declared on line %d\n", lineno, name, fields->lineno);
				exit(1);
			}
		}
	}
}

//x, y int
//x int
//x = 5
//x, y int = 5, 6
void symVAR(VARDECL *var, SymbolTable *s, int tabs) {
	if(var == NULL) return;
	else if (var->next != NULL){
		symVAR(var->next, s, tabs);
	}

	switch(var->kind){
		case k_VarDeclKindType:
			symVAR_decl(var->val.type_only.id_list, var->val.type_only.type, s, tabs);
			break;

		case k_VarDeclKindExpr:
	        //do not store expr, type inferred in type check
			symVAR_declExpr(var, s, tabs);
			break;

		case k_VarDeclKindBoth:
			symVAR_both(var->val.exp_type.id_list, var->val.exp_type.type, var->val.exp_type.expr_list, s, tabs);
			break;
	}
}

void checkMainorInit(SymbolTable *table, char *name, int lineno){
	if(table->parent != NULL){
		if(table->parent->parent == NULL){
			if(strcmp("main", name) == 0){
				fflush(stdout);
				fprintf(stderr, "Error: (line %d) main must be a function\n", lineno);
				exit(1);
			}
			else if(strcmp("init", name) == 0){
				fflush(stdout);
				fprintf(stderr, "Error: (line %d) init must be a function\n", lineno);
				exit(1);
			}
		}
	}
}

//1. check for blank id, main/init, redeclaration
//2. check for valid type sym existing
//3. iterate through id_list
//4. assign the type to the var symbol
//5. put var into symbol table
void symVAR_decl(EXP *id, TYPE *t, SymbolTable *table, int tabs) {
	SYMBOL *sym;
	SYMBOL *type_sym;
	char *name = id->val.identifier.name;

	if(strcmp("_", name) == 0) {
		sym = makeSymbol(name, varSymKind);
		//check the type
		type_sym = symMakeParentTypeHelper(t, table);
		sym->val.varType = type_sym;
		//put into AST
		id->val.identifier.sym = sym;
	}
	else{
		checkMainorInit(table, name, t->lineno);
		redeclCheck(id->lineno, name, table);

		sym = makeSymbol(name, varSymKind);
		type_sym = symMakeParentTypeHelper(t, table);
		sym->val.varType = type_sym;
		//put the symbol into table
		putVar(sym, table, t->lineno);
		if(print) printVar(sym, t, tabs, true);
	    //put into AST
		id->val.identifier.sym = sym;
	}
	
	id = id->next;

	while(id != NULL){
		name = id->val.identifier.name;
		if(strcmp("_", name) == 0) {
			sym = makeSymbol(name, varSymKind);
			sym->val.varType = type_sym;
			id->val.identifier.sym = sym;
			id = id->next;
			continue;
		}
		checkMainorInit(table, name, t->lineno);
		redeclCheck(id->lineno, name, table);
		sym = makeSymbol(name, varSymKind);
		sym->val.varType = type_sym;
		putVar(sym, table, t->lineno);

        //put into AST
		id->val.identifier.sym = sym;
		if(print) printVar(sym, t, tabs, true);

		id = id->next;
	}
}

//x = 5
//x, y = z, 8
void symVAR_declExpr(VARDECL *var, SymbolTable *table, int tabs) {

    //id_list and expr_list are stored in order
	EXP *id = var->val.exp_only.id_list;
	EXP *expr = var->val.exp_only.expr_list;

	SYMBOL *sym;
	SYMBOL *temp;
	char *name;

    symEXP(expr, table);
    //iterate though the two lists
	while(id){
        //1. check for blank id, main/init, redeclaration
		name = id->val.identifier.name;
		if(strcmp("_", name) == 0) {
			sym = makeSymbol(name, varSymKind);
			temp = makeSymbol("", inferSymKind);
			sym->val.varType = temp;
			id->val.identifier.sym = sym;
			id = id->next;
			//expr = expr->next;
			continue;
		}
		checkMainorInit(table, name, var->lineno);
		redeclCheck(var->lineno, name, table);

        //2. create var symbol
		sym = makeSymbol(name, varSymKind);

		//3. all types are inferred in typecheck
        temp = makeSymbol("", inferSymKind);
		sym->val.varType = temp;

        //4. put var into symbol table
		putVar(sym, table, var->lineno);
        //5. put into AST
		id->val.identifier.sym = sym;

		if(print) printVar(sym, NULL, tabs, false);
	
		id = id->next;
		//expr = expr->next;
	}
}

void symVAR_both(EXP *id, TYPE *t, EXP *expr, SymbolTable *table, int tabs) {
	SYMBOL *sym;
	SYMBOL *type_sym;
	char *name = id->val.identifier.name;

	if(strcmp("_", name) == 0) {
		sym = makeSymbol(name, varSymKind);
		type_sym = symMakeParentTypeHelper(t, table);
		sym->val.varType = type_sym;
		id->val.identifier.sym = sym;
		//sym check the rhs
		symEXP(expr, table);
	}
	else{
		checkMainorInit(table, name, t->lineno);
		redeclCheck(id->lineno, name, table);

		sym = makeSymbol(name, varSymKind);
		type_sym = symMakeParentTypeHelper(t, table);
		sym->val.varType = type_sym;

		//sym check the rhs
		symEXP(expr, table);

	    //put the symbol into table
		putVar(sym, table, t->lineno);

		if(print) printVar(sym, t, tabs, true);
	 
	    //put into AST
		id->val.identifier.sym = sym;
	}
	
	id = id->next;

	while(id != NULL){
		name = id->val.identifier.name;
		if(strcmp("_", name) == 0) {
			sym = makeSymbol(name, varSymKind);
			sym->val.varType = type_sym;
			id->val.identifier.sym = sym;
			id = id->next;
			continue;
		}
		checkMainorInit(table, name, t->lineno);
		redeclCheck(id->lineno, name, table);
		sym = makeSymbol(name, varSymKind);
		sym->val.varType = type_sym;

		//sym check the rhs
		//symEXP(expr, table);
		//expr = expr -> next;
		putVar(sym, table, t->lineno);

        //put into AST
		id->val.identifier.sym = sym;
		if(print) printVar(sym, t, tabs, true);

		id = id->next;
	}
}

void printScopeIn(int tabNum){
	if(print){
		//printf("\n");
		prettyTabs(tabNum);
		printf("{\n");
	}
}

void printScopeOut(int tabNum){
	if(print){
	    prettyTabs(tabNum);
	   	printf("}\n");
	}
}


void symSTMTS(STMT *s, SymbolTable* symbolTable, int tabNum) {
    if (s == NULL){
        return;
    }

    if (s->next != NULL){
        symSTMTS(s->next, symbolTable, tabNum);
    }
    EXP *lhs, *rhs;
    SymbolTable *newscope;
    SymbolTable *initscope;

    switch (s->kind) {
        case k_StatementKindBlock:
            newscope = scopeSymbolTable(symbolTable);
            printScopeIn(tabNum);
            symSTMTS(s->val.block_stmt, newscope, tabNum+1);
            printScopeOut(tabNum);
            break;
        case k_StatementKindWhile:
            //check condition
            symEXP(s->val.while_stmt.cond, symbolTable);
            symSTMTS(s->val.while_stmt.body, symbolTable, tabNum);
            break;
        case k_StatementKindIf:
            //build new scope for init stmt 
        	initscope = scopeSymbolTable(symbolTable);
        	printScopeIn(tabNum);
        	symSTMTS(s->val.if_stmt.opt_cond, initscope, tabNum+1);
            //check condition inside the init scope
            symEXP(s->val.if_stmt.cond, initscope);
            //build new scope for body
            newscope = scopeSymbolTable(initscope);
            printScopeIn(tabNum+1);
            //check body inside new scope
            symSTMTS(s->val.if_stmt.body, newscope, tabNum+2);
            printScopeOut(tabNum+1);
            //check else part
            if (s->val.if_stmt.elseif_part != NULL){
                symSTMTS(s->val.if_stmt.elseif_part, initscope, tabNum+1);
            }
            printScopeOut(tabNum);
            break;
        case k_StatementKindElseIf:
        	initscope = scopeSymbolTable(symbolTable);
        	printScopeIn(tabNum);
            symSTMTS(s->val.elseif_stmt.opt_cond, initscope, tabNum+1);
            //check condition
            symEXP(s->val.elseif_stmt.cond, initscope);
            newscope = scopeSymbolTable(initscope);
            printScopeIn(tabNum+1);
            //check body
            symSTMTS(s->val.elseif_stmt.body, newscope, tabNum+2);
            printScopeOut(tabNum+1);
            //check else part
            if (s->val.elseif_stmt.elseif_part != NULL){
                symSTMTS(s->val.elseif_stmt.elseif_part, initscope, tabNum+1);
            }
            printScopeOut(tabNum);
            break;
        case k_StatementKindElse:
        	newscope = scopeSymbolTable(symbolTable);
        	printScopeIn(tabNum);
            symSTMTS(s->val.else_stmt.body, scopeSymbolTable(newscope), tabNum+1);
            printScopeOut(tabNum);
            break;
        case k_StatementKindAssign:
        	if(s->val.assign_stmt.lhs->kind == k_ExpressionKindIdentifier){
        		char *name = s->val.assign_stmt.lhs->val.identifier.name;
        		if(strcmp(name, "true") == 0 || strcmp(name, "false") == 0){
        			SYMBOL *sym = getSymbol(symbolTable, name, s->lineno);
        			if(sym->isConstant){
        				fprintf(stderr, "Error: line(%d): invalid assignment\n", s->lineno);
        				exit(1);
        			}
        			else{
        				s->val.assign_stmt.lhs->val.identifier.sym = getSymbol(symbolTable, name, s->lineno);
        			}
        		}
        	}
            //check expression
            symEXP(s->val.assign_stmt.lhs, symbolTable);
            //check the identifier
            symEXP(s->val.assign_stmt.rhs, symbolTable);

            break;
        case k_StatementKindEmpty:
            break;
        case k_StatementKindFor:
        	initscope = scopeSymbolTable(symbolTable);
        	printScopeIn(tabNum);
            symSTMTS(s->val.for_stmt.first, initscope, tabNum+1);
            symEXP(s->val.for_stmt.second, initscope);
            symSTMTS(s->val.for_stmt.third, initscope, tabNum+1);
            //don't need to open new scope here because it is a block stmt in tree
            symSTMTS(s->val.for_stmt.body, initscope, tabNum+1);
            printScopeOut(tabNum);
            break;
        case k_StatementKindPrintln:
            symEXP(s->val.println_stmt.expr_list, symbolTable);
            break;
        case k_StatementKindPrint:
            symEXP(s->val.print_stmt.expr_list, symbolTable);
            break;
        case k_StatementKindVarDecl:
            symVAR(s->val.var_decl, symbolTable, tabNum);
            break;
        case k_StatementKindTypeDecl:
            symTYPE(s->val.type_decl, symbolTable, tabNum);
            break;
        case k_StatementKindReturn:
        	if(s->val.return_stmt.expr != NULL){
        		symEXP(s->val.return_stmt.expr, symbolTable);
        		/*
        		if(strcmp(s->val.return_stmt.func_ref->name->val.identifier.name, "_") != 0)
            		s->val.return_stmt.func_sym = getSymbol(symbolTable, s->val.return_stmt.func_ref->name->val.identifier.name, s->lineno);
        		*/
        	}
            break;
        case k_StatementKindSwitch:
        	initscope = scopeSymbolTable(symbolTable);
        	printScopeIn(tabNum);
            symSTMTS(s->val.switch_stmt.opt_cond, initscope, tabNum+1);
            symEXP(s->val.switch_stmt.cond, initscope);
            symCASE(s->val.switch_stmt.caseClauses, initscope, tabNum+1);
            printScopeOut(tabNum);
            break;
        case k_StatementKindExp:
            symEXP(s->val.exp_stmt.expr, symbolTable);
            break;
        case k_StatementKindShortDecl:
            lhs = s->val.short_decl.id_list;
            rhs = s->val.short_decl.expr_list;
            symEXP(rhs, symbolTable);
       		symShortDecl(lhs, symbolTable, tabNum);
            break;
        case k_StatementKindInc:
            symEXP(s->val.inc_stmt.expr, symbolTable);
            break;
        case k_StatementKindDec:
            symEXP(s->val.dec_stmt.expr, symbolTable);
            break;
        case k_StatementKindContinue:
        case k_StatementKindBreak:
            break;
    }
}

//if already declared, do nothing here
//if not, and it is not blank identifier add a symbol that has null type
//ensure at least one var is not declared in current scope (check current scope only)
//iterate through all lhs
void symShortDecl(EXP* lhs, SymbolTable *symbolTable, int tabNum){
	int count = 0;
    Node *declared = NULL; //keep track of the newly declared symbol in short decl
    Node *temp;
    int lineno = lhs->lineno;
    SYMBOL *newSym;

    while (lhs != NULL) {
    	//skip blank identifier
		if (strcmp("_", lhs->val.identifier.name) != 0) {
            //find in local table
    		if (localSymbolTableCheck(symbolTable, lhs->val.identifier.name) == NULL) {
                count++; //increment the count
                //not declared before
                newSym = makeSymbol(lhs->val.identifier.name, varSymKind);
                newSym->val.varType = makeSymbol("", inferSymKind);

                Node *newNode = malloc(sizeof(Node));
                newNode->val = newSym;
                newNode->next = NULL;

                //check if the newly declared is already on the lhs
                temp = declared;
                while(temp){
                	if(strcmp(temp->val->name, newSym->name) == 0){
                		fprintf(stderr, "Error: (line %d) repeated identifier %s on lhs of short declaration.\n", lhs->lineno, newSym->name);
    					exit(1);
                	}
                	temp = temp->next;
                }	
                //add to AST
                lhs->val.identifier.sym = newSym;
                
                //add to declared list
                if(declared == NULL){
                	declared = newNode;
                }else{
					newNode->next = declared;
					declared = newNode;
                }
                if(print){
                	prettyTabs(tabNum);
                	printf("%s [variable] = <infer>\n", lhs->val.identifier.name);
                	fflush(stdout);
                }
            }
            else {
                //declared already
                SYMBOL *sym = getSymbol(symbolTable, lhs->val.identifier.name, lhs->lineno);
                //check if variable type
                if(sym->kind != varSymKind){
                	fprintf(stderr, "Error: (line %d) %s is not a variable type.\n", lhs->lineno, sym->name);
    				exit(1);
                }
            	lhs->val.identifier.sym = sym; 
            	sym->declaredBefore = true;
            }
        }
        else{
        	lhs->val.identifier.sym = makeSymbol("_", nullSymKind);
        	lhs->val.identifier.sym->val.varType = makeSymbol("_", inferSymKind);
        }
 
        lhs = lhs->next;
    }

    if (count == 0) {
        // short decl needs to have at least one undeclared var
    	fprintf(stderr, "Error: (line %d) short declaration contains no new variables.\n", lineno);
    	exit(1);
    }

    //add all the newly declared variable to symbol table
    while(declared){
    	putVar(declared->val, symbolTable, declared->val->lineno);
    	declared = declared -> next;
    }
}

void symCASE(CASE *c, SymbolTable *symbolTable, int tabNum) {
    if (c == NULL) return;
    
    if (c->next != NULL) {
        symCASE(c->next, symbolTable, tabNum);
    }
    
    SymbolTable *table = scopeSymbolTable(symbolTable);
    printScopeIn(tabNum);
    switch (c->kind) {
        case k_SwitchKindCase:
            symEXP(c->val.case_exp.expr_list, table);
            symSTMTS(c->val.case_exp.body, table, tabNum+1);
            break;
        case k_SwitchKindDefault:
            symSTMTS(c->val.default_exp.body, table, tabNum+1);
            break;
    }
    printScopeOut(tabNum);
}

SYMBOL* symEXP(EXP *e, SymbolTable *symbolTable){
    if (e == NULL) return NULL;
    
    if (e->next != NULL) {
        symEXP(e->next, symbolTable);
    }
    
    switch(e->kind){
        case k_ExpressionKindBinary:
            symEXP(e->val.binary.lhs, symbolTable);
            symEXP(e->val.binary.rhs, symbolTable);
            break;
        case k_ExpressionKindUnary:
            symEXP(e->val.unary.expr, symbolTable);
            break;
         //builtin
        case k_ExpressionKindAppend:
            symEXP(e->val.append.id, symbolTable);
            symEXP(e->val.append.added, symbolTable);
            break;
        case k_ExpressionKindLen:
            symEXP(e->val.len.expr, symbolTable);
            break;
        case k_ExpressionKindCap:
            symEXP(e->val.cap.expr, symbolTable);
            break;
        case k_ExpressionKindParen:
            symEXP(e->val.paren.expr, symbolTable);
            break;
        case k_ExpressionKindFuncCall:
            symEXP(e->val.func_call.name, symbolTable);
            symEXP(e->val.func_call.args, symbolTable);
            break;
        case k_ExpressionKindFieldAccess:
            //we need to access to struct fields and add to AST
            // current approach only access current scope not the strcut's scope
            //if x.y.z , then x and y are both structs symbols, need to add them to AST
        	symEXP(e->val.access_field.id, symbolTable);
            break;
        case k_ExpressionKindArrayIndex:
            symEXP(e->val.array_index.array, symbolTable);
            symEXP(e->val.array_index.index, symbolTable);
            break;
        case k_ExpressionKindIdentifier:
        	if(strcmp(e->val.identifier.name, "_") != 0){
        		e->val.identifier.sym = getSymbol(symbolTable, e->val.identifier.name, e->lineno);
            	break;
        	}
        	else{
        		e->val.identifier.sym = makeSymbol("_", nullSymKind);
        		e->val.identifier.sym->val.varType = makeSymbol("_", inferSymKind);
        	}
			
    }
}