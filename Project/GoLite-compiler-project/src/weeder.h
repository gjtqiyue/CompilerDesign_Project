#ifndef WEED_H
#define WEED_H

#include "tree.h"

void weedPROGRAM(PROGRAM *prog);
void weedDECLARATION(DECLARATION *decls);
void weedFUNCDECL(FUNCDECL *func);
void weedPARAM(PARAM *params);
void weedVARDECL(VARDECL *vardecls);
void weedTYPEDECL(TYPEDECL *typedecls);
void weedTYPE(TYPE *type);
void weedTYPE_Struct(STRUCTFIELD *fields);

void weedSTMTS (STMT *stmts);
void weedSTMT_Exp(EXP *expr);
void weedSTMT_Assign(STMT *assign);
void weedSTMT_Return(EXP *expr, STMT *stmt);
void weedCond(EXP *cond);
void weedSTMT_ForThird(STMT *third);
void weedSTMT_SwitchCase(CASE *cases);
void weedSTMT_ForBody(STMT *stmts);
void weedSTMT_CaseBody(STMT *stmts);
void weedIdList(EXP *list);

void weedEXP_Primary(EXP *expr);
int weedEXP(EXP *exprs);
void weedEXP_FuncArgs(EXP *args);
void weedEXP_Blank(EXP *expr);
void weedEXP_Array(EXP *expr);
void weedEXP_Target(EXP *expr);
void weedEXP_Field(EXP *expr);
void weedEXP_ArrayIndex(EXP *expr);

void checkBalance(EXP *lhs, EXP *rhs, int lineno);

//terminating statement
bool weedTerminatingSTMT(STMT *stmt);
bool hasNoBreakSTMT(STMT *stmt);

#endif
