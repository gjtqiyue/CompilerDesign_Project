%{
    #include <stdio.h>
    #include <stdlib.h>
    #include "miniTree.h"

    extern int yylineno;
    extern PROG *root;

    void yyerror(const char *s) { 
        fprintf(stderr, "Error: (line %d) %s\n", yylineno, s);
	    exit(1); 
    }
    int yylex();
%}

%union {
    int intval;
    float floatval;
    char* stringval;
    char* identifier;
    struct PROG *prog;
    struct TYPE *type;
    struct EXP *exp;
    struct TERM *term;
}

%type  <prog> programs program dcl if else while print read assign
%type  <type> type
%type  <exp> exp
%token <identifier> tIDENTIFIER 
%token <intval> tINTVAL 
%token <floatval> tFLOATVAL 
%token <stringval> tSTRINGVAL
%token <intval> tTRUE tFALSE
%token tVAR tINT tFLOAT tBOOL tSTRING tIF tELSE tWHILE tREAD tPRINT
%token tCOLON tASSIGN tSEMICOLON
%token tPLUS tMINUS tTIMES tDIV tEQUAL tNOTEQUAL tGEQ tLEQ tGREATER tSMALLER tAND tOR tNOT
%token tLEFTPAREN tRIGHTPAREN tLEFTCURLY tRIGHTCURLY
%token tUMINUS
%token tCOMMENT

/* specify the level of left association, prioritize the lower token */
%left tOR
%left tAND
%left tGEQ tLEQ tGREATER tSMALLER tEQUAL tNOTEQUAL
%left tPLUS tMINUS
%left tTIMES tDIV
%left tUMINUS tNOT

/* For line numbers while parsing */
%locations

/* Error messages */
%error-verbose

%start programs
%%
programs : program                                                              { root = $1; }
        ;
program : program tCOMMENT                                                      { $$ = $1; }
        | program if                                                            { $$ = $2; $$->next = $1; }
        | program while                                                         { $$ = $2; $$->next = $1; }
        | program dcl                                                           { $$ = $2; $$->next = $1; }
        | program assign                                                        { $$ = $2; $$->next = $1; }
        | program read                                                          { $$ = $2; $$->next = $1; }
        | program print                                                         { $$ = $2; $$->next = $1; }
        |                                                                       %empty { $$ = NULL; }
        ;
dcl : tVAR tIDENTIFIER tASSIGN exp tSEMICOLON                                   { $$ = makePROG_decl($2, $4); }            
    | tVAR tIDENTIFIER tCOLON type tASSIGN exp tSEMICOLON                       { $$ = makePROG_declType($2, $4, $6); }
    ;
type : tINT                                                                     { $$ = makeTYPE_intType(); }
    | tFLOAT                                                                    { $$ = makeTYPE_floatType(); }
    | tSTRING                                                                   { $$ = makeTYPE_stringType(); }
    | tBOOL                                                                     { $$ = makeTYPE_boolType(); }
    ;
if : tIF tLEFTPAREN exp tRIGHTPAREN tLEFTCURLY program tRIGHTCURLY else         { $$ = makePROG_if($3, $6, $8); }
    ;
else : tELSE tLEFTCURLY program tRIGHTCURLY                                     { $$ = makePROG_else($3); }
    | tELSE if                                                                  { $$ = makePROG_else($2); }
    |                                                                           %empty { $$ = NULL; }
    ;
while : tWHILE tLEFTPAREN exp tRIGHTPAREN tLEFTCURLY program tRIGHTCURLY        { $$ = makePROG_while($3, $6); }
    ;
assign : tIDENTIFIER tASSIGN exp tSEMICOLON                                     { $$ = makePROG_assign($1, $3); }
    ;
read : tREAD tLEFTPAREN tIDENTIFIER tRIGHTPAREN tSEMICOLON                      { $$ = makePROG_read($3); }
    ;
print : tPRINT tLEFTPAREN exp tRIGHTPAREN tSEMICOLON                            { $$ = makePROG_print($3); }
    ;
exp : exp tPLUS exp                                                             { $$ = makeEXP_binary(k_ExpressionKindAdd, $1, $3); }
    | exp tMINUS exp                                                            { $$ = makeEXP_binary(k_ExpressionKindMinus, $1, $3); }
    | exp tTIMES exp                                                            { $$ = makeEXP_binary(k_ExpressionKindTimes, $1, $3); }
    | exp tDIV exp                                                              { $$ = makeEXP_binary(k_ExpressionKindDivide, $1, $3); }
    | tNOT exp                                                                  { $$ = makeEXP_unary(k_ExpressionKindNot, $2); }
    | tMINUS exp %prec tUMINUS                                                  { $$ = makeEXP_unary(k_ExpressionKindUMinus, $2); }
    | tLEFTPAREN exp tRIGHTPAREN                                                { $$ = makeEXP_paren($2); }
    | exp tAND exp                                                              { $$ = makeEXP_binary(k_ExpressionKindAnd, $1, $3); }
    | exp tOR exp                                                               { $$ = makeEXP_binary(k_ExpressionKindOr, $1, $3); }
    | exp tNOTEQUAL exp                                                         { $$ = makeEXP_binary(k_ExpressionKindNotEqual, $1, $3); }
    | exp tEQUAL exp                                                            { $$ = makeEXP_binary(k_ExpressionKindEqual, $1, $3); }
    | exp tGEQ exp                                                              { $$ = makeEXP_binary(k_ExpressionKindGEQ, $1, $3); }
    | exp tLEQ exp                                                              { $$ = makeEXP_binary(k_ExpressionKindLEQ, $1, $3); }
    | exp tGREATER exp                                                          { $$ = makeEXP_binary(k_ExpressionKindGreater, $1, $3); }
    | exp tSMALLER exp                                                          { $$ = makeEXP_binary(k_ExpressionKindSmaller, $1, $3); }
    | tIDENTIFIER                                                               { $$ = makeTERM_identifier($1); }
    | tFLOATVAL                                                                 { $$ = makeTERM_floatLiteral($1); }
    | tINTVAL                                                                   { $$ = makeTERM_intLiteral($1); }
    | tSTRINGVAL                                                                { $$ = makeTERM_stringLiteral($1); }
    | tTRUE                                                                     { $$ = makeTERM_boolLiteral(1); }
    | tFALSE                                                                    { $$ = makeTERM_boolLiteral(0); }
    ;
