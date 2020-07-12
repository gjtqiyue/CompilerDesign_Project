%{
#include <stdio.h>
#include <stdlib.h>
#include "y.tab.h"
#include "main.h"
%}

DIGIT [0-9]
INT 0|([1-9]{DIGIT}*)
FLOAT {INT}\.({DIGIT}+)
ALL_INT ({DIGIT}*)
ALL_FLOAT ([0-9]*\.[0-9]+)

%option yylineno

/* Rule */
%%
[\r\n]+
[ \t]+ 			        

var 			        { if (token == 1) printf("tVAR\n"); return tVAR; }
float			        { if (token == 1) printf("tFLOAT\n"); return tFLOAT; }
int			            { if (token == 1) printf("tINT\n"); return tINT; }
string			        { if (token == 1) printf("tSTRING\n"); return tSTRING; }
bool			        { if (token == 1) printf("tBOOL\n"); return tBOOL; }
True			        { if (token == 1) printf("tTRUE\n"); return tTRUE; } 
False			        { if (token == 1) printf("tFALSE\n"); return tFALSE; }
if			            { if (token == 1) printf("tIF\n"); return tIF; }
else			        { if (token == 1) printf("tELSE\n"); return tELSE; }
while			        { if (token == 1) printf("tWHILE\n"); return tWHILE; }
read			        { if (token == 1) printf("tREAD\n"); return tREAD; }
print			        { if (token == 1) printf("tPRINT\n"); return tPRINT; }

":"			            { if (token == 1) printf("tCOLON\n"); return tCOLON; }
"="			            { if (token == 1) printf("tASSIGN\n"); return tASSIGN; }
";"			            { if (token == 1) printf("tSEMICOLON\n"); return tSEMICOLON; }

"+" 			        { if (token == 1) printf("tPLUS\n"); return tPLUS; }
"-" 			        { if (token == 1) printf("tMINUS\n"); return tMINUS; }
"*" 			        { if (token == 1) printf("tTIMES\n"); return tTIMES; }
"/" 			        { if (token == 1) printf("tDIV\n"); return tDIV; }
"==" 			        { if (token == 1) printf("tEQUAL\n"); return tEQUAL; }
"!=" 			        { if (token == 1) printf("tNOTEQUAL\n"); return tNOTEQUAL; }
">=" 			        { if (token == 1) printf("tGEQ\n"); return tGEQ; }
"<=" 			        { if (token == 1) printf("tLEQ\n"); return tLEQ; }
">" 			        { if (token == 1) printf("tGREATER\n"); return tGREATER; }
"<" 			        { if (token == 1) printf("tSMALLER\n"); return tSMALLER; }
"&&" 			        { if (token == 1) printf("tAND\n"); return tAND; }
"\|\|" 			        { if (token == 1) printf("tOR\n"); return tOR; }
"!"                     { if (token == 1) printf("tNOT\n"); return tNOT; }
"(" 			        { if (token == 1) printf("tLEFTPAREN\n"); return tLEFTPAREN; }
")" 			        { if (token == 1) printf("tRIGHTPAREN\n"); return tRIGHTPAREN; }
"{"			            { if (token == 1) printf("tLEFTCURLY\n"); return tLEFTCURLY; }
"}"			            { if (token == 1) printf("tRIGHTCURLY\n"); return tRIGHTCURLY; }

{INT}			        { yylval.intval = atoi(yytext); if (token == 1) printf("tINTVAL: %s\n", yytext); return tINTVAL; }
{FLOAT}			        { yylval.floatval = atof(yytext); if (token == 1) printf("tFLOATVAL: %s\n", yytext); return tFLOATVAL; }
(\"[^\"]*\")		    { yylval.stringval = strdup(yytext); if (token == 1) printf("tSTRINGVAL: %s\n", yytext); return tSTRINGVAL; }			
[a-zA-Z_][a-zA-Z0-9_]*	{ yylval.identifier = strdup(yytext); if (token == 1) printf("tIDENTIFIER: %s\n", yytext); return tIDENTIFIER; }

\#.*\n			        return tCOMMENT;

{ALL_INT}               { fprintf(stderr, "Error: (line %d) invalid integer '%s'\n", yylineno, yytext); exit(1); }
{ALL_FLOAT}             { fprintf(stderr, "Error: (line %d) invalid float '%s'\n", yylineno, yytext); exit(1); }
.			            { fprintf(stderr, "Error: (line %d) unexpected character '%s'\n", yylineno, yytext); exit(1); }
%%

int token;
