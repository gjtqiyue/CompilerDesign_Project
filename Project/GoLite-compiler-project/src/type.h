#ifndef TYPECHECK_H
#define TYPECHECK_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tree.h"
#include "symbol.h"

typedef struct Node Node;

// struct Node {
//     SYMBOL* val;
//     Node* next;
// };

void typePROGRAM(PROGRAM* prog);
void typeDECLARATION(DECLARATION* decl);
void typeFUNCDECL(FUNCDECL* func);
void typeVARDECL(VARDECL* var);
void typeSTMTS(STMT *s);
void typeCase(CASE* cluase, Node* type);
Node* typeEXP(EXP *e);

SYMBOL* searchStructFields(SYMBOL* sym, char* id);
char* getTypePrintName(SYMBOL* type);
char* getVarType(SYMBOL* type, int lineno);
SYMBOL* getSliceArrayBaseType(SYMBOL* t);
int getSliceArrayBaseDim(SYMBOL* t);
SYMBOL* getArraySliceType(SYMBOL* sym);
int checkLValues(EXP* t);
int compareTypeList(Node* t1, Node* t2, int lineno);
void checkIndex(SYMBOL* t, int lineno);
SYMBOL* checkBOOL(SYMBOL *type, int lineno);
SYMBOL* checkINT(SYMBOL *type, int lineno);
SYMBOL* checkArithmetic(SYMBOL *type, int lineno);
SYMBOL* checkArithmeticOrStringBinary(SYMBOL *t1, SYMBOL *t2, int lineno);
SYMBOL* checkComparableBinary(SYMBOL* lhs, SYMBOL*rhs, int lineno);
SYMBOL* checkOrderedBinary(SYMBOL* lhs, SYMBOL*rhs, int lineno, char* opera);
SYMBOL* checkArithmeticBinary(SYMBOL* lhs, SYMBOL*rhs, int lineno);
SYMBOL* checkBitWiseBinary(SYMBOL* lhs, SYMBOL*rhs, int lineno);
SYMBOL* checkArithmeticOrString(SYMBOL *type, int lineno);
SYMBOL* checkEqual(SYMBOL* s1, SYMBOL* s2, int lineno);
SYMBOL* checkBitWise(SYMBOL* type, int lineno);
void updateType(SYMBOL* t1, SYMBOL* t2);
int checkParentType(SYMBOL* expr, SYMBOL* t, int lineno);
Node* makeParamList(Node* sym);
int checkBaseType(SYMBOL* s);
SYMBOL* appendHelper(SYMBOL* sym);
#endif