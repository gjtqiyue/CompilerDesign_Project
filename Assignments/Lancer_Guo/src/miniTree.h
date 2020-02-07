#ifndef MINITREE_H
#define MINITREE_H

#include "symbol.h"

extern int yylineno;

typedef enum {
	k_StatementKindWhile,
	k_StatementKindIf,
	k_StatementKindElse,
	k_StatementKindRead,
	k_StatementKindPrint,
	k_StatementKindAssign,
	k_StatementKindDeclaration,
	k_StatementKindDeclarationType,
} StatementKind;

typedef enum {
	k_ExpressionKindAdd,
	k_ExpressionKindMinus,
	k_ExpressionKindTimes,
	k_ExpressionKindDivide,
	k_ExpressionKindAnd,
	k_ExpressionKindOr,
	k_ExpressionKindNot,
	k_ExpressionKindParentheses,
	k_ExpressionKindGEQ,
	k_ExpressionKindLEQ,
	k_ExpressionKindGreater,
	k_ExpressionKindSmaller,
	k_ExpressionKindEqual,
	k_ExpressionKindNotEqual,
	k_ExpressionKindUMinus,
	k_TermKindIdentifier,
	k_TermKindIntLiteral,
	k_TermKindFloatLiteral,
	k_TermKindStringLiteral,
	k_TermKindBoolLiteral
} ExpressionKind;

typedef enum {
	k_TypeKindInt,
	k_TypeKindFloat,
	k_TypeKindString,
	k_TypeKindBool
} TypeKind;

typedef struct PROG PROG;
typedef struct TYPE TYPE;
typedef struct EXP EXP;
typedef struct TERM TERM;

struct PROG {
	StatementKind kind;
	int lineno;
	
	union {
		struct { EXP *cond; PROG *body; } while_stmt;
		struct { EXP *cond; PROG *body; PROG *elseif_stmt; } if_stmt;
		struct { PROG *body; } else_stmt;
		struct { EXP *name; TYPE *type; EXP *exp; } decl_type;
		struct { EXP *name; EXP *exp; } decl;
		struct { EXP *name; EXP *exp; } assign;
		struct { EXP *input; } read;
		struct { EXP *output; } print;
	} val;
	PROG *next;
};

struct TYPE {
	TypeKind kind;
	int lineno;
};

struct EXP {
	ExpressionKind kind;
	int lineno;

	union {
		struct { EXP *lhs; EXP *rhs; SYMBOL *lsym; SYMBOL *rsym; } binary;
		struct { EXP *exp; SYMBOL *sym; } unary;
		struct { EXP *exp; SYMBOL *sym; } paren;
		struct { char* identifier; SYMBOL *sym; } identifier;
		struct { char* stringLiteral; SYMBOL *sym; } stringLiteral;
		struct { int intLiteral; SYMBOL *sym; } intLiteral;
		struct { float floatLiteral; SYMBOL *sym; } floatLiteral;
		struct { int boolLiteral; SYMBOL *sym; } boolLiteral;
	} val;
};

// Function declaration
PROG *makePROG_while (EXP *cond, PROG *body);
PROG *makePROG_if (EXP *cond, PROG *body, PROG *elsePart);
PROG *makePROG_else (PROG *elseBody);
PROG *makePROG_declType (char* identifier, TYPE *type, EXP *exp);
PROG *makePROG_decl (char* identifier, EXP *exp);
PROG *makePROG_assign (char* identifier, EXP *exp);
PROG *makePROG_read (char* identifier);
PROG *makePROG_print (EXP *exp);

EXP *makeEXP_unary (ExpressionKind kind, EXP *exp);
EXP *makeEXP_binary (ExpressionKind kind, EXP *lhs, EXP *rhs);
EXP *makeEXP_paren (EXP *exp);

EXP *makeTERM_intLiteral (int intLiteral);
EXP *makeTERM_floatLiteral (float floatLiteral);
EXP *makeTERM_stringLiteral (char* stringLiteral);
EXP *makeTERM_identifier (char* identifier);
EXP *makeTERM_boolLiteral (int boolLiteral);

TYPE *makeTYPE_intType ();
TYPE *makeTYPE_floatType ();
TYPE *makeTYPE_stringType ();
TYPE *makeTYPE_boolType ();

#endif
