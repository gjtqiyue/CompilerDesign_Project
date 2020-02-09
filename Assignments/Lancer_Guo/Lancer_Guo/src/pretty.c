#include "pretty.h"
#include "main.h"
#include <stdio.h>
#include <stdlib.h>

int prettySymbol = 0;

void prettyPROG(PROG *p, int tabs)
{
    if (p == NULL){
        return;
    }

    if (p->next != NULL){
        prettyPROG(p->next, tabs);
    }

    switch (p->kind){
        case k_StatementKindWhile:
            if (prettySymbol == 0){
                printTabs(tabs);
                printf("while");
                printf(" ( ");
                prettyEXP(p->val.while_stmt.cond);
                printf(" ) ");
                printf("{ \n");
                prettyPROG(p->val.while_stmt.body, tabs++);
                printTabs(tabs);
                printf("} \n");
            }
            else {
                printTabs(tabs);
                printf("{ \n");
                prettyPROG(p->val.while_stmt.body, tabs++);
                printTabs(tabs);
                printf("} \n");
            }
            break;
        case k_StatementKindIf:
            if (prettySymbol == 0){
                printTabs(tabs);
                printf("if");
                printf(" ( ");
                prettyEXP(p->val.if_stmt.cond);
                printf(" ) ");
                printf("{ \n");
                prettyPROG(p->val.if_stmt.body, tabs+1);
                printTabs(tabs);
                printf("} \n");
                //else part
                if (p->val.if_stmt.elseif_stmt != NULL) {
                    printTabs(tabs);
                    printf("else { \n");
                    prettyPROG(p->val.if_stmt.elseif_stmt, tabs);
                    printTabs(tabs);
                    printf("} \n");
                }
            }
            else{
                printTabs(tabs);
                printf("{ \n");
                prettyPROG(p->val.if_stmt.body, tabs+1);
                printTabs(tabs);
                printf("} \n");
                if (p->val.if_stmt.elseif_stmt != NULL) {
                    printTabs(tabs);
                    printf("{ \n");
                    prettyPROG(p->val.if_stmt.elseif_stmt, tabs);
                    printTabs(tabs);
                    printf("} \n");
                }
            }
            break;
        case k_StatementKindElse:
            prettyPROG(p->val.else_stmt.body, tabs+1);
            break;
        case k_StatementKindRead:
            if (prettySymbol == 0){
                printTabs(tabs);
                prettyEXP(p->val.read.input);
            }
            break;
        case k_StatementKindPrint:
            if (prettySymbol == 0){
                printTabs(tabs);
                printf("print");
                printf("(");
                prettyEXP(p->val.print.output);
                printf(");\n");
            }
            break;
        case k_StatementKindAssign:
            if (prettySymbol == 0){
                printTabs(tabs);
                prettyEXP(p->val.assign.name);
                printf(" = ");
                prettyEXP(p->val.assign.exp);
                printf(";\n");
            }
            break;
        case k_StatementKindDeclaration:
            if (prettySymbol == 0){
                printTabs(tabs);
                printf("var ");
                prettyEXP(p->val.decl.name);
                printf(" = ");
                prettyEXP(p->val.decl.exp);
                printf(";\n");
            }
            else{
                printTabs(tabs);
                prettyEXP(p->val.decl.name);
                printf("\n");
            }
            break;
        case k_StatementKindDeclarationType:
            if (prettySymbol == 0){
                printTabs(tabs);
                printf("var ");
                prettyEXP(p->val.decl_type.name);
                printf(" : ");
                prettyTYPE(p->val.decl_type.type);
                printf(" = ");
                prettyEXP(p->val.decl_type.exp);
                printf(";\n");
            }
            else{
                printTabs(tabs);
                prettyEXP(p->val.decl_type.name);
                printf("\n");
            }
            break;
    }
}

void prettyTYPE(TYPE *t)
{
    switch (t->kind){
        case k_TypeKindInt:
            printf("int");
            break;
        case k_TypeKindFloat:
            printf("float");
            break;
        case k_TypeKindString:
            printf("string");
            break;
        case k_TypeKindBool:
            printf("bool");
            break;
    }
}

void prettyEXP(EXP *e){
    switch (e->kind) {
        case k_ExpressionKindAdd:
            prettyEXP(e->val.binary.lhs);
            printf(" + ");
            prettyEXP(e->val.binary.rhs);
            break;
        case k_ExpressionKindMinus:
            prettyEXP(e->val.binary.lhs);
            printf(" - ");
            prettyEXP(e->val.binary.rhs);
            break;
        case k_ExpressionKindTimes:
            prettyEXP(e->val.binary.lhs);
            printf(" * ");
            prettyEXP(e->val.binary.rhs);
            break;
        case k_ExpressionKindDivide:
            prettyEXP(e->val.binary.lhs);
            printf(" / ");
            prettyEXP(e->val.binary.rhs);
            break;
        case k_ExpressionKindNot:
            printf("!");
            prettyEXP(e->val.unary.exp);
            break;
        case k_ExpressionKindUMinus:
            printf("-");
            prettyEXP(e->val.unary.exp);
            break;
        case k_ExpressionKindGEQ:
            prettyEXP(e->val.binary.lhs);
            printf(" >= ");
            prettyEXP(e->val.binary.rhs);
            break;
        case k_ExpressionKindLEQ:
            prettyEXP(e->val.binary.lhs);
            printf(" <= ");
            prettyEXP(e->val.binary.rhs);
            break;
        case k_ExpressionKindGreater:
            prettyEXP(e->val.binary.lhs);
            printf(" > ");
            prettyEXP(e->val.binary.rhs);
            break;
        case k_ExpressionKindSmaller:
            prettyEXP(e->val.binary.lhs);
            printf(" < ");
            prettyEXP(e->val.binary.rhs);
            break;
        case k_ExpressionKindEqual:
            prettyEXP(e->val.binary.lhs);
            printf(" == ");
            prettyEXP(e->val.binary.rhs);
            break;
        case k_ExpressionKindNotEqual:
            prettyEXP(e->val.binary.lhs);
            printf(" != ");
            prettyEXP(e->val.binary.rhs);
            break;
        case k_ExpressionKindAnd:
            prettyEXP(e->val.binary.lhs);
            printf(" && ");
            prettyEXP(e->val.binary.rhs);
            break;
        case k_ExpressionKindOr:
            prettyEXP(e->val.binary.lhs);
            printf(" || ");
            prettyEXP(e->val.binary.rhs);
            break;
        case k_ExpressionKindParentheses:
            printf("(");
            prettyEXP(e->val.paren.exp);
            printf(")");
            break;
        case k_TermKindIdentifier:
            if (prettySymbol == 0){
                printf("%s", e->val.identifier.identifier);
            }
            else{
                printf("%s: ", e->val.identifier.sym->name);
                if (e->val.identifier.sym->type == NULL){
                    printf("<Infer>");
                }
                else {
                    prettyTYPE(e->val.identifier.sym->type);
                }
            }
            break;
        case k_TermKindIntLiteral:
            printf("%i", e->val.intLiteral.intLiteral);
            break;
        case k_TermKindFloatLiteral:
            printf("%f", e->val.floatLiteral.floatLiteral);
            break;
        case k_TermKindStringLiteral:
            printf("%s", e->val.stringLiteral.stringLiteral);
            break;
        case k_TermKindBoolLiteral:
            if (e->val.boolLiteral.boolLiteral == 1) {
                printf("True");
            }
            else {
                printf("False");
            }
            break;
    }
}

void printTabs(int num){
    for (int i=0; i<num; i++){
        printf("\t");
    }
}