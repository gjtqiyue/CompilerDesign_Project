#ifndef PRETTY_H
#define PRETTY_H

#include "tree.h"

void printTab();
void pretty_id(EXP *id);
void pretty_type(TYPE *type);

void pretty_struct_field(STRUCTFIELD *sf);

void pretty_Signature(FUNCDECL *decl);
void pretty_Param_list(PARAM *list);
void pretty_Parameter_Unit(PARAM *unit);
void pretty_Identifier_list(EXP *list);
void prettyProgram(PROGRAM *program);
void pretty_Top_Declaration(DECLARATION *decl);
void pretty_Var_Declaration_List(VARDECL *list);
void pretty_Type_Declaration_List(TYPEDECL *list);


void pretty_Type_Decl(TYPEDECL *decl);
void pretty_Var_Decl(VARDECL *d);
void pretty_Funtion_Decl(FUNCDECL *decl);
void prettyStatement(STMT *stmt);
void pretty_For_STMT(STMT *for_stmt);
void pretty_While_STMT(STMT *while_stmt);
void pretty_If_STMT(STMT *if_stmt);
void pretty_Else_If_STMT(STMT *curr);
void pretty_Else_STMT(STMT *curr);
void pretty_Println_STMT(STMT *curr);
void pretty_Print_STMT(STMT *curr);
void pretty_Return_STMT(STMT *curr);
void pretty_Switch_On(STMT *stmt);
void pretty_Switch_Case(CASE *c);
void prettyFor(STMT *for_stmt);
void pretty_Simple_STMT(STMT *simple);
void pretty_Assign_STMT(STMT *simple);
void pretty_Exp_STMT(STMT *simple);
void pretty_Short_STMT(STMT *simple);
void pretty_Inc_STMT(STMT *simple);
void pretty_Dec_STMT(STMT *simple);
void prettyExpression_List(EXP *exp_list);
void prettyExpression(EXP *e);
void prettyPrimary_Expression(EXP *e);
void prettyUnary(EXP *e);
void prettyBinary(EXP *e);
void prettyAppend(EXP *e);
void prettyLen(EXP *e);
void prettyCap(EXP *e);
void prettyFuncCall(EXP *e);
void prettyField(EXP *e);
void prettyArrayIndex(EXP *e);

#endif
