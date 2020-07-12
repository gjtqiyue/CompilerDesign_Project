#ifndef TREE_H
#define TREE_H
#include <stdbool.h>

extern int yylineno;
typedef struct SYMBOL SYMBOL;
typedef struct SymbolTable SymbolTable;

typedef enum {
    k_DeclarationKindFunction,
    k_DeclarationKindVar,
    k_DeclarationKindType
} DeclarationKind;

typedef enum {
    k_SwitchKindCase,
    k_SwitchKindDefault
} SwitchKind;

typedef enum {
    k_StatementKindEmpty,
    k_StatementKindExp,
    k_StatementKindFor,
    k_StatementKindWhile,
	k_StatementKindIf,
	k_StatementKindElseIf,
	k_StatementKindElse,
	k_StatementKindPrintln,
	k_StatementKindPrint,
	k_StatementKindAssign,
	k_StatementKindVarDecl,
	k_StatementKindShortDecl,
	k_StatementKindTypeDecl,
	k_StatementKindInc,
	k_StatementKindDec,
	k_StatementKindReturn,
	k_StatementKindSwitch,
	k_StatementKindContinue,
	k_StatementKindBreak,
	k_StatementKindBlock
} StatementKind;

typedef enum {
	k_ExpressionKindUnary,
	k_ExpressionKindBinary,
	k_ExpressionKindParen,
	k_ExpressionKindIdentifier,
	k_ExpressionKindIntLiteral,
	k_ExpressionKindFloatLiteral,
	k_ExpressionKindRuneLiteral,
	k_ExpressionKindInterStringLiteral,
	k_ExpressionKindRawStringLiteral,
	k_ExpressionKindBoolLiteral,
	k_ExpressionKindAppend,
	k_ExpressionKindLen,
	k_ExpressionKindCap,
	k_ExpressionKindFuncCall,
	k_ExpressionKindFieldAccess,
	k_ExpressionKindArrayIndex
} ExpressionKind;

typedef enum {
	k_TypeKindSingle,   //int, float, bool, rune, raw/inter string
	k_TypeKindSlice,
	k_TypeKindArray,
	k_TypeKindStruct
} TypeKind;

typedef enum {
    k_VarDeclKindType,
    k_VarDeclKindExpr,
    k_VarDeclKindBoth
} VarDeclKind;


typedef struct PROGRAM PROGRAM;
typedef struct DECLARATION DECLARATION;
typedef struct FUNCDECL FUNCDECL;
typedef struct VARDECL VARDECL;
typedef struct TYPEDECL TYPEDECL;
typedef struct STMT STMT;
typedef struct PARAM PARAM;
typedef struct TYPE TYPE;
typedef struct EXP EXP;
typedef struct CASE CASE;
typedef struct STRUCTFIELD STRUCTFIELD;
typedef struct CLASS CLASS;


struct PROGRAM {
    int lineno;
    char* package;
    DECLARATION* decl;
    CLASS *class_list;
};

struct CLASS {
	char *name;
	SYMBOL *structClass;
	CLASS *next;
};

struct DECLARATION {
    DeclarationKind kind;
    int lineno;
    
    union {
        FUNCDECL *func_decl;
        VARDECL *var_decl;
        TYPEDECL *type_decl;
    } val;
    
    DECLARATION *next;
};

struct PARAM {
    int lineno;
    EXP *id_list;
    TYPE *type;
    PARAM* next;
};

struct FUNCDECL {
    int lineno;
    EXP *name;
    PARAM *params; 
    TYPE *returnType; 
    STMT *body;
    SymbolTable *localScope;
    SYMBOL *sym;
};

struct VARDECL {
    VarDeclKind kind;
    int lineno;
    
    union {
        struct {EXP *id_list; TYPE *type;} type_only;
        struct {EXP *id_list; EXP *expr_list;} exp_only;
        struct {EXP *id_list; TYPE *type; EXP *expr_list;} exp_type;
    } val;
    
    VARDECL* next;  //var declaration list
};

struct TYPEDECL {   //for makeTYPEDECL()
    int lineno;
    EXP *id;
    TYPE *type;
    TYPEDECL *next;
};

struct TYPE {
	TypeKind kind;
	int lineno;
	
	union {
	    EXP* identifier;  //single
	    struct {TYPE *type;} tSlice;
	    struct {int size; TYPE* type;} tArray;
	    struct {STRUCTFIELD *fields;} tStruct;
	} val;
	
	TYPE *next;
};

struct STRUCTFIELD {
    int lineno;
    EXP *id_list;
    TYPE *type;
    STRUCTFIELD *next;
};


struct STMT {
	StatementKind kind;
	int lineno;
	
	union {
	    VARDECL *var_decl;
	    TYPEDECL *type_decl;
	    STMT *block_stmt;
		struct {EXP *lhs; char *kind; EXP *rhs;} assign_stmt;
		struct {EXP *expr;} exp_stmt;
		struct {EXP *id_list; EXP *expr_list;} short_decl;
		struct {EXP *expr_list;} print_stmt;
		struct {EXP *expr_list;} println_stmt;
		struct {EXP *expr; bool post;} inc_stmt;
		struct {EXP *expr; bool post;} dec_stmt;
		struct {EXP *expr; FUNCDECL* func_ref;} return_stmt;
		struct {STMT *opt_cond; EXP *cond; STMT *body; STMT *elseif_part;} if_stmt;
		struct {STMT *opt_cond; EXP *cond; STMT *body; STMT *elseif_part;} elseif_stmt;
		struct {STMT *body;} else_stmt;
		struct {STMT *opt_cond; EXP *cond; CASE *caseClauses;} switch_stmt;
		struct {EXP *cond; STMT *body;} while_stmt;
		struct {STMT *first; EXP *second; STMT *third; STMT *body;} for_stmt;
	} val;
	
	STMT *next;
};

struct CASE {
    SwitchKind kind;
    int lineno;
    
    union {
        struct {EXP *expr_list; STMT *body;} case_exp;
        struct {STMT *body;} default_exp;
    } val;
    
    CASE *next;
};


struct EXP {
	ExpressionKind kind;
	TYPE *type;
	int lineno;
	char *tmp_name;

	union {
		struct { EXP *lhs; EXP *rhs; char* opera;} binary;
		struct { EXP *expr; char* opera;} unary;
		struct { EXP *expr;} paren;
		struct { char* name; SYMBOL *sym; } identifier;
		struct { char var;} runeLiteral;
		struct { char* var;} rawstringLiteral;
		struct { char* var;} interstringLiteral;
		struct { int var;} intLiteral;
		struct { float var;} floatLiteral;
		struct { int var;} boolLiteral;
		struct { EXP *expr;} len;
        struct { EXP *expr;} cap;
        struct { EXP *name; EXP *args; } func_call;
        struct { EXP *id; EXP *added; } append;
        struct { EXP *array; EXP *index; } array_index;
        struct { EXP *id; EXP *field; } access_field;
	} val;
	
	EXP *next;
};

// Function declaration
PROGRAM *makePROGRAM(char *package_name, DECLARATION *decl);

DECLARATION *makeDECL_func(EXP *name, PARAM *params, TYPE *returnType, STMT *body);
PARAM *makePARAM(VARDECL *param);
DECLARATION *makeDECL_var(VARDECL *vardecl);
DECLARATION *makeDECL_type(TYPEDECL *typedecl);

TYPEDECL *makeTYPEDECL(EXP *id, TYPE *type);

VARDECL *makeVARDECL_type(EXP *id_list, TYPE *type);
VARDECL *makeVARDECL_exp(EXP *id_list, EXP *expr_list);
VARDECL *makeVARDECL_both(EXP *id_list, TYPE *type, EXP *expr_list);

TYPE *makeTYPE_single(EXP *id);
TYPE *makeTYPE_slice(TYPE *type);
TYPE *makeTYPE_array(int size, TYPE *type);
TYPE *makeTYPE_struct(STRUCTFIELD *field);
STRUCTFIELD *makeSTRUCT_field(EXP *id, TYPE *type);

STMT *makeSTMT_empty();
STMT *makeSTMT_vardecl(DECLARATION *var_decl);
STMT *makeSTMT_typedecl(DECLARATION *type_decl);
STMT *makeSTMT_if (STMT *opt_cond, EXP *cond, STMT *body, STMT *elsePart);
STMT *makeSTMT_elseif (STMT *opt_cond, EXP *cond, STMT *body, STMT *elsePart);
STMT *makeSTMT_else (STMT *elseBody);
STMT *makeSTMT_assign (EXP* id, char* opera, EXP *expr);
STMT *makeSTMT_print (EXP *expr_list);
STMT *makeSTMT_println(EXP *expr_list);
STMT *makeSTMT_short(EXP *list1, EXP *list2);
STMT *makeSTMT_inc(EXP *expr, bool post);
STMT *makeSTMT_dec(EXP *expr, bool post);
STMT *makeSTMT_return(EXP *expr);
STMT *makeSTMT_switch(STMT *opt_cond, EXP *cond, CASE *body);
STMT *makeSTMT_exp(EXP *expr);
CASE *makeSWITCH_case(EXP *cond, STMT *body);
CASE *makeSWITCH_default(STMT *body);
STMT *makeSTMT_while(EXP *cond, STMT *body);
STMT *makeSTMT_for(STMT *first, EXP *second, STMT *third, STMT *body);
STMT *makeSTMT_break();
STMT *makeSTMT_continue();
STMT *makeSTMT_block(STMT *block_stmt);

EXP *makeEXP_unary (EXP *expr, char *opera);
EXP *makeEXP_binary (EXP *lhs, char *opera, EXP *rhs);
EXP *makeEXP_paren (EXP *expr);
EXP *makeEXP_append(EXP *expr1, EXP *expr2);
EXP *makeEXP_len(EXP *expr);
EXP *makeEXP_cap(EXP *expr);
EXP *makeEXP_func_call(EXP *name, EXP *expr_list);
EXP *makeEXP_array_index(EXP *iden, EXP *idx);
EXP *makeEXP_access_field(EXP *expr, EXP *field);

EXP *makeTERM_intLiteral (int intLiteral);
EXP *makeTERM_floatLiteral (float floatLiteral);
EXP *makeTERM_stringLiteral (char* stringLiteral);
EXP *makeTERM_rawStringLiteral (char* stringLiteral);
EXP *makeTERM_identifier (char* name);
EXP *makeTERM_boolLiteral (int boolLiteral);
EXP *makeTERM_runeLiteral (char rune);
#endif
