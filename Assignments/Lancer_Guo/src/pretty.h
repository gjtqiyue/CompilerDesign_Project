#ifndef PRETTY_H
#define PRETTY_H

#include "miniTree.h"

void prettyPROG(PROG *p, int tabs);
void prettyTYPE(TYPE *t);
void prettyEXP(EXP *e);
void prettyTERM(TERM *t);
void printTabs(int num);
//char* printType(TYPE *t);

#endif