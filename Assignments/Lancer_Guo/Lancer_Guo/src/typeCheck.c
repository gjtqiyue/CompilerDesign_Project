#include "typeCheck.h"
#include <stdlib.h>
#include <stdio.h>
#include "string.h"


TYPE* typeEXP(EXP *e) {
    TYPE *t, *lhs, *rhs;
    switch (e->kind) {
        case k_TermKindIdentifier:
            e->type = e->val.identifier.sym->type;
            return e->val.identifier.sym->type;

        case k_TermKindIntLiteral:
            t = malloc(sizeof(TYPE));
            t->kind = k_TypeKindInt;
            t->lineno = e->lineno;
            e->type = t;
            return t;

        case k_TermKindFloatLiteral:
            t = malloc(sizeof(TYPE));
            t->kind = k_TypeKindFloat;
            t->lineno = e->lineno;
             e->type = t;
            return t;

        case k_TermKindStringLiteral:
            t = malloc(sizeof(TYPE));
            t->kind = k_TypeKindString;
            t->lineno = e->lineno;
             e->type = t;
            return t;

        case k_TermKindBoolLiteral:
            t = malloc(sizeof(TYPE));
            t->kind = k_TypeKindBool;
            t->lineno = e->lineno;
            e->type = t;
            return t;

        case k_ExpressionKindAdd:
        case k_ExpressionKindMinus:
        case k_ExpressionKindTimes:
        case k_ExpressionKindDivide:
            lhs = typeEXP(e->val.binary.lhs);
            rhs = typeEXP(e->val.binary.rhs);
            TYPE* res = compareArithmetic(lhs, rhs);
            if (res != NULL) {
                e->type = res;
                return e->type;
            }
            else if (res == NULL) {
                if (lhs->kind == rhs->kind && lhs->kind == k_TypeKindString) {
                    e->type = lhs;
                    return e->type;
                }
            }
            else {
                return NULL;
            }

        case k_ExpressionKindAnd:
        case k_ExpressionKindOr:
            lhs = typeEXP(e->val.binary.lhs);
            rhs = typeEXP(e->val.binary.rhs);
            checkBOOL(lhs);
            checkBOOL(rhs);
            if (lhs->kind == rhs->kind) {
                e->type = lhs;
                return e->type;
            }
            else{
                return NULL;
            }

        case k_ExpressionKindNot:
            t = typeEXP(e->val.unary.exp);
            checkBOOL(t);
            e->type = t;
            return e->type;
            
        case k_ExpressionKindParentheses:
            e->type = typeEXP(e->val.paren.exp);
            return e->type;

        case k_ExpressionKindGEQ:
        case k_ExpressionKindLEQ:
        case k_ExpressionKindGreater:
        case k_ExpressionKindSmaller:
        case k_ExpressionKindEqual:
        case k_ExpressionKindNotEqual:
            lhs = typeEXP(e->val.binary.lhs);
            rhs = typeEXP(e->val.binary.rhs);
            e->type = checkBinaryComparison(lhs, rhs);    
            //return boolean
            return e->type;

        case k_ExpressionKindUMinus:
            e->type = typeEXP(e->val.unary.exp);
            checkArithmetic(e->type);
            return e->type;
    }
}

void typePROG(PROG *p) {
    if (p == NULL){
        return;
    }

    if (p->next != NULL){
        typePROG(p->next);
    }

    TYPE *type, *lhs, *rhs;
    switch (p->kind)
    {
    case k_StatementKindWhile:
        checkBOOL(typeEXP(p->val.while_stmt.cond));
        typePROG(p->val.while_stmt.body);
        break;
    case k_StatementKindIf:
        checkBOOL(typeEXP(p->val.if_stmt.cond));
        typePROG(p->val.if_stmt.body);
        if (p->val.if_stmt.elseif_stmt != NULL) {
            typePROG(p->val.if_stmt.elseif_stmt);
        }
        break;
    case k_StatementKindElse:
        typePROG(p->val.else_stmt.body);
        break;
    case k_StatementKindRead:
        //check expression
        typeEXP(p->val.read.input);
        break;
    case k_StatementKindPrint:
        typeEXP(p->val.print.output);
        break;
    case k_StatementKindAssign:
        //check expression
        lhs = typeEXP(p->val.assign.name);
        rhs = typeEXP(p->val.assign.exp);  
        checkAssign(lhs, rhs);
        break;
    case k_StatementKindDeclaration:
        //get the type of the right hand side
        type = typeEXP(p->val.decl.exp);
        //update the symbol stored in the AST
        p->val.decl.name->val.identifier.sym->type = type;
        p->val.decl.name->type = type;
        break;
    case k_StatementKindDeclarationType:
        type = typeEXP(p->val.decl_type.exp);
        //update the symbol stored in the AST
        checkAssign(p->val.decl_type.name->val.identifier.sym->type, type);
        break;
    }
}

TYPE* checkBOOL(TYPE *type) {
    if (type->kind == k_TypeKindBool){
        return type;
    }
    else {
        fprintf(stderr, "Error: line(%d) the expression is [%s], expect [BOOL]\n", type->lineno, printType(type));
        exit(1);
    }
    return NULL;
}

TYPE* checkAssign(TYPE *lhs, TYPE *rhs) {
    if (lhs->kind != rhs->kind) {
        fprintf(stderr, "Error: line(%d) the assignment has different types [%s, %s]\n", lhs->lineno, printType(lhs), printType(rhs));
        exit(1);
    }
    return lhs;
}

TYPE* compareArithmetic(TYPE *lhs, TYPE *rhs) {
    if (lhs->kind == rhs->kind && (lhs->kind == k_TypeKindInt || lhs->kind == k_TypeKindFloat)) {
        //return int type
        return lhs;
    }
    else if (lhs->kind == k_TypeKindFloat || rhs->kind == k_TypeKindFloat){
        //return float type
        TYPE* res = malloc(sizeof(TYPE));
        res->kind = k_TypeKindFloat;
        res->lineno = lhs->lineno;
        return res;
    }
    else{
        fprintf(stderr, "Error: line(%d) variable types in the expression are not compatibale [%s, %s]\n", lhs->lineno, printType(lhs), printType(rhs));
        exit(1);
    }
    return NULL;
}

TYPE* checkArithmetic(TYPE *type) {
    if (type->kind != k_TypeKindFloat || type->kind != k_TypeKindInt) {
        fprintf(stderr, "Error: line(%d) the expression has type [%s], expect [INT] or [FLOAT]\n", type->lineno, printType(type));
        exit(1);
    }
    return type;
}

TYPE* checkBinaryComparison(TYPE *lhs, TYPE *rhs){
    TYPE* res = malloc(sizeof(TYPE));
    res->kind = k_TypeKindBool;
    res->lineno = lhs->lineno;
    if (lhs->kind == rhs->kind) {
        return res;
    }
    else if ((lhs->kind == k_TypeKindFloat ||
              lhs->kind == k_TypeKindInt) &&
             (rhs->kind == k_TypeKindFloat ||
              rhs->kind == k_TypeKindInt))
        return res;
    else {
        fprintf(stderr, "Error: line(%d) the comparison expression has different types [%s, %s]\n", lhs->lineno, printType(lhs), printType(rhs));
        exit(1);
    }
    return NULL;
}

char* printType(TYPE* type) {
    switch (type->kind) {
        case k_TypeKindInt:
            return "INT";
        case k_TypeKindFloat:
            return "FLOAT";
        case k_TypeKindString:
            return "STRING";
        case k_TypeKindBool:
            return "BOOL";
    }
    return NULL;
}
