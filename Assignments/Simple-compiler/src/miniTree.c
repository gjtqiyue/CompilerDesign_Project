#include "miniTree.h"
#include <stdlib.h>

extern int yylineno;

TYPE *makeTYPE_intType ()
{
    TYPE *t =  malloc(sizeof(TYPE));
    t->lineno = yylineno;
    t->kind = k_TypeKindInt;
    return t;
}

TYPE *makeTYPE_floatType ()
{
    TYPE *t =  malloc(sizeof(TYPE));
    t->lineno = yylineno;
    t->kind = k_TypeKindFloat;
    return t;
}

TYPE *makeTYPE_stringType ()
{
    TYPE *t =  malloc(sizeof(TYPE));
    t->lineno = yylineno;
    t->kind = k_TypeKindString;
    return t;
}

TYPE *makeTYPE_boolType ()
{
    TYPE *t =  malloc(sizeof(TYPE));
    t->lineno = yylineno;
    t->kind = k_TypeKindBool;
    return t;
}

EXP *makeTERM_intLiteral (int intLiteral)
{
    EXP *t = malloc(sizeof(EXP));
    t->lineno = yylineno;
    t->kind = k_TermKindIntLiteral;
    t->val.intLiteral.intLiteral = intLiteral;
    return t;
}

EXP *makeTERM_floatLiteral (float floatLiteral)
{
    EXP *t = malloc(sizeof(EXP));
    t->lineno = yylineno;
    t->kind = k_TermKindFloatLiteral;
    t->val.floatLiteral.floatLiteral = floatLiteral;
    return t;
}

EXP *makeTERM_stringLiteral (char* stringLiteral)
{
    EXP *t = malloc(sizeof(EXP));
    t->lineno = yylineno;
    t->kind = k_TermKindStringLiteral;
    t->val.stringLiteral.stringLiteral = stringLiteral;
    return t;
}

EXP *makeTERM_boolLiteral (int boolLiteral)
{
    EXP *t = malloc(sizeof(EXP));
    t->lineno = yylineno;
    t->kind = k_TermKindBoolLiteral;
    t->val.boolLiteral.boolLiteral = boolLiteral;
    return t;
}

EXP *makeTERM_identifier (char* identifier)
{
    EXP *t = malloc(sizeof(EXP));
    t->lineno = yylineno;
    t->kind = k_TermKindIdentifier;
    t->val.identifier.identifier = identifier;
    return t;
}

EXP *makeEXP_unary (ExpressionKind kind, EXP *exp)
{
    EXP *e = malloc(sizeof(EXP));
    e->lineno = yylineno;
    e->kind = kind;
    e->val.unary.exp = exp;
    return e;
}

EXP *makeEXP_binary (ExpressionKind kind, EXP *lhs, EXP *rhs)
{
    EXP *e = malloc(sizeof(EXP));
    e->lineno = yylineno;
    e->kind = kind;
    e->val.binary.lhs = lhs;
    e->val.binary.rhs = rhs;
    return e;
}

EXP *makeEXP_paren (EXP *exp)
{
    EXP *e = malloc(sizeof(EXP));
    e->lineno = yylineno;
    e->kind = k_ExpressionKindParentheses;
    e->val.paren.exp = exp;
    return e;
}

PROG *makePROG_while (EXP *cond, PROG *body )
{
    PROG *p = malloc(sizeof(PROG));
    p->lineno = yylineno;
    p->kind = k_StatementKindWhile;
    p->val.while_stmt.cond = cond;
    p->val.while_stmt.body = body;
    return p;
}

PROG *makePROG_if (EXP *cond, PROG *body, PROG *elsePart )
{
    PROG *p = malloc(sizeof(PROG));
    p->lineno = yylineno;
    p->kind = k_StatementKindIf;
    p->val.if_stmt.cond = cond;
    p->val.if_stmt.body = body;
    p->val.if_stmt.elseif_stmt = elsePart;
    return p;
}
PROG *makePROG_else (PROG *elseBody)
{
    PROG *p = malloc(sizeof(PROG));
    p->lineno = yylineno;
    p->kind = k_StatementKindElse;
    p->val.else_stmt.body = elseBody;
    return p;
}

PROG *makePROG_declType (char* identifier, TYPE *type, EXP *exp )
{
    PROG *p = malloc(sizeof(PROG));
    p->lineno = yylineno;
    p->kind = k_StatementKindDeclarationType;
    p->val.decl_type.name = makeTERM_identifier(identifier);
    p->val.decl_type.type = type;
    p->val.decl_type.exp = exp;
    return p;
}

PROG *makePROG_decl (char* identifier, EXP *exp )
{
    PROG *p = malloc(sizeof(PROG));
    p->lineno = yylineno;
    p->kind = k_StatementKindDeclaration;
    p->val.decl.name = makeTERM_identifier(identifier);
    p->val.decl.exp = exp;
    return p;
}

PROG *makePROG_assign (char* identifier, EXP *exp )
{
    PROG *p = malloc(sizeof(PROG));
    p->lineno = yylineno;
    p->kind = k_StatementKindAssign;
    p->val.assign.name = makeTERM_identifier(identifier);
    p->val.assign.exp = exp;
    return p;
}

PROG *makePROG_read (char* identifier )
{
    PROG *p = malloc(sizeof(PROG));
    p->lineno = yylineno;
    p->kind = k_StatementKindRead;
    p->val.read.input = makeTERM_identifier(identifier);
    return p;
}

PROG *makePROG_print (EXP *exp )
{
    PROG *p = malloc(sizeof(PROG));
    p->lineno = yylineno;
    p->kind = k_StatementKindPrint;
    p->val.print.output = exp;
    return p;
}