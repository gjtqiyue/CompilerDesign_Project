#ifndef TYPECHECK_H
#define TYPECHECK_H

#include "miniTree.h"

void typePROG(PROG *p);
TYPE* typeEXP(EXP *e);
TYPE* compareArithmetic(TYPE* lhs, TYPE* rhs);
TYPE* checkArithmetic(TYPE* type);
TYPE* checkBinaryComparison(TYPE* lhs, TYPE* rhs);
TYPE* checkAssign(TYPE* lhs, TYPE* rhs);
TYPE* checkBOOL(TYPE* type);
char* printType(TYPE* type);

#endif