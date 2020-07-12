#include <stdlib.h>
#include "tree.h"

char *strdup(const char*);
extern int yylineno;

/*Program nodes*/
PROGRAM *makePROGRAM(char* package_name, DECLARATION *decl) {
    PROGRAM *p = malloc(sizeof(PROGRAM));
    p->lineno = yylineno;
    p->package = package_name;
    p->decl = decl;
    
    return p;
}

/*Declaration nodes*/
DECLARATION *makeDECL_func(EXP *name, PARAM *params, TYPE *returnType, STMT *body) {
    DECLARATION *d = malloc(sizeof(DECLARATION));
    FUNCDECL *f = malloc(sizeof(FUNCDECL));
    d->lineno = yylineno;
    d->kind = k_DeclarationKindFunction;
    d->val.func_decl = f;
    d->val.func_decl->name = name;
    d->val.func_decl->params = params;
    d->val.func_decl->returnType = returnType;
    d->val.func_decl->body = body;
    
    return d;
}

PARAM *makePARAM(VARDECL *param) {
    PARAM *p = malloc(sizeof(PARAM));
    p->lineno = yylineno;
    p->id_list = param->val.type_only.id_list;
    p->type = param->val.type_only.type;
    
    return p;
}


DECLARATION *makeDECL_var(VARDECL *vardecl) {
    DECLARATION *d = malloc(sizeof(DECLARATION));
    d->lineno = yylineno;
    d->kind = k_DeclarationKindVar;
    d->val.var_decl = vardecl;
    
    return d;
}

DECLARATION *makeDECL_type(TYPEDECL *typedecl) {
    DECLARATION *d = malloc(sizeof(DECLARATION));
    d->lineno = yylineno;
    d->kind = k_DeclarationKindType;
    d->val.type_decl = typedecl;
    
    return d;
}

TYPEDECL *makeTYPEDECL(EXP* id, TYPE *type) {
    TYPEDECL *td = malloc(sizeof(TYPEDECL));
    td->lineno = yylineno;
    td->id = id;
    td->type = type;
    
    return td;
}

VARDECL *makeVARDECL_type(EXP *id_list, TYPE *type) {
    VARDECL *v = malloc(sizeof(VARDECL));
    v->lineno = yylineno;
    v->kind = k_VarDeclKindType;
    v->val.type_only.id_list = id_list;
    v->val.type_only.type = type;
    
    return v;
}

VARDECL *makeVARDECL_exp(EXP *id_list, EXP *expr_list) {
    VARDECL *v = malloc(sizeof(VARDECL));
    v->lineno = yylineno;
    v->kind = k_VarDeclKindExpr;
    v->val.exp_only.id_list = id_list;
    v->val.exp_only.expr_list = expr_list;
    
    return v;
}

VARDECL *makeVARDECL_both(EXP *id_list, TYPE *type, EXP *expr_list) {
    VARDECL *v = malloc(sizeof(VARDECL));
    v->lineno = yylineno;
    v->kind = k_VarDeclKindBoth;
    v->val.exp_type.id_list = id_list;
    v->val.exp_type.type = type;
    v->val.exp_type.expr_list = expr_list;
    
    return v;
}

TYPE *makeTYPE_single(EXP *id) {
    TYPE *t = malloc(sizeof(TYPE));
    t->lineno = yylineno;
    t->kind = k_TypeKindSingle;
    t->val.identifier = id;
    
    return t;
}

TYPE *makeTYPE_slice(TYPE *type) {
    TYPE *t = malloc(sizeof(TYPE));
    t->lineno = yylineno;
    t->kind = k_TypeKindSlice;
    t->val.tSlice.type = type;
    
    return t;
}


TYPE *makeTYPE_array(int size, TYPE *type) {
    TYPE *t = malloc(sizeof(TYPE));
    t->lineno = yylineno;
    t->kind = k_TypeKindArray;
    t->val.tArray.type = type;
    t->val.tArray.size = size;
    
    return t;
}

TYPE *makeTYPE_struct(STRUCTFIELD *field) {
    TYPE *t = malloc(sizeof(TYPE));
    t->lineno = yylineno;
    t->kind = k_TypeKindStruct;
    t->val.tStruct.fields = field;
    
    return t;
}

STRUCTFIELD *makeSTRUCT_field(EXP *id, TYPE *type) {
    STRUCTFIELD *sf = malloc(sizeof(STRUCTFIELD));
    sf->lineno = yylineno;
    sf->id_list = id;
    sf->type = type;
    
    return sf;
}

STMT *makeSTMT_vardecl(DECLARATION *var_decl) {
    STMT *s = malloc(sizeof(STMT));
    s->lineno = yylineno;
    s->kind = k_StatementKindVarDecl;
    s->val.var_decl = var_decl->val.var_decl;

    return s;
}

STMT *makeSTMT_typedecl(DECLARATION *type_decl) {
    STMT *s = malloc(sizeof(STMT));
    s->lineno = yylineno;
    s->kind = k_StatementKindTypeDecl;
    s->val.type_decl = type_decl->val.type_decl;

    return s;
}

STMT *makeSTMT_empty() {
    STMT *s = malloc(sizeof(STMT));
    s->lineno = yylineno;
    s->kind = k_StatementKindEmpty;
    s->next = NULL;
    
    return s;
}

STMT *makeSTMT_if (STMT *opt_cond, EXP *cond, STMT *body, STMT *elsePart) {
    STMT *s = malloc(sizeof(STMT));
    s->lineno = yylineno;
    s->kind = k_StatementKindIf;
    s->val.if_stmt.opt_cond = opt_cond;
    s->val.if_stmt.cond = cond;
    s->val.if_stmt.body = body;
    s->val.if_stmt.elseif_part = elsePart;
    
    return s;
}

STMT *makeSTMT_elseif (STMT *opt_cond, EXP *cond, STMT *body, STMT *elsePart) {
    STMT *s = malloc(sizeof(STMT));
    s->lineno = yylineno;
    s->kind = k_StatementKindElseIf;
    s->val.elseif_stmt.opt_cond = opt_cond;
    s->val.elseif_stmt.cond = cond;
    s->val.elseif_stmt.body = body;
    s->val.elseif_stmt.elseif_part = elsePart;
    
    return s;
}

STMT *makeSTMT_else (STMT *elseBody) {
    STMT *s = malloc(sizeof(STMT));
    s->lineno = yylineno;
    s->kind = k_StatementKindElse;
    s->val.else_stmt.body = elseBody;
    
    return s;
}

STMT *makeSTMT_assign (EXP* id, char* opera, EXP *expr) {
    STMT *s = malloc(sizeof(STMT));
    s->lineno = yylineno;
    s->kind = k_StatementKindAssign;
    s->val.assign_stmt.lhs = id;
    s->val.assign_stmt.kind = opera;
    s->val.assign_stmt.rhs = expr;
    
    return s;
}

STMT *makeSTMT_print (EXP *expr_list) {
    STMT *s = malloc(sizeof(STMT));
    s->lineno = yylineno;
    s->kind = k_StatementKindPrint;
    s->val.print_stmt.expr_list = expr_list;
    
    return s;
}

STMT *makeSTMT_println(EXP *expr_list) {
    STMT *s = malloc(sizeof(STMT));
    s->lineno = yylineno;
    s->kind = k_StatementKindPrintln;
    s->val.println_stmt.expr_list = expr_list;
    
    return s;
}

STMT *makeSTMT_short(EXP *list1, EXP *list2) {
    STMT *s = malloc(sizeof(STMT));
    s->lineno = yylineno;
    s->kind = k_StatementKindShortDecl;
    s->val.short_decl.id_list = list1;
    s->val.short_decl.expr_list = list2;
    
    return s;
}

STMT *makeSTMT_inc(EXP *expr, bool post) {
    STMT *s = malloc(sizeof(STMT));
    s->lineno = yylineno;
    s->kind = k_StatementKindInc;
    s->val.inc_stmt.expr = expr;
    s->val.inc_stmt.post = post;
    
    return s;
}

STMT *makeSTMT_dec(EXP *expr, bool post) {
    STMT *s = malloc(sizeof(STMT));
    s->lineno = yylineno;
    s->kind = k_StatementKindDec;
    s->val.dec_stmt.expr = expr;
    s->val.dec_stmt.post = post;
    
    return s;
}

STMT *makeSTMT_return(EXP *expr) {
    STMT *s = malloc(sizeof(STMT));
    s->lineno = yylineno;
    s->kind = k_StatementKindReturn;
    s->val.return_stmt.expr = expr;
    
    return s;
}

STMT *makeSTMT_switch(STMT *opt_cond, EXP *cond, CASE *body) {
    STMT *s = malloc(sizeof(STMT));
    s->lineno = yylineno;
    s->kind = k_StatementKindSwitch;
    s->val.switch_stmt.opt_cond = opt_cond;
    s->val.switch_stmt.cond = cond;
    s->val.switch_stmt.caseClauses = body;
    
    return s;
}

CASE *makeSWITCH_case(EXP *cond, STMT *body) {
    CASE *c = malloc(sizeof(CASE));
    c->lineno = yylineno;
    c->kind = k_SwitchKindCase;
    c->val.case_exp.expr_list = cond;
    c->val.case_exp.body = body;
    
    return c;
}

CASE *makeSWITCH_default(STMT *body) {
    CASE *c = malloc(sizeof(CASE));
    c->lineno = yylineno;
    c->kind = k_SwitchKindDefault;
    c->val.default_exp.body = body;
    
    return c;
}

STMT *makeSTMT_while(EXP *cond, STMT *body) {
    STMT *s = malloc(sizeof(STMT));
    s->lineno = yylineno;
    s->kind = k_StatementKindWhile;
    s->val.while_stmt.cond = cond;
    s->val.while_stmt.body = body;
    
    return s;
}


STMT *makeSTMT_for(STMT *first, EXP *second, STMT *third, STMT *body) {
    STMT *s = malloc(sizeof(STMT));
    s->lineno = yylineno;
    s->kind = k_StatementKindFor;
    s->val.for_stmt.first = first;
    s->val.for_stmt.second = second;
    s->val.for_stmt.third = third;
    s->val.for_stmt.body = body;
    
    return s;
}


STMT *makeSTMT_break() {
    STMT *s = malloc(sizeof(STMT));
    s->lineno = yylineno;
    s->kind = k_StatementKindBreak;
    
    return s;
}

STMT *makeSTMT_continue() {
    STMT *s = malloc(sizeof(STMT));
    s->lineno = yylineno;
    s->kind = k_StatementKindContinue;
    
    return s;
}

STMT *makeSTMT_block(STMT *block_stmt) {
    STMT *s = malloc(sizeof(STMT));
    s->lineno = yylineno;
    s->kind = k_StatementKindBlock;
    s->val.block_stmt = block_stmt;
    
    return s;
}

STMT *makeSTMT_exp(EXP *expr) {
    STMT *s = malloc(sizeof(STMT));
    s->lineno = yylineno;
    s->kind = k_StatementKindExp;
    s->val.exp_stmt.expr = expr;
    
    return s;
}

EXP *makeEXP_unary(EXP *expr, char *opera) {
    EXP *e = malloc(sizeof(EXP));
    e->lineno = yylineno;
    e->kind = k_ExpressionKindUnary;
    e->val.unary.expr = expr;
    e->val.unary.opera = opera;
    
    return e;
}

EXP *makeEXP_binary(EXP *lhs, char *opera, EXP *rhs) {
    EXP *e = malloc(sizeof(EXP));
    e->lineno = yylineno;
    e->kind = k_ExpressionKindBinary;
    e->val.binary.lhs = lhs;
    e->val.binary.opera = opera;
    e->val.binary.rhs = rhs;
    
    return e;
}

EXP *makeEXP_paren(EXP *expr) {
    EXP *e = malloc(sizeof(EXP));
    e->lineno = yylineno;
    e->kind = k_ExpressionKindParen;
    e->val.paren.expr = expr;
    
    return e;
}

EXP *makeTERM_intLiteral (int intLiteral) {
    EXP *e = malloc(sizeof(EXP));
    e->lineno = yylineno;
    e->kind = k_ExpressionKindIntLiteral;
    e->val.intLiteral.var = intLiteral;
    
    return e;
}

EXP *makeTERM_floatLiteral (float floatLiteral) {
    EXP *e = malloc(sizeof(EXP));
    e->lineno = yylineno;
    e->kind = k_ExpressionKindFloatLiteral;
    e->val.floatLiteral.var = floatLiteral;
    
    return e;
}

EXP *makeTERM_stringLiteral (char* stringLiteral) {
    EXP *e = malloc(sizeof(EXP));
    e->lineno = yylineno;
    e->kind = k_ExpressionKindInterStringLiteral;
    e->val.interstringLiteral.var = stringLiteral;
    
    return e;
}

EXP *makeTERM_rawStringLiteral (char* stringLiteral) {
    EXP *e = malloc(sizeof(EXP));
    e->lineno = yylineno;
    e->kind = k_ExpressionKindRawStringLiteral;
    e->val.rawstringLiteral.var = stringLiteral;
    
    return e;
}

EXP *makeTERM_identifier (char* name) {
    EXP *e = malloc(sizeof(EXP));
    e->lineno = yylineno;
    e->kind = k_ExpressionKindIdentifier;
    e->val.identifier.name = name;
    
    return e;
}

EXP *makeTERM_boolLiteral (int boolLiteral) {
    EXP *e = malloc(sizeof(EXP));
    e->lineno = yylineno;
    e->kind = k_ExpressionKindBoolLiteral;
    e->val.boolLiteral.var = boolLiteral;
    
    return e;
}

EXP *makeTERM_runeLiteral (char runeLiteral) {
    EXP *e = malloc(sizeof(EXP));
    e->lineno = yylineno;
    e->kind = k_ExpressionKindRuneLiteral;
    e->val.runeLiteral.var = runeLiteral;
    
    return e;
}

EXP *makeEXP_append(EXP *expr1, EXP *expr2) {
    EXP *e = malloc(sizeof(EXP));
    e->lineno = yylineno;
    e->kind = k_ExpressionKindAppend;
    e->val.append.id = expr1;
    e->val.append.added = expr2;
    
    return e;
}

EXP *makeEXP_len(EXP *expr) {
    EXP *e = malloc(sizeof(EXP));
    e->lineno = yylineno;
    e->kind = k_ExpressionKindLen;
    e->val.len.expr = expr;
    
    return e;
}

EXP *makeEXP_cap(EXP *expr) {
    EXP *e = malloc(sizeof(EXP));
    e->lineno = yylineno;
    e->kind = k_ExpressionKindCap;
    e->val.cap.expr = expr;
    
    return e;
}

EXP *makeEXP_func_call(EXP *name, EXP *expr_list) {
    EXP *e = malloc(sizeof(EXP));
    e->lineno = yylineno;
    e->kind = k_ExpressionKindFuncCall;
    e->val.func_call.name = name;
    e->val.func_call.args = expr_list;
    
    return e;
}

EXP *makeEXP_array_index(EXP *array, EXP *idx) {
    EXP *e = malloc(sizeof(EXP));
    e->lineno = yylineno;
    e->kind = k_ExpressionKindArrayIndex;
    e->val.array_index.array = array;
    e->val.array_index.index = idx;
    
    return e;
}

EXP *makeEXP_access_field(EXP *expr, EXP *field) {
    EXP *e = malloc(sizeof(EXP));
    e->lineno = yylineno;
    e->kind = k_ExpressionKindFieldAccess;
    e->val.access_field.id = expr;
    e->val.access_field.field = field;
    
    return e;
}



