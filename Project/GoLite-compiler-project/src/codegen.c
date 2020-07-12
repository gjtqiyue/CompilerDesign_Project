#include "codegen.h"
/*
* Variable section
*/
int insideLoop = 0;
int caseFound = 0;
int notPrintBreak = 0;
int tmp_count;
int var_count;
FUNCDECL *mainFunc = NULL;
int initIndex = 0;
FILE* file;
/*
* Code section
*/
void printTabs(FILE *f, int n)
{
    for(int i = 0; i < n; i++)
    {
        fprintf(f, "\t");
    }
}

void codePROGRAM(PROGRAM *program, char* file_name, FILE *f){
    codePrintSliceClass(f); //add back later

    //assign file
    file = f;
    
    //1st traversal to code the struct classes
    PROGRAM *p = program;
    //generate classes, if two structs has the same fields and same name, same order, don't need to create another class
    codeBuildStructClass(p, f, 0);

    fprintf(f, "public class");
    fprintf(f, " %s ", file_name);
    fprintf(f, "{\n");
    codePrintEscapeCharMethod(f);
    codeDeclaration(program->decl, 1, f);

    //true false
    //printTabs(f, 1);
    //fprintf(f, "public static boolean __golitec_bool_true = true;\n");
    //printTabs(f, 1);
    //fprintf(f, "public static boolean __golitec_bool_false = false;\n");

    //main function
    codeMain(1, f);
    fprintf(f,"}\n");
}

void codeMain(int tabs, FILE *f) {
    printTabs(f,tabs);
    fprintf(f,"public static void main(String[] args) throws CloneNotSupportedException");
    fprintf(f,"{\n");

    //try catch index out of bound exception
    printTabs(f, tabs+1);
    fprintf(f,"try{\n");
    //call init function
    int index = 0;
    while (index != initIndex) {
        printTabs(f, tabs+2);
        fprintf(f,"init_%d();\n", index);
        index++;
    }
    //main body
    if (mainFunc != NULL)
        codeSTMTS(f, mainFunc->body, tabs+2);
    else
        fprintf(f,"No main function found\n");

    printTabs(f, tabs+1);
    fprintf(f,"} catch (Exception e) {\n");
    printTabs(f, tabs+2);
    fprintf(f,"e.printStackTrace();\n");
    printTabs(f, tabs+2);
    fprintf(f,"System.exit(1);\n");
    printTabs(f, tabs+1);
    fprintf(f,"}\n");
    printTabs(f, tabs);
    fprintf(f,"}\n");
}

void codeDeclaration(DECLARATION *decl, int tabs, FILE *f){
    if (decl == NULL) return;
    else codeDeclaration(decl->next, tabs, f);

    switch (decl->kind) {
        case k_DeclarationKindFunction:
            codeFunc(decl->val.func_decl, tabs, f);
            break;
        case k_DeclarationKindVar:
            codeVar(decl->val.var_decl, tabs, f);
            break;
        case k_DeclarationKindType:
            //symTYPE(decl->val.type_decl, symbolTable, tabs);
            break;
    }
}

void codeVar(VARDECL *var, int tabs, FILE *f){
    if(var == NULL) return;
    else codeVar(var->next, tabs, f);

    switch(var->kind){
        case k_VarDeclKindType:
            codeVar_decl(var->val.type_only.id_list, NULL, tabs, f);
            break;

        case k_VarDeclKindExpr:
            codeVar_expr(var->val.exp_only.id_list, var->val.exp_only.expr_list, tabs, f);
            break;

        case k_VarDeclKindBoth:
            codeVar_decl(var->val.exp_type.id_list, var->val.exp_type.expr_list, tabs, f);
            break;
    }
    
}

//int temp = 5;
//int a = temp;
void printVariableDecl(bool top_level, char *type, char *var_name, char *value, int tabs, FILE *f){
    printTabs(f, tabs);
    if(top_level) fprintf(f, "public static %s %s = %s;\n", type, var_name, value);
    else fprintf(f, "%s %s = %s;\n", type, var_name, value);
}

void printVariableDecl2(bool top_level, char *type, char *var_name, int tabs, FILE *f){
    printTabs(f, tabs);
    if(top_level) fprintf(f, "public static %s %s = ", type, var_name);
    else fprintf(f, "%s %s = ", type, var_name);
}

char *renameVariable(SYMBOL *sym){
    char num[10];
    char *renamed = malloc(100*sizeof(char));
    //sprintf(num, "%d", sym->tableDepth);
    sprintf(num, "%d", var_count);
    strcpy(renamed, "__golitec__");
    strcat(renamed, sym->name);
    strcat(renamed, "__");
    strcat(renamed, num);
    var_count++;
    sym->rename = renamed;
    return renamed;
}

char *tmpVariable(SYMBOL *sym){
    char *temp = malloc(100*sizeof(char));
    //strcat(temp, sym->rename);
    strcat(temp, "__golitec__");
    strcat(temp, "__");
    strcat(temp, "tmp");
    char num[10];
    sprintf(num, "%d", tmp_count);
    strcat(temp, num);
    tmp_count++;
    return temp;
}

char *tmpVariable2(){
    char *temp = malloc(100*sizeof(char));
    strcat(temp, "__golitec__tmp");
    char num[10];
    sprintf(num, "%d", tmp_count);
    strcat(temp, num);
    tmp_count++;
    return temp;
}

char *blankVariable(){
    char *temp = malloc(100*sizeof(char));
    strcpy(temp, "__golitec__");
    strcat(temp, "blank");
    char num[10];
    sprintf(num, "%d", tmp_count);
    strcat(temp, num);
    tmp_count++;
    return temp;
}


char *varTypeName(SYMBOL *varSym){
    char *value;
    char *type;
    char *dimSize; //array
    char *class;   //array

    switch(varSym->kind){
        case typeSymKind:
            type = codeBaseType(varSym);
            break;

        case sliceSymKind:
            class = getSliceTypeNameHelper(varSym->val.parentType);
            type = getListTypeName(class);
            //set dimension
            dimSize = getSliceDimWithSize(varSym->val.parentType);
            varSym->dimension = getArrayDimNumber(dimSize);
            break;

        case arraySymKind:
            class = getArrayBaseType(varSym->val.parentType);
            //printf("class is %s\n", class);
            
            dimSize = getArrayDimWithSize(varSym->val.parentType);
            //printf("dim is %d\n", getArrayDimNumber(dimSize));
            varSym->dimension = getArrayDimNumber(dimSize); //set dimension
            type = getListTypeName(class);
            //printf("type is %s\n", type);
            char* sizeArray = getArraySize(dimSize);
            //printf("size is %s\n", sizeArray);
            break;

        case structSymKind:
            //type = codeStructType(sym);
            type = varSym->structClassName;
            break;
        case nullSymKind:
            printf("nullSymKind");
            exit(1);
    }
    return type;
}

char *structInitValue(char *className){
    char *init = malloc((10+strlen(className))*sizeof(char));
    strcat(init, "new ");
    strcat(init, className);
    strcat(init, "()");
    return init;
}

//for all types except array
char *varTypeValue(SYMBOL *varSym, char *type){
    char *value;
    char *sizeArray;
    char *dimSize;

    switch(varSym->kind){
        case typeSymKind:
            value = initValue(type);
            break;

        case arraySymKind:
            // printf("value type is %s\n", type);
            
            // printf("size is %s\n", sizeArray);
            value = getArrayTypeInit(varSym, type);
            //printf("value is %s\n", value);
            break;

        case sliceSymKind:
            value = getSliceTypeInit(varSym, type);
            //printf("value is %s\n", value);
            break;

        case structSymKind:
            value = structInitValue(type);
            break;

        case nullSymKind:
            printf("nullSymKind");
            exit(1);
    }
    return value;
}



//if expr is null, then we don't code default value
void codeVar_decl(EXP *id, EXP *expr, int tabs, FILE *f){
    char *header;
    bool isTopLevel;
    char *value;
    char *type;
    char *dimSize; //array
    char *class;   //array
    char *var_name;
    char *temp_name;
    SYMBOL *sym = id->val.identifier.sym;
    SYMBOL *varSym;

    if(strcmp(id->val.identifier.name, "_") == 0){
        temp_name = blankVariable();
    }
    else{
        var_name = renameVariable(sym);
        temp_name = tmpVariable(sym);
    }

    if(sym->tableDepth == 1) isTopLevel = true; //top-level decl
    else isTopLevel = false;

    if(sym->val.varType == NULL){
        varSym = sym;
        //printf("varType is null"); //blank id
    }else{
        varSym = sym->val.varType;
    }

    type = varTypeName(varSym);
    if(expr == NULL) value = varTypeValue(varSym, type);
    
    if(expr == NULL) {
        printVariableDecl(isTopLevel, type, temp_name, value, tabs, f);
        if(strcmp(id->val.identifier.name, "_") != 0){
            printVariableDecl(isTopLevel, type, var_name, temp_name, tabs, f);
        }
    }
    else{
        printVariableDecl2(isTopLevel, type, temp_name, tabs, f);
        codeEXP(f, expr);
        fprintf(f, ";\n");
        if(strcmp(id->val.identifier.name, "_") != 0){
            printVariableDecl(isTopLevel, type, var_name, temp_name, tabs, f);
        }
        expr = expr->next;
    }

    
    id = id->next;
    while(id){
        sym = id->val.identifier.sym;
        if(strcmp(id->val.identifier.name, "_") == 0){
            temp_name = blankVariable();
        }
        else{
            var_name = renameVariable(sym);
            temp_name = tmpVariable(sym);
        }
       
        if(expr == NULL) {
            printVariableDecl(isTopLevel, type, temp_name, value, tabs, f);
            if(strcmp(id->val.identifier.name, "_") != 0){
                printVariableDecl(isTopLevel, type, var_name, temp_name, tabs, f);
            }
        }
        else{
            printVariableDecl2(isTopLevel, type, temp_name, tabs, f);
            codeEXP(f, expr);
            fprintf(f, ";\n");
            if(strcmp(id->val.identifier.name, "_") != 0){
                printVariableDecl(isTopLevel, type, var_name, temp_name, tabs, f);
            }
            expr = expr->next;
        }
        id = id->next;
    }
}

void codeVar_expr(EXP *id, EXP *expr, int tabs, FILE *f){
    char *header;
    bool isTopLevel;
    char *type;
    char *dimSize; //array
    char *class;   //array
    char *var_name;
    char *temp_name;
    SYMBOL *sym;
    SYMBOL *varSym;

    while(id){
        sym = id->val.identifier.sym;
        if(strcmp(id->val.identifier.name, "_") == 0){
            temp_name = blankVariable();
        }
        else{
            var_name = renameVariable(sym);
            temp_name = tmpVariable(sym);
        }

        if(sym->tableDepth == 1) isTopLevel = true; //top-level decl
        else isTopLevel = false;

        if(sym->val.varType == NULL){
            varSym = sym;
            //printf("varType is null"); //blank id
        }else{
            varSym = sym->val.varType;
        }
        
        type = varTypeName(varSym);
        printVariableDecl2(isTopLevel, type, temp_name, tabs, f);
        codeEXP(f, expr);
        fprintf(f, ";\n");
        if(strcmp(id->val.identifier.name, "_") != 0){
            printVariableDecl(isTopLevel, type, var_name, temp_name, tabs, f);
        }

        expr = expr->next;
        id = id->next;
    }
}

//symbol passed here is sym->varType->parentType
char *getSliceTypeName(SYMBOL *sym){
    char *class = malloc(100*sizeof(char));
    strcat(class, "__golitec_slice<");
    strcat(class, getSliceTypeNameHelper(sym->val.parentType));
    strcat(class, ">");
    return class;
}

char *getSliceDimWithSize(SYMBOL *sym) {
    char* str = malloc(100*sizeof(char));
    SYMBOL *type = sym;

    while(type && (type->kind == arraySymKind || type->kind == sliceSymKind)){
        //intermetdiate nodes
        if(strcmp(type->name, "")== 0){
            strcat(str, "[");
            char buf[10];
            sprintf(buf, "%d", type->arraySize);
            strcat(str, buf);
            strcat(str, "]");
            type = type->val.parentType;
            //fprintf(f, "111 %s\n", str);
        }
        //typedef nodes
        else {
            type = type->val.parentType;
        }
    }
    return str;
}

//pass in the return of getSliceTypeName
char *getSliceTypeInit(SYMBOL* varSym, char* type){
    char* dimSize = getSliceDimWithSize(varSym->val.parentType);
    int dim = getSliceDimNumber(dimSize);
    //printf("dim is %d\n", dim);
    varSym->dimension = dim;
    //printf("sym name is %s\n", varSym->name);
    char* sizeArray = getSliceSize(dimSize);
    char* class = getSliceTypeNameHelper(varSym->val.parentType);
    //printf("%s\n", class);

    type = getSliceTypeName(varSym->val.parentType);

    char *str = malloc(100*sizeof(char));
    char buffer[100];
    sprintf(buffer, "new %s (%d, %d, %s, %s.class, null);", type, dim, dim, sizeArray, class);
    strcat(str, buffer);
    return str;
}

//primitive change int -> Integer
char *getSliceTypeNameHelper(SYMBOL *sym){
    char *class;
    char *dimSize;

    switch(sym->kind){
        case typeSymKind:
            return convertPrimitive(codeBaseType(sym));
        case arraySymKind:
            //TODO: double check
            return getSliceTypeNameHelper(sym->val.parentType);
            break;
        case sliceSymKind:
            return getSliceTypeNameHelper(sym->val.parentType);
            break;
        case structSymKind:
            //TODO return name
            return sym->structClassName;
            break;
    }
}

char *getSliceDim(char *dim) {
    char* str = malloc(100*sizeof(char));
    int i = 0;
    int j = 0;
    
    while(dim[i] != '\0'){
        if(dim[i] == ']' || dim[i] == '[') {
            str[j] = dim[i];
            j++;
        }
        i++;
    }
    return str;
}

char* getSliceSize(char *dim) {
    char* str = malloc(100*sizeof(char));
    int i = 0;
    int j = 0;
    while(dim[i] != '\0'){
        if(dim[i] == '[') {
            str[j] = dim[i+1];
            j++;
        }
        i++;
    }
    str[i] = '\0';
    i=0;
    char* str2 = malloc(100*sizeof(char));
    strcat(str2, "new int[]{");
    for (i=0; i<j-1; i++) {
        char buffer[4];
        if (str[i] == ']') {
            sprintf(buffer, "0");
        }
        else {
            sprintf(buffer, "%d", str[i] - '0');
        }
        strcat(str2, buffer);
        strcat(str2, ", ");
    }
    char buffer[4];
    if (str[i] == ']') {
        sprintf(buffer, "0");
    }
    else {
        sprintf(buffer, "%d", str[i] - '0');
    }
    strcat(str2, buffer);
    strcat(str2, "}");
    return str2;
}

int getSliceDimNumber(char *dim) {
    char* str = malloc(100*sizeof(char));
    int i = 0;
    int j = 0;
    while(dim[i] != '\0'){
        if(dim[i] == '[') {
            str[j] = dim[i+1];
            j++;
        }
        i++;
    }
    return j;
}


char *convertPrimitive(char *typeString){
    if(strcmp(typeString, "int") == 0 || strcmp(typeString, "rune") == 0) return "Integer";
    if(strcmp(typeString, "float64") == 0 || strcmp(typeString, "double") == 0) return "Double";
    if(strcmp(typeString, "bool") == 0 || strcmp(typeString, "boolean") == 0) return "Boolean";
    if(strcmp(typeString, "string") == 0 || strcmp(typeString, "rawstring") == 0 || strcmp(typeString, "String") == 0) return "String";
    else {
       fprintf(stderr, "Error: %s is not one of the primitive types\n", typeString);
       exit(1);
   }
}


//string need to be init to "", slice need to init
//Arrays.fill(array, value);
char *fillDefaultArray(SYMBOL *sym, char *varName, char *baseType, char *dim, int tabs, FILE *f){
    char *fillValue;
    while(sym){
        if(sym->kind == arraySymKind){
            sym = sym->val.parentType;
        }else{
            break;
        }
    }

    if(strcmp(baseType, "String") == 0){
        fillValue = "\"\"";
        printForLoop(baseType, dim, varName, fillValue, tabs, f);
    }
    //slice
    //Arrays.fill(var, new __Slice<..>(); )
    else if (strcmp(baseType, "Object") == 0){
        //char *typename = getSliceTypeName(sym);
       // printf("typename: %s\n", typename);
        //fillValue = getSliceTypeInit(typename);
        fillValue = getSliceTypeInit(sym, getSliceTypeName(sym));
        printForLoop(baseType, dim, varName, fillValue, tabs, f);
    }
}
/*
 String[][][] s = new String[5][5][5];
            type  dim  newIndex: oldIndex
        for(String[][] row : s){
            for(String[] row1: row)
            Arrays.fill(row1, "");
        }

type  dim
String[] = new String[5];
Arrays.fill(row1, "");
*/
void *printForLoop(char *type, char *dim, char *oldIndex, char *fillValue, int tabs, FILE *f){
    if(strcmp(dim, "[]") == 0){
        printTabs(f, tabs);
        fprintf(f, "Arrays.fill(%s, %s);\n", oldIndex, fillValue);
        //fprintf(f, "exit printForLoop\n");
        //fflush(stdout);
    }
    else{
        char *newIndex = blankVariable();
        char *newDim = malloc(strlen(dim)*sizeof(char)); //strips off a dimension
        snprintf(newDim, strlen(dim)-1, "%s", dim);
        
        printTabs(f, tabs);
        fprintf(f, "for (%s%s %s: %s){\n", type, newDim, newIndex, oldIndex);
        printForLoop(type, newDim, newIndex, fillValue, tabs+1, f);
        printTabs(f, tabs);
        fprintf(f, "}\n");
    }
}



char *getArrayTypeName(char *type){
    // char *dim = getArrayDim(dimSize); //[][]
    // char* str = malloc(100*sizeof(char));
    // strcat(str, class);
    // strcat(str, dim);
    // return str; //String[][]
    char *class = malloc(100*sizeof(char));
    strcat(class, "__golitec_array<");
    strcat(class, type);
    strcat(class, ">");
    return class;
}

char *getListTypeName(char *type) {
    char *class = malloc(100*sizeof(char));
    strcat(class, "__golitec_list<");
    strcat(class, type);
    strcat(class, ">");
    return class;
}

char *getArrayTypeInit(SYMBOL* varSym, char *type){
    char* dimSize = getArrayDimWithSize(varSym->val.parentType);
    int dim = getArrayDimNumber(dimSize);
    varSym->dimension = dim;
    char* sizeArray = getArraySize(dimSize);
    char* class = getArrayBaseType(varSym->val.parentType);

    type = getArrayTypeName(class);

    char *str = malloc(100*sizeof(char));
    char buffer[100];
    sprintf(buffer, "new %s (%d, %d, %s, %s.class, null);", type, dim, dim, sizeArray, class);
    strcat(str, buffer);
    return str;
}

char *getArrayDimWithSize(SYMBOL *sym){
    char* str = malloc(100*sizeof(char));
    SYMBOL *type = sym;

    while(type && (type->kind == arraySymKind || type->kind == sliceSymKind)){
        //intermetdiate nodes
        if(strcmp(type->name, "")== 0){
            strcat(str, "[");
            char buf[10];
            sprintf(buf, "%d", type->arraySize);
            strcat(str, buf);
            strcat(str, "]");
            type = type->val.parentType;
            //fprintf(f, "111 %s\n", str);
        }
        //typedef nodes
        else {
            type = type->val.parentType;
        }
    }
    return str;
}

char *getArrayDim(char *dim){
    char* str = malloc(100*sizeof(char));
    int i = 0;
    int j = 0;
    
    while(dim[i] != '\0'){
        if(dim[i] == ']' || dim[i] == '[') {
            str[j] = dim[i];
            j++;
        }
        i++;
    }
    return str;
}

int getArrayDimNumber(char *dim) {
    char* str = malloc(100*sizeof(char));
    int i = 0;
    int j = 0;
    while(dim[i] != '\0'){
        if(dim[i] == '[') {
            str[j] = dim[i+1];
            j++;
        }
        i++;
    }
    return j;
}

char* getArraySize(char *dim) {
    //printf("dim %s\n", dim);
    char* str = malloc(100*sizeof(char));
    int i = 0;
    int j = 0;
    
    while(dim[i] != '\0'){
        if(dim[i] == '[') {
            i++;
            while(dim[i] != ']'){
                str[j] = dim[i];
                j++;
                i++;
            }
            str[j] = ',';
            j++;
        }
        i++;
    }
    str[j-1] = '\0';
    
    i=0;
    char* str2 = malloc(100*sizeof(char));
    strcat(str2, "new int[]{");
    strcat(str2, str);
    strcat(str2, "}");
    //printf("str2: %s", str2);
    return str2;
}

bool checkIfBaseType(char *name){
    if(strcmp(name, "int") == 0 || strcmp(name, "float64") == 0 || strcmp(name, "rune") == 0 ||
                strcmp(name, "bool") == 0 || strcmp(name, "string") == 0 || strcmp(name, "rawstring") == 0){
        return true;
    }
    return false;
}

char *getArrayBaseType(SYMBOL *sym){
    SYMBOL *type = sym;
    char *name = type->name;

    while(type){
        if(type->kind == typeSymKind){
            if(checkIfBaseType(name)) return convertPrimitive(convertBaseType(name));
        }
        else if(type->kind == structSymKind){
            return sym->structClassName;
        }
        //else if type array, keep recursing
        type = type->val.parentType;
        name = type->name;
    }
}


//int, double, char, boolean, String
char *codeBaseType(SYMBOL *sym){
    char *typeString;
    SYMBOL *type = sym;

    if(sym->val.parentType != NULL){
        sym = sym->val.parentType;
        while(sym->val.parentType){
            sym = sym->val.parentType;
        }
    }
    if (sym->kind == typeSymKind) {
        return convertBaseType(sym->name);
    }
    else {
        return sym->structClassName;
    }
    
    /*
    while(type){
        if(checkIfBaseType(type->name)){
            //printf("base type: %s\n", type->name);
            typeString = type->name;
            break;
        }
        else{
            //printf("base type: %s\n", type->name);
            type = type->val.parentType;
        }
    }
    return convertBaseType(typeString);
    */
}

char *convertBaseType(char *typeString){
    if(strcmp(typeString, "int") == 0) return "int";
    if(strcmp(typeString, "float64") == 0) return "double";
    if(strcmp(typeString, "rune") == 0) return "int";
    if(strcmp(typeString, "bool") == 0) return "boolean";
    if(strcmp(typeString, "string") == 0) return "String";
    if(strcmp(typeString, "rawstring") == 0) return "String";
    else {
        fprintf(stderr, "Error: %s is not one of the base types\n", typeString);
        exit(1);
    }
}

char *initValue(char *typeString){
    if(strcmp(typeString, "int") == 0) return "0";
    if(strcmp(typeString, "double") == 0) return "0.0";
    if(strcmp(typeString, "char") == 0) return "0";
    if(strcmp(typeString, "boolean") == 0) return "false";
    if(strcmp(typeString, "String") == 0) return "\"\"";
}

void codeBuildStructClass(PROGRAM *p, FILE *f, int tabs){
    CLASS *classes = p->class_list;
    while(classes){
        fprintf(f, "class ");
        fprintf(f, "%s",  classes->name);
        fprintf(f, " extends Object implements Cloneable {\n");
        printClassField(classes->structClass->val.structFields, f, tabs+1);
        fprintf(f, "\n");
        printEqualsMethod(classes->name, classes->structClass->val.structFields, f, tabs+1);
        fprintf(f, "\n");
         printCloneMethod(classes->name, classes->structClass->val.structFields, f, tabs+1);
        fprintf(f, "\n");
        fprintf(f, "}\n\n");

        classes = classes->next;
    }
}

void printClassField(SYMBOL *sym, FILE *f, int tabs){
    if(sym == NULL) return;
    printClassField(sym->next, f, tabs);

    char *value;
    char *type;
    char *var_name = malloc((strlen(sym->name)+12)*sizeof(char));
    SYMBOL *varSym;

    if(strcmp(sym->name, "_") != 0){
        varSym = sym->val.varType;
        type = varTypeName(varSym);
        value = varTypeValue(varSym, type);
        strcat(var_name, "__golitec_");
        strcat(var_name, sym->name);
        sym->type = type;
        sym->rename = var_name;
        printVariableDecl(false, type, var_name, value, tabs, f);
        fflush(stdout);
    }
}

void printEqualsHelper(SYMBOL *sym, FILE *f){
    if(sym == NULL) return;

    SYMBOL *s = sym;
    int count = 0;
    while (s != NULL) {
        if(strcmp(s->name, "_") != 0){
            count++;
        }
        s = s->next;
    }
    if(count == 0) {
        fprintf(f, "true");
        return;
    }
    
    s = sym;
    while (count > 0) {
        if (strcmp(s->name, "_") != 0) {
            if(checkPrimitive(s->type)){
                fprintf(f, "this.%s == o.%s", s->rename, s->rename);
            }
            else{
                fprintf(f, "this.%s.equals(o.%s)", s->rename, s->rename);
            }
            if (count > 1) {
                fprintf(f, " && ");
            }
            count--;
        }
        s = s->next;
    }
}

void printEqualsMethod(char *className, SYMBOL *sym, FILE *f, int tabs){
    printTabs(f,tabs);
    fprintf(f, "@Override\n");
    printTabs(f, tabs);
    fprintf(f, "public boolean equals(Object s) {\n");
    printTabs(f, tabs+1);
    fprintf(f, "%s o = (%s)s;\n", className, className);
    printTabs(f, tabs+1);
    fprintf(f, "return ");
    if(sym == NULL) fprintf(f, "true");
    else printEqualsHelper(sym, f);
    fprintf(f, ";\n");
    printTabs(f, tabs);
    fprintf(f, "}\n");
}

void printCloneMethod(char *className, SYMBOL *sym, FILE *f, int tabs){
     printTabs(f,tabs);
     fprintf(f, "@Override\n");
     printTabs(f, tabs);
     fprintf(f, "public %s clone() throws CloneNotSupportedException {\n", className);
     printTabs(f, tabs+1);
     fprintf(f, "%s o = new %s();\n", className, className);
     printCloneHelper(sym, f, tabs+1);
     printTabs(f, tabs+1);
     fprintf(f, "return o;\n");
     printTabs(f, tabs);
     fprintf(f, "}\n");
 }

 void printCloneHelper(SYMBOL *sym, FILE *f, int tabs){
     if(sym == NULL) return;
     else if (sym->next != NULL){
         printCloneHelper(sym->next, f, tabs);
     }

     if(strcmp(sym->name, "_") != 0){
         printTabs(f, tabs);
        if(checkPrimitive(sym->type)){
             fprintf(f, "o.%s = this.%s;\n", sym->rename, sym->rename);
         }
         else{
             fprintf(f, "o.%s = this.%s.clone();\n", sym->rename, sym->rename);
         }
     }

 }


void printParam(PARAM* param, FILE *f){
    if(param == NULL) return;
    else if (param->next != NULL){
        printParam(param->next, f);
        fprintf(f, ", ");
        fflush(stdout);
    }
    EXP* exp = param->id_list;
    char* typeName = varTypeName(exp->val.identifier.sym->val.varType);

    while(exp){
        fprintf(f, "%s ", typeName);
        fflush(stdout);
        if(strcmp(exp->val.identifier.name, "_") == 0){
            fprintf(f, "%s", blankVariable());
            fflush(stdout);
        }else{
            fprintf(f, "%s", renameVariable(exp->val.identifier.sym));
            fflush(stdout);
        }
        exp = exp->next;
        if(exp) fprintf(f, ", ");
        fflush(stdout);
    }
}

void codeFunc(FUNCDECL *func, int tabs, FILE *f) {
    if (strcmp("_", func->name->val.identifier.name) == 0){
        return;
    }

    if (strcmp("main", func->name->val.identifier.name) == 0) {
        mainFunc = func;
    }
    else if (strcmp("init", func->name->val.identifier.name) == 0) {
        printTabs(f, tabs);
        fprintf(f, "public static void init_%d() throws CloneNotSupportedException ", initIndex);
        initIndex++;
        fprintf(f, "{\n");
        codeSTMTS(f, func->body, tabs+1);
        printTabs(f, tabs);
        fprintf(f, "}\n");
    }
    else {
        printTabs(f, tabs);
        fprintf(f, "public static ");
        if(func->sym->val.func.returnSymRef == NULL){
            fprintf(f, "void ");
        }else{
            fprintf(f, "%s ", varTypeName(func->sym->val.func.returnSymRef));
        }
        fprintf(f, "%s", renameVariable(func->sym));
        fprintf(f, "(");
        fflush(stdout);
        //print argument
        PARAM* param = func->params;
        printParam(param, f);
        fprintf(f, ") throws CloneNotSupportedException");
        fprintf(f, "{\n");
        //foreach argument, redclare it
        param = func->params;
        while (param != NULL) {
            EXP* exp = param->id_list;
            while (exp != NULL) {
                SYMBOL *sym = exp->val.identifier.sym;
                SYMBOL *varType;
                //printf("reached here, name: %s\n", exp->val.identifier.name);
                if(strcmp(exp->val.identifier.name, "_") == 0){
                    exp = exp->next;
                    continue;
                }
                varType = sym->val.varType;
                if(varType->kind == arraySymKind || varType->kind == structSymKind || varType->kind == sliceSymKind){
                    sym->isFuncParam = true;
                    char *str = malloc((strlen(sym->rename)+10)*sizeof(char));
                    strcat(str, sym->rename);
                    strcat(str, "__param");
                    sym->funcParamName = str;
                    char *str2 = malloc((strlen(sym->rename)+10)*sizeof(char));
                    strcat(str2, sym->rename);
                    strcat(str2, ".clone()");
                    printVariableDecl(false, varTypeName(varType), str, str2, tabs, f);
                }
                exp = exp->next;
            }
            param = param->next;
        }
        codeSTMTS(f, func->body, tabs+1);

        //now clear up the funcParam flag for all the params
        param = func->params;
        while(param){
            EXP* exp = param->id_list;
            while(exp){
                exp->val.identifier.sym->isFuncParam = false;
                exp->val.identifier.sym->funcParamName = NULL;
                exp = exp->next;
            }
            param = param->next;
        }
        printTabs(f, tabs);
        fprintf(f, "}\n");
    }
    
}

void codeSTMTS(FILE *f, STMT *s, int tabs) {
    if (s == NULL){
        return;
    }

    if (s->next != NULL){
        codeSTMTS(f, s->next, tabs);
    }
    
    EXP *lhs, *rhs, *returnType;
    char* str;
    switch (s->kind)
    {
        case k_StatementKindEmpty:
            break;
        case k_StatementKindContinue:
            printTabs(f, tabs);
            fprintf(f, "continue;\n");
            break;
        case k_StatementKindBreak:
            
            if (notPrintBreak == 0) {
                if (insideLoop == 1) {
                    printTabs(f, tabs);
                    fprintf(f, "if (true) {");
                    fprintf(f, "throw new Exception();\n");
                    fprintf(f, "}");
                }
                else {
                printTabs(f, tabs);
                fprintf(f, "break;\n");
                }
            }
            
            break;
        case k_StatementKindBlock:
            // printTabs(f, tabs);
            codeSTMTS(f, s->val.block_stmt, tabs);
            break;
        case k_StatementKindVarDecl:
            //fprintf(f, "reach inner scope var decl\n");
            codeVar(s->val.var_decl, tabs, f);
            break;
        case k_StatementKindTypeDecl:
            //do nothing because everything is handled in symbol
            break;
        case k_StatementKindReturn:
            //print type
            printTabs(f, tabs);
            EXP *e = s->val.return_stmt.expr;
            SYMBOL* returnSym = s->val.return_stmt.func_ref->sym->val.func.returnSymRef;
            if(returnSym == NULL){
                fprintf(f, "return;\n ");
                break;
            }
            char* typeName = varTypeName(returnSym);
            fprintf(f, "return ");
            switch (returnSym->kind) {
                case typeSymKind:
                    codeEXP(f, e);
                    break;
                case arraySymKind:
                    if (convertArraySliceAccess(e, 0, NULL) == 0) {
                        if (checkIfBaseType(typeName) == 1) {
                            //fprintf(f, "%s", str);
                        }
                        else {
                            fprintf(f, ".clone()");
                        }
                    }
                    else {
                        fprintf(f, ".clone()");
                    }
                    break;
                case sliceSymKind:
                    //TODO: get array type name
                    convertArraySliceAccess(e, 0, NULL);
                    //fprintf(f, "%s", str);
                    break;
                case structSymKind:
                    //TODO: get struct type name
                    codeEXP(f, e);
                    fprintf(f, ".clone()");
                    break;
            }
                
            //printf("%s\n", typeName);
            fprintf(f, ";\n");
            // //return temp name
            // printTabs(f, tabs);
            // fprintf(f, "return ");
            // fprintf(f, "var_return;\n");
            break;
        case k_StatementKindShortDecl:
            //fprintf(f, "inside short decl");
            codeAssignOrShortDecl(s->val.short_decl.id_list, s->val.short_decl.expr_list, "", tabs, f, false);
            break;
        case k_StatementKindAssign:
            codeAssignOrShortDecl(s->val.assign_stmt.lhs, s->val.assign_stmt.rhs, s->val.assign_stmt.kind, tabs, f, true);
            break;
        // 9. Loops: infinite, while, 3-part;
        case k_StatementKindWhile:
            printTabs(f, tabs);
            
            fprintf(f, "try {\n");
            printTabs(f, tabs);
            fprintf(f, "while (");
            if (s->val.while_stmt.cond == NULL) {
                fprintf(f, "true) {\n");
            }
            else {
                codeEXP(f, s->val.while_stmt.cond);
                fprintf(f, ") {\n");
            }
            insideLoop = 1;
            codeSTMTS(f, s->val.while_stmt.body, tabs+1);
            printTabs(f, tabs);
            fprintf(f, "}\n");
            
            printTabs(f, tabs);
            fprintf(f, "} catch (Exception e) {\n");
            printTabs(f, tabs);
            fprintf(f, "}\n");
            
            insideLoop = 0;
            break;
        case k_StatementKindFor:
            printTabs(f, tabs);
            //first process all the pre statement
            codeSTMTS(f, s->val.for_stmt.first, tabs);
            //condition
            printTabs(f, tabs);
            fprintf(f, "while (");
            codeEXP(f, s->val.for_stmt.second);
            fprintf(f, ") {\n");
            //body
            insideLoop = 1;
            printTabs(f, tabs+1);
            fprintf(f, "try {\n");
            codeSTMTS(f, s->val.for_stmt.body, tabs+2);
            printTabs(f, tabs+1);
            fprintf(f, "} catch (Exception e) {\n");
            printTabs(f, tabs+1);
            fprintf(f, "} finally {\n");
            //exit condition
            codeSTMTS(f, s->val.for_stmt.third, tabs+2);
            fprintf(f, "\n");
            //finish
            printTabs(f, tabs+1);
            fprintf(f, "}\n");
            printTabs(f, tabs);
            fprintf(f, "}\n");
            insideLoop = 0;
            break;
        // 11. Functions: pass-by-value and return-by-value semantics;
        case k_StatementKindExp:
            //printf("reach exp stmt\n");
            printTabs(f, tabs);
            codeEXP(f, s->val.exp_stmt.expr);
            fprintf(f, ";\n");
            break;
        // 10. Printing: output formats, newlines;
        case k_StatementKindPrint:
            lhs = s->val.print_stmt.expr_list;
            
            while (lhs != NULL) {
                //print expr one by one
                SYMBOL* sym = typeEXP(lhs)->val;
                //interstring
                if(strcmp(sym->name, "string") == 0){
                    printTabs(f, tabs);
                    fprintf(f, "printEscapeChar(");
                    codeEXP(f, lhs);
                    fprintf(f, ");\n");     
                }
                else{
                    //printf("type %s\n", sym->name);
                    //printf("after type %s\n", varTypeName(sym));
                    char *symType = varTypeName(sym);
                    if(strcmp(symType, "double") == 0){
                        printTabs(f, tabs);
                        fprintf(f, "System.out.print(String.format(\"%%+6.6e\", ");
                        codeEXP(f, lhs);
                        fprintf(f, "));\n");
                    }
                    else{
                        printTabs(f, tabs);
                        fprintf(f, "System.out.print(");
                        codeEXP(f, lhs);
                        fprintf(f, ");\n");
                    }
                }
                
                lhs = lhs->next;
            }
            break;
        case k_StatementKindPrintln:
            lhs = s->val.println_stmt.expr_list;
            int isLast_println = 0;
            while (lhs != NULL) {
                if (lhs->next == NULL) {
                    isLast_println = 1;
                }
                //print expr one by one
                //interstring
                SYMBOL* sym = typeEXP(lhs)->val;
                if(strcmp(sym->name, "string") == 0){
                    printTabs(f, tabs);
                    fprintf(f, "printEscapeChar(");
                    codeEXP(f, lhs);
                    fprintf(f, ");\n");
                    if (isLast_println == 0){
                        printTabs(f, tabs);
                        fprintf(f, "System.out.print(\" \");\n");
                    }       
                }
                else{
                    char *symType = varTypeName(sym);
                    if (strcmp(symType, "double") == 0) {
                        printTabs(f, tabs);
                        fprintf(f, "System.out.print(String.format(\"%%+6.6e\", ");
                        codeEXP(f, lhs);
                        if (isLast_println == 0)
                            fprintf(f, ") + \" \");\n");
                        else
                            fprintf(f, "));\n");
                    }
                    else {
                        printTabs(f, tabs);
                        fprintf(f, "System.out.print(");
                        codeEXP(f, lhs);
                        if (isLast_println == 0)
                            fprintf(f, " + \" \");\n");
                        else
                            fprintf(f, ");\n");
                    }
                }

                lhs = lhs->next;
            }
            printTabs(f, tabs);
            fprintf(f, "System.out.print(\"\\n\");\n");
            break;
        case k_StatementKindIf:
            //need to rename
            //Is this optional?
            if (s->val.if_stmt.opt_cond) codeSTMTS(f, s->val.if_stmt.opt_cond, tabs);
            printTabs(f, tabs);
            fprintf(f, "if (");
            codeEXP(f, s->val.if_stmt.cond);
            fprintf(f, ") {\n");
            codeSTMTS(f, s->val.if_stmt.body, tabs+1);
            printTabs(f, tabs);
            fprintf(f, "}\n");
            if (s->val.if_stmt.elseif_part != NULL) {
                codeSTMTS(f, s->val.if_stmt.elseif_part, tabs);
            }
            break;
        case k_StatementKindElseIf:
            printTabs(f, tabs);
            fprintf(f, "else {\n");
            codeSTMTS(f, s->val.if_stmt.opt_cond, tabs+1);
            printTabs(f, tabs+1);
            fprintf(f, "if (");
            codeEXP(f, s->val.if_stmt.cond);
            fprintf(f, ") {\n");
            codeSTMTS(f, s->val.if_stmt.body, tabs+2);
            printTabs(f, tabs+1);
            fprintf(f, "}\n");
            if (s->val.elseif_stmt.elseif_part != NULL) {
                codeSTMTS(f, s->val.elseif_stmt.elseif_part, tabs+1);
            }
            printTabs(f, tabs);
            fprintf(f, "}\n");
            break;
        case k_StatementKindElse:
            printTabs(f, tabs);
            fprintf(f, "else {\n");
            codeSTMTS(f, s->val.else_stmt.body, tabs+1);
            printTabs(f, tabs);
            fprintf(f, "}\n");
            break;
        case k_StatementKindSwitch:
            caseFound = 0;
            
            if (s->val.switch_stmt.opt_cond != NULL)
                codeSTMTS(f, s->val.switch_stmt.opt_cond, tabs);
            if (s->val.switch_stmt.cond != NULL) {
                //s->val.switch_stmt.cond->kind != k_ExpressionKindBoolLiteral
                
                if (s->val.switch_stmt.cond->kind == k_ExpressionKindIdentifier
                    && s->val.switch_stmt.cond->val.identifier.sym->kind == varSymKind
                    && strcmp(varTypeName(s->val.switch_stmt.cond->val.identifier.sym->val.varType), "boolean") == 0) {
                    fprintf(f, "try {\n");
                    switchCondBool(s->val.switch_stmt.cond, s->val.switch_stmt.caseClauses, tabs+1, f);
                    printTabs(f, tabs);
                    fprintf(f, "} catch (Exception e) {\n");
                    printTabs(f, tabs);
                    fprintf(f, "}\n");
                    
                    break;
                }
                
                insideLoop = 1;
                
                fprintf(f, "try {\n");
                printTabs(f, tabs);
                fprintf(f, "switch (");
                codeEXP(f, s->val.switch_stmt.cond);
                fprintf(f, ") {\n");

                codeCase(s->val.switch_stmt.caseClauses, tabs+1, f);
                printTabs(f, tabs);
                fprintf(f, "}\n");
                
                printTabs(f, tabs);
                fprintf(f, "} catch (Exception e) {\n");
                printTabs(f, tabs);
                fprintf(f, "}\n");
            }

            //if there is no cond, we convert switch to if
            else {
                printTabs(f, tabs);
                fprintf(f, "try {\n");
                convertSwitchToIf(s->val.switch_stmt.caseClauses, tabs+1, f);
                printTabs(f, tabs);
                fprintf(f, "} catch (Exception e) {\n");
                printTabs(f, tabs);
                fprintf(f, "}\n");
            }
            
            caseFound = 0;
            insideLoop = 0;
            
            break;
        // inc and dec can only be int, float64 or rune
        case k_StatementKindInc:
            if(s->val.inc_stmt.expr->kind == k_ExpressionKindArrayIndex){
                //get the index
                char *index = tmpVariable2();
                printVariableDecl2(false, "int", index, tabs, f);
                codeEXP(f, s->val.inc_stmt.expr->val.array_index.index);
                fprintf(f, ";\n");

                //store the previous result in temp var
                SYMBOL* sym = typeEXP(s->val.inc_stmt.expr)->val;
                char *symType = varTypeName(sym);;
                char *tmp = tmpVariable2();
                printVariableDecl2(false, symType, tmp, tabs, f);
                //codeEXP(f, s->val.inc_stmt.expr);
                convertArraySliceAccess(s->val.inc_stmt.expr, 0, index);
                fprintf(f, ";\n");

                //increasing the value
                printTabs(f, tabs);
                fprintf(f, "%s++;\n", tmp);

                //store back the increased value
                printTabs(f, tabs);
                convertArraySliceAssign(s->val.inc_stmt.expr, 0, tmp, index);
                fprintf(f, ";\n");
            }
            else {
                printTabs(f, tabs);
                codeEXP(f, s->val.inc_stmt.expr);
                fprintf(f, "++;\n");
            }
            break;
        case k_StatementKindDec:
            if(s->val.dec_stmt.expr->kind == k_ExpressionKindArrayIndex){
                 //get the index
                char *index = tmpVariable2();
                printVariableDecl2(false, "int", index, tabs, f);
                codeEXP(f, s->val.dec_stmt.expr->val.array_index.index);
                fprintf(f, ";\n");

                //store the previous result in temp var
                SYMBOL* sym = typeEXP(s->val.dec_stmt.expr)->val;
                char *symType = varTypeName(sym);;
                char *tmp = tmpVariable2();
                printVariableDecl2(false, symType, tmp, tabs, f);
                //codeEXP(f, s->val.inc_stmt.expr);
                convertArraySliceAccess(s->val.dec_stmt.expr, 0, index);
                fprintf(f, ";\n");

                //increasing the value
                printTabs(f, tabs);
                fprintf(f, "%s--;\n", tmp);

                //store back the increased value
                printTabs(f, tabs);
                convertArraySliceAssign(s->val.dec_stmt.expr, 0, tmp, index);
                fprintf(f, ";\n");
            }
            else {
                printTabs(f, tabs);
                codeEXP(f, s->val.dec_stmt.expr);
                fprintf(f, "--;\n");
            }
            break;
    }
}
   
void codeEXP(FILE *f, EXP *e) {
    if (e == NULL) {
        return;
    }

    //printf("enter typeEXP %d\n", e->kind);
    // SYMBOL *resType;
    // SYMBOL *t, *expr, *lhs, *rhs;
    EXP *exp;
    SYMBOL *t;
    char *str;
    switch (e->kind) {
        case k_ExpressionKindIdentifier:
            if(e->val.identifier.sym) {
                if(strcmp(e->val.identifier.sym->name, "true")==0 || strcmp(e->val.identifier.sym->name, "false")==0){
                    if(e->val.identifier.sym->isConstant){
                        fprintf(f, "%s", e->val.identifier.sym->name);
                    }else{
                        fprintf(f, "%s", e->val.identifier.sym->rename);
                    }
                }
                else if(e->val.identifier.sym->isFuncParam){
                    fprintf(f, "%s", e->val.identifier.sym->funcParamName);
                }
                else {
                    //printf("rename %s\n", e->val.identifier.sym->rename);
                    fprintf(f, "%s", e->val.identifier.sym->rename);
                }
            }
            //struct access field
            else {
                //printf("struct access %s\n", e->val.identifier.name);
                fflush(stdout);
                fprintf(f, "__golitec_%s", e->val.identifier.name);
            }
            break;
        case k_ExpressionKindIntLiteral:
            //printf("reach int\n");
            fprintf(f, "%d", e->val.intLiteral.var);
            break;
        case k_ExpressionKindFloatLiteral:
            //fprintf(f, "reach float\n");
            fprintf(f, "%f", e->val.floatLiteral.var);
            break;
        case k_ExpressionKindInterStringLiteral:
            //fprintf(f, "reach interstring\n");
            //printf("string is%s\n", e->val.interstringLiteral.var);
            fprintf(f, "%s", e->val.interstringLiteral.var);
            break;
        case k_ExpressionKindRawStringLiteral:
            //printf("reach rawstring\n");
            fprintf(f, "\"%s\"", e->val.rawstringLiteral.var);
            break;
        case k_ExpressionKindBoolLiteral:
            //printf("reach bool\n");
            e->val.boolLiteral.var == 1 ? fprintf(f, "__golitec_bool_true") : fprintf(f, "__golitec_bool_false");
            break;
        case k_ExpressionKindRuneLiteral:
            //printf("reach rune\n");
            if (e->val.runeLiteral.var == '\a') fprintf(f, "(int)'\\a'");
            else if (e->val.runeLiteral.var == '\b') fprintf(f, "(int)'\\b'");
            else if (e->val.runeLiteral.var == '\f') fprintf(f, "(int)'\\f'");
            else if (e->val.runeLiteral.var == '\n') fprintf(f, "(int)'\\n'");
            else if (e->val.runeLiteral.var == '\t') fprintf(f, "(int)'\\t'");
            else if (e->val.runeLiteral.var == '\v') fprintf(f, "(int)'\\v'");
            else if (e->val.runeLiteral.var == '\r') fprintf(f, "(int)'\\r'");
            else if (e->val.runeLiteral.var == '\\') fprintf(f, "(int)'\\\'");
            else if (e->val.runeLiteral.var == '\'') fprintf(f, "(int)'\\'");
            else fprintf(f, "(int)'%c'", e->val.runeLiteral.var);

            break;
        case k_ExpressionKindBinary:
            //codeEXP(f, e->val.binary.lhs);
            codeBinary(e->val.binary.lhs, e->val.binary.opera, e->val.binary.rhs, f);
            //fprintf(f, " %s ", e->val.binary.opera);
            //codeEXP(f, e->val.binary.rhs);
            break;
        case k_ExpressionKindUnary:
            if(strcmp(e->val.unary.opera, "^") == 0){
                fprintf(f, " ~");
            }
            else fprintf(f, " %s", e->val.unary.opera);
            codeEXP(f, e->val.unary.expr);
            break;
        case k_ExpressionKindParen:
            fprintf(f, " (");
            codeEXP(f, e->val.paren.expr);
            fprintf(f, ") ");
            break;
        // 12. Built-ins: cap, append, and len
        case k_ExpressionKindCap:
            // cap
            codeEXP(f, e->val.cap.expr);
            fprintf(f, ".getCapacity()");
            
            break;
        case k_ExpressionKindAppend:
            //redecalare new variable
            if (convertArraySliceAccess(e->val.append.id, 0, NULL) == 1) {
                //addElement
                //fprintf(f, "%s", str);
                fprintf(f, ".addElement(");
                codeEXP(f, e->val.append.added);
                fprintf(f, ")");
            }
            else {
                //addList
                //fprintf(f, "%s", str);
                fprintf(f, ".addList(");
                codeEXP(f, e->val.append.added);
                fprintf(f, ")");
            }
            break;
        case k_ExpressionKindLen:
        
            codeEXP(f, e->val.cap.expr);
            SYMBOL *n = typeEXP(e->val.cap.expr)->val;
            if (n->kind == typeSymKind && (strcmp(n->name, "string") == 0 || strcmp(n->name, "rawstring") == 0)) {
                fprintf(f, ".length()");
            }
            else {
                fprintf(f, ".getSize()");
            }
            break;
        // TODO: field select, array access and function call
        case k_ExpressionKindFuncCall:

            if (e->val.func_call.name->kind == k_ExpressionKindParen){
                t = e->val.func_call.name->val.paren.expr->val.identifier.sym;
            }
            else {
                t = e->val.func_call.name->val.identifier.sym;
            }
            Node* expr = typeEXP(e->val.func_call.args);

            //type cast
            if (t->kind == typeSymKind || t->kind == structSymKind) {
                //type cast
                //getting the top most parent type of a type cast
                if(t->val.parentType != NULL){
                    t = t->val.parentType;
                    while(t->val.parentType){
                        t = t->val.parentType;
                    }
                }

                EXP *arg = e->val.func_call.args;
                if (strcmp(t->name, "string") == 0 || strcmp(t->name, "rawstring") == 0) {
                    fprintf(f, "String.valueOf(");
                    SYMBOL *sym = typeEXP(arg)->val;
                    if(sym->val.parentType != NULL){
                        sym = sym->val.parentType;
                        while(sym->val.parentType){
                            sym = sym->val.parentType;
                        }
                    }
                    if (strcmp(sym->name, "int") == 0 || strcmp(sym->name, "rune") == 0) {
                        fprintf(f, "(char)");
                    }
                }
                else {
                    fprintf(f, "(%s)(", convertBaseType(t->name));
                }
                codeEXP(f, arg);
                fprintf(f, ")");   
            }
            else if (t->kind == funcSymKind) {
                fprintf(f, "%s(", e->val.func_call.name->val.identifier.sym->rename);
                EXP *arg = e->val.func_call.args;
                while (arg != NULL && arg->next != NULL) {
                    codeEXP(f, arg);
                    fprintf(f, ", ");
                    arg = arg->next;
                }
                codeEXP(f, arg);
                fprintf(f, ")");
            }
            
            break;
        case k_ExpressionKindArrayIndex:
            //printf("reach array index\n");
            //getter
            convertArraySliceAccess(e, 0, NULL);
            //printf("res is %s\n", str);
            break;
        case k_ExpressionKindFieldAccess:
            //printf("reach struct field");
            codeEXP(f, e->val.access_field.id);
            fprintf(f, ".");
            codeEXP(f, e->val.access_field.field);
            break;
    }
}

int convertArraySliceAccess(EXP* e, int depth, char *index) {
    int dim = 0;
    //printf("hello");
    if (e->kind == k_ExpressionKindIdentifier) {
        if(e->val.identifier.sym->isFuncParam){
            fprintf(file, "%s", e->val.identifier.sym->funcParamName);
        }
        else 
            fprintf(file, "%s", e->val.identifier.sym->rename);
        //printf("sym var name is %d\n", e->val.identifier.sym->val.varType->dimension);
        //printf("array dim is %d and name is %s\n", e->val.identifier.sym->val.varType->dimension, e->val.identifier.sym->name);
        return e->val.identifier.sym->val.varType->dimension - depth;
    }
    else if (e->kind == k_ExpressionKindFieldAccess) {
        //printf("hello");
        codeEXP(file, e);
        e = e->val.access_field.field;
        //printf("sym var name is, %d\n", e->val.identifier.sym->val.varType->dimension);
        return e->val.identifier.sym->val.varType->dimension - depth;
    }
    else if (e->kind == k_ExpressionKindFuncCall) {
        codeEXP(file, e);
        e = e->val.func_call.name;
        //printf("sym var name is %s, %d\n", e->val.identifier.sym->val.func.returnSymRef->name, e->val.identifier.sym->val.func.returnSymRef->dimension);
        return e->val.identifier.sym->val.func.returnSymRef->dimension - depth;
    }
    else {
        dim = convertArraySliceAccess(e->val.array_index.array, depth+1, index);
    }

    
    if (dim == 0 && depth == 0) {
        fprintf(file, ".getElement(");
        if(index == NULL) codeEXP(file, e->val.array_index.index);
        else fprintf(file, "%s", index);
        fprintf(file, ")");
    }
    else {
        fprintf(file, ".getList(");
        if(index == NULL) codeEXP(file, e->val.array_index.index);
        else fprintf(file, "%s", index);
        fprintf(file, ")");
    }

    return dim;
}

int convertArraySliceAssign(EXP* e, int depth, char* value, char* index) {
    int dim = 0;
    //printf("hello");
    if (e->kind == k_ExpressionKindIdentifier) {
        if(e->val.identifier.sym->isFuncParam){
            fprintf(file, "%s", e->val.identifier.sym->funcParamName);
        }
        else 
            fprintf(file, "%s", e->val.identifier.sym->rename);
        //printf("sym var name is %d\n", e->val.identifier.sym->val.varType->dimension);
        //printf("array dim is %d and difference is %d\n", e->val.identifier.sym->val.varType->dimension, e->val.identifier.sym->val.varType->dimension - depth);
        return e->val.identifier.sym->val.varType->dimension - depth;
    }
    else if (e->kind == k_ExpressionKindFieldAccess) {
        //printf("hello");
        codeEXP(file, e);
        e = e->val.access_field.field;
        //printf("sym var name is, %d\n", e->val.identifier.sym->val.varType->dimension);
        return e->val.identifier.sym->val.varType->dimension - depth;
    }
    else if (e->kind == k_ExpressionKindFuncCall) {
        codeEXP(file, e);
        e = e->val.func_call.name;
        //printf("sym var name is %s, %d\n", e->val.identifier.sym->val.func.returnSymRef->name, e->val.identifier.sym->val.func.returnSymRef->dimension);
        return e->val.identifier.sym->val.func.returnSymRef->dimension - depth;
    }
    else {
        dim = convertArraySliceAssign(e->val.array_index.array, depth+1, value, index);
    }

    
    if (dim == 0 && depth == 0) {
        fprintf(file, ".setElement(");
        fprintf(file, "%s", value);
        fprintf(file, ", ");
        if(index == NULL) codeEXP(file, e->val.array_index.index);
        else fprintf(file, "%s", index);
        fprintf(file, ")");
    }
    else if (dim != 0 && depth == 0) {
        //printf("%d, %d", dim, depth);
        fprintf(file, ".setList(");
        fprintf(file, "%s", value);
        fprintf(file, ", ");
        if(index == NULL) codeEXP(file, e->val.array_index.index);
        else fprintf(file, "%s", index);
        fprintf(file, ")");
    }
    else {
        fprintf(file, ".getList(");
        if(index == NULL) codeEXP(file, e->val.array_index.index);
        else fprintf(file, "%s", index);
        fprintf(file, ")");
    }

    return dim;
}

bool checkPrimitive(char *type){
    if(strcmp(type, "int") == 0 || strcmp(type, "boolean") == 0
        || strcmp(type, "double") == 0)
        return true;
    return false;
}

bool checkString(char *type){
    if(strcmp(type, "String") == 0) return true;
    else return false;
}


void codeBinary(EXP *lhs, char *opera, EXP *rhs, FILE *f){
    //TODO does typeEXP return base type?  nope
    SYMBOL *sym = typeEXP(lhs)->val;
    char *symType = varTypeName(sym);
    bool isPrimitive = checkPrimitive(symType);
    
    //comparable
    if(strcmp(opera, "==") == 0){
        codeEXP(f, lhs);
        if(!checkPrimitive(symType)){
            fprintf(f, ".equals(");
            codeEXP(f, rhs);
            fprintf(f, ")");
        }else{
            fprintf(f, " == ");
            codeEXP(f, rhs);
        }
    }
    else if (strcmp(opera, "!=") == 0){
        if(!checkPrimitive(symType)){
            fprintf(f, " !(");
            codeEXP(f, lhs);
            fprintf(f, ".equals(");
            codeEXP(f, rhs);
            fprintf(f, ")) ");
        }else{
            codeEXP(f, lhs);
            fprintf(f, " != ");
            codeEXP(f, rhs);
        }

    }
    //ordered
    else if (0 == strcmp(opera, "<") ||
             0 == strcmp(opera, "<=") ||
             0 == strcmp(opera, ">") ||
             0 == strcmp(opera, ">=")){
        fprintf(f, "(");
        codeEXP(f, lhs);
        if(checkString(symType)){
            fprintf(f, ".compareTo(");
            codeEXP(f, rhs);
            fprintf(f, ") %s 0 ", opera);
        }
        else{
            fprintf(f, " %s ", opera);
            codeEXP(f, rhs);
        }
        fprintf(f, ")");
    }
    else if (strcmp(opera, "&^") == 0){
        codeEXP(f, lhs);
        fprintf(f, " &(~");
        codeEXP(f, rhs);
        fprintf(f, ") ");
    }
    // +, -, / , * , %, |, &, <<, >>, ||, &&, ^
    else{
        codeEXP(f, lhs);
        fprintf(f, " %s ", opera);
        codeEXP(f, rhs);
    }
    
}

void printVarAssign(char *var_name, char *value, char* kind, int tabs, FILE *f){
    printTabs(f, tabs);
    if(strcmp(kind, "&^=") == 0){
        fprintf(f, "%s = %s &(~%s);\n", var_name, var_name, value);
    }else{
        fprintf(f, "%s %s %s;\n", var_name, kind, value);
    }
}

void printLValueAssign(EXP *e, char *value, char* kind, int tabs, FILE *f){
    printTabs(f, tabs);
    codeEXP(f,e);
    if(strcmp(kind, "&^=") == 0){
        fprintf(f, " = ");
        codeEXP(f, e);
        fprintf(f, " &(~%s);\n", value);
        //fprintf(f, " = %s &(~%s);\n", codeEXP(f, e), value);
    }
    else{
        fprintf(f, " %s %s;\n", kind, value);
    }
}

char *getExprType(EXP *e){
    SYMBOL *sym = typeEXP(e)->val;
    return varTypeName(sym);
}

//for array index, struct field acess
void codeAssignLValue(char *type, EXP *id, EXP *expr, int tabs, FILE *f){
    char *temp_name;

    temp_name = tmpVariable2();
    id->tmp_name = temp_name;

    printVariableDecl2(false, type, temp_name, tabs, f);
    fflush(stdout);
    codeEXP(f, expr);
    fprintf(f, ";\n");
}

void codeAssignIden(char *type, EXP *id, EXP *expr, int tabs, FILE *f){
    SYMBOL *sym;
    sym = id->val.identifier.sym;
    char *temp_name;

    if(strcmp(sym->name, "_") == 0){
        temp_name = blankVariable();
    }
    else{
        //renameVariable(sym);
        temp_name = tmpVariable(sym);
        id->tmp_name = temp_name;
        sym->type = type;
    }

    printVariableDecl2(false, type, temp_name, tabs, f);
    fflush(stdout);
    codeEXP(f, expr);

    if(strcmp(sym->name, "_") != 0){
        if(sym->val.varType->kind == arraySymKind || sym->val.varType->kind == structSymKind){
            fprintf(f, ".clone()");
        }
    }
    fprintf(f, ";\n");
}

void codeAssignOrShortDecl(EXP *id, EXP *expr, char *kind, int tabs, FILE *f, bool isAssign){
    char *type;
    SYMBOL *sym;
    EXP *id2 = id;

    while(id){
        type = getExprType(expr);
        if(id->kind == k_ExpressionKindIdentifier){
            codeAssignIden(type, id, expr, tabs, f);
        }
        else{
            codeAssignLValue(type, id, expr, tabs, f);
        }
        id = id->next;
        expr = expr->next;
    }

    while(id2){
        if(id2->kind == k_ExpressionKindIdentifier){
            sym = id2->val.identifier.sym;
            if(strcmp(sym->name, "_") == 0){
                id2 = id2->next;
                continue;
            }
            //assignment
            if(isAssign){
                if(sym->isFuncParam){
                    printVarAssign(sym->funcParamName, id2->tmp_name, kind, tabs, f);
                }
                else printVarAssign(sym->rename, id2->tmp_name, kind, tabs, f);
            }
            //short decl of already decalred var
            else if(sym->declaredBefore && sym->rename != NULL){
                //printf("name of delcared var : %s\n", sym->name);
                printVarAssign(sym->rename, id2->tmp_name, "=", tabs, f);
            }
            //short decl of new var
            else{
                //printf("reached here\n");
               // fflush(stdout);
                renameVariable(sym);
               // printf("rename: %s\n", sym->rename);
                printVariableDecl(false, sym->type, sym->rename, id2->tmp_name, tabs, f);
            }
        }
        else if(id2->kind == k_ExpressionKindArrayIndex){
            //rename, assign to sym->rename
            printTabs(f, tabs);
            convertArraySliceAssign(id2, 0, id2->tmp_name, NULL);
            //fprintf(f, "%s", str);
            fprintf(f, ";\n");
        }
        //struct access
        else{
            printLValueAssign(id2, id2->tmp_name, kind, tabs, f);
        }
        id2 = id2->next;
    }
}


void convertSwitchToIf(CASE * caseClauses, int tabs, FILE *f) {
    if (caseClauses == NULL) return;
    
    if (caseClauses->next != NULL){
       convertSwitchToIf(caseClauses->next,tabs,f);
    }
    
    CASE * caseTmp = caseClauses;
    int hasBreak = 0;
    //caseFound = 0;
    
   // int ifelse = 0; // 0 = if , 1 = else if
    STMT * cpSTMT;
    //printTabs(f, tabs);
    if (caseTmp->kind == k_SwitchKindCase) {
        caseFound = 1;
        printTabs(f, tabs);
        fprintf(f, "if (");
        
        EXP * tmp = caseTmp->val.case_exp.expr_list;
                
        while (tmp) {
            codeEXP(f, tmp);
            tmp = tmp->next;
            if (tmp != NULL) fprintf(f, " || ");
        }
                
        fprintf(f, ") {\n");
        //printTabs(f, tabs);
        //case body,
        STMT * tmpSTMT = caseTmp->val.case_exp.body;
                
        while (tmpSTMT) {
            if (tmpSTMT->kind == k_StatementKindBreak) {
                hasBreak = 1;
                notPrintBreak = 1;
            }
            tmpSTMT = tmpSTMT->next;
        }
        
        codeSTMTS(f, caseTmp->val.case_exp.body, tabs);
        if (hasBreak == 1) {
            printTabs(f, tabs);
            fprintf(f,"throw new Exception();\n");
        }
        printTabs(f, tabs);
        fprintf(f, "}\n");
        }
    
        else {
            notPrintBreak = 1;
            if (caseFound == 0) {
                printTabs(f, tabs);
                codeSTMTS(f, caseTmp->val.default_exp.body, tabs);
            }
        }
        //caseTmp = caseTmp->next;
        notPrintBreak = 0;
}

void codeCase(CASE * caseClauses, int tabs, FILE *f) {
    caseFound = 0;
    
    if (caseClauses == NULL) {
        return;
    }
    
    if (caseClauses->next != NULL){
       codeCase(caseClauses->next,tabs,f);
    }
    
    CASE * caseTmp = caseClauses;
    
    if (caseTmp->kind == k_SwitchKindCase) {
        caseFound = 1;
        EXP* expr = caseTmp->val.case_exp.expr_list;
        while (expr) {
            printTabs(f, tabs);
            fprintf(f,"case ");
            codeEXP(f, expr);
            fprintf(f," :\n");
            expr = expr->next;
        }
        codeSTMTS(f, caseTmp->val.case_exp.body, tabs+1);
    }
    else {
        if (caseFound == 0) {
        printTabs(f, tabs);
        fprintf(f,"default: \n");
        codeSTMTS(f, caseTmp->val.default_exp.body, tabs+1);
        }
    }
}

void switchCondBool(EXP *cond, CASE * caseClauses, int tabs, FILE *f) {
    if (caseClauses == NULL) return;
     
     if (caseClauses->next != NULL){
        switchCondBool(cond, caseClauses->next,tabs,f);
     }
     
     CASE * caseTmp = caseClauses;
     int hasBreak = 0;
     caseFound = 0;
     
     STMT * cpSTMT;
     //printTabs(f, tabs);
     if (caseTmp->kind == k_SwitchKindCase) {
         caseFound = 1;
         printTabs(f, tabs);
         fprintf(f, "if (");
         
         EXP * tmp = caseTmp->val.case_exp.expr_list;
                 
         while (tmp) {
             codeEXP(f, tmp);
             fprintf(f, " == ");
             codeEXP(f, cond);
             tmp = tmp->next;
             if (tmp != NULL) fprintf(f, " || ");
         }
                 
         fprintf(f, ") {\n");
         //case body,
         STMT * tmpSTMT = caseTmp->val.case_exp.body;
                 
         while (tmpSTMT) {
             if (tmpSTMT->kind == k_StatementKindBreak) {
                 hasBreak = 1;
                 notPrintBreak = 1;
             }
             tmpSTMT = tmpSTMT->next;
         }
                 
         codeSTMTS(f, caseTmp->val.case_exp.body, tabs);
         if (hasBreak == 1) {
             printTabs(f, tabs);
             fprintf(f,"throw new Exception();\n");
         }
         printTabs(f, tabs);
         fprintf(f, "}\n");
         }
     
         else {
             notPrintBreak = 1;
             if (caseFound == 0) {
                 printTabs(f, tabs);
                 codeSTMTS(f, caseTmp->val.default_exp.body, tabs);
             }
         }
         //caseTmp = caseTmp->next;
         notPrintBreak = 0;
}



void codePrintSliceClass(FILE *f){
    fprintf(f, "import java.lang.reflect.InvocationTargetException;\n");
    fprintf(f, "import java.lang.reflect.Method;\n");
    fprintf(f, "import java.util.*;\n");
    fprintf(f, "  \n");
    fprintf(f, "abstract class __golitec_list<T> extends Object implements Cloneable {\n");
    fprintf(f, "    protected __golitec_list<T> root;\n");
    fprintf(f, "    protected Class<T> type;\n");
    fprintf(f, "    protected int[] all_size;\n");
    fprintf(f, "    protected int size;\n");
    fprintf(f, "    protected int capacity = 0;\n");
    fprintf(f, "    protected int depth;  //dimension\n");
    fprintf(f, "    protected int dimension;\n");
    fprintf(f, "    protected boolean isPrimitive;\n");
    fprintf(f, "    \n");
    fprintf(f, "    \n");
    fprintf(f, "    protected abstract T getElement(int idx);\n");
    fprintf(f, "    protected abstract __golitec_list<T> getList(int idx);\n");
    fprintf(f, "    protected abstract void setElement(T element, int idx);\n");
    fprintf(f, "    protected abstract void setList(__golitec_list<T> l, int idx);\n");
    fprintf(f, "    protected abstract __golitec_list<T> addElement(T element);\n");
    fprintf(f, "    protected abstract __golitec_list<T> addList();\n");
    fprintf(f, "    protected abstract __golitec_list<T> addList(__golitec_list<T> list);\n");
    fprintf(f, "    protected abstract int getSize();\n");
    fprintf(f, "    protected abstract int getCapacity();\n");
    fprintf(f, "    protected abstract void printElement();\n");
    fprintf(f, "    protected abstract __golitec_list<T> clone() throws CloneNotSupportedException;\n");
    fprintf(f, "    public abstract boolean equals(__golitec_list<T> list);\n");
    fprintf(f, "}\n");
    fprintf(f, "\n");
    fprintf(f, "class __golitec_array<T> extends __golitec_list<T> {\n");
    fprintf(f, "    ArrayList<__golitec_list<T>> list;\n");
    fprintf(f, "    ArrayList<T> elem;\n");
    fprintf(f, "\n");
    
    fprintf(f, "    @SuppressWarnings(\"unchecked\")\n");
    fprintf(f, "    public __golitec_array (int d, int dim, int[] s, Class<T> t, __golitec_list<T> root) {\n");
    fprintf(f, "        if (d == dim) {\n");
    fprintf(f, "            this.root = this;\n");
    fprintf(f, "        }\n");
    fprintf(f, "        else {\n");
    fprintf(f, "            this.root = root;\n");
    fprintf(f, "        }\n");
    fprintf(f, "        all_size = s;\n");
    fprintf(f, "        capacity = s[dim - d];\n");
    fprintf(f, "        depth = d;\n");
    fprintf(f, "        type = t;\n");
    fprintf(f, "        dimension = dim;\n");
    fprintf(f, "        size = s[dim - d];\n");
    fprintf(f, "        //initialize variable based on the current dimension\n");
    fprintf(f, "        if (d == 1) {\n");
    fprintf(f, "            elem = new ArrayList<T>();\n");
    fprintf(f, "            //populate element\n");
    fprintf(f, "            for (int i=0; i<size; i++) {\n");
    fprintf(f, "                if (t.equals(Integer.class)) {\n");
    fprintf(f, "                    elem.add((T)(Object)Integer.valueOf(0));\n");
    fprintf(f, "                }\n");
    fprintf(f, "                else if (t.equals(Float.class)) {\n");
    fprintf(f, "                    elem.add((T)(Object)Double.valueOf(0.0));\n");
    fprintf(f, "                }\n");
    fprintf(f, "                else if (t.equals(String.class)) {\n");
    fprintf(f, "                    elem.add((T)(Object)new String(\"\"));\n");
    fprintf(f, "                }\n");
    fprintf(f, "                else if (t.equals(Boolean.class)) {\n");
    fprintf(f, "                    elem.add((T)(Object)Boolean.FALSE);\n");
    fprintf(f, "                }\n");
    fprintf(f, "                else {\n");
    fprintf(f, "                    try {\n");
    fprintf(f, "                        elem.add((T)t.getConstructors()[0].newInstance());\n");
    fprintf(f, "                    } catch (InstantiationException | IllegalAccessException | IllegalArgumentException\n");
    fprintf(f, "                            | InvocationTargetException | SecurityException e) {\n");
    fprintf(f, "                        // TODO Auto-generated catch block\n");
    fprintf(f, "                        e.printStackTrace();\n");
    fprintf(f, "                    }\n");
    fprintf(f, "                }\n");
    fprintf(f, "            }\n");
    fprintf(f, "        }\n");
    fprintf(f, "        //list case\n");
    fprintf(f, "        else {\n");
    fprintf(f, "            list = new ArrayList<__golitec_list<T>>();\n");
    fprintf(f, "            if (size > 0) { //we have array type\n");
    fprintf(f, "                if (dim - d + 1 == dim - 1 && s[dim - d + 1] == 0) {\n");
    fprintf(f, "                    for (int i=0; i<size; i++) {\n");
    fprintf(f, "                        list.add(new __golitec_slice<T>(d-1, dim, s, t, this.root));\n");
    fprintf(f, "                    }\n");
    fprintf(f, "                }\n");
    fprintf(f, "                else {\n");
    fprintf(f, "                    for (int i=0; i<size; i++) {\n");
    fprintf(f, "                        list.add(new __golitec_array<T>(d-1, dim, s, t, this.root));\n");
    fprintf(f, "                    }\n");
    fprintf(f, "                }\n");
    fprintf(f, "            }\n");
    fprintf(f, "            else {  //we have slice type\n");
    fprintf(f, "                System.out.println(\"create slice\");\n");
    fprintf(f, "                for (int i=0; i<size; i++) {\n");
    fprintf(f, "                    list.add(new __golitec_slice<T>(d-1, dim, s, t, this.root));\n");
    fprintf(f, "                }\n");
    fprintf(f, "            }\n");
    fprintf(f, "        }\n");
    fprintf(f, "        \n");
    fprintf(f, "    }\n");
    fprintf(f, "    \n");
    
    
    fprintf(f, "    @Override\n");
    fprintf(f, "    protected void setElement(T element, int idx) {\n");
    fprintf(f, "        if (depth == 1) {\n");
    fprintf(f, "            elem.set(idx, element);\n");
    fprintf(f, "        }\n");
    fprintf(f, "    }\n");
    fprintf(f, "    \n");
    fprintf(f, "    @Override\n");
    
    
    fprintf(f, "    protected void setList(__golitec_list<T> l, int idx) {\n");
    fprintf(f, "        if (depth != 1) {\n");
    fprintf(f, "            list.set(idx, l);\n");
    fprintf(f, "        }\n");
    fprintf(f, "    }\n");
    fprintf(f, "    \n");
    fprintf(f, "    @Override\n");
    
    
    fprintf(f, "    protected __golitec_list<T> addElement(T element) { return null; }\n");
    fprintf(f, "    \n");
    fprintf(f, "    @Override\n");
    
    
    fprintf(f, "    protected __golitec_list<T> addList() { return null; }\n");
    fprintf(f, "    \n");
    fprintf(f, "    @Override\n");
    
    
    fprintf(f, "    protected T getElement(int idx) {\n");
    fprintf(f, "        if (depth == 1) {\n");
    fprintf(f, "            return elem.get(idx);\n");
    fprintf(f, "        }\n");
    fprintf(f, "        else {\n");
    fprintf(f, "            try {\n");
    fprintf(f, "                throw new Exception();\n");
    fprintf(f, "            }\n");
    fprintf(f, "            catch (Exception e) {\n");
    fprintf(f, "                \n");
    fprintf(f, "            }\n");
    fprintf(f, "            return null;\n");
    fprintf(f, "        }\n");
    fprintf(f, "    }\n");
    fprintf(f, "    \n");
    fprintf(f, "    @Override\n");
    
    
    fprintf(f, "    protected __golitec_list<T> getList(int idx) {\n");
    fprintf(f, "        if (depth != 1) {\n");
    fprintf(f, "            return list.get(idx);\n");
    fprintf(f, "        }\n");
    fprintf(f, "        else {\n");
    fprintf(f, "            try {\n");
    fprintf(f, "                throw new Exception();\n");
    fprintf(f, "            }\n");
    fprintf(f, "            catch (Exception e) {\n");
    fprintf(f, "                System.out.println(\"Can't get list when depth is 1\");\n");
    fprintf(f, "            }\n");
    fprintf(f, "            return null;\n");
    fprintf(f, "        }\n");
    fprintf(f, "    }\n");
    fprintf(f, "    \n");
    
    
    fprintf(f, "    @Override\n");
    fprintf(f, "    protected int getSize() {\n");
    fprintf(f, "        return all_size[dimension - depth];\n");
    fprintf(f, "    }\n");
    fprintf(f, "    \n");
    fprintf(f, "    @Override\n");
    
    
    fprintf(f, "    protected int getCapacity() {\n");
    fprintf(f, "        return all_size[dimension - depth];\n");
    fprintf(f, "    }\n");
    fprintf(f, "\n");
    fprintf(f, "    @Override\n");
    
    
    fprintf(f, "    protected __golitec_list<T> clone() throws CloneNotSupportedException {\n");
    fprintf(f, "        __golitec_array<T> c = new __golitec_array<T>(depth, dimension, all_size, type, null);\n");
    fprintf(f, "        c.all_size = new int[dimension];\n");
    fprintf(f, "        for (int i=0; i<dimension; i++) {\n");
    fprintf(f, "            c.all_size[i]= all_size[i]; \n");
    fprintf(f, "        }\n");
    fprintf(f, "        c.capacity = capacity;\n");
    fprintf(f, "        c.isPrimitive = isPrimitive;\n");
    fprintf(f, "        if (list != null) { \n");
    fprintf(f, "            for (int i=0; i<list.size(); i++) {\n");
    fprintf(f, "                __golitec_list<T> temp = list.get(i).clone();\n");
    fprintf(f, "                temp.root = c;\n");
    fprintf(f, "                c.list.set(i, temp);\n");
    fprintf(f, "                \n");
    fprintf(f, "            }\n");
    fprintf(f, "        }\n");
    fprintf(f, "        if (elem != null) { \n");
    fprintf(f, "            for (int i=0; i<elem.size(); i++) {\n");
    fprintf(f, "                c.elem.set(i, elem.get(i));\n");
    fprintf(f, "            }\n");
    fprintf(f, "        }\n");
    fprintf(f, "            \n");
    fprintf(f, "        return c;\n");
    fprintf(f, "    }\n");
    fprintf(f, "\n");
    
    
    fprintf(f, "    @Override\n");
    fprintf(f, "    protected void printElement() {\n");
    fprintf(f, "        if (depth == 1) {\n");
    fprintf(f, "            for (T e : elem) {\n");
    fprintf(f, "                System.out.println(e);\n");
    fprintf(f, "            }\n");
    fprintf(f, "        }\n");
    fprintf(f, "    }\n");
    fprintf(f, "\n");
    
    
    fprintf(f, "    @Override\n");
    fprintf(f, "    public boolean equals(__golitec_list<T> list) {\n");
    fprintf(f, "        if (list.depth == depth) {\n");
    fprintf(f, "            if (list.getCapacity() == getCapacity() && list.getSize() == getSize() && list.type == type) {\n");
    fprintf(f, "                boolean ans = true;\n");
    fprintf(f, "                if (depth == 1) {\n");
    fprintf(f, "                    for (int i=0; i<getSize(); i++) {\n");
    fprintf(f, "                        ans = ans && this.elem.get(i).equals(list.getElement(i));\n");
    fprintf(f, "                    }\n");
    fprintf(f, "                }\n");
    fprintf(f, "                else {\n");
    fprintf(f, "                    for (int i=0; i<getSize(); i++) {\n");
    fprintf(f, "                        ans = ans && this.list.get(i).equals(list.getList(i));\n");
    fprintf(f, "                    }\n");
    fprintf(f, "                }\n");
    fprintf(f, "                return ans;\n");
    fprintf(f, "            }\n");
    fprintf(f, "            else {\n");
    fprintf(f, "                return false;\n");
    fprintf(f, "            }\n");
    fprintf(f, "            \n");
    fprintf(f, "        }\n");
    fprintf(f, "        else {\n");
    fprintf(f, "            return false;\n");
    fprintf(f, "        }\n");
    fprintf(f, "    }\n");
    fprintf(f, "\n");
    
    
    fprintf(f, "    @Override\n");
    fprintf(f, "    protected __golitec_list<T> addList(__golitec_list<T> list) {\n");
    fprintf(f, "        // TODO Auto-generated method stub\n");
    fprintf(f, "        return null;\n");
    fprintf(f, "    }\n");
    fprintf(f, "}\n");
    fprintf(f, "\n");
    fprintf(f, "class __golitec_slice<T> extends __golitec_list<T> {\n");
    fprintf(f, "    ArrayList<__golitec_list<T>> list;\n");
    fprintf(f, "    ArrayList<T> elem;\n");
    fprintf(f, "\n");
    
    
    fprintf(f, "    public __golitec_slice (int d, int dim, int[] s, Class<T> t, __golitec_list<T> root) {\n");
    fprintf(f, "        if (d == dim) {\n");
    fprintf(f, "            this.root = this;\n");
    fprintf(f, "        }\n");
    fprintf(f, "        else {\n");
    fprintf(f, "            this.root = root;\n");
    fprintf(f, "        }\n");
    fprintf(f, "        all_size = s;\n");
    fprintf(f, "        capacity = 0;\n");
    fprintf(f, "        depth = d;\n");
    fprintf(f, "        type = t;\n");
    fprintf(f, "        dimension = dim;\n");
    fprintf(f, "        //initialize variable based on the current dimension\n");
    fprintf(f, "        if (d == 1) {\n");
    fprintf(f, "            elem = new ArrayList<T>();\n");
    fprintf(f, "        }\n");
    fprintf(f, "        //list case\n");
    fprintf(f, "        else {\n");
    fprintf(f, "            list = new ArrayList<__golitec_list<T>>();\n");
    fprintf(f, "        }\n");
    fprintf(f, "        \n");
    fprintf(f, "    }\n");
    fprintf(f, "    \n");
    
    
    fprintf(f, "    @Override\n");
    fprintf(f, "    protected void setElement(T element, int idx) {\n");
    fprintf(f, "        try {\n");
    fprintf(f, "            if (idx + 1 > size) \n");
    fprintf(f, "                throw new Exception(\"ERROR: index out of bounds\");\n");
    fprintf(f, "            if (depth == 1) {\n");
    fprintf(f, "                elem.set(idx, element);\n");
    fprintf(f, "            }\n");
    fprintf(f, "        }\n");
    fprintf(f, "        catch (Exception e) {\n");
    fprintf(f, "            e.printStackTrace();\n");
    fprintf(f, "            System.exit(1);\n");
    fprintf(f, "        }\n");
    fprintf(f, "    }\n");
    fprintf(f, "    \n");
    fprintf(f, "    @Override\n");
    
    
    fprintf(f, "    protected void setList(__golitec_list<T> l, int idx) {\n");
    fprintf(f, "        try {\n");
    fprintf(f, "            if (idx + 1 > size) \n");
    fprintf(f, "                throw new Exception(\"ERROR: index out of bounds\");\n");
    fprintf(f, "            if (depth != 1) \n");
    fprintf(f, "                list.set(idx, l);\n");
    fprintf(f, "        } catch (Exception e) {\n");
    fprintf(f, "            e.printStackTrace();\n");
    fprintf(f, "            System.exit(1);\n");
    fprintf(f, "        }\n");
    fprintf(f, "    }\n");
    fprintf(f, "    \n");
    
    
    fprintf(f, "    @Override\n");
    fprintf(f, "    protected __golitec_list<T> addElement(T element) {\n");
    fprintf(f, "        //System.out.println(\"add\");\n");
    fprintf(f, "        if (depth == 1) {\t\t\n");
    fprintf(f, "            elem.add(element);\n");
    fprintf(f, "            if (size == 0) {\n");
    fprintf(f, "                capacity = 2;\n");
    fprintf(f, "            }\n");
    fprintf(f, "            size++;\n");
    fprintf(f, "            \n");
    fprintf(f, "            if (size > capacity) {\n");
    fprintf(f, "                capacity *= 2;\n");
    fprintf(f, "                try {\n");
    fprintf(f, "                    __golitec_list<T> c = this.deepClone();\n");
    fprintf(f, "                      elem.remove(elem.size()-1);\n");
    fprintf(f, "                    size--;\n");
    fprintf(f, "                     capacity /= 2;\n");
    fprintf(f, "                     return c;\n");
    fprintf(f, "                } catch (CloneNotSupportedException e) {\n");
    fprintf(f, "                    // TODO Auto-generated catch block\n");
    fprintf(f, "                    e.printStackTrace();\n");
    fprintf(f, "                }\n");
    fprintf(f, "            }\n");
    fprintf(f, "            else {\n");
    fprintf(f, "                return this;\n");
    fprintf(f, "            }\n");
    fprintf(f, "        }\n");
    fprintf(f, "        \n");
    fprintf(f, "        return null;\n");
    fprintf(f, "    }\n");
    fprintf(f, "    \n");
    
    
    fprintf(f, "    @Override\n");
    fprintf(f, "    protected __golitec_list<T> addList() {\n");
    fprintf(f, "        if (depth != 1) {\n");
    fprintf(f, "            __golitec_list<T> l = null;\n");
    fprintf(f, "            if (all_size[dimension - depth + 1] == 0) {\n");
    fprintf(f, "                l = new __golitec_slice<T>(depth - 1, dimension, all_size, type, root);\n");
    fprintf(f, "            }\n");
    fprintf(f, "            else {\n");
    fprintf(f, "                l = new __golitec_array<T>(depth - 1, dimension, all_size, type, root);\n");
    fprintf(f, "            }\n");
    fprintf(f, "            \n");
    fprintf(f, "            list.add(l);\n");
    fprintf(f, "            if (size == 0) {\n");
    fprintf(f, "                capacity = 2;\n");
    fprintf(f, "            }\n");
    fprintf(f, "            size++;\n");
    fprintf(f, "            \n");
    fprintf(f, "            if (size > capacity) {\n");
    fprintf(f, "                capacity *= 2;\n");
    fprintf(f, "                try {\n");
    fprintf(f, "                    __golitec_list<T> c = this.deepClone();\n");
    fprintf(f, "                      list.remove(list.size()-1);\n");
    fprintf(f, "                    size--;\n");
    fprintf(f, "                     capacity /= 2;\n");
    fprintf(f, "                     return c;\n");
    fprintf(f, "                } catch (CloneNotSupportedException e) {\n");
    fprintf(f, "                    // TODO Auto-generated catch block\n");
    fprintf(f, "                    e.printStackTrace();\n");
    fprintf(f, "                }\n");
    fprintf(f, "            }\n");
    fprintf(f, "            else {\n");
    fprintf(f, "                return this;\n");
    fprintf(f, "            }\n");
    fprintf(f, "        }\n");
    fprintf(f, "        \n");
    fprintf(f, "        return null;\n");
    fprintf(f, "    }\n");
    fprintf(f, "    \n");
    
    
    fprintf(f, "    @Override\n");
    fprintf(f, "    protected T getElement(int idx){\n");
    fprintf(f, "        try {\n");
    fprintf(f, "            if (idx + 1 > size) \n");
    fprintf(f, "                throw new Exception(\"ERROR: index out of bounds\");\n");
    fprintf(f, "            if (depth == 1) {\n");
    fprintf(f, "                return elem.get(idx);\n");
    fprintf(f, "            }\n");
    fprintf(f, "            else {\n");
    fprintf(f, "                try {\n");
    fprintf(f, "                    throw new Exception();\n");
    fprintf(f, "                }\n");
    fprintf(f, "                catch (Exception e) {\n");
    fprintf(f, "                    \n");
    fprintf(f, "                }\n");
    fprintf(f, "                return null;\n");
    fprintf(f, "            }\n");
    fprintf(f, "        } catch (Exception e) {\n");
    fprintf(f, "            e.printStackTrace();\n");
    fprintf(f, "            System.exit(1);\n");
    fprintf(f, "        }\n");
    fprintf(f, "        return null;\n");
    fprintf(f, "    }\n");
    fprintf(f, "    \n");
    
    
    fprintf(f, "    @Override\n");
    fprintf(f, "    protected __golitec_list<T> getList(int idx) {\n");
    fprintf(f, "        try {\n");
    fprintf(f, "            if (idx + 1 > size) \n");
    fprintf(f, "                throw new Exception(\"ERROR: index out of bounds\");\n");
    fprintf(f, "            if (depth != 1) {\n");
    //fprintf(f, "                System.out.println(\"size of list:\" + list.size());\n");
    fprintf(f, "                return list.get(idx);\n");
    fprintf(f, "            }\n");
    fprintf(f, "            else {\n");
    fprintf(f, "                try {\n");
    fprintf(f, "                    throw new Exception();\n");
    fprintf(f, "                }\n");
    fprintf(f, "                catch (Exception e) {\n");
    fprintf(f, "                    System.out.println(\"Can't get list when depth is 1\");\n");
    fprintf(f, "                }\n");
    fprintf(f, "                return null;\n");
    fprintf(f, "            }\n");
    fprintf(f, "        } catch (Exception e) {\n");
    fprintf(f, "            e.printStackTrace();\n");
    fprintf(f, "            System.exit(1);\n");
    fprintf(f, "        }\n");
    fprintf(f, "        return null;\n");
    fprintf(f, "    }\n");
    fprintf(f, "    \n");

    fprintf(f, "    @Override\n");
    fprintf(f, "    protected int getSize() {\n");
    fprintf(f, "        return size;\n");
    fprintf(f, "    }\n");
    fprintf(f, "    \n");

    fprintf(f, "    @Override\n");
    fprintf(f, "    protected int getCapacity() {\n");
    fprintf(f, "        return capacity;\n");
    fprintf(f, "    }\n");
    fprintf(f, "\n");
    fprintf(f, "    \n");
    
    fprintf(f, "    @SuppressWarnings(\"unchecked\")\n");
    fprintf(f, "    @Override\n");
    fprintf(f, "    protected __golitec_list<T> clone() throws CloneNotSupportedException {\n");
    fprintf(f, "        __golitec_slice<T> c = new __golitec_slice<T>(depth, dimension, all_size, type, null);\n");
    fprintf(f, "        //System.out.println(depth + \" \" + dimension + \" \" + capacity);\n");
    fprintf(f, "        c.all_size = new int[dimension];\n");
    fprintf(f, "        for (int i=0; i<dimension; i++) {\n");
    fprintf(f, "            c.all_size[i]= all_size[i]; \n");
    fprintf(f, "        }\n");
    fprintf(f, "        c.capacity = capacity;\n");
    fprintf(f, "        c.isPrimitive = isPrimitive;\n");
    fprintf(f, "        c.size = size;\n");
    fprintf(f, "        c.list = list;\n");
    fprintf(f, "        c.elem = elem;\n");
    fprintf(f, "        return c;\n");
    fprintf(f, "    }\n");
    fprintf(f, "\n");

    fprintf(f, "    @SuppressWarnings(\"unchecked\")\n");
    fprintf(f, "    protected __golitec_list<T> deepClone() throws CloneNotSupportedException {\n");
    fprintf(f, "        __golitec_slice<T> c = new __golitec_slice<T>(depth, dimension, all_size, type, null);\n");
    fprintf(f, "        //System.out.println(depth + \" \" + dimension + \" \" + capacity);\n");
    fprintf(f, "        c.all_size = new int[dimension];\n");
    fprintf(f, "        for (int i=0; i<dimension; i++) {\n");
    fprintf(f, "            c.all_size[i]= all_size[i]; \n");
    fprintf(f, "        }\n");
    fprintf(f, "        c.capacity = capacity;\n");
    fprintf(f, "        c.isPrimitive = isPrimitive;\n");
    fprintf(f, "        c.size = size;\n");
    fprintf(f, "        if (list != null) { \n");
    fprintf(f, "            if (list.size() != 0) {\n");
    fprintf(f, "                //System.out.println(\"Clone\");\n");
    fprintf(f, "                for (int i=0; i<list.size(); i++) {\n");
    fprintf(f, "                    __golitec_list<T> temp = list.get(i).clone();\n");
    fprintf(f, "                    temp.root = c;\n");
    fprintf(f, "                    c.list.add(temp);\n");
    fprintf(f, "                }\n");
    fprintf(f, "            }\n");
    fprintf(f, "        }\n");
    fprintf(f, "        if (elem != null) { \n");
    fprintf(f, "            if (elem.size() != 0) {\n");
    fprintf(f, "                //System.out.println(\"Clone element\");\n");
    fprintf(f, "                for (int i=0; i<elem.size(); i++) {\n");
    fprintf(f, "                    if (type.equals(Integer.class) || \n");
    fprintf(f, "                        type.equals(Float.class) || \n");
    fprintf(f, "                        type.equals(Boolean.class) || \n");
    fprintf(f, "                        type.equals(String.class)) {\n");
    fprintf(f, "                        c.elem.add(elem.get(i));\n");
    fprintf(f, "                    }\n");
    fprintf(f, "                    else {\n");
    fprintf(f, "                        Method clone = null;\n");
    fprintf(f, "                        try {\n");
    fprintf(f, "                            clone = elem.get(i).getClass().getMethod(\"clone\");\n");
    fprintf(f, "                            Object[] args = new Object[0];\n");
    fprintf(f, "                            c.elem.add((T) clone.invoke(elem.get(i), args));\n");
    fprintf(f, "                        } catch (NoSuchMethodException | SecurityException | IllegalAccessException | IllegalArgumentException\n");
    fprintf(f, "                                | InvocationTargetException e) {\n");
    fprintf(f, "                            throw new RuntimeException(e);\n");
    fprintf(f, "                        }\n");
    fprintf(f, "                    }\n");
    fprintf(f, "                    \n");
    fprintf(f, "                }\t\t\n");
    fprintf(f, "            }\n");
    fprintf(f, "        }\n");
    fprintf(f, "            \n");
    fprintf(f, "        return c;\n");
    fprintf(f, "    }\n");
    fprintf(f, "\n");

    fprintf(f, "    @Override\n");
    fprintf(f, "    protected void printElement() {\n");
    fprintf(f, "        if (depth == 1) {\n");
    fprintf(f, "            for (T e : elem) {\n");
    fprintf(f, "                System.out.println(e);\n");
    fprintf(f, "            }\n");
    fprintf(f, "        }\n");
    fprintf(f, "    }\n");
    fprintf(f, "\n");
    fprintf(f, "    @Override\n");
    fprintf(f, "    public boolean equals(__golitec_list<T> list) {\n");
    fprintf(f, "        if (list.depth == depth) {\n");
    fprintf(f, "            if (list.getCapacity() == getCapacity() && list.getSize() == getSize() && list.type == type) {\n");
    fprintf(f, "                boolean ans = true;\n");
    fprintf(f, "                if (depth == 1) {\n");
    fprintf(f, "                    for (int i=0; i<getSize(); i++) {\n");
    fprintf(f, "                        ans = ans && this.elem.get(i).equals(list.getElement(i));\n");
    fprintf(f, "                    }\n");
    fprintf(f, "                }\n");
    fprintf(f, "                else {\n");
    fprintf(f, "                    for (int i=0; i<getSize(); i++) {\n");
    fprintf(f, "                        ans = ans && this.list.get(i).equals(list.getList(i));\n");
    fprintf(f, "                    }\n");
    fprintf(f, "                }\n");
    fprintf(f, "                return ans;\n");
    fprintf(f, "            }\n");
    fprintf(f, "            else {\n");
    fprintf(f, "                return false;\n");
    fprintf(f, "            }\n");
    fprintf(f, "            \n");
    fprintf(f, "        }\n");
    fprintf(f, "        else {\n");
    fprintf(f, "            return false;\n");
    fprintf(f, "        }\n");
    fprintf(f, "    }\n");
    fprintf(f, "\n");
    fprintf(f, "    @Override\n");
    
    fprintf(f, "    protected __golitec_list<T> addList(__golitec_list<T> l) {\n");
    fprintf(f, "        if (depth != 1) {\n");
    fprintf(f, "            __golitec_list<T> l1 = null;\n");
    fprintf(f, "            if (all_size[dimension - depth + 1] == 0) {\n");
    /*
    fprintf(f, "                l1 = new __golitec_slice<T>(depth - 1, dimension, all_size, type, root);\n");
    fprintf(f, "                l1.depth = depth - 1;\n");
    fprintf(f, "                l1.dimension = dimension;\n");
    fprintf(f, "                l1.all_size = all_size;\n");
    fprintf(f, "                l1.type = type;\n");
    fprintf(f, "                l1.root = root;\n");
    fprintf(f, "                l1.size = size;\n");
    */
    fprintf(f, "                l1 = l;\n");
    fprintf(f, "            }\n");
    fprintf(f, "            else {\n");
    fprintf(f, "                l1 = new __golitec_array<T>(depth - 1, dimension, all_size, type, root);\n");
    fprintf(f, "                l1.depth = depth - 1;\n");
    fprintf(f, "                l1.dimension = dimension;\n");
    fprintf(f, "                l1.all_size = all_size;\n");
    fprintf(f, "                l1.type = type;\n");
    fprintf(f, "                l1.root = root;\n");
    fprintf(f, "                l1.size = size;\n");
    fprintf(f, "            }\n");
    fprintf(f, "            \n");
    fprintf(f, "            list.add(l1);\n");
    fprintf(f, "            if (size == 0) {\n");
    fprintf(f, "                capacity = 2;\n");
    fprintf(f, "            }\n");
    fprintf(f, "            size++;\n");
    fprintf(f, "            \n");
    fprintf(f, "            if (size >= capacity) {\n");
    fprintf(f, "                capacity *= 2;\n");
    fprintf(f, "                try {\n");
    fprintf(f, "                    __golitec_list<T> c = this.deepClone();\n");
    fprintf(f, "                    list.remove(list.size()-1);\n");
    fprintf(f, "                    size--;\n");
    fprintf(f, "                    capacity /= 2;\n");
    fprintf(f, "                    return c;\n");
    fprintf(f, "                } catch (CloneNotSupportedException e) {\n");
    fprintf(f, "                    // TODO Auto-generated catch block\n");
    fprintf(f, "                    e.printStackTrace();\n");
    fprintf(f, "                }\n");
    fprintf(f, "            }\n");
    fprintf(f, "            else {\n");
    fprintf(f, "                return this;\n");
    fprintf(f, "            }\n");
    fprintf(f, "        }\n");
    fprintf(f, "        \n");
    fprintf(f, "        return null;\n");
    fprintf(f, "    }\n");
    fprintf(f, "}\n");
    fprintf(f, "\n");

}

void codePrintEscapeCharMethod(FILE *f){
    fprintf(f, "    public static void printEscapeChar(String s){\n");
    fprintf(f, "        String ret = \"\";\n");
    fprintf(f, "        for(int i = 0; i < s.length(); i++){\n");
    fprintf(f, "            if(s.charAt(i) != '~')\n");
    fprintf(f, "                ret = ret + s.charAt(i);\n");
    fprintf(f, "            else{\n");
    fprintf(f, "                if(i+1 < s.length()){\n");
    fprintf(f, "                    if(s.charAt(i+1) == 'a') {\n");
    fprintf(f, "                        System.out.print(ret);\n");
    fprintf(f, "                        System.out.print((char)7);\n");
    fprintf(f, "                        ret = \"\";\n");
    fprintf(f, "                        i++;\n");
    fprintf(f, "                    }\n");
    fprintf(f, "                    else if (s.charAt(i+1) == 'v') {\n");
    fprintf(f, "                        System.out.print(ret);\n");
    fprintf(f, "                        System.out.print((char)11);\n");
    fprintf(f, "                        ret = \"\";\n");
    fprintf(f, "                        i++;\n");
    fprintf(f, "                    }\n");
    fprintf(f, "                }\n");
    fprintf(f, "                else {\n");
    fprintf(f, "                        System.out.print(ret);\n");
    fprintf(f, "                        System.out.print('~');\n");
    fprintf(f, "                        ret = \"\";\n"); 
    fprintf(f, "                }\n");
    fprintf(f, "            }\n");
    fprintf(f, "        }\n");
    fprintf(f, "        System.out.print(ret);\n");
    fprintf(f, "    }\n\n");
}


