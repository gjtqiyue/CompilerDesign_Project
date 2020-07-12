#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "weeder.h"


int inSwitch = 0, inFor = 0, inElse = 0, inIf = 0, isStruct = 0, inFunction = 0, checkBlank = 0, numReturnStmt = 0;
FUNCDECL *funcRef;

void weedPROGRAM(PROGRAM *prog) {
    if (strcmp(prog->package, "_") == 0) {
        fprintf(stderr, "Error: (line %d) Package name cannot be blank identifier.\n", prog->lineno);
        exit(1);
    }
    
    weedDECLARATION(prog->decl);
}

void weedDECLARATION(DECLARATION *decls) {
    DECLARATION *cur = decls;
    
    while (cur) {
        switch (cur->kind) {
            case k_DeclarationKindFunction:
                weedFUNCDECL(cur->val.func_decl);
                break;
            case k_DeclarationKindVar:
                weedVARDECL(cur->val.var_decl);
                break;
            case k_DeclarationKindType:
                weedTYPEDECL(cur->val.type_decl);
                break;
        }
        cur = cur->next;
    }
}

void weedFUNCDECL(FUNCDECL *func) {
    //weed the func name later?
    inFunction = 1;
	numReturnStmt = 0;
    funcRef = func;

    weedPARAM(func->params);
    weedTYPE(func->returnType);

    if(func->returnType != NULL){
        STMT *body = func->body;
        if (weedTerminatingSTMT(body) == false) {
            fprintf(stderr, "Error: (line %d) function %s does not have a terminating statement.\n", func->lineno, func->name->val.identifier.name);
            exit(1);
        }
    }

    //normal weeding, don't need to check for terminating statements
    weedSTMTS(func->body);
    
    /*
	//make sure there is a return type if function's return type is not NULL
	if (func->returnType != NULL && numReturnStmt == 0) {
		fprintf(stderr, "Error: (line %d) A return statement is required but none is present.\n", func->lineno);
        exit(1);
	}
    */

    funcRef = NULL;
	numReturnStmt = 0;
    inFunction = 0;
}


bool weedTerminatingSTMT(STMT *last){

    if(last == NULL) return false;
    bool if_terminated;
    bool else_terminated;

    switch(last->kind){
        case k_StatementKindBlock:
            ////printf("reached block\n");
            return weedTerminatingSTMT(last->val.block_stmt);

        case k_StatementKindIf:
            ////printf("reached if\n");
            if_terminated = weedTerminatingSTMT(last->val.if_stmt.body);
            else_terminated = weedTerminatingSTMT(last->val.if_stmt.elseif_part);
            return if_terminated && else_terminated;

        case k_StatementKindElseIf:
            ////printf("reached else if\n");
            if_terminated = weedTerminatingSTMT(last->val.elseif_stmt.body);
            else_terminated = weedTerminatingSTMT(last->val.elseif_stmt.elseif_part);
            return if_terminated && else_terminated;

        case k_StatementKindElse:
            ////printf("reached else\n");
            return weedTerminatingSTMT(last->val.else_stmt.body);

        case k_StatementKindWhile:
            if(last->val.while_stmt.cond != NULL) return false;
            STMT *while_body = last->val.while_stmt.body->val.block_stmt;
            return hasNoBreakSTMT(while_body);

        case k_StatementKindFor:
            if(last->val.for_stmt.second != NULL) return false;
            STMT *for_body = last->val.for_stmt.body->val.block_stmt;
            return hasNoBreakSTMT(for_body);

        case k_StatementKindSwitch:;
            CASE *cases = last->val.switch_stmt.caseClauses;
            //needs one default case
            bool hasBreak = false;
            while(cases){
                if(cases->kind == k_SwitchKindDefault) {
                    hasBreak = true;
                    break;
                }
                cases = cases->next;
            }
            if(hasBreak == false) return 0;

            //all cases has to be terminating and no break statements
            cases = last->val.switch_stmt.caseClauses;
            bool case_terminated;

            while(cases){
                if(cases->kind == k_SwitchKindDefault){
                    if(!hasNoBreakSTMT(cases->val.default_exp.body)) return false;
                    case_terminated = weedTerminatingSTMT(cases->val.default_exp.body);
                }
                else{
                    if(!hasNoBreakSTMT(cases->val.case_exp.body)) return false;
                    case_terminated = weedTerminatingSTMT(cases->val.case_exp.body);
                }

                if(case_terminated == false) return 0;
                cases = cases->next;
            }
            return true;

        case k_StatementKindReturn:
            return true; //terminating statement

        default:
            return false; //non-terminating statement

    }
}

bool hasNoBreakSTMT(STMT *stmt){
    STMT *cur = stmt;

    while(cur){
        if(cur->kind == k_StatementKindBreak) return false;
        cur = cur->next;
    }
    return true;
}

void weedPARAM(PARAM *params) {
    PARAM *param = params;
    
    while (param) {
        weedTYPE(param->type);
        
        param = param->next;
    }
}

void weedVARDECL(VARDECL *vardecls) {
    VARDECL *vardecl = vardecls;
    
    while (vardecl) {
        switch (vardecl->kind) {
            case k_VarDeclKindType:
                weedTYPE(vardecl->val.type_only.type);
                break;
            case k_VarDeclKindExpr:
                checkBalance(vardecl->val.exp_only.id_list, vardecl->val.exp_only.expr_list, vardecl->lineno);
                break;
            case k_VarDeclKindBoth:
                checkBalance(vardecl->val.exp_type.id_list, vardecl->val.exp_type.expr_list, vardecl->lineno);
                weedTYPE(vardecl->val.exp_type.type);
                break;
        }
        
        vardecl = vardecl->next;
    }
}



void weedTYPEDECL(TYPEDECL *typedecls) {
    TYPEDECL *typedecl = typedecls;
    
    while (typedecl) {
        if (typedecl->id->kind != k_ExpressionKindIdentifier) {
            fprintf(stderr, "Error: (line %d) Type declaration name must be an identifier.\n", typedecl->lineno);
            exit(1);
        }
        
        weedTYPE(typedecl->type);
        
        typedecl = typedecl->next;
    }
}

void weedTYPE(TYPE *t) {
    if (t == NULL) {
	   return;
    }
    switch (t->kind) {
        case k_TypeKindSingle:
            weedEXP_Blank(t->val.identifier);
            break;
        case k_TypeKindSlice:
            weedTYPE(t->val.tSlice.type);
            break;
        case k_TypeKindArray:
            //prevent non-positive size
            if (t->val.tArray.size <= 0) {
                fprintf(stderr, "Error: (line %d) Array length must be positive, current size %d.\n", t->lineno, t->val.tArray.size);
                exit(1);
            }
            weedTYPE(t->val.tArray.type);
            break;
        case k_TypeKindStruct:
            //struct type
            weedTYPE_Struct(t->val.tStruct.fields);
            break;
        default:
            fprintf(stderr, "Error: (line %d) Unrecognized type.\n", t->lineno);
            exit(1);
    }
}

void weedTYPE_Struct(STRUCTFIELD *fields) {
    STRUCTFIELD *field = fields;
    
    while (field) {
        weedTYPE(field->type);
        
        field = field->next;
    }
}


void weedSTMTS (STMT *stmts) {
    if(stmts == NULL) return;
    weedSTMTS(stmts->next);
    bool inForNow;
    STMT *curSTMT = stmts;
    //printf("weed current statement is null ? %d\n", curSTMT == NULL);
    
        switch (curSTMT->kind) {
        case k_StatementKindEmpty:
            break;
        case k_StatementKindExp:
            weedSTMT_Exp(curSTMT->val.exp_stmt.expr);
            break;
        case k_StatementKindFor:
            if (inFor == 1) inForNow = true; //inside another for loop
            else inForNow = false;
            inFor = 1;
            if(curSTMT->val.for_stmt.first->kind == k_StatementKindExp && strcmp(curSTMT->val.for_stmt.first->val.exp_stmt.expr->val.identifier.name, "_") == 0){
                fprintf(stderr, "Error: (line %d) for loop initialization may not contain the blank identifier\n", curSTMT->lineno);
                exit(1);
            }
            weedSTMT_ForThird(curSTMT->val.for_stmt.third);
            weedSTMT_ForBody(curSTMT->val.for_stmt.body);
            if(inForNow) inFor = 1;
            else inFor = 0;
            break;
        case k_StatementKindWhile:
            if (inFor == 1) inForNow = true; //inside another for loop
            else inForNow = false;
			inFor = 1;
            //printf("weed while\n");
            //weedCond(curSTMT->val.while_stmt.cond);
            //printf("%d\n", curSTMT->val.while_stmt.body == NULL);
            weedSTMT_ForBody(curSTMT->val.while_stmt.body);
			if(inForNow) inFor = 1;
            else inFor = 0;
            break;
        case k_StatementKindIf:
			inIf = 1;
           // printf("weed IF line %d \n", stmts->lineno);
            weedSTMTS(curSTMT->val.if_stmt.opt_cond);
            weedEXP(curSTMT->val.if_stmt.cond);
            weedSTMTS(curSTMT->val.if_stmt.body);
            if (curSTMT->val.if_stmt.elseif_part != NULL) {
                weedSTMTS(curSTMT->val.if_stmt.elseif_part);
            }
			inIf = 0;
            break;
        case k_StatementKindElseIf:
            weedSTMTS(curSTMT->val.elseif_stmt.opt_cond);
            weedEXP(curSTMT->val.elseif_stmt.cond);
            weedSTMTS(curSTMT->val.elseif_stmt.body);
            if (curSTMT->val.elseif_stmt.elseif_part != NULL) {
                weedSTMTS(curSTMT->val.elseif_stmt.elseif_part);
            }
            break;
        case k_StatementKindElse:
			inElse = 1;
            //printf("weed else\n");
            weedSTMTS(curSTMT->val.else_stmt.body);
			inElse = 0;
            break;
        case k_StatementKindPrintln:
            if (curSTMT->val.println_stmt.expr_list != NULL) {
                weedEXP(curSTMT->val.println_stmt.expr_list);
            }
            break;
        case k_StatementKindPrint:
             if (curSTMT->val.print_stmt.expr_list != NULL) {
                weedEXP(curSTMT->val.print_stmt.expr_list);
            }
            break;
        case k_StatementKindAssign:
            weedSTMT_Assign(curSTMT);
            break;
        case k_StatementKindVarDecl:
            weedVARDECL(curSTMT->val.var_decl);
            break;
        case k_StatementKindShortDecl:
	    	weedIdList(curSTMT->val.short_decl.id_list);
            checkBlank = 1;
            weedEXP(curSTMT->val.short_decl.expr_list);
            checkBlank = 0;
            checkBalance(curSTMT->val.short_decl.id_list, curSTMT->val.short_decl.expr_list, curSTMT->lineno);
            break;
        case k_StatementKindTypeDecl:
            weedTYPEDECL(curSTMT->val.type_decl);
            break;
        case k_StatementKindInc:
            //TODO: can weed later with primary expression
            checkBlank = 1;
            weedEXP(curSTMT->val.inc_stmt.expr);
            checkBlank = 0;
            break;
        case k_StatementKindDec:
            checkBlank = 1;
            weedEXP(curSTMT->val.dec_stmt.expr);
            checkBlank = 0;
            break;
        case k_StatementKindReturn:
            weedSTMT_Return(curSTMT->val.return_stmt.expr, curSTMT);
            break;
        case k_StatementKindSwitch:
            weedEXP_Blank(curSTMT->val.switch_stmt.cond);
			inSwitch = 1;
            weedSTMT_SwitchCase(curSTMT->val.switch_stmt.caseClauses);
			inSwitch = 0;
            break;
        case k_StatementKindContinue:
            if (inFor == 0){
                fprintf(stderr, "Error: (line %d) continue statement must be in a for loop.\n", curSTMT->lineno);
                exit(1);
            }
            break;
        case k_StatementKindBreak:
            if (inSwitch == 0 && inFor == 0){
                fprintf(stderr, "Error: (line %d) break statement must be in a switch or for loop.\n", curSTMT->lineno);
                exit(1);
            }
            break;

        case k_StatementKindBlock:
            //printf("weed block\n");
            weedSTMTS(curSTMT->val.block_stmt);
            break;
        }
            
    //printf("finish\n");
}

void weedIdList(EXP *list) {
    EXP *cur = list;
	while (cur) {
		if (cur->kind != k_ExpressionKindIdentifier) {
			fprintf(stderr, "Error: (line %d) Left hand side of short declaration must be an identifier.\n", cur->lineno);
         	exit(1);
		}
		cur = cur->next;
	}
}

void weedSTMT_Exp(EXP *expr) {
    if (expr->kind != k_ExpressionKindFuncCall) {
         fprintf(stderr, "Error: (line %d) Expression statement must be a function call.\n", expr->lineno);
         exit(1);
    }
    weedEXP(expr);
}

void weedSTMT_Assign(STMT *assign) {
    checkBlank = 1;
    weedEXP(assign->val.assign_stmt.rhs);
    checkBlank = 0;
    checkBalance(assign->val.assign_stmt.lhs, assign->val.assign_stmt.rhs, assign->lineno); 
}

void weedSTMT_Return(EXP *expr, STMT *return_stmt) {
    //printf("reach weeder\n");
	if (inFunction == 0) {
	fprintf(stderr, "Error: (line %d) Return statement is only allowed in function body.\n", return_stmt->lineno);
	   	exit(1);
	}

    //printf("weeder %d\n", funcRef == NULL);
    return_stmt->val.return_stmt.func_ref = funcRef;
    numReturnStmt += 1;	//update return stmt count

    if (expr == NULL) {
        return;
    }
	

    // if (funcRef == NULL) {
    //     fprintf(stderr, "Error: (line %d) Return statement cannot find function reference in weeder.\n", return_stmt->lineno);
    //     exit(1);
    // }
    // if (expr != NULL && funcRef->returnType == NULL) {
    //     fprintf(stderr, "Error: (line %d) The function returns type void.\n", return_stmt->lineno);
    //     exit(1);
    // }
    // if (expr == NULL && funcRef->returnType != NULL) {
    //     fprintf(stderr, "Error: (line %d) The function's return expression is not privided.\n", return_stmt->lineno);
    //     exit(1);
    // }
    
/*
    if(expr == NULL) //no return expression
    {
		if (funcRef == NULL) {
			fprintf(stderr, "Error: (line %d) Return statement cannot find function reference in weeder.\n", expr->lineno);
	   		exit(1);
		}

        if (funcRef->returnType != NULL) {
            fprintf(stderr, "Error: (line %d) The function requires a return expression.\n", expr->lineno);
	   		exit(1);
        }
    }
    else if (expr != NULL && funcRef->returnType == NULL) {
        fprintf(stderr, "Error: (line %d) The function requires a return expression.\n", expr->lineno);
	   	exit(1);
    }
    else if (expr == NULL && funcRef->returnType != NULL) {
        fprintf(stderr, "Error: (line %d) The function return nothing but a expression is given.\n", expr->lineno);
	   	exit(1);
    }
*/
    
	//multiple return type
    if (expr->next != NULL) {
	   fprintf(stderr, "Error: (line %d) Only one return type is allowed.\n", return_stmt->lineno);
	   exit(1);
	}
	weedEXP(expr);
    
}

void weedSTMT_ForThird(STMT *third) {
    if (third == NULL) return;
    if (third->kind == k_StatementKindShortDecl) {
        fprintf(stderr, "Error: (line %d) The third part of the for loop cannot be short decl.\n", third->lineno);
        exit(1);
    }
    weedSTMTS(third);
}

void weedSTMT_ForBody(STMT *stmts) {
	if (stmts == NULL) return;

	STMT *stmt = stmts;
    int break_num = 0;
	int cond_num = 0;
	while (stmt) {
		if (stmt->kind == k_StatementKindContinue) {
			cond_num++;
		}	
		if (cond_num > 1) {
			fprintf(stderr, "Error: (line %d) Multiple continue statement detected.\n", stmt->lineno);
			exit(1);
		}
        if (stmt->kind == k_StatementKindBreak) {
            break_num++;
        }   
        if (break_num > 1) {
            fprintf(stderr, "Error: (line %d) Multiple break statement detected.\n", stmt->lineno);
            exit(2);
        }
		stmt = stmt->next;
	}
	weedSTMTS(stmts);
}

void weedSTMT_SwitchCase(CASE *cases) {
    if (cases == NULL) return;
    
    int num_default = 0;
    CASE *cur = cases;

    while (cur) {
        switch (cur->kind){
            case k_SwitchKindDefault:
                num_default++;
                if (num_default > 1) {
                    fprintf(stderr, "Error: (line %d) Switch statement can only have one default statement.\n", cur->lineno);
                    exit(1);
                }
				weedSTMT_CaseBody(cur->val.default_exp.body);
                break;
        	case k_SwitchKindCase:
				//weedEXP(cur->val.case_exp.expr_list);
				weedSTMT_CaseBody(cur->val.case_exp.body);
                break;
        }
        cur = cur->next;
    }
}

void weedSTMT_CaseBody(STMT *stmts) {
	if (stmts == NULL) return;
	STMT *stmt = stmts;
	int break_num = 0;
	while (stmt) {
		if (stmt->kind == k_StatementKindBreak) {
			break_num++;
		}	
		if (break_num > 1) {
			fprintf(stderr, "Error: (line %d) Multiple break statement detected.\n", stmt->lineno);
			exit(1);
		}
		stmt = stmt->next;
	}
	weedSTMTS(stmts);
}


/****************************************************/
/*Expression*/

void weedEXP_Primary(EXP *expr) {
    if (expr == NULL) return;
    if (expr->next != NULL) {
        fprintf(stderr, "Error: (line %d) Invalid syntax, expression list is allowed\n", expr->lineno);
                    exit(1);
    }
    
    switch (expr->kind) {
        case k_ExpressionKindBinary:
        case k_ExpressionKindUnary:
        case k_ExpressionKindAppend:
        case k_ExpressionKindLen:
        case k_ExpressionKindCap:
            fprintf(stderr, "Error: (line %d) Expression has to be a primary expression\n", expr->lineno);
                exit(1);
        default:
            break;
    }
}

int weedEXP(EXP *exprs) {
    int num = 0;
    EXP *expr = exprs;
    
    while (expr) {
        if (checkBlank == 1) {
            weedEXP_Blank(expr);
        }
    
        switch (expr->kind) {
            case k_ExpressionKindBinary:
                weedEXP_Blank(expr->val.binary.lhs);
                weedEXP_Blank(expr->val.binary.rhs);
                weedEXP(expr->val.binary.lhs);
                weedEXP(expr->val.binary.rhs);
                break;
            case k_ExpressionKindUnary:
                weedEXP_Blank(expr->val.unary.expr);
                weedEXP(expr->val.unary.expr);
                break;
            case k_ExpressionKindParen:
                weedEXP(expr->val.paren.expr);
                break;
            case k_ExpressionKindFuncCall:
                weedEXP_Primary(expr->val.func_call.name);
                weedEXP_Blank(expr->val.func_call.name);
                weedEXP_FuncArgs(expr->val.func_call.args);
                break;
            case k_ExpressionKindFieldAccess:
                weedEXP_Target(expr->val.access_field.id);
                weedEXP_Field(expr->val.access_field.field);
                break;
            case k_ExpressionKindArrayIndex:
                weedEXP_Array(expr->val.array_index.array);
                weedEXP_ArrayIndex(expr->val.array_index.index);
                break;
            case k_ExpressionKindAppend:
                weedEXP_Primary(expr->val.append.id);
                weedEXP_Blank(expr->val.append.id);
                weedEXP_Blank(expr->val.append.added);
                break;
            case k_ExpressionKindLen:
                weedEXP_Primary(expr->val.len.expr);
                weedEXP_Blank(expr->val.len.expr);
                break;
            case k_ExpressionKindCap:
                weedEXP_Primary(expr->val.cap.expr);
                weedEXP_Blank(expr->val.cap.expr);
            case k_ExpressionKindIdentifier:
	        case k_ExpressionKindIntLiteral:
	        case k_ExpressionKindFloatLiteral:
	        case k_ExpressionKindRuneLiteral:
	        case k_ExpressionKindInterStringLiteral:
	        case k_ExpressionKindRawStringLiteral:
	        case k_ExpressionKindBoolLiteral:
	            break;
        }
        num++;
        expr = expr->next;
    }
    return num;
}

void weedEXP_FuncArgs(EXP *args) {
    if (args == NULL) return;
    EXP *arg = args;
    
    while (arg) {
        weedEXP_Blank(arg);
        weedEXP(arg);
        arg = arg->next;
    }
}

void weedEXP_Blank(EXP *expr) {
    if (expr == NULL) return;
    if (expr->kind == k_ExpressionKindIdentifier && strcmp(expr->val.identifier.name, "_") == 0) {
         fprintf(stderr, "Error: (line %d) cannot use blank identifier here\n", expr->lineno);
                exit(1);
    }   
}

void weedEXP_Array(EXP *expr) {
    if (expr == NULL) return;
    weedEXP_Primary(expr);
    weedEXP_Blank(expr);
}

void weedEXP_ArrayIndex(EXP *expr) {
    if (expr == NULL) return;
    weedEXP_Blank(expr);
    weedEXP(expr);
    /*
    if (expr->kind != k_ExpressionKindIdentifier && expr->kind != k_ExpressionKindIntLiteral) {
        fprintf(stderr, "Error: (line %d) Array access index must be a integer or identifier\n", expr->lineno);
        exit(1);
    }
    */
}

void weedEXP_Target(EXP *expr) {
    if (expr == NULL) return;
    weedEXP_Primary(expr);
    weedEXP_Blank(expr);
}

void weedEXP_Field(EXP *expr) {
    if (expr == NULL) return;
    weedEXP_Primary(expr);
    weedEXP_Blank(expr);  
}

void checkBalance(EXP *lhs, EXP *rhs, int lineno) {
    int c1 = weedEXP(lhs);
    int c2 = weedEXP(rhs);
    
    if (c1 != c2) {
        fprintf(stderr, "Error: (line %d) Different number of expressions from lhs and rhs [%d, %d].\n", lineno, c1, c2);
        exit(1);
    }
}



