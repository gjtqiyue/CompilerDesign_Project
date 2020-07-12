#ifndef TYPECHECK_H
#define TYPECHECK_H

#include "miniTree.h"

void typePROG(PROG *p);
TYPE* typeEXP(EXP *e);
TYPE* compareArithmetic(TYPE* lhs, TYPE* rhs, int lineno);
TYPE* checkArithmetic(TYPE* type, int lineno);
TYPE* checkBinaryComparison(TYPE* lhs, TYPE* rhs, int lineno);
TYPE* checkAssign(TYPE* lhs, TYPE* rhs, int lineno);
TYPE* checkBOOL(TYPE* type, int lineno);
char* printType(TYPE* type);

#endif