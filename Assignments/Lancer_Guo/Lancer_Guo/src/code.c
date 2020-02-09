#include <stdio.h>
#include <stdlib.h>
#include "code.h"
#include "main.h"

FILE* f;

void codePROG(PROG *p, int tabs) {
    if (f == NULL) {
        printf("no file pointer\n");
    }

    if (p == NULL) {
        return;
    }

    if (p->next != NULL) {
        codePROG(p->next, tabs);
    }

    switch (p->kind) {
        case k_StatementKindWhile:
            codeTabs(tabs);
            fprintf(f, "%s", "while ( ");
            codeEXP(p->val.while_stmt.cond);
            fprintf(f, "%s", " ) \n");
            codePROG(p->val.while_stmt.body, tabs++);
            codeTabs(tabs);
            fprintf(f, "%s", "} \n");
            break;

        case k_StatementKindIf:
            codeTabs(tabs);
            fprintf(f, "%s","if");
            fprintf(f, "%s"," ( ");
            codeEXP(p->val.if_stmt.cond);
            fprintf(f, "%s"," ) ");
            fprintf(f, "%s","{ \n");
            codePROG(p->val.if_stmt.body, tabs+1);
            codeTabs(tabs);
            fprintf(f, "%s","} \n");
            //else part
            if (p->val.if_stmt.elseif_stmt != NULL) {
                codeTabs(tabs);
                fprintf(f, "%s","else { \n");
                codePROG(p->val.if_stmt.elseif_stmt, tabs);
                codeTabs(tabs);
                fprintf(f, "%s","} \n");
            }
            break;

        case k_StatementKindElse:
            codePROG(p->val.else_stmt.body, tabs+1);
            break;

        case k_StatementKindRead:
            codeTabs(tabs);
            char* readVar = p->val.read.input->val.identifier.identifier;
            switch(p->val.read.input->val.identifier.sym->type->kind) {
                case k_TypeKindInt:
                    fprintf(f, "%s%s%s","scanf(\"%d\", &", readVar, ");\n");    
                    break;
                case k_TypeKindFloat:
                    fprintf(f, "%s%s%s","scanf(\"%lf\", &", readVar, ");\n"); 
                    break;
                case k_TypeKindString:
                    fprintf(f, "%s%s", readVar, " = malloc(1024*sizeof(char));\n"); 
                    codeTabs(tabs);
                    fprintf(f, "%s%s%s","scanf(\"%s\", ", readVar, ");\n"); 
                    break;
                case k_TypeKindBool:
                    fprintf(f, "%s", "char LancerNiubi[1024];\n"); 
                    codeTabs(tabs);
                    fprintf(f, "%s", "scanf(\"%s\", LancerNiubi);\n"); 
                    codeTabs(tabs);
                    fprintf(f, "%s%s%s", "if (strcmp(LancerNiubi, \"true\") == 0) { ", readVar, " = true; }\n");
                    codeTabs(tabs);
                    fprintf(f, "%s%s%s", "if (strcmp(LancerNiubi, \"false\") == 0) { ", readVar, " = false; }\n");
                    break;
            }
            break;

        case k_StatementKindPrint:
            codeTabs(tabs);
            //char* nameLiteral = p->val.print.output->val.identifier.identifier;
            switch(p->val.print.output->type->kind) {
                case k_TypeKindInt:
                    fprintf(f, "%s","printf(\"%d \\n\", ");
                    codeEXP(p->val.print.output);
                    fprintf(f, "%s", ");\n");    
                    break;
                case k_TypeKindFloat:
                    fprintf(f, "%s","printf(\"%f \\n\", ");
                    codeEXP(p->val.print.output);
                    fprintf(f, "%s", ");\n"); 
                    break;
                case k_TypeKindString:
                    fprintf(f, "%s","printf(\"%s \\n\", ");
                    codeEXP(p->val.print.output);
                    fprintf(f, "%s", ");\n"); 
                    break;
                case k_TypeKindBool:
                    fprintf(f, "%s","printf(\"%s \\n\", ");
                    codeEXP(p->val.print.output);
                    fprintf(f, "%s", " ? \"true\" : \"false\");\n"); 
                    break;
            }
            break;

        case k_StatementKindAssign:
            codeTabs(tabs);
            codeEXP(p->val.assign.name);
            fprintf(f, "%s"," = ");
            codeEXP(p->val.assign.exp);
            fprintf(f, "%s",";\n");
            break;

        case k_StatementKindDeclaration:
            codeTabs(tabs);
            codeTYPE(p->val.decl.name->type);
            codeEXP(p->val.decl.name);
            fprintf(f, "%s", " = ");
            codeEXP(p->val.decl.exp);
            fprintf(f, "%s", ";\n");
            break;

        case k_StatementKindDeclarationType:
            codeTabs(tabs);
            codeTYPE(p->val.decl_type.type);
            codeEXP(p->val.decl_type.name);
            fprintf(f, "%s", " = ");
            codeEXP(p->val.decl_type.exp);
            fprintf(f, "%s", ";\n");
            break;
    }
}


void codeTYPE(TYPE *t)
{
    switch (t->kind){
        case k_TypeKindInt:
            fprintf(f, "%s","int ");
            break;
        case k_TypeKindFloat:
            fprintf(f, "%s","float ");
            break;
        case k_TypeKindString:
            fprintf(f, "%s","char* ");
            break;
        case k_TypeKindBool:
            fprintf(f, "%s","bool ");
            break;
    }
}

void codeEXP(EXP *e){
    if (e == NULL) {
        printf("null\n");
        return;
    }
    switch (e->kind) {
        case k_ExpressionKindAdd:
            codeEXP(e->val.binary.lhs);
            fprintf(f, "%s"," + ");
            codeEXP(e->val.binary.rhs);
            break;
        case k_ExpressionKindMinus:
            codeEXP(e->val.binary.lhs);
            fprintf(f, "%s"," - ");
            codeEXP(e->val.binary.rhs);
            break;
        case k_ExpressionKindTimes:
            codeEXP(e->val.binary.lhs);
            fprintf(f, "%s"," * ");
            codeEXP(e->val.binary.rhs);
            break;
        case k_ExpressionKindDivide:
            codeEXP(e->val.binary.lhs);
            fprintf(f, "%s"," / ");
            codeEXP(e->val.binary.rhs);
            break;
        case k_ExpressionKindNot:
            fprintf(f, "%s","( !");
            codeEXP(e->val.unary.exp);
            fprintf(f, "%s"," )");
            break;
        case k_ExpressionKindUMinus:
            fprintf(f, "%s","( -");
            codeEXP(e->val.unary.exp);
            fprintf(f, "%s"," )");
            break;
        case k_ExpressionKindGEQ:
            fprintf(f, "%s","( ");
            codeEXP(e->val.binary.lhs);
            fprintf(f, "%s"," >= ");
            codeEXP(e->val.binary.rhs);
            fprintf(f, "%s"," )");
            break;
        case k_ExpressionKindLEQ:
            fprintf(f, "%s","( ");
            codeEXP(e->val.binary.lhs);
            fprintf(f, "%s"," <= ");
            codeEXP(e->val.binary.rhs);
            fprintf(f, "%s"," )");
            break;
        case k_ExpressionKindGreater:
            fprintf(f, "%s","( ");
            codeEXP(e->val.binary.lhs);
            fprintf(f, "%s"," > ");
            codeEXP(e->val.binary.rhs);
            fprintf(f, "%s"," )");
            break;
        case k_ExpressionKindSmaller:
            fprintf(f, "%s","( ");
            codeEXP(e->val.binary.lhs);
            fprintf(f, "%s"," < ");
            codeEXP(e->val.binary.rhs);
            fprintf(f, "%s"," )");
            break;
        case k_ExpressionKindEqual:
            fprintf(f, "%s","( ");
            codeEXP(e->val.binary.lhs);
            fprintf(f, "%s"," == ");
            codeEXP(e->val.binary.rhs);
            fprintf(f, "%s"," )");
            break;
        case k_ExpressionKindNotEqual:
            fprintf(f, "%s","( ");
            codeEXP(e->val.binary.lhs);
            fprintf(f, "%s"," != ");
            codeEXP(e->val.binary.rhs);
            fprintf(f, "%s"," )");
            break;
        case k_ExpressionKindAnd:
            fprintf(f, "%s","( ");
            codeEXP(e->val.binary.lhs);
            fprintf(f, "%s"," && ");
            codeEXP(e->val.binary.rhs);
            fprintf(f, "%s"," )");
            break;
        case k_ExpressionKindOr:
            codeEXP(e->val.binary.lhs);
            fprintf(f, "%s"," || ");
            codeEXP(e->val.binary.rhs);
            break;
        case k_ExpressionKindParentheses:
            fprintf(f, "%s","(");
            codeEXP(e->val.paren.exp);
            fprintf(f, "%s",")");
            break;
        case k_TermKindIdentifier:
            fprintf(f, "%s", e->val.identifier.identifier);
            break;
        case k_TermKindIntLiteral:
            
            fprintf(f, "%i", e->val.intLiteral.intLiteral);
            break;
        case k_TermKindFloatLiteral:
            fprintf(f, "%f", e->val.floatLiteral.floatLiteral);
            break;
        case k_TermKindStringLiteral:
            fprintf(f, "%s", e->val.stringLiteral.stringLiteral);
            break;
        case k_TermKindBoolLiteral:
            if (e->val.boolLiteral.boolLiteral == 1) {
                fprintf(f, "%s","true");
            }
            else {
                fprintf(f, "%s","false");
            }
            break;
    }
}

void codeTabs(int num){
    for (int i=0; i<num; i++){
        fprintf(f, "%s", "\t");
    }
}