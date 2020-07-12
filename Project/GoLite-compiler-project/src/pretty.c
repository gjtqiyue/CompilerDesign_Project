#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pretty.h"


int tabNum = 0;
int isStructResult = 0;

//prints tabs
void printTab()
{
    for(int i = 0; i < tabNum; i++)
    {
        printf("\t");
    }
}

//prints single id 
void pretty_id(EXP *id) {
    printf("%s", id->val.identifier.name);
}

void pretty_type(TYPE *type) {
    switch(type->kind){
        case k_TypeKindSingle:
            pretty_id(type->val.identifier);
            break;
        case k_TypeKindSlice:
            printf("[]");
            if(type->next != NULL){
                pretty_type(type->next);
            }
            pretty_type(type->val.tSlice.type);
            break;
        case k_TypeKindArray:
            printf("[%d]", type->val.tArray.size);
            if(type->next != NULL){
                pretty_type(type->next);
            }
            pretty_type(type->val.tArray.type);
            break;
        case k_TypeKindStruct:
            printf("struct { \n");
            tabNum++;
            pretty_struct_field(type->val.tStruct.fields);
            tabNum--;
            printTab();
            printf("}");
            break;
        default : fprintf(stderr, "Error: Unrecognised TypeK\n");
            exit(1);
    }
}


void pretty_struct_field(STRUCTFIELD *sf){
    if (sf == NULL) 
        return;
    if (sf->next != NULL) {
        pretty_struct_field(sf->next);
    }
    printTab();
    pretty_Identifier_list(sf->id_list);
    printf(" ");
    pretty_type(sf->type);
    printf("\n");
}


void pretty_Signature(FUNCDECL *decl){
  printf("(");
  pretty_Param_list(decl->params); 
  printf(")");
  if (decl->returnType != NULL) {
    printf(" ");
    pretty_type(decl->returnType);
  }
}

//prints parameters 
void pretty_Param_list(PARAM *params){
    if (params == NULL) 
        return;
    if (params->next != NULL) {
        pretty_Param_list(params->next);
        printf(", ");
    }
    pretty_Parameter_Unit(params);
}

//print one unit of parameters
void pretty_Parameter_Unit(PARAM *unit){
  pretty_Identifier_list(unit->id_list);
  printf(" ");
  pretty_type(unit->type);
}

//prints identifier list
void pretty_Identifier_list(EXP *list){
  EXP *id_list = list;

  while (id_list){
    pretty_id(id_list);
    id_list = id_list->next;
    if(id_list) printf(", ");
  }
}

//prints a program
void prettyProgram(PROGRAM *program) {
    
    if (!program) return;
    
    if (strlen(program->package) != 0) {
        printf("package %s;\n", program->package);
    }
    
    pretty_Top_Declaration(program->decl);
}

void pretty_Top_Declaration(DECLARATION *decl) {
    if (decl == NULL) 
        return;
    if (decl->next != NULL) {
        pretty_Top_Declaration(decl->next);
    }

    switch (decl->kind) {
        case k_DeclarationKindFunction:
            pretty_Funtion_Decl(decl->val.func_decl); break;
        case k_DeclarationKindVar:
            pretty_Var_Declaration_List(decl->val.var_decl); break;
        case k_DeclarationKindType:
            pretty_Type_Declaration_List(decl->val.type_decl); break;
        default: fprintf(stderr, "Error: Unrecognized Top Declaration Type\n"); exit(1);
    }
}

void pretty_Var_Declaration_List(VARDECL *decl) {
    if (decl == NULL) 
        return;
    if (decl->next != NULL) {
        pretty_Var_Declaration_List(decl->next);
    }
    printTab();
    pretty_Var_Decl(decl);
    printf("\n");
}

void pretty_Var_Decl(VARDECL *decl) {
    printf("var ");
   switch(decl->kind) {
      case k_VarDeclKindType:
          pretty_Identifier_list(decl->val.type_only.id_list);
          printf(" ");
          pretty_type(decl->val.type_only.type);
          break;
      case k_VarDeclKindExpr:
          pretty_Identifier_list(decl->val.exp_only.id_list);
          printf(" = ");
          prettyExpression_List(decl->val.exp_only.expr_list);
          break;
      case k_VarDeclKindBoth:
          pretty_Identifier_list(decl->val.exp_type.id_list);
          printf(" ");
          pretty_type(decl->val.exp_type.type);
          printf(" = ");
          prettyExpression_List(decl->val.exp_type.expr_list);
          break;
      default: fprintf(stderr, "Error: Unrecognised Type\n"); exit(1);
  }

}

void pretty_Type_Declaration_List(TYPEDECL *decl) {
    if (decl == NULL) 
        return;
    if (decl->next != NULL) {
        pretty_Type_Declaration_List(decl->next);
    }
    printTab();
    pretty_Type_Decl(decl);
    printf("\n");
}

void pretty_Type_Decl(TYPEDECL *decl) {
    printf("type ");
    pretty_id(decl->id);
    printf(" ");
    pretty_type(decl->type);
}


void pretty_Funtion_Decl(FUNCDECL *decl) {
    printTab();
    printf("func ");
    pretty_id(decl->name);
    pretty_Signature(decl);
    printTab();
    printf(" {\n");
    tabNum++;
    //printTab();
    prettyStatement(decl->body);
    tabNum--;
    printTab();
    printf("}\n");
}

//TODO
void prettyStatement(STMT *stmt) {
    STMT *curr = stmt;
    if (curr == NULL) {
    	return;
    }
    if (curr->next != NULL) {
    	prettyStatement(curr->next);
    }
    
    switch (curr->kind) {
    	//TOOD empty case
    	case k_StatementKindEmpty:
            printf("here in empty\n");
    		break;
        case k_StatementKindExp:
            printTab();
            pretty_Exp_STMT(curr);
            break;
        case k_StatementKindFor:
            printTab();
            pretty_For_STMT(curr);
            break;
        case k_StatementKindWhile:
            printTab();
            pretty_While_STMT(curr);
            break;
        case k_StatementKindIf:
            printTab();
            pretty_If_STMT(curr);
            break;
        case k_StatementKindElseIf:
            printTab();
            pretty_Else_If_STMT(curr);
            break;
        case k_StatementKindElse:
            printTab();
            pretty_Else_STMT(curr);
            break;
        case k_StatementKindPrintln:
            printTab();
            pretty_Println_STMT(curr);
            break;
        case k_StatementKindPrint:
            printTab();
            pretty_Print_STMT(curr);
            break;
        case k_StatementKindAssign:
            printTab();
            pretty_Assign_STMT(curr);
            break;
        case k_StatementKindVarDecl:
            printTab();
            pretty_Var_Decl(curr->val.var_decl);
            break;
        case k_StatementKindShortDecl:
            printTab();
            pretty_Short_STMT(curr);
            break;
        case k_StatementKindTypeDecl:
            printTab();
            pretty_Type_Decl(curr->val.type_decl);
            break;
        case k_StatementKindInc:
            printTab();
            pretty_Inc_STMT(curr);
            break;
        case k_StatementKindDec:
            printTab();
            pretty_Dec_STMT(curr);
            break;
        case k_StatementKindReturn:
            printTab();
            pretty_Return_STMT(curr);
            break;
        case k_StatementKindSwitch:
            printTab();
            printf("switch ");
            pretty_Switch_On(curr);
            printf(" {\n");
            tabNum++;
            pretty_Switch_Case(curr->val.switch_stmt.caseClauses);
            tabNum--;
            printTab();
            printf("}");
            break;
        case k_StatementKindContinue:
            printTab();
            printf("continue");
            break;
        case k_StatementKindBreak:
            printTab();
            printf("break");
            break;
        default:
            fprintf(stderr, "Error: Unrecognised Type\n"); 
            exit(1);
    }
    printf("\n");
}

void pretty_For_STMT(STMT *for_stmt) {
	    printf("for ");
        
		prettyFor(for_stmt);

	    printf(" {\n");
        tabNum++;
	    
	    prettyStatement(for_stmt->val.for_stmt.body);
        
        tabNum--;
        printTab();
	    printf("}");
}

void pretty_While_STMT(STMT *s) {
        printf("for ");
        
		prettyExpression(s->val.while_stmt.cond);
		
		printf(" {\n");
        tabNum++;
	    
	    prettyStatement(s->val.while_stmt.body);
        
        tabNum--;
        printTab();
	    printf("}");
}

void pretty_If_STMT(STMT *s) {
    printf("if ");

    if (s->val.if_stmt.opt_cond != NULL) {
        pretty_Simple_STMT(s->val.if_stmt.opt_cond);
        printf("; ");
    }

    //if block part
    prettyExpression(s->val.if_stmt.cond);
    printf(" {\n");
    tabNum++;
    //printTab();
    prettyStatement(s->val.if_stmt.body);
    tabNum--;

    if (s->val.if_stmt.elseif_part != NULL) {
        printTab();
        printf("}\n"); 
       // prettyStatement(s->val.elseif_stmt.elseif_part);
        
        if (s->val.if_stmt.elseif_part->kind == k_StatementKindElseIf) {
            pretty_Else_If_STMT(s->val.if_stmt.elseif_part);
        } else if (s->val.if_stmt.elseif_part->kind == k_StatementKindElse){
            pretty_Else_STMT(s->val.if_stmt.elseif_part);
        }
        
    } else {
        printTab();
        printf("}"); 
    }
}

void pretty_Else_If_STMT(STMT *curr) {
    printTab();
    printf("else if ");
    if (curr->val.elseif_stmt.opt_cond != NULL) {
        pretty_Simple_STMT(curr->val.elseif_stmt.opt_cond);
        printf("; ");
    }
    prettyExpression(curr->val.elseif_stmt.cond);
    printf(" {\n");
    tabNum++;
    prettyStatement(curr->val.elseif_stmt.body);
    tabNum--;
    //printTab();
    //printf("}\n");

    if (curr->val.elseif_stmt.elseif_part != NULL) {
        printTab();
        printf("}\n"); 
        //prettyStatement(curr->val.elseif_stmt.elseif_part);
        if (curr->val.elseif_stmt.elseif_part->kind == k_StatementKindElseIf) {
            pretty_Else_If_STMT(curr->val.elseif_stmt.elseif_part);
        } else if (curr->val.elseif_stmt.elseif_part->kind == k_StatementKindElse){
            pretty_Else_STMT(curr->val.elseif_stmt.elseif_part);
        }
    }else{
        printTab();
        printf("}"); 
    }
}

void pretty_Else_STMT(STMT *curr) {
    printTab();
    printf("else {\n");
    tabNum++;
    prettyStatement(curr->val.else_stmt.body);
    tabNum--;
    printTab();
    printf("}");
}

void pretty_Println_STMT(STMT *curr) {
    printf("println(");
    if (curr->val.println_stmt.expr_list!= NULL) {
        prettyExpression_List(curr->val.println_stmt.expr_list);
    }
    printf(")");
}

void pretty_Print_STMT(STMT *curr) {
    printf("print(");
    if (curr->val.print_stmt.expr_list!= NULL) {
        prettyExpression_List(curr->val.print_stmt.expr_list);
    }
    printf(")");
}
    
    
void pretty_Return_STMT(STMT *curr) {
    printf("return ");
    if (curr->val.return_stmt.expr != NULL) {
        prettyExpression(curr->val.return_stmt.expr);
    }
}

void pretty_Switch_On(STMT *stmt) {
	if (stmt->val.switch_stmt.opt_cond != NULL) {
		pretty_Simple_STMT(stmt->val.switch_stmt.opt_cond);
		printf("; ");
	}	

	if (stmt->val.switch_stmt.cond != NULL) {
		prettyExpression(stmt->val.switch_stmt.cond);
	}
}

void pretty_Switch_Case(CASE *c) {
  if (c == NULL) {
    return;
  }
  pretty_Switch_Case(c->next);

  switch(c->kind) {
    case k_SwitchKindCase: 
      printTab();
      printf("case ");
      prettyExpression_List(c->val.case_exp.expr_list);
      break;
    case k_SwitchKindDefault:
      printTab();
      printf("default");
      break;
    default: 
      fprintf(stderr, "Error: Not valid switchK\n"); exit(1);
      
  }
  printf(":\n");
  tabNum++;
  prettyStatement(c->val.case_exp.body);
  tabNum--;
}

void prettyFor(STMT *s) {
    pretty_Simple_STMT(s->val.for_stmt.first);
    printf("; ");

    if (s->val.for_stmt.second != NULL){
		prettyExpression(s->val.for_stmt.second);   
    }
    printf("; ");   

    pretty_Simple_STMT(s->val.for_stmt.third);
}

void pretty_Simple_STMT(STMT *simple) {
   switch(simple->kind) {
        case k_StatementKindEmpty: break;
        case k_StatementKindFor: break;
        case k_StatementKindWhile: break;
        case k_StatementKindIf: break;
        case k_StatementKindElseIf: break;
        case k_StatementKindElse: break;
        case k_StatementKindPrintln: break;
        case k_StatementKindPrint: break;
        case k_StatementKindVarDecl: break;
        case k_StatementKindTypeDecl: break;
        case k_StatementKindReturn: break;
        case k_StatementKindSwitch: break;
        case k_StatementKindContinue: break;
        case k_StatementKindBreak: break;
        case k_StatementKindExp:
            pretty_Exp_STMT(simple);
            break;
        case k_StatementKindAssign:
            pretty_Assign_STMT(simple);
            break;
        case k_StatementKindShortDecl:
            pretty_Short_STMT(simple);
            break;
        case k_StatementKindInc:
            pretty_Inc_STMT(simple);
            break;
        case k_StatementKindDec:
            pretty_Dec_STMT(simple);
            break;
        default : 
            fprintf(stderr, "Error: Not valid StatementK in pretty_Simple_STMT\n"); 
            exit(1);
   }
}

void pretty_Assign_STMT(STMT *simple) {
    prettyExpression_List(simple->val.assign_stmt.lhs);
    printf(" %s ", simple->val.assign_stmt.kind); 
    prettyExpression_List(simple->val.assign_stmt.rhs);
}

void pretty_Exp_STMT(STMT *simple) {
    if (simple->val.exp_stmt.expr != NULL) {
        prettyExpression(simple->val.exp_stmt.expr);
    }
}

void pretty_Short_STMT(STMT *simple) {
    prettyExpression_List(simple->val.short_decl.id_list);
    printf(" := ");
    prettyExpression_List(simple->val.short_decl.expr_list);
}

void  pretty_Inc_STMT(STMT *simple) {
    if (simple->val.inc_stmt.post == false) {
        printf("++");
        prettyExpression(simple->val.inc_stmt.expr);
    } else {
        prettyExpression(simple->val.inc_stmt.expr);
        printf("++");
    }  
}

void pretty_Dec_STMT(STMT *simple) {
    if (simple->val.dec_stmt.post == false) {
        printf("--");
        prettyExpression(simple->val.dec_stmt.expr);
    } else {
        prettyExpression(simple->val.dec_stmt.expr);
        printf("--");
    }  
}

void prettyExpression_List(EXP *exp_list) {
    if(exp_list == NULL) return;

    while (exp_list->next != NULL) {
        prettyExpression(exp_list);
        printf(", ");
        exp_list = exp_list->next;
    }
    prettyExpression(exp_list);
}

void prettyExpression(EXP *e) {
    if (e == NULL) {
        return;
    }

    switch(e->kind) {
        //unary
        case k_ExpressionKindUnary:
            prettyUnary(e);
            break;
        //binary
        case k_ExpressionKindBinary:
            prettyBinary(e);
            break;
        //builtin
        case k_ExpressionKindAppend:
            prettyAppend(e);
            break;
        case k_ExpressionKindLen:
            prettyLen(e);
            break;
        case k_ExpressionKindCap:
            prettyCap(e);
            break;
        //primary
        case k_ExpressionKindParen:
        case k_ExpressionKindIdentifier:
        case k_ExpressionKindIntLiteral:
        case k_ExpressionKindFloatLiteral:
        case k_ExpressionKindRuneLiteral:
        case k_ExpressionKindInterStringLiteral:
        case k_ExpressionKindRawStringLiteral:
        case k_ExpressionKindBoolLiteral:
        case k_ExpressionKindFuncCall:
        case k_ExpressionKindFieldAccess:
        case k_ExpressionKindArrayIndex:
            prettyPrimary_Expression(e);
            break;

        default:                
            fprintf(stderr, "Error: (line: %d) Not a valid expression kind\n", e->lineno); exit(1);
        }
}

void prettyPrimary_Expression(EXP *e) {

    switch (e->kind) {
        case k_ExpressionKindParen:
            prettyExpression(e->val.paren.expr);
            break;
        case k_ExpressionKindIdentifier:
            printf("%s", e->val.identifier.name); 
            break;
        case k_ExpressionKindIntLiteral:
            printf("%d", e->val.intLiteral.var);
            break;
        case k_ExpressionKindFloatLiteral:
            printf("%f", e->val.floatLiteral.var);
            break;
        case k_ExpressionKindRuneLiteral:
            printf("'%c'", e->val.runeLiteral.var);
            break;
        case k_ExpressionKindInterStringLiteral:
            printf("\"%s\"", e->val.interstringLiteral.var);
            break;
        case k_ExpressionKindRawStringLiteral:
            printf("%s", e->val.rawstringLiteral.var);
            break;
        case k_ExpressionKindBoolLiteral:
            printf("%d", e->val.boolLiteral.var);
            break;  
        case k_ExpressionKindFuncCall:
            prettyFuncCall(e);
            break;
        case k_ExpressionKindFieldAccess:
            prettyField(e);
            break;
        case k_ExpressionKindArrayIndex:
            prettyArrayIndex(e);
            break;
        default:                
            fprintf(stderr, "Error: (line: %d) Not a valid expression kind\n", e->lineno); exit(1);
        }
    }

void prettyUnary(EXP *e) {

    if (!e->val.unary.opera) {
        fprintf(stderr, "Error: (line: %d) Not a valid unaryOpExp kind\n", e->lineno); 
        exit(1);
    }

    printf("%s", e->val.unary.opera);
    printf("(");
    prettyExpression(e->val.unary.expr);
    printf(")");
}

void prettyBinary(EXP *e) {
    printf("(");
    prettyExpression(e->val.binary.lhs);

    if (e->val.binary.opera == NULL) {
        fprintf(stderr, "Error: (line: %d) Not a valid binaryOpExp kind\n", e->lineno); exit(1);
    }

    printf(" %s ", e->val.binary.opera);
    prettyExpression(e->val.binary.rhs);
    printf(")");
}

//builtin
void prettyAppend(EXP *e) {
    printf("append(");
    prettyExpression(e->val.append.id);
    printf(", ");
    prettyExpression(e->val.append.added);
    printf(")");
}

void prettyLen(EXP *e) {
    printf("len(");
    prettyExpression(e->val.len.expr);
    printf(")");
}

void prettyCap(EXP *e) {
    printf("cap(");
    prettyExpression(e->val.cap.expr);
    printf(")");
}

//func call
void prettyFuncCall(EXP *e) {
    pretty_id(e->val.func_call.name);
    printf("(");
    prettyExpression_List(e->val.func_call.args);
    printf(")");
}

//field selector
void prettyField(EXP *e) {
    prettyPrimary_Expression(e->val.access_field.id);
     printf(".");
    prettyPrimary_Expression(e->val.access_field.field);
}

//index
void prettyArrayIndex(EXP *e) {
    prettyPrimary_Expression(e->val.array_index.array);
    printf("[");
    prettyExpression(e->val.array_index.index);
    printf("]");
}

