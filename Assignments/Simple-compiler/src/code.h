#ifndef CODE_H
#define CODE_H

#include "miniTree.h" 

void codePROG(PROG *p, int tabs);
void codeEXP(EXP *e);
void codeTYPE(TYPE *t);
void codeStringCat(EXP *e, char *name, int tabs);
void codeTabs(int num);

#endif