#ifndef CODEGEN_H
#define CODEGEN_H

#include <string.h>
#include <stdio.h>
#include "tree.h"
#include "symbol.h"
#include "pretty.h"
#include "type.h"

extern Node* typeEXP(EXP *e);

void printTabs(FILE *f, int n);
void codePROGRAM(PROGRAM *program, char* file_name, FILE *fp);

/******* renaming vars *******/
char *renameVariable(SYMBOL *sym);
char *tmpVariable(SYMBOL *sym);
char *tmpVariable2();
char *blankVariable();

/******* typing *******/
char *varTypeName(SYMBOL *varSym);
char *varTypeValue(SYMBOL *varSym, char *type);
char *getExprType(EXP *e);

/******* var decl *******/
void codeDeclaration(DECLARATION *decl, int tabs, FILE *fp);
void codeVar(VARDECL *var, int tabs, FILE *fp);
void printVariableDecl(bool top_level, char *type, char *var_name, char *value, int tabs, FILE *f);
void printVariableDecl2(bool top_level, char *type, char *var_name, int tabs, FILE *f);
void codeVar_decl(EXP *id, EXP *expr, int tabs, FILE *fp);
void codeVar_expr(EXP *id, EXP *expr, int tabs, FILE *fp);

/******* slice *******/
void codePrintSliceClass(FILE *fp);
char *getSliceTypeName(SYMBOL *sym);
char *getSliceTypeInit(SYMBOL *sym, char* sliceClassName);
char *getSliceTypeNameHelper(SYMBOL *sym);
char *getSliceDimWithSize(SYMBOL *sym);
char *getSliceDim(char *dim);
char* getSliceSize(char *dim);
int getSliceDimNumber(char *dim);

/******* struct *******/
void codeBuildStructClass(PROGRAM *p, FILE *f, int tabs);
void printClassField(SYMBOL *sym, FILE *f, int tabs);
char *structInitValue(char *className);
void printEqualsHelper(SYMBOL *sym, FILE *f);
void printEqualsMethod(char *className, SYMBOL *sym, FILE *f, int tabs);
void printCloneMethod(char *className, SYMBOL *sym, FILE *f, int tabs);
void printCloneHelper(SYMBOL *sym, FILE *f, int tabs);


/******* array *******/
char *fillDefaultArray(SYMBOL *sym, char *varName, char *baseType, char *dim, int tabs, FILE *f); //string & object
void *printForLoop(char *type, char *dim, char *oldIndex, char *fillValue, int tabs, FILE *f);
char *getArrayTypeName(char *class);
char *getArrayTypeInit(SYMBOL *varSym, char *type);
char *getArrayDimWithSize(SYMBOL *sym);
char *getArrayDim(char *dim);
char *getArrayBaseType(SYMBOL *sym);
char* getArraySize(char *dim);
int getArrayDimNumber(char *dim);

/******* array slice access & built in *********/
char *getListTypeName(char *type);
int convertArraySliceAccess(EXP* e, int depth, char *index);
int convertArraySliceAssign(EXP* e, int depth, char* value, char *index);

/******* base type *******/
char *codeBaseType(SYMBOL *sym);
char *convertBaseType(char *typeString);
char *initValue(char *type);
bool checkIfBaseType(char *name);
char *convertPrimitive(char *typeString);

/******* function *******/
void codeMain(int tabs, FILE *f);
void codeFunc(FUNCDECL *func, int tabs, FILE *f);
void printParam(PARAM* param, FILE *f);

/******* statement & exp *******/
void codeSTMTS(FILE *f, STMT *s, int tabs);
void codeEXP(FILE *f, EXP *e);

/******* assignment & short decl *******/
void codeAssignOrShortDecl(EXP *id, EXP *expr, char *kind, int tabs, FILE *f, bool isAssign);
void printVarAssign(char *var_name, char *value, char* kind, int tabs, FILE *f);
void printLValueAssign(EXP *e, char *value, char* kind, int tabs, FILE *f);
void codeAssignLValue(char *type, EXP *id, EXP *expr, int tabs, FILE *f);
void codeAssignIden(char *type, EXP *id, EXP *expr, int tabs, FILE *f);


/******* switch *******/
//void convertSwitchToIf(CASE * caseClauses, int tabs, FILE *f);
void convertSwitchToIf(CASE * caseClauses, int tabs, FILE *f);
STMT* recurSTMT(STMT* start, STMT *target);
void codeCase(CASE * caseClauses, int tabs, FILE *f);
void switchCondBool(EXP *cond, CASE * caseClauses, int tabs, FILE *f);

/******* exp *******/
bool checkPrimitive(char *type);
bool checkString(char *type);
void codeBinary(EXP *lhs, char *opera, EXP *rhs, FILE *f);
void codePrintEscapeCharMethod(FILE *f);
#endif
