%{ 
    #include <stdio.h>
    #include <stdlib.h>
    #include <stdbool.h>
    #include "tree.h"
    int yylex();
    extern PROGRAM *root;
    extern int yylineno;
    void yyerror(const char *s) { fprintf(stderr, "Error: (line %d) %s\n", yylineno, s); exit(1); }
%}

%locations
%error-verbose

%code requires
{
    #include "tree.h"
}

// define yylval
%union {
    int intval;
    float floatval;
    char *idval;
    char runeval;
    char *interstringval;
    char *rawstringval;
    int boolval;
    EXP *expr;
    TYPE *type;
    STMT *stmt;
    PROGRAM *program;
    DECLARATION *decl;
    FUNCDECL *func_decl;
    VARDECL *var_decl;
    TYPEDECL *type_decl;
    CASE *switch_case;
    PARAM *param;
    STRUCTFIELD *structfield;
} 

%type <program> program
%type <decl> declarations func_decl var_decl type_decl 
%type <param> parameter_list signature
%type <var_decl> parameter v_decl decl_list
%type <stmt> statement_list statement block_stmt empty_stmt assign_stmt exp_stmt short_decl print_stmt println_stmt inc_stmt 
%type <stmt> dec_stmt return_stmt if_stmt else_stmt simpleStmt switch_stmt for_stmt 
%type <switch_case> exprCaseClauses
%type <expr> expression expr_list id_list
%type <type_decl> t_decl type_list
%type <structfield> field_decl
%type <type> type 
%token <intval> tINTVAL
%token <floatval> tFLOATVAL
%token <idval> tIDENTIFIER
%token <interstringval> tINTERSTRINGVAL
%token <rawstringval> tRAWSTRINGVAL
%token <runeval> tRUNEVAL

%token tBREAK tCASE tCHAN tCONST tCONTINUE tDEFAULT tDEFER tELSE tFALLTHROUGH tFOR tFUNC tGO tGOTO tIF tIMPORT tMAP tPACKAGE tRANGE tRETURN tSELECT tSTRUCT tSWITCH 
tTYPE tVAR tPRINT tPRINTLN tAPPEND tLEN tCAP

%token tPLUS tMINUS tTIMES tDIV
%token tEQUALS tNE tGE tLE tGT tLT
%token tAND tOR tNOT
%token tLPAREN tRPAREN tLBRACE tRBRACE 
%token tCOLON tASSIGN
%token UNARY
%token tBWAND tPLUSASSIGN tBWANDASSIGN tBWOR tMINUSASSIGN tBWORASSIGN tBWXOR
%token tTIMESASSIGN tBWXORASSIGN tARROW tLSHIFT tDIVASSIGN tLSHIFTASSIGN tINC tDEFINE tCOMMA
%token tREM tRSHIFT tREMASSIGN tRSHIFTASSIGN tDEC tELLIPSIS tDOT tBWANDNOT tBWANDNOTASSIGN tLBRACKET tRBRACKET

/* associativity and precedence Levels */ 
%left tOR
%left tAND
%left tEQUALS tNE tGE tLE tGT tLT
%left tPLUS tMINUS tBWOR tBWXOR 
%left tTIMES tDIV 
%left tREM tLSHIFT tRSHIFT tBWAND tBWANDNOT
%left UNARY
%left tDOT tLBRACKET
%left tLPAREN tRPAREN

%start program
%% 

// weed out blank identifier 
program     : tPACKAGE tIDENTIFIER ';' declarations { root = makePROGRAM($2, $4); }
            ;
            
/* top level declarations */ 
declarations: declarations func_decl ';'            { $$ = $2; $$->next = $1; }
            | declarations type_decl ';'            { $$ = $2; $$->next = $1; }
            | declarations var_decl ';'             { $$ = $2; $$->next = $1; }
            | %empty                                { $$ = NULL; }
            ;
            
/* function declarations*/
func_decl   : tFUNC tIDENTIFIER signature type tLBRACE statement_list tRBRACE    { $$ = makeDECL_func(makeTERM_identifier($2), $3, $4, $6); }
            | tFUNC tIDENTIFIER signature tLBRACE statement_list tRBRACE         { $$ = makeDECL_func(makeTERM_identifier($2), $3, NULL, $5); }
            ;

signature   : tLPAREN parameter_list tRPAREN        { $$ = $2; }
            | tLPAREN tRPAREN                       { $$ = NULL; }
            ;
            
parameter_list : parameter                          { $$ = makePARAM($1); $$->next=NULL; }
            | parameter_list tCOMMA parameter       { $$ = makePARAM($3); $$->next=$1; }
            ;
            
parameter   : id_list type                          { $$ = makeVARDECL_type($1, $2); }
            ;

block_stmt  : tLBRACE statement_list tRBRACE        { $$ = makeSTMT_block($2);}
            ;

statement_list: %empty                              { $$ = NULL; }
            | statement_list statement ';'          { $$ = $2; $$->next=$1; }
            ;
     
/* type declarations */
type_decl: tTYPE t_decl                         { $$ = makeDECL_type($2); }
            | tTYPE tLPAREN type_list tRPAREN   { $$ = makeDECL_type($3); }
            ;

type_list : type_list t_decl ';'                { $$ = $2; $$->next = $1; }
            | %empty                            { $$ = NULL; }
            ;
        
t_decl: tIDENTIFIER type                        { $$ = makeTYPEDECL(makeTERM_identifier($1), $2); }
        ;                       
        
type: tIDENTIFIER                               { $$ = makeTYPE_single(makeTERM_identifier($1)); }                         
    | tLPAREN type tRPAREN                      { $$ = $2; }  
    | tLBRACKET tRBRACKET type                  { $$ = makeTYPE_slice($3); }      //slice
    | tLBRACKET tINTVAL tRBRACKET type          { $$ = makeTYPE_array($2, $4); }  //array
    | tSTRUCT tLBRACE field_decl tRBRACE        { $$ = makeTYPE_struct($3); }     //struct
    ; 
        
field_decl:  field_decl id_list type ';'        { $$ = makeSTRUCT_field($2, $3); $$->next = $1; }
            | %empty                            { $$ = NULL; }
            ;
        

/* var declarations*/
var_decl: tVAR v_decl                           { $$ = makeDECL_var($2); $$->next = NULL; }
            | tVAR tLPAREN decl_list tRPAREN    { $$ = makeDECL_var($3); } 
            ;
            
decl_list:  decl_list v_decl ';'                { $$ = $2; $$->next = $1; }
            | %empty                            { $$ = NULL; }
            ;
            
v_decl:     id_list type                        { $$ = makeVARDECL_type($1, $2); }
            | id_list tASSIGN expr_list         { $$ = makeVARDECL_exp($1, $3); }
            | id_list type tASSIGN expr_list    { $$ = makeVARDECL_both($1, $2, $4); }
            ;
            
id_list:    tIDENTIFIER tCOMMA id_list          { $$ = makeTERM_identifier($1); $$->next = $3; }
            | tIDENTIFIER                       { $$ = makeTERM_identifier($1); $$->next = NULL; }
            ;

            
/* statements */
statement   : empty_stmt                        { $$ = $1; }
            | assign_stmt                       { $$ = $1; }
            | exp_stmt                          { $$ = $1; }
            | var_decl                          { $$ = makeSTMT_vardecl($1); }
            | type_decl                         { $$ = makeSTMT_typedecl($1); }
            | short_decl                        { $$ = $1; }
            | print_stmt                        { $$ = $1; }
            | println_stmt                      { $$ = $1; }
            | inc_stmt                          { $$ = $1; }
            | dec_stmt                          { $$ = $1; }
            | return_stmt                       { $$ = $1; }
            | if_stmt                           { $$ = $1; }
            | switch_stmt                       { $$ = $1; }
            | for_stmt                          { $$ = $1; }
            | tBREAK                            { $$ = makeSTMT_break(); }
            | tCONTINUE                         { $$ = makeSTMT_continue(); }
            | block_stmt                        { $$ = $1; }
            ;
            
empty_stmt  : %empty                            { $$ = makeSTMT_empty(); }
            ;

assign_stmt : expr_list tASSIGN expr_list           { $$ = makeSTMT_assign($1, "=", $3); }
           | expression tMINUSASSIGN expression     { $$ = makeSTMT_assign($1, "-=", $3); }
           | expression tPLUSASSIGN expression      { $$ = makeSTMT_assign($1, "+=", $3); }
           | expression tTIMESASSIGN expression     { $$ = makeSTMT_assign($1, "*=", $3); }
           | expression tDIVASSIGN expression       { $$ = makeSTMT_assign($1, "/=", $3); }
           | expression tBWANDASSIGN expression     { $$ = makeSTMT_assign($1, "&=", $3); }
           | expression tBWANDNOTASSIGN expression  { $$ = makeSTMT_assign($1, "&^=", $3); }
           | expression tBWORASSIGN expression      { $$ = makeSTMT_assign($1, "|=", $3); }
           | expression tBWXORASSIGN expression     { $$ = makeSTMT_assign($1, "^=", $3); }
           | expression tLSHIFTASSIGN expression    { $$ = makeSTMT_assign($1, "<<=", $3); }
           | expression tRSHIFTASSIGN expression    { $$ = makeSTMT_assign($1, ">>=", $3); }
           | expression tREMASSIGN expression       { $$ = makeSTMT_assign($1, "%=", $3); }
            ;
            
exp_stmt : expression   { $$ = makeSTMT_exp($1); };
            ;
            
// weed: x[0] := 4 is invalid
short_decl  : expr_list tDEFINE expr_list       { $$ = makeSTMT_short($1, $3); }
            ;
            
print_stmt  : tPRINT tLPAREN expr_list tRPAREN  { $$ = makeSTMT_print($3); }
            | tPRINT tLPAREN tRPAREN            { $$ = makeSTMT_print(NULL); }
            ;
            
println_stmt : tPRINTLN tLPAREN expr_list tRPAREN    { $$ = makeSTMT_println($3); }
            | tPRINTLN tLPAREN tRPAREN               { $$ = makeSTMT_println(NULL); }
            ;

//weed out _            
inc_stmt    : expression tINC                   { $$ = makeSTMT_inc($1, true); }
            ;
     
//weed out _              
dec_stmt    : expression tDEC                   { $$ = makeSTMT_dec($1, true); }
            ;
            
return_stmt : tRETURN expression                { $$ = makeSTMT_return($2); }
            | tRETURN                           { $$ = makeSTMT_return(NULL); }
            ;
            
if_stmt     : tIF expression tLBRACE statement_list tRBRACE                          { $$ = makeSTMT_if(NULL, $2, $4, NULL); }
            | tIF expression tLBRACE statement_list tRBRACE else_stmt                { $$ = makeSTMT_if(NULL, $2, $4, $6); }
            | tIF simpleStmt ';' expression tLBRACE statement_list tRBRACE           { $$ = makeSTMT_if($2, $4, $6, NULL); }
            | tIF simpleStmt ';' expression tLBRACE statement_list tRBRACE else_stmt { $$ = makeSTMT_if($2, $4, $6, $8); }
            ;
            
else_stmt   : tELSE tIF expression tLBRACE statement_list tRBRACE else_stmt          { $$ = makeSTMT_elseif(NULL, $3, $5, $7); }
            | tELSE tIF expression tLBRACE statement_list tRBRACE                    { $$ = makeSTMT_elseif(NULL, $3, $5, NULL); }
            | tELSE tIF simpleStmt ';' expression tLBRACE statement_list tRBRACE     { $$ = makeSTMT_elseif($3, $5, $7, NULL); }
            | tELSE tIF simpleStmt ';' expression tLBRACE statement_list tRBRACE else_stmt { $$ = makeSTMT_elseif($3, $5, $7, $9); }
            | tELSE tLBRACE statement_list tRBRACE                                   { $$ = makeSTMT_else($3); }
            ;

simpleStmt  : empty_stmt | exp_stmt | dec_stmt | inc_stmt | assign_stmt | short_decl { $$ = $1; }
            ;

// weed for one default
switch_stmt : tSWITCH simpleStmt ';' expression tLBRACE exprCaseClauses tRBRACE     { $$ = makeSTMT_switch($2, $4, $6); }
            | tSWITCH simpleStmt ';' tLBRACE exprCaseClauses tRBRACE                { $$ = makeSTMT_switch($2, NULL, $5); }
            | tSWITCH expression tLBRACE exprCaseClauses tRBRACE                    { $$ = makeSTMT_switch(NULL, $2, $4); }
            | tSWITCH tLBRACE exprCaseClauses tRBRACE                               { $$ = makeSTMT_switch(NULL, NULL, $3); }
            ;

exprCaseClauses : exprCaseClauses tCASE expr_list tCOLON statement_list             { $$ = makeSWITCH_case($3, $5); $$->next = $1; }
            | exprCaseClauses tDEFAULT tCOLON statement_list                        { $$ = makeSWITCH_default($4); $$->next = $1; }
            | %empty                                                                { $$ = NULL; }
            ;
       
for_stmt    : tFOR block_stmt                                                       { $$ = makeSTMT_while(NULL, $2); }
            | tFOR expression block_stmt                                            { $$ = makeSTMT_while($2, $3); }
            | tFOR simpleStmt ';' expression ';' simpleStmt block_stmt              { $$ = makeSTMT_for($2, $4, $6, $7); }
            | tFOR simpleStmt ';' ';' simpleStmt block_stmt                         { $$ = makeSTMT_for($2, NULL, $5, $6); }
            ;

/* expressions */
expr_list : expression tCOMMA expr_list                                             { $$ = $1; $$->next = $3; }
            | expression                                                            { $$ = $1; }
            ;

//weed out _  
expression  : expression tPLUS expression                                           { $$ = makeEXP_binary($1, "+", $3); }
            | expression tMINUS expression                                          { $$ = makeEXP_binary($1, "-", $3); }
            | expression tTIMES expression                                          { $$ = makeEXP_binary($1, "*", $3); }
            | expression tEQUALS expression                                         { $$ = makeEXP_binary($1, "==", $3); }
            | expression tDIV expression                                            { $$ = makeEXP_binary($1, "/", $3); }
            | expression tOR expression                                             { $$ = makeEXP_binary($1, "||", $3); }
            | expression tAND expression                                            { $$ = makeEXP_binary($1, "&&", $3); }
            | expression tNE expression                                             { $$ = makeEXP_binary($1, "!=", $3); }
            | expression tGE expression                                             { $$ = makeEXP_binary($1, ">=", $3); }
            | expression tLE expression                                             { $$ = makeEXP_binary($1, "<=", $3); }
            | expression tGT expression                                             { $$ = makeEXP_binary($1, ">", $3); }
            | expression tLT expression                                             { $$ = makeEXP_binary($1, "<", $3); }
            | expression tBWOR expression                                           { $$ = makeEXP_binary($1, "|", $3); }
            | expression tBWAND expression                                          { $$ = makeEXP_binary($1, "&", $3); }
            | expression tBWXOR expression                                          { $$ = makeEXP_binary($1, "^", $3); }
            | expression tLSHIFT expression                                         { $$ = makeEXP_binary($1, "<<", $3); }
            | expression tREM expression                                            { $$ = makeEXP_binary($1, "%", $3); }
            | expression tRSHIFT expression                                         { $$ = makeEXP_binary($1, ">>", $3); }
            | expression tBWANDNOT expression                                       { $$ = makeEXP_binary($1, "&^", $3); }
            | tPLUS expression %prec UNARY                                          { $$ = makeEXP_unary($2, "+"); }
            | tMINUS expression %prec UNARY                                         { $$ = makeEXP_unary($2, "-"); }
            | tBWXOR expression %prec UNARY                                         { $$ = makeEXP_unary($2, "^"); }
            | tNOT expression %prec UNARY                                           { $$ = makeEXP_unary($2, "!"); }
            | tLPAREN expression tRPAREN                                            { $$ = makeEXP_paren($2); }
            | tAPPEND tLPAREN expression tCOMMA expression tRPAREN                  { $$ = makeEXP_append($3, $5); }
            | tLEN tLPAREN expression tRPAREN                                       { $$ = makeEXP_len($3); }
            | tCAP tLPAREN expression tRPAREN                                       { $$ = makeEXP_cap($3); }
            | expression tLPAREN expr_list tRPAREN                                  { $$ = makeEXP_func_call($1, $3); }  //func call w/param
            | expression tLPAREN tRPAREN                                            { $$ = makeEXP_func_call($1, NULL); }  //func call wo/param
            | expression tLBRACKET expression tRBRACKET                             { $$ = makeEXP_array_index($1, $3); }  //x[0]
            | expression tDOT tIDENTIFIER                                           { $$ = makeEXP_access_field($1, makeTERM_identifier($3)); }  //x[0].y should be okay too
            | tINTVAL                                                               { $$ = makeTERM_intLiteral($1); }
            | tFLOATVAL                                                             { $$ = makeTERM_floatLiteral($1); }
            | tRUNEVAL                                                              { $$ = makeTERM_runeLiteral($1); }
            | tRAWSTRINGVAL                                                         { $$ = makeTERM_rawStringLiteral($1); }
            | tINTERSTRINGVAL                                                       { $$ = makeTERM_stringLiteral($1); }
            | tIDENTIFIER                                                           { $$ = makeTERM_identifier($1); }
            ;
            

%%
