%{
    #include <stdio.h>
    #include <stdlib.h>
    extern int yylineno;
    void yyerror(const char *s) { 
        fprintf(stderr, "Error: (line %d) %s\n", yylineno, s);
	    exit(1); 
    }
    int yylex();
%}

%union {
    int intval;
    float floatval;
    char *stringval;
    char *identifier;
}

%token <identifier> tIDENTIFIER 
%token <intval> tINTVAL 
%token <floatval> tFLOATVAL 
%token <stringval> tSTRINGVAL
%token tVAR tINT tFLOAT tBOOL tSTRING tTRUE tFALSE tIF tELSE tWHILE tREAD tPRINT
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

%start program

%%
program : tCOMMENT program
        | if program
        | while program
        | dcl program
        | stmts program
        |
        ;

dcl : tVAR tIDENTIFIER tASSIGN exp tSEMICOLON
    | tVAR tIDENTIFIER tCOLON type tASSIGN exp tSEMICOLON
    ;
type : tINT | tFLOAT | tSTRING | tBOOL
    ;
if : tIF tLEFTPAREN exp tRIGHTPAREN tLEFTCURLY program tRIGHTCURLY else 
    ;
else : tELSE tLEFTCURLY program tRIGHTCURLY
    |
    ;
while : tWHILE tLEFTPAREN exp tRIGHTPAREN tLEFTCURLY program tRIGHTCURLY
    ;
stmts : tIDENTIFIER tASSIGN exp tSEMICOLON
    | read
    | print
    ;
read : tREAD tLEFTPAREN tIDENTIFIER tRIGHTPAREN tSEMICOLON
    ;
print : tPRINT tLEFTPAREN exp tRIGHTPAREN tSEMICOLON
    ;
exp : exp tPLUS exp
    | exp tMINUS exp
    | exp tTIMES exp
    | exp tDIV exp
    | tNOT exp
    | tMINUS exp %prec tUMINUS
    | tLEFTPAREN exp tRIGHTPAREN
    | exp tAND exp
    | exp tOR exp
    | exp tNOTEQUAL exp
    | exp tEQUAL exp
    | exp tGEQ exp
    | exp tLEQ exp
    | exp tGREATER exp
    | exp tSMALLER exp
    | term
    ;
term : tIDENTIFIER | tFLOATVAL | tINTVAL | tSTRINGVAL | tTRUE | tFALSE
    ;
