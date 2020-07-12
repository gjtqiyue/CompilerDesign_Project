#ifndef SYMBOL_H
#define SYMBOL_H
#define HASHSIZE 317
#include <stdbool.h>
#include "tree.h"

extern bool print;

typedef struct Node Node;
typedef struct SYMBOL SYMBOL;
typedef struct SymbolTable SymbolTable;

struct Node {
    SYMBOL* val;
    int isValue;
    Node* next;
};

typedef enum SymbolKind{
	inferSymKind,
    varSymKind,
    funcSymKind,
    typeSymKind,  //singleTypeKind
    structSymKind, 
    sliceSymKind, //used only for temp symbol that are intermediate nodes in the parent type list
    arraySymKind, //used only for temp symbol that are intermediate nodes in the parent type list
    nullSymKind
} SymbolKind;

struct SYMBOL {
    char *name;
    int arraySize;
    enum SymbolKind kind;
    int lineno;
    int isConstant;
    int isBaseType;
    int tableDepth;
    char *rename;   //for codegen
    bool declaredBefore; //for codegen
    char *tmp_name; //for codegen
    char *type;     //for codegen
    char *structClassName; //for codegen
    int dimension; //for codegen
    char *funcParamName; //for codegen
    bool isFuncParam;

    union{
        SYMBOL *varType;        //variable type
        SYMBOL *parentType;     //type hierachy
        SYMBOL *structFields;   //struct 
        struct {Node *funcParams; SYMBOL *returnSymRef;} func;   //func
    } val;

    SYMBOL *next;
};

struct SymbolTable {
    SYMBOL *varTable[HASHSIZE];
    SYMBOL *typeTable[HASHSIZE];
    SYMBOL *funcTable[HASHSIZE];
    SymbolTable *parent;
};

extern char *getTypePrintName(SYMBOL* type);

int Hash(char *str);
SymbolTable* initSymbolTable();
void prettyTabs(int n);
SYMBOL* makeSymbol(char* id, SymbolKind kind);
void addPredefinedTypeSymbol(SymbolTable *symbolTable);
SymbolTable* scopeSymbolTable(SymbolTable *parent);
SYMBOL* localSymbolTableCheck(SymbolTable *t, char* id);
void putVar(SYMBOL *s, SymbolTable *t, int lineno);
void putType(SYMBOL *s, SymbolTable *t, int lineno); 
void putFunc(SYMBOL *s, SymbolTable *t, int lineno); 
void redeclCheck(int lineno, char *id, SymbolTable *t);
SYMBOL* getSymbol(SymbolTable *t, char* id, int lineno);
void symPROGRAM(PROGRAM *s);
void symDECLARATION(DECLARATION *decl, SymbolTable *symbolTable, int tabs);
Node* symFUNC_decl(EXP *id, TYPE *t, SymbolTable *table, int tabs);
void symFUNC_param(SYMBOL* func, PARAM *param, SymbolTable *table, int tabs);
void symFUNC_mainAndInit(FUNCDECL *func);
void symFUNC(FUNCDECL *func, SymbolTable *s, int tabNum);
void printFuncParams(PARAM *params);
void printType(TYPE *type);
void printStruct(STRUCTFIELD *sf);
void symTYPE(TYPEDECL *type_decl, SymbolTable *s, int tabs);
void printSingleType(SYMBOL *sym);
void printStructSym(SYMBOL *sym);
void printSymType(SYMBOL *sym);
void makeParentType(TYPE *t, SYMBOL *type_sym, SymbolTable *table);
SYMBOL* symMakeParentTypeHelper(TYPE *t, SymbolTable *table);
void checkRecursiveType(char *name, TYPE *t);
void checkRecursiveStruct(char *name, TYPE* structType);
SYMBOL* symTYPE_struct(STRUCTFIELD *field_list, SYMBOL *struct_symbol, SymbolTable *table);
SYMBOL* lookUpType(TYPE *t, SymbolTable *table);
char* getTypeName(TYPE *t);
void checkStructId_List(SYMBOL *s, char *name, int lineno);
void symCheckType(SYMBOL *type_sym);
void symVAR(VARDECL *var, SymbolTable *s, int tabs);
void checkMainorInit(SymbolTable *table, char *name, int lineno);
void symVAR_decl(EXP *id, TYPE *t, SymbolTable *table, int tabs);
void symVAR_declExpr(VARDECL *var, SymbolTable *table, int tabs);
void symVAR_both(EXP *id, TYPE *t, EXP *expr, SymbolTable *table, int tabs);
void symSTMTS(STMT *s, SymbolTable* symbolTable, int tabNum);
void symCASE(CASE *c, SymbolTable *symbolTable, int tabNum);
SYMBOL* symEXP(EXP *e, SymbolTable *symbolTable);
void printVar(SYMBOL *sym, TYPE *t, int tabs, bool mode);
void printScopeIn(int tabNum);
void printScopeOut(int tabNum);
void symShortDecl(EXP* lhs, SymbolTable *symbolTable, int tabNum);
bool checkIsBaseType(TYPE *t);
void assignTableDepth(SYMBOL *s, SymbolTable *t);

CLASS *createNewClass(SYMBOL *sym);
char *addStructClass(SYMBOL *sym);
bool checkEqualStruct(SYMBOL *struct1, SYMBOL *struct2);


#endif
