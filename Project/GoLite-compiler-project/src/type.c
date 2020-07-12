#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "type.h"

int dimCounter = 0;
int originalDim = 0;
int isIndexAccess = 0;

void typePROGRAM(PROGRAM* prog) {
    typeDECLARATION(prog->decl);
}

void typeDECLARATION(DECLARATION* decl) {
    if (decl == NULL) return;
    if (decl->next != NULL) {
        typeDECLARATION(decl->next);
    }

    switch (decl->kind) {
        case k_DeclarationKindFunction:
            typeFUNCDECL(decl->val.func_decl);
            break;
        case k_DeclarationKindVar:
            typeVARDECL(decl->val.var_decl);
            break;
        case k_DeclarationKindType:
            break;
    }
}

void typeFUNCDECL(FUNCDECL* func) {
    typeSTMTS(func->body);
}

void typeVARDECL(VARDECL* var) {
    if (var == NULL) return;
    
    if (var->next != NULL) {
        typeVARDECL(var->next);
    }
    ////printf("reach var decl\n");
    Node *rhs, *lhs;
    switch (var->kind) {
        case k_VarDeclKindType:
            //all variables should be in the symbol table now, so we do nothing here
            break;
        case k_VarDeclKindExpr:
            //lhs is inferred type, so we need to evaulate rhs and update the symbol variable
            lhs = typeEXP(var->val.exp_only.id_list);     //return list of type symbol, might be empty
            rhs = typeEXP(var->val.exp_only.expr_list);   //return list of type symbol
            while (lhs != NULL) {
                if (rhs->val->kind == nullSymKind) {
                    fprintf(stderr, "Error: (line %d) cannot assign void type to variable.\n", var->lineno);
                    exit(1);
                }
                if (lhs->val->kind == inferSymKind && rhs != NULL) {
                    ////printf("update symbol to [%s]\n", getTypePrintName(rhs->val));
                    updateType(lhs->val, rhs->val);
                    ////printf("%d\n", (var->val.exp_only.id_list)->val.identifier.sym->val.varType->val.parentType->isBaseType);
                }
                else if (strcmp(lhs->val->name, "_") == 0 && rhs != NULL) {
                    updateType(lhs->val, rhs->val);
                }
                else {
                    fprintf(stderr, "Error: (line %d) var declaration without type has either lhs type assigned or rhs type void.\n", var->lineno);
                    exit(1);
                }
                lhs = lhs->next;
                rhs = rhs->next;
            }
            break;
        case k_VarDeclKindBoth:
            // we compare lhs and rhs
            // both lhs and rhs are list of typekind Symbol
            //////printf("kind is %d\n", var->val.exp_type.expr_list->kind);
            rhs = typeEXP(var->val.exp_type.expr_list);
            lhs = typeEXP(var->val.exp_type.id_list);
            if(strcmp(lhs->val->name, "_") == 0 && rhs != NULL) {
                     updateType(lhs->val, rhs->val);
             }
            while (lhs != NULL && rhs != NULL) {
                if (lhs->val->kind == nullSymKind) {

                }
                else if (lhs != NULL && rhs != NULL) {
                    //compare type
                    checkEqual(lhs->val, rhs->val, var->lineno);
                }
                else {
                    fprintf(stderr, "Error: (line %d) var declaration with type has either lhs type or rhs type empty", var->lineno);
                    exit(1);
                }
                lhs = lhs->next;
                rhs = rhs->next;
            }
            break;
    }
}

void typeSTMTS(STMT *s) {
    if (s == NULL){
        return;
    }

    if (s->next != NULL){
        typeSTMTS(s->next);
    }
    
    Node *lhs, *rhs, *returnType;
    SYMBOL *type;
    switch (s->kind)
    {
        case k_StatementKindEmpty:
        case k_StatementKindContinue:
        case k_StatementKindBreak:
            break;
        case k_StatementKindBlock:
            typeSTMTS(s->val.block_stmt);
            break;
        case k_StatementKindVarDecl:
            ////printf("reach inner scope var decl\n");
            typeVARDECL(s->val.var_decl);
            break;
        case k_StatementKindExp:
            ////printf("reach exp stmt\n");
            type = s->val.exp_stmt.expr->val.func_call.name->val.identifier.sym;
            if (type->kind != funcSymKind) {
                fprintf(stderr, "Error: (line %d) expression statement can only be function call.\n", s->lineno);
                exit(1);
            }
            typeEXP(s->val.exp_stmt.expr);
            break;
        case k_StatementKindTypeDecl:
            //do nothing because everything is handled in symbol
            break;
        case k_StatementKindReturn:
            
            type = s->val.return_stmt.func_ref->sym->val.func.returnSymRef;
            
            if (s->val.return_stmt.expr != NULL) {
                
                if (type == NULL) {
                    fprintf(stderr, "Error: (line %d) The function has return type of void.\n", s->lineno);
                    exit(1);
                }
                
                Node* n = typeEXP(s->val.return_stmt.expr);
                ////printf("hello\n");
                checkEqual(n->val, type, s->lineno);
            }
            else {
                
                if (type != NULL) {
                    fprintf(stderr, "Error: (line %d) The function's return expression is not privided.\n", s->lineno);
                    exit(1);
                }
                ////printf("reach return %d\n", s->val.return_stmt.expr != NULL);
            }
            
            break;
        case k_StatementKindShortDecl:

            lhs = typeEXP(s->val.short_decl.id_list);
            rhs = typeEXP(s->val.short_decl.expr_list);
            //go through the list of varType
            while (lhs != NULL) {
                // rhs cannot be void
                if (rhs->val->kind == nullSymKind) {
                    fprintf(stderr, "Error: (line %d) void cannot be used as a value in short declaration", s->lineno);
                    exit(1);
                }
                // lhs
                if (lhs->val->kind == nullSymKind) {
                    ////printf("reach blank\n");
                    updateType(lhs->val, rhs->val);
                }
                else if (lhs->val->kind == inferSymKind) {
                    ////printf("inferred type %d\n", rhs->val->tableDepth);
                    //inferred type, update the new symbol data
                    updateType(lhs->val, rhs->val);
                }
                else {
                    //compare type
                    checkEqual(lhs->val, rhs->val, s->lineno);
                }
                lhs = lhs->next;
                rhs = rhs->next;
            }
            break;
        case k_StatementKindAssign:
            // lhs must be all Lvalue
            checkLValues(s->val.assign_stmt.lhs);
            ////printf("reach assign\n");
            lhs = typeEXP(s->val.assign_stmt.lhs);
            
            rhs = typeEXP(s->val.assign_stmt.rhs);
            //printf("%s\n", rhs->val->name);
            if (strcmp(s->val.assign_stmt.kind, "=") == 0) {
                //normal assignment
                ////printf("compare type\n");
                compareTypeList(lhs, rhs, s->lineno);
            }
            else {
                //special kind
                if (lhs->next != NULL || rhs->next != NULL) {
                    fprintf(stderr, "Error: (%d) operation assignment can only have 1 expression on both sides", s->lineno);
                }

                if (strcmp(s->val.assign_stmt.kind, "-=") == 0 ||
                    strcmp(s->val.assign_stmt.kind, "*=") == 0 ||
                    strcmp(s->val.assign_stmt.kind, "/=") == 0) {
                    checkArithmeticBinary(lhs->val, rhs->val, s->lineno);                    
                    compareTypeList(lhs, rhs, s->lineno);
                }
                else if (strcmp(s->val.assign_stmt.kind, "+=") == 0) {
                    checkArithmeticOrStringBinary(lhs->val, rhs->val, s->lineno);                    
                    compareTypeList(lhs, rhs, s->lineno);
                }
                else {
                    checkBitWiseBinary(lhs->val, rhs->val, s->lineno);
                    compareTypeList(lhs, rhs, s->lineno);
                }
            }
            break;
        case k_StatementKindPrint:
            lhs = typeEXP(s->val.print_stmt.expr_list);
            ////printf("%d\n", lhs == NULL);
            while (lhs != NULL) {
                if (lhs->val->kind != typeSymKind) {
                    fprintf(stderr, "Error: line(%d) Print statument contains incompatiable type [%s].\n", s->lineno, getTypePrintName(lhs->val));
                    exit(1);
                }
                lhs = lhs->next;
            }
            break;
        case k_StatementKindPrintln:
            lhs = typeEXP(s->val.println_stmt.expr_list);
            ////printf("%d\n", lhs->val == NULL);
            while (lhs != NULL) {
                if (lhs->val->kind != typeSymKind) {
                    fprintf(stderr, "Error: line(%d) Print exptects base type [received %s].\n", s->lineno, getTypePrintName(lhs->val));
                    exit(1);
                }
                lhs = lhs->next;
            }
            break;
        case k_StatementKindWhile:
            if (s->val.while_stmt.cond != NULL) {
                type = typeEXP(s->val.while_stmt.cond)->val;
                checkBOOL(type, s->lineno);
            }
            typeSTMTS(s->val.while_stmt.body);
            break;
        case k_StatementKindFor:
            typeSTMTS(s->val.for_stmt.first);
            returnType = typeEXP(s->val.for_stmt.second);
            if (returnType != NULL) checkBOOL(returnType->val, s->lineno);
            typeSTMTS(s->val.for_stmt.third);
            typeSTMTS(s->val.for_stmt.body);
            break;
        case k_StatementKindIf:
            typeSTMTS(s->val.if_stmt.opt_cond); 
            returnType = typeEXP(s->val.if_stmt.cond);
            if (returnType != NULL) checkBOOL(returnType->val, s->lineno);
            typeSTMTS(s->val.if_stmt.body);
            if (s->val.if_stmt.elseif_part != NULL) {
                typeSTMTS(s->val.if_stmt.elseif_part);
            }
            break;
        case k_StatementKindElseIf:
            typeSTMTS(s->val.elseif_stmt.opt_cond);
            returnType = typeEXP(s->val.elseif_stmt.cond);
            if (returnType != NULL) checkBOOL(returnType->val, s->lineno);
            typeSTMTS(s->val.elseif_stmt.body);
            if (s->val.elseif_stmt.elseif_part != NULL) {
                typeSTMTS(s->val.if_stmt.elseif_part);
            }
            break;
        case k_StatementKindElse:
            typeSTMTS(s->val.else_stmt.body);
            break;
        case k_StatementKindSwitch:
            typeSTMTS(s->val.switch_stmt.opt_cond);
            Node* res = typeEXP(s->val.switch_stmt.cond);
            
            if (res != NULL && strcmp(res->val->name, "void") == 0) {
                fprintf(stderr, "Error: (line %d) switch statement's condition cannot be type void.\n", s->val.switch_stmt.cond->lineno);
                exit(1);
            }

            ////printf("reach here\n");
            typeCase(s->val.switch_stmt.caseClauses, res);
            break;
            // inc and dec can only be int, float64 or rune
        case k_StatementKindInc:
            checkArithmetic(typeEXP(s->val.inc_stmt.expr)->val, s->lineno);
            checkLValues(s->val.inc_stmt.expr);
            break;
        case k_StatementKindDec:
            checkArithmetic(typeEXP(s->val.dec_stmt.expr)->val, s->lineno);
            checkLValues(s->val.dec_stmt.expr);
            break;
    }

}

void typeCase(CASE* clause, Node* type) {
    if (clause == NULL) return;
    if (clause->next != NULL) {
        typeCase(clause->next, type);
    }
    
    switch (clause->kind) {
        case k_SwitchKindCase:
            if (type == NULL) {
                Node *return_list = typeEXP(clause->val.case_exp.expr_list);
                while (return_list != NULL) {
                    if (strcmp(getTypePrintName(return_list->val), "bool") != 0) {
                        fprintf(stderr, "Error: (line %d) switch statement expresion type is incompatible with case type [%s != bool].\n", clause->lineno, getTypePrintName(return_list->val));
                        exit(1);
                    }
                    return_list = return_list->next;
                }
            }
            else if (type->val->kind == typeSymKind) {
                Node *return_list = typeEXP(clause->val.case_exp.expr_list);
                while (return_list != NULL) {
                    checkEqual(type->val, return_list->val, clause->lineno);
                    return_list = return_list->next;
                }
            }
            else {
                fprintf(stderr, "Error: (line %d) switch statement expresion has to be a comparable type\n", clause->lineno);
                exit(1);
            }
            typeSTMTS(clause->val.case_exp.body);
            break;
        case k_SwitchKindDefault:
            typeSTMTS(clause->val.default_exp.body);
            break;
    }
    
}

// it returns a variable symbol
Node* typeEXP(EXP *e) {
    if (e == NULL) {
        return NULL;
    }

    ////printf("enter typeEXP %d\n", e->kind);
    SYMBOL *resType;    
    SYMBOL *t, *expr, *lhs, *rhs;
    switch (e->kind) {
        case k_ExpressionKindIdentifier:
            ////printf("reach identifier %s\n", e->val.identifier.name);
            ////printf("here %d\n", (e->val.identifier.sym == NULL));
            if (strcmp(e->val.identifier.name, "_") == 0) {
                //resType = makeSymbol("_", nullSymKind);
                resType = e->val.identifier.sym->val.varType;
                break;
            }
            else if (strcmp(e->val.identifier.name, "true") == 0 || strcmp(e->val.identifier.name, "false") == 0){
                if(e->val.identifier.sym == NULL){
                    //real true value of true/false
                    t = malloc(sizeof(SYMBOL));
                    t->name = "bool";
                    t->kind = typeSymKind;
                    t->lineno = e->lineno;
                    t->isBaseType = 1;
                    t->tableDepth = 0;
                    resType = t;
                }else{
                    //redefined value of true/false
                    resType = e->val.identifier.sym->val.varType;
                }
            }
            else if (e->val.identifier.sym->kind == varSymKind) {
                
                ////printf("here %s\n",e->val.identifier.sym->val.varType->name);
                resType = e->val.identifier.sym->val.varType;
            }
            else if (e->val.identifier.sym->kind == sliceSymKind ||                                
                     e->val.identifier.sym->kind == arraySymKind) {
                resType = e->val.identifier.sym->val.varType;
                ////printf("slice array %s\n", (e->val.identifier.sym->val.varType->name));
            }
            else {
                fprintf(stderr, "Error: (line %d) {%s} cannot be used as value.\n", e->lineno, e->val.identifier.name);
                exit(1);
            }
            // else if (e->val.identifier.sym->kind == funcSymKind ||                                            
            //          e->val.identifier.sym->kind == typeSymKind ||
            //          e->val.identifier.sym->kind == structSymKind) 
            // {
            //     ////printf("typeEXP detect function / type sym kind\n");
            //     resType = e->val.identifier.sym;
            // }
            
            break;
        case k_ExpressionKindIntLiteral:
            ////printf("reach int\n");
            t = malloc(sizeof(SYMBOL));
            t->name = "int";
            t->kind = typeSymKind;
            t->lineno = e->lineno;
            t->isBaseType = 1;
            t->tableDepth = 0;
            resType = t;
            break;
        case k_ExpressionKindFloatLiteral:
            ////printf("reach float\n");
            t = malloc(sizeof(SYMBOL));
            t->name = "float64";
            t->kind = typeSymKind;
            t->lineno = e->lineno;
            t->isBaseType = 1;
            t->tableDepth = 0;
            resType = t;
            break;
        case k_ExpressionKindInterStringLiteral:
            ////printf("reach interstring\n");
            t = malloc(sizeof(SYMBOL));
            t->name = "string";
            t->kind = typeSymKind;
            t->lineno = e->lineno;
            t->isBaseType = 1;
            t->tableDepth = 0;
            resType = t;
            break;
        case k_ExpressionKindRawStringLiteral:
            ////printf("reach rawstring\n");
            t = malloc(sizeof(SYMBOL));
            t->name = "string";
            t->kind = typeSymKind;
            t->lineno = e->lineno;
            t->isBaseType = 1;
            t->tableDepth = 0;
            resType = t;
            break;
        case k_ExpressionKindBoolLiteral:
            ////printf("reach bool\n");
            t = malloc(sizeof(SYMBOL));
            t->name = "bool";
            t->kind = typeSymKind;
            t->lineno = e->lineno;
            t->isBaseType = 1;
            t->tableDepth = 0;
            resType = t;
            break;
        case k_ExpressionKindRuneLiteral:
            ////printf("reach rune\n");
            t = malloc(sizeof(SYMBOL));
            t->name = "rune";
            t->kind = typeSymKind;
            t->lineno = e->lineno;
            t->isBaseType = 1;
            t->tableDepth = 0;
            resType = t;
            break;
        case k_ExpressionKindBinary:
            ////printf("reach binary\n");
            lhs = typeEXP(e->val.binary.lhs)->val;
            rhs = typeEXP(e->val.binary.rhs)->val;
            
            if (    0 == strcmp(e->val.binary.opera, "%") ||  
                    0 == strcmp(e->val.binary.opera, "<<") || 
                    0 == strcmp(e->val.binary.opera, ">>") || 
                    0 == strcmp(e->val.binary.opera, "&") || 
                    0 == strcmp(e->val.binary.opera, "^") ||
                    0 == strcmp(e->val.binary.opera, "|") ||
                    0 == strcmp(e->val.binary.opera, "&^"))
            {
                resType = checkBitWiseBinary(lhs, rhs, e->lineno);
            }
            else if (0 == strcmp(e->val.binary.opera, "+"))
            {
                resType = checkArithmeticOrStringBinary(lhs, rhs, e->lineno);
            }
            else if (0 == strcmp(e->val.binary.opera, "-") || 
                     0 == strcmp(e->val.binary.opera, "*")) 
            {
                resType = checkArithmeticBinary(lhs, rhs, e->lineno);            
            }
            else if (0 == strcmp(e->val.binary.opera, "/")) {
                t = checkArithmeticBinary(lhs, rhs, e->lineno);
                
                if (t != NULL && 0 == strcmp(t->name, "int")) {
                    if (e->val.binary.rhs->kind == k_ExpressionKindIntLiteral) {
                        if (e->val.binary.rhs->val.intLiteral.var == 0) {
                            //print error
                            fprintf(stderr, "Error: line(%d) Divide by 0\n", e->lineno);
                            exit(1);
                        }
                    }
                }
                else if (t!= NULL && 0 == strcmp(t->name, "float64")) {
                    if (e->val.binary.rhs->kind == k_ExpressionKindFloatLiteral) {
                        if (e->val.binary.rhs->val.floatLiteral.var == 0) {
                            fprintf(stderr, "Error: line(%d) Divide by 0\n", e->lineno);
                            exit(1);
                        }
                    }
                }
                resType = t;
                ////printf("here\n");
            }
            else if (0 == strcmp(e->val.binary.opera, "==") || 
                     0 == strcmp(e->val.binary.opera, "!=") )
            {
                checkComparableBinary(lhs, rhs, e->lineno);
                resType = resType = makeSymbol("bool", typeSymKind);
            }
            
            else if (0 == strcmp(e->val.binary.opera, "<") ||  
                     0 == strcmp(e->val.binary.opera, "<=") || 
                     0 == strcmp(e->val.binary.opera, ">") || 
                     0 == strcmp(e->val.binary.opera, ">="))
            {
                checkOrderedBinary(lhs,rhs, e->lineno, e->val.binary.opera);
                resType = makeSymbol("bool", typeSymKind);
            }
            else if(strcmp(e->val.binary.opera, "||") == 0){
                checkBOOL(lhs, e->lineno);
                checkBOOL(rhs, e->lineno);
                checkComparableBinary(lhs, rhs, e->lineno);
                resType = makeSymbol(getTypePrintName(lhs), typeSymKind);
                resType->tableDepth = lhs->tableDepth;
            }
            else if(strcmp(e->val.binary.opera, "&&") == 0){
                checkBOOL(lhs, e->lineno);
                checkBOOL(rhs, e->lineno);
                checkComparableBinary(lhs, rhs, e->lineno);
                resType = makeSymbol(getTypePrintName(lhs), typeSymKind);
                resType->tableDepth = lhs->tableDepth;
            }
            
            break;
           
        case k_ExpressionKindUnary:   
            t = typeEXP(e->val.unary.expr)->val;
            ////printf("reach unary\n");
            if (0 == strcmp(e->val.unary.opera, "+") ||
                0 == strcmp(e->val.unary.opera, "-")) 
            {
                resType = checkArithmetic(t, e->lineno);
            }
            else if (0 == strcmp(e->val.unary.opera, "!")) 
            {
                resType = checkBOOL(t, e->lineno);
            }
            else if (0 == strcmp(e->val.unary.opera, "^")) 
            {
                resType = checkBitWise(t, e->lineno);
            }
            break;
        case k_ExpressionKindParen:
            t = typeEXP(e->val.paren.expr)->val;
            resType = t;
            break;
        case k_ExpressionKindLen: 
            t = typeEXP(e->val.len.expr)->val;
            //check if t has a type string or array or slice
            if (strcmp(getTypePrintName(t), "string") == 0 || 
                strcmp(getTypePrintName(t), "rawstring") == 0 || 
                t->kind == arraySymKind || 
                t->kind == sliceSymKind) 
            {
                resType = makeSymbol("int", typeSymKind);
            }
            else {
                fprintf(stderr, "Error: line(%d) Only string, array or slice can call Len()\n", e->lineno);
                exit(1);
            }
            break;
        case k_ExpressionKindCap:
            t = typeEXP(e->val.cap.expr)->val;
            //check if t has a type string or array or slice
            ////printf("%d\n", t->kind);
            if (t->kind == sliceSymKind || 
                t->kind == arraySymKind) 
            {
                resType = makeSymbol("int", typeSymKind);
            }
            else {
                fprintf(stderr, "Error: line(%d) Only array or slice can call Cap()\n", e->lineno);
                exit(1);
            }
            break;
        case k_ExpressionKindAppend:
            t = typeEXP(e->val.append.id)->val;
            //check if t has a type string or array or slice
            ////printf("%s, %d\n", t->name, t->kind);
            if (t->kind != sliceSymKind) 
            {
                fprintf(stderr, "Error: line(%d) Only slice can call Append()\n", e->lineno);
                exit(1);
            }
            resType = t;
            t = typeEXP(e->val.append.added)->val;                                                                                       ;
            if (t == NULL) {
                fprintf(stderr, "Error: line(%d) Added object must have a type\n", e->lineno);
                exit(1);
            }
            
            //checkEqual(getArraySliceType(resType), t, e->lineno);
            ////printf("heelo\n");
            ////printf("%s\n", getTypePrintName(appendHelper(resType)));
            checkEqual(appendHelper(resType), t, e->lineno);
            
            break;
        // TODO: field select, array access and function call
        case k_ExpressionKindFuncCall:
            if (e->val.func_call.name->kind == k_ExpressionKindParen){
                t = e->val.func_call.name->val.paren.expr->val.identifier.sym;
            }
            else {
                t = e->val.func_call.name->val.identifier.sym;
            }
            Node* expr = typeEXP(e->val.func_call.args);
            
            //type cast
            ////printf("func call %s %d\n", t->name, t->tableDepth);
            if (t->kind == typeSymKind || t->kind == structSymKind) {
                ////printf("type cast\n");
                if (expr == NULL) {
                    fprintf(stderr, "Error: (line %d) type cast requires an expression.\n",e->lineno);
                    exit(1);
                }
                if (expr->next != NULL) {
                    fprintf(stderr, "Error: (line %d) too many arguments for type case\n",e->lineno);
                    exit(1);
                }
                //check if expr in parens is comparable to casting type
                ////printf("%s\n", expr->val->name);
                if (expr->val->kind == t->kind) {
                    //check if both resolve to numeric type
                    //check if type is string
                    //check if parent types contain the casting type 
                    if (checkParentType(expr->val, t, e->lineno) == 1 || checkParentType(t, expr->val, e->lineno) == 1) {
                        //do nothing
                        ////printf("find parent type match\n");
                    }
                    else if (strcmp(getTypePrintName(t), "string") == 0 || strcmp(getTypePrintName(t), "rawstring") == 0) {
                        ////printf("find string type match\n");
                        checkBitWise(expr->val, e->lineno);
                    }
                    else {
                        ////printf("find num type match\n");
                        checkArithmetic(t, e->lineno);
                        checkArithmetic(expr->val, e->lineno);
                    }
                }
                else {
                    fprintf(stderr, "Error: (line %d) invalid expression for type cast, the expression type must resolve to a base type\n",e->lineno);
                    exit(1);
                }
                //if pass, return the resulting type which is t;
                ////printf("type cast return %d\n", t->tableDepth);
                resType = t;
            }
            //func call
            else if (t->kind == funcSymKind) {
                //func call
                ////printf("func call\n");
                SYMBOL* returnType = t->val.func.returnSymRef;
                ////printf("hello\n");
                Node* params = makeParamList(t->val.func.funcParams);

                if (returnType == NULL) {
                    returnType = makeSymbol("void", nullSymKind);
                }

                if (params == NULL && expr == NULL) {
                    //resType = makeSymbol("", nullSymKind);
                    resType = returnType;
                }
                else { 
                    compareTypeList(params, expr, e->lineno);
                    resType = returnType;
                }
                ////printf("finish function call\n");
            }
            else {
                fprintf(stderr, "Error: (line %d) [%s] is neither a function nor a type.\n", e->lineno, t->name);
                exit(1);
            }
            break;
        case k_ExpressionKindArrayIndex:
            //printf("reach array index\n");
            if (isIndexAccess == 0) {
                isIndexAccess = 1;
            }
            dimCounter += 1;
            t = typeEXP(e->val.array_index.array)->val;

            // ////printf("isbasetype? %s, %d\n",t->val.parentType->name, t->isBaseType);
            // while (t->isBaseType == 0) {
            //     t = t->val.parentType;
            // }
            
            //compare index dimension if it is a index access
            if (isIndexAccess == 1) {
                ////printf("reset index access %s\n", t->name);
                isIndexAccess == 0;
                originalDim = getSliceArrayBaseDim(t);
                //if dimCounter > originalDim means exceed max dimension
                
            }
            //printf("array's name is %s\n", getTypePrintName(t));
            //printf("dim %d, original %d\n", dimCounter, originalDim);

            if (dimCounter > originalDim) {
                fprintf(stderr, "Error: line(%d) expression has unmatched dimension, it has %d dimension but %d indices are found.\n", e->lineno, originalDim, dimCounter);
                exit(1);
            }
            ////printf("%d %d\n", dimCounter, originalDim);
            if (t != NULL) {
                //single identifier
                if (t->kind == typeSymKind || t->kind == structSymKind) {
                    resType = t;
                }
                else if (t->kind == sliceSymKind || t->kind == arraySymKind) {
                    if (strcmp(t->name, "slice") == 0 || strcmp(t->name, "array") == 0) {
                        t = t->val.parentType;
                    }
                    if (dimCounter < originalDim) {
                        //if access index is less than actual dimension, we return a slice or array kind
                        int count = dimCounter;
                        while (count > 0) {
                            t = t->val.parentType;
                            count--;
                        }
                        ////printf("type is %s\n", getTypePrintName(t));
                        resType = t;
                    }
                    else {
                        //get the base type
                        resType = getSliceArrayBaseType(t);
                        //printf("base type is %s\n", resType->name);
                        ////printf("%d, %d\n", originalDim, (resType == NULL));
                    }
                }
                else {
                    fprintf(stderr, "Error: line(%d) indexing variable must be or have a parent type of slice or array\n", t->lineno);
                    exit(1);
                }
                // /*
                //     typedef c []int
                //     var x c
                //     c is a typeKind but has parent of sliceKind
                //     so we need to check this case
                // */
                // if (type->kind == typeSymKind && (type.parentType != sliceSymKind && type->parentType != arraySymKind)) {
                //     fprintf(stderr, "Error: line(%d) indexing variable must be or have a parent type of slice or array\n", type->lineno);
                //     exit(1);
                // }   
                dimCounter = 0;
            }
            // check index
            t = typeEXP(e->val.array_index.index)->val;
            checkIndex(t, e->lineno);
            break;
        case k_ExpressionKindFieldAccess:
            ////printf("reach field access\n");
            t = typeEXP(e->val.access_field.id)->val;

            if (t->kind != structSymKind) {
                fprintf(stderr, "Error: line(%d) the field access expression has type [%s] Expecting [struct]\n", e->lineno, getTypePrintName(t));
                exit(1);
            }

            // search to base type of struct
            // deal the case 
            // type p struct {}
            // type q p
            // q.x ...
            while (t->isBaseType == 0) {
                t = t->val.parentType;
            }

            //here t is a struct symbol
            //t = typeEXP(e->val.access_field.field)->val;
            char* field = e->val.access_field.field->val.identifier.name;
            resType = searchStructFields(t->val.structFields, field);
            
            ////printf("access %s\n", resType->name);
            if (resType == NULL) {
                fprintf(stderr, "Error: line(%d) The struct structure does not have a memeber called [%s].\n", e->lineno, field);
                exit(1);
            }
            e->val.access_field.field->val.identifier.sym = resType;
            char *rename = malloc(sizeof(char)*(strlen(resType->name)+12));
            strcat(rename, "__golitec_");
            strcat(rename, resType->name);
            resType->rename = rename;
            resType = resType->val.varType;
            break;
    }
    ////printf("finish exp\n");
    // return a list of type
    Node* n = malloc(sizeof(Node));
    n->val = resType;
    // if (type_list == NULL) {
    //     type_list = n;
    //     return type_list;
    // }
    // else {
    //     type_list->next = n;
    //     return type_list;
    // }
    if (e->next != NULL) {
        //////printf("gg %s\n", e->next->val.identifier.sym->name);
        n->next = typeEXP(e->next);
    }
    return n;
}

// void traverseStruct(SYMBOL* structType) {
//     if (structType->kind == k_TypeKindStruct) {
//         STRUCTFIELD* field = structType->val.tStruct.fields;
//         while (field != NULL) {
//             //create symbol based on type
            
//             field = field->next;
//         }
//     }
// }

SYMBOL* appendHelper(SYMBOL* sym) {
    SYMBOL* res;
    int count = 0;
    res = sym;
    while (res->kind != typeSymKind && res->kind != structSymKind) {
        if (strcmp(res->name, "") == 0) {
            count++;
        }
        if (count == 2) {
            break;
        }
        res = res->val.parentType;
    }
    return res;
}


// check if one of the parent type in expr is t
int checkParentType(SYMBOL* expr, SYMBOL* t, int lineno) {

    SYMBOL* s = expr;
    while (s != NULL) {
        if (strcmp(getTypePrintName(s), getTypePrintName(t)) == 0) {
            if (checkBaseType(t) == 1) {
                return 1;
            }
            else {
                fprintf(stderr, "Error: (line %d) invalid expression for type cast, the expression type must resolve to a base type\n",lineno);
                exit(1);
            }
                
        }
        s = s->val.parentType;
    }
    return 0;
}

SYMBOL* searchStructFields(SYMBOL* sym, char* id) {
    if (sym->kind == varSymKind) {
        if (strcmp(id, sym->name) == 0) {
            return sym;
        }
        else if (sym->next != NULL) {
            return searchStructFields(sym->next, id);
        }
        else {
            return NULL;
        }
    }
    return NULL;
}

// return the base type in string 
char* getTypePrintName(SYMBOL* type) {
    char* str = malloc(100*sizeof(char));
    SYMBOL* temp;
    switch (type->kind) {
        case varSymKind:
            str = getTypePrintName(type->val.varType);
            break;
        case typeSymKind:
            str = type->name;
            break;
        case sliceSymKind:
            temp = type;
            
            if (strcmp(temp->name, "") == 0) {
                strcat(str, "[]");
            }
            strcat(str, getTypePrintName(temp->val.parentType));
            break;
        case arraySymKind:
            temp = type;
            if (strcmp(temp->name, "") == 0) {
                strcat(str, "[");
                char buf[10];
                sprintf(buf, "%d", temp->arraySize);
                strcat(str, buf);
                strcat(str, "]");
            }
            strcat(str, getTypePrintName(temp->val.parentType));
            break;
        case structSymKind:
            if (strcmp(type->name, "") == 0) {
                strcat(str, "struct");
            }
            else {
                strcat(str, type->name);
            }
            break;
        case nullSymKind:
            strcat(str, type->name);
            break;
    }
    
    return str;
}

// return the base type in string 
char* getVarType(SYMBOL* type, int lineno) {
    if (type->kind == varSymKind) {
            return getTypePrintName(type->val.varType);
    }
    else {
        fprintf(stderr, "Error: line(%d) The argument is not a variable type.", lineno);
        exit(1);
    }
}


/*
    get the base type of slice object or array object
    to handle case like 
    type x [5]int
    type y x
    var o y
    we first find the place we encounter []
    then we find the first non-[] symbol and return its type
*/
SYMBOL* getSliceArrayBaseType(SYMBOL* t) {
    SYMBOL* sym = t;
    int bracketFlag = 0;
    int baseType = 0;
    
    while (sym != NULL) {
        if (sym->isBaseType == 1) {
            ////printf("baseType %s ", sym->name);
            baseType = 1;
        }

        if ((sym->kind == typeSymKind || sym->kind == structSymKind) && baseType == 1) {
            ////printf("returnType %s ", sym->name);
            return sym;
        }
        else {
            if (strcmp(sym->name, "") != 0) {
                ////printf("interNOde %s ", sym->name);
            }
            else {
                ////printf("bracket %s ", sym->name);
            }
            sym = sym->val.parentType;
        }
    }
    return sym;
}

int getSliceArrayBaseDim(SYMBOL* t) {
    
    SYMBOL* sym = t;
    // int bracketFlag = 0;
    int dim = 0;
    int baseType = 0;

    while (sym != NULL) {
        // if (sym->val.parentType == NULL) {
        //     return dim;
        // }
        
        if (sym->isBaseType == 1) {
            ////printf("baseType %s ", sym->name);
            baseType = 1;
        }

        if (sym->kind == typeSymKind && baseType == 1) {
            ////printf("returnType %s %d", sym->name, dim);
            return dim;
        }
        else {
            if (strcmp(sym->name, "") != 0) {
                ////printf("interNOde %s ", sym->name);
            }
            else {
                
                dim++;
                ////printf("bracket %s %d ", sym->name, dim);
            }
            sym = sym->val.parentType;
        }
    }
    return dim;
}

SYMBOL* getArraySliceType(SYMBOL* sym) {
    SYMBOL *s = sym->val.parentType;
    while (strcmp(s->name, "") == 0 || s->isBaseType != 1) {
        s = s->val.parentType;
    }
    return s;
}


int checkLValues(EXP* t) {
    if (t == NULL) return 0;
    if (t->next != NULL) {
        if (checkLValues(t->next) == 0) {
            return 0;
        }
    }
    
    switch (t->kind) {
        case k_ExpressionKindParen:
            return checkLValues(t->val.paren.expr);
        case k_ExpressionKindIdentifier:
            if (strcmp(t->val.identifier.name, "_") == 0) {
                return 1;
            }
            else if (t->val.identifier.sym->isConstant == 0) {
                return 0;
            }
            return 1;
        case k_ExpressionKindArrayIndex:
            return checkLValues(t->val.array_index.array);
        case k_ExpressionKindFieldAccess:
            //EXP *id; EXP *field;
            return checkLValues(t->val.access_field.id);
        case k_ExpressionKindFuncCall:
            if (t->val.func_call.name->val.identifier.sym->kind == funcSymKind) {
                SYMBOL* temp = t->val.func_call.name->val.identifier.sym->val.func.returnSymRef;
                if (temp->kind == sliceSymKind) {
                    return 1;
                }
                else {
                    fprintf(stderr, "Error: (line %d) the expression must be a L value.\n", t->lineno);
                    exit(1);
                }
            }
            break;
        default:
            fprintf(stderr, "Error: (line %d) the expression must be a L value.\n", t->lineno);
            exit(1);
    }
}

// UPDATED
int compareTypeList(Node* t1, Node* t2, int lineno) {
    if (t1 == NULL || t2 == NULL) {
        fprintf(stderr, "Error: line(%d) Too few arguments passed in.\n", lineno);
        exit(1);
        return 0;
    }
    
    //single, array, slice all needs to recurse
    if(t1->next != NULL) {
        compareTypeList(t1->next, t2->next, lineno);
    }

    if (t1->next == NULL && t2->next != NULL) {
        ////printf("too many arg %s %s\n", t2->val->name, t2->next->val->name);
        fprintf(stderr, "Error: line(%d) Too many arguments passed in.\n", lineno);
        exit(1);
    }
    
    ////printf("Lhs in: %s\n", getTypePrintName(t1->val));
    ////printf("Rhs in: %s\n", getTypePrintName(t2->val));
    if (strcmp(t1->val->name, "_") == 0){
        return 1;
    }
    else if (strcmp(getTypePrintName(t1->val), "_") == 0) {
        return 1;
    }
    else {
        //TODO
        //printf("check equal\n");
        checkEqual(t1->val, t2->val, lineno);
        return 1;
    }

    fprintf(stderr, "Error: line(%d) Provided expressions have different types [%s, %s].\n", lineno, getTypePrintName(t1->val), getTypePrintName(t2->val));
    exit(1);
}

void checkIndex(SYMBOL* t, int lineno) {
    if (t == NULL) return;
    SYMBOL* temp = t;
    while (temp->val.parentType != NULL) {
        temp = temp->val.parentType;
    }
    // if (count != dim) {
    //     fprintf(stderr, "Error: line(%d) expression has unmatched dimension, it has %d dimension but %d indices are found.\n", lineno, dim, count);
    //     exit(1);
    // }
    
    if (strcmp(temp->name, "int") != 0) {
        fprintf(stderr, "Error: line(%d) array / slice index is of type [%s], expecting [int].\n", lineno, temp->name);
        exit(1);
    }
}

// Updated
SYMBOL* checkBOOL(SYMBOL *type, int lineno) {
    if (type == NULL) return NULL;

    if (type->kind == varSymKind) {
        return checkBOOL(type->val.varType, lineno);
    }

    SYMBOL* tmp = type;
    while (tmp->kind != structSymKind && tmp->val.parentType != NULL) {
        tmp = tmp->val.parentType;
    }

    ////printf("check bool %s\n", tmp->name);
    if (tmp->kind == typeSymKind && strcmp(tmp->name, "bool") == 0){
        return type;
    }
    else {
        fprintf(stderr, "Error: line(%d) the expression is [%s], expect [bool]\n", lineno, getTypePrintName(type));
        exit(1);
    }
    return NULL;
}

// check integer, should I check for rune?
SYMBOL* checkINT(SYMBOL *type, int lineno) {
    if (type->kind == typeSymKind && strcmp(type->name, "int") == 0){
        return type;
    }
    return NULL;
}

SYMBOL* checkArithmetic(SYMBOL *type, int lineno) {
    SYMBOL* tmp = type;
    while (tmp->kind != structSymKind && tmp->val.parentType != NULL) {
        if (tmp->kind == typeSymKind)
            tmp = tmp->val.parentType;
        else
            break;
    }
    char* name = getTypePrintName(tmp);
    ////printf("%s\n", name);
    if (strcmp(name, "int") != 0 && 
        strcmp(name, "float64") != 0 &&
        strcmp(name, "rune") != 0) 
    {
        fprintf(stderr, "Error: line(%d) the expression has type [%s], expect [int], [float64] or [rune]\n", lineno, name);
        exit(1);
    }
    return type;
}

SYMBOL* checkArithmeticOrStringBinary(SYMBOL *t1, SYMBOL *t2, int lineno) {
    SYMBOL* r1 = checkArithmeticOrString(t1, lineno);
    SYMBOL* r2 = checkArithmeticOrString(t2, lineno);
    return checkEqual(r1, r2, lineno);
}

SYMBOL* checkArithmeticOrString(SYMBOL *type, int lineno) {
    SYMBOL* tmp = type;
    while (tmp->kind != structSymKind && tmp->val.parentType != NULL) {
        tmp = tmp->val.parentType;
    }
    char* name = getTypePrintName(tmp);
    if (strcmp(name, "int") != 0 && 
        strcmp(name, "float64") != 0 &&
        strcmp(name, "rune") != 0 &&
        strcmp(name, "string") != 0 && 
        strcmp(name, "rawstring") != 0) 
    {
        fprintf(stderr, "Error: line(%d) the expression has type [%s], expect [int], [float64], [rune] or [string]\n", lineno, name);
        exit(1);
    }
    return type;
}

// compare if two variables are capable to compare with each other
SYMBOL* checkComparableBinary(SYMBOL* lhs, SYMBOL*rhs, int lineno){
    if (lhs->kind == rhs->kind && strcmp(lhs->name, rhs->name) == 0) {
        if (lhs->kind == structSymKind) {
            //all the fields needs to be comparable
            lhs = lhs->val.structFields;
            rhs = rhs->val.structFields;
            while (lhs != NULL) {
                checkComparableBinary(lhs, rhs, lineno);
                checkComparableBinary(lhs->val.varType, rhs->val.varType, lineno);
                lhs = lhs->next;
                rhs = rhs->next;
            }
            //checkEqual(lhs, rhs, lineno);
        }
        else if (lhs->kind == arraySymKind) {
            ////printf("check slice\n");
            int dim1 = getSliceArrayBaseDim(lhs);
            int dim2 = getSliceArrayBaseDim(rhs);
            ////printf("%d %d", dim1, dim2);
            if (dim1 != dim2 || lhs->arraySize != rhs->arraySize) {
                fprintf(stderr, "Error: line(%d) arrays are not comparable [%s != %s].\n", lineno, getTypePrintName(lhs), getTypePrintName(rhs));
                exit(1);
            }
            checkComparableBinary(lhs->val.parentType, rhs->val.parentType, lineno);
            
        }
        else if (lhs->kind == sliceSymKind) {
            fprintf(stderr, "Error: line(%d) slice type is detected in relational op [expected comparable (same underlying type, no slices)].\n", lineno);
            exit(1);
        }
        // else if (strcmp(lhs->name, "int") != 0 &&
        //         strcmp(lhs->name, "float64") != 0 &&
        //         strcmp(lhs->name, "rune") != 0 &&
        //         strcmp(lhs->name, "string") != 0 &&
        //         strcmp(lhs->name, "bool") != 0)
        // {
        //     fprintf(stderr, "Error: line(%d) expressions passed in can not be compared [%s, %s].\n", lineno, lhs->name, rhs->name);
        //     exit(1);
        // }
        SYMBOL* sym = makeSymbol("bool", typeSymKind);
        return sym;
    }
    fprintf(stderr, "Error: line(%d) expression provided are not comparable [%s != %s].\n", lineno, lhs->name, rhs->name);
    exit(1);
}

SYMBOL* checkOrderedBinary(SYMBOL* lhs, SYMBOL*rhs, int lineno, char* opera){
    SYMBOL* tmp = lhs;
    SYMBOL* tmp2 = rhs;
    // check array and slice
    if (tmp->kind == sliceSymKind || tmp->kind == arraySymKind) {
        fprintf(stderr, "Error: line(%d) the expression has type [%s], cannot be ordered, expected ordered (int, rune, float64, string)\n", lineno, getTypePrintName(tmp));
        exit(1);
    }
    
    
    //integer, float, rune or string
    if (tmp->kind == tmp2->kind && strcmp(tmp->name, tmp2->name) == 0) {
        // check struct and recurse to find the base type
        while (tmp->kind != structSymKind && tmp->val.parentType != NULL) {
            tmp = tmp->val.parentType;
        }
        while (tmp->kind != structSymKind && tmp2->val.parentType != NULL) {
            tmp2 = tmp2->val.parentType;
        }
        if (strcmp(tmp->name, "int") != 0 &&
            strcmp(tmp->name, "float64") != 0 &&
            strcmp(tmp->name, "rune") != 0 &&
            strcmp(tmp->name, "string") != 0 &&
            strcmp(tmp->name, "rawstring") != 0) {
            fprintf(stderr, "Error: line(%d) incompatible type in relational op %s [received %s and %s, expected ordered (int, rune, float64, string)].\n", lineno, opera, getTypePrintName(lhs), getTypePrintName(rhs));
            exit(1);
        }
        SYMBOL* sym = lhs;
        return sym;
    }
    fprintf(stderr, "Error: line(%d) expression has imcompatiable types [%s != %s].\n", lineno, lhs->name, rhs->name);
    exit(1);
}

SYMBOL* checkArithmeticBinary(SYMBOL* lhs, SYMBOL*rhs, int lineno){
    SYMBOL* s1 = checkArithmetic(lhs, lineno);
    SYMBOL* s2 = checkArithmetic(rhs, lineno);
    return checkEqual(s1, s2, lineno);
}

SYMBOL* checkBitWiseBinary(SYMBOL* lhs, SYMBOL* rhs, int lineno){
    SYMBOL* s1 = checkBitWise(lhs, lineno);
    SYMBOL* s2 = checkBitWise(rhs, lineno);
    return checkEqual(s1, s2, lineno);
}

// compare if two variable have the exactly same type
SYMBOL* checkEqual(SYMBOL* s1, SYMBOL* s2, int lineno) {
    if (s1 == NULL || s2 == NULL) {
        ////printf("check euqal with null value\n");
    }
    //printf("comapre %s, %s, kind %d, %d\n", getTypePrintName(s1), getTypePrintName(s2), s1->kind, s2->kind);
    // if (s1 == s2) {
    //     return s1;
    // }
    SYMBOL *lhs, *rhs;
    if (s1->kind == s2->kind) {
        if (strcmp(s1->name, s2->name) == 0 && s1->kind == structSymKind) {
            lhs = s1;
            rhs = s2;
            // search to the base type of both struct
            while (lhs->isBaseType == 0) {
                lhs = lhs->val.parentType;
            }
            while (rhs->isBaseType == 0) {
                rhs = rhs->val.parentType;
            }
            // then compare struct fields
            lhs = lhs->val.structFields;
            rhs = rhs->val.structFields;
            while (lhs != NULL) {
                checkEqual(lhs, rhs, lineno);
                checkEqual(lhs->val.varType, rhs->val.varType, lineno);
                lhs = lhs->next;
                rhs = rhs->next;
            }
            return s1;
        }
        // type except struct has to have the exact same type address, otherwise even tho they have the same name and kind, they are not the same
        else if (s1->kind == typeSymKind && strcmp(s1->name, s2->name) == 0) {
            //TODO: fix this
            ////printf("%d %d\n", s1->tableDepth, s2->tableDepth);
            if (checkBaseType(s1) == 0 && checkBaseType(s2) == 0 && s1->tableDepth != s2->tableDepth) {
                fprintf(stderr, "Error: (line %d) incompatible types [%s != %s]\n", lineno, s1->name, s2->name);
                exit(1);
            }
            return s1;
        }
        else if (s1->kind == sliceSymKind) {
            ////printf("slice type\n");
            if (strcmp(getTypePrintName(s1), getTypePrintName(s2)) != 0) {
                // if (checkParentType(getArraySliceType(s1), getArraySliceType(s2), lineno) == 1 || checkParentType(getArraySliceType(s1), getArraySliceType(s2), lineno) == 1) {
                //     return s1;
                // }
                fprintf(stderr, "Error: (line %d) incompatiable types [%s != %s]\n", lineno, getTypePrintName(s1), getTypePrintName(s2));
                exit(1);
            }
            return s1;
        }
        else if (s1->kind == arraySymKind) {
            if (strcmp(getTypePrintName(s1), getTypePrintName(s2)) != 0) {
                //check if they have parent type
                if (s1->val.parentType->arraySize != s2->val.parentType->arraySize) {
                    fprintf(stderr, "Error: (line %d) incompatiable types [%s != %s]\n", lineno, getTypePrintName(s1), getTypePrintName(s2));
                    exit(1);
                }
                // if (checkParentType(getArraySliceType(s1), getArraySliceType(s2), lineno) == 1 || checkParentType(getArraySliceType(s1), getArraySliceType(s2), lineno) == 1) {
                //     return s1;
                // }
                fprintf(stderr, "Error: (line %d) incompatiable types [%s != %s]\n", lineno, getTypePrintName(s1), getTypePrintName(s2));
                exit(1);
            }
            return s1;
        }
        else if (strcmp(s1->name, s2->name) == 0) {
            return s1;
        }
    }

    fprintf(stderr, "Error: (line %d) incompatiable types [%s != %s]\n", lineno, s1->name, s2->name);
    exit(1);
}

SYMBOL* checkBitWise(SYMBOL* type, int lineno){
    SYMBOL* tmp = type;
    // array and slice are not bitwise comparable
    if (tmp->kind == sliceSymKind || tmp->kind == arraySymKind) {
        fprintf(stderr, "Error: line(%d) the expression has type [%s], expect [int], [rune]\n", lineno, getTypePrintName(tmp));
        exit(1);
    }
    // take the top most parent
    while (tmp->kind != structSymKind && tmp->val.parentType != NULL) {
        tmp = tmp->val.parentType;
    }
    char* name = getTypePrintName(tmp);
    if (strcmp(name, "int") != 0 && 
        strcmp(name, "rune") != 0) 
    {
        fprintf(stderr, "Error: line(%d) the expression has type [%s], expect [int], [rune]\n", lineno, name);
        exit(1);
    }
    return type;
}

int checkBaseType(SYMBOL* s) {
    if (strcmp(s->name, "int") != 0 &&
        strcmp(s->name, "float64") != 0 &&
        strcmp(s->name, "rune") != 0 &&
        strcmp(s->name, "string") != 0 &&
        strcmp(s->name, "rawstring") != 0 &&
        strcmp(s->name, "bool") != 0)
    {
        return 0;
    }
    return 1;
}

void updateType(SYMBOL* t1, SYMBOL* t2) {
    t1->name = t2->name;
    t1->kind = t2->kind;
    t1->lineno = t2->lineno;
    t1->arraySize = t2->arraySize;
    t1->val = t2->val;
    t1->tableDepth = t2->tableDepth;
    t1->isBaseType = t2->isBaseType;
    t1->structClassName = t2->structClassName;
}

Node* makeParamList(Node* sym) {
    //revert param list
    if (sym == NULL) return NULL;

    Node* n = malloc(sizeof(Node));
    Node* ptr2 = n;
    ptr2->val = sym->val->val.varType;

    Node* ptr = sym;
    ////printf("%s\n", ptr->val->name);
    while (ptr->next != NULL) {
        Node* prev = malloc(sizeof(Node));
        prev->next = ptr2;
        ptr2 = prev;
        ptr = ptr->next;
        ptr2->val = ptr->val->val.varType;
        ////printf("%s\n", ptr->val->name);
    }
    return ptr2;
}






