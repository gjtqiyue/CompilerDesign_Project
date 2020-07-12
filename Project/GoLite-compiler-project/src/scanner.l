%{
	#include <stdio.h>
	#include <stdbool.h>
	#include <string.h>
	#include "y.tab.h"
	extern bool tokens;
	#define YY_USER_ACTION yylloc.first_line = yylloc.last_line = yylineno;
	#define RETURN(token) lastToken = token; return token;
	//BLOCKCOMMENT \/\*(([^*\n]|\*+[^*/\n])*(\n)*([^*\n]|\*+[^*/\n])*)*\n*\*+\/
	int lastToken = -1;
	bool CheckSemicolon() {
		return (lastToken == tIDENTIFIER ||
			    lastToken == tINTVAL ||
			    lastToken == tFLOATVAL ||
			    lastToken == tRUNEVAL ||
			    lastToken == tINTERSTRINGVAL ||
			    lastToken == tRAWSTRINGVAL ||
			    lastToken == tBREAK ||
			    lastToken == tCONTINUE ||
			    lastToken == tFALLTHROUGH ||
			    lastToken == tRETURN ||
			    lastToken == tINC ||
			    lastToken == tDEC ||
			    lastToken == tRPAREN ||
			    lastToken == tRBRACKET ||
			    lastToken == tRBRACE 
			    );
	}

%}

%option yylineno
%option noinput
%option nounput

DECIMAL_DIGIT [0-9]
BINARY_DIGIT [0-1]
OCTAL_DIGIT [0-7]
HEX_DIGIT {DECIMAL_DIGIT}|[A-F]|[a-f]
DECIMAL 0|([1-9]{DECIMAL_DIGIT}*)
OCTAL {OCTAL_DIGIT}{OCTAL_DIGIT}*
HEX 0[xX]{HEX_DIGIT}{HEX_DIGIT}*
FLOAT {DECIMAL}*\.{DECIMAL_DIGIT}*

LETTER [a-zA-Z]|_
IDENTIFIER ({LETTER})({LETTER}|{DECIMAL_DIGIT})*
 
ESCAPE_SEQ [(\\a)(\\b)(\\f)(\\n)(\\t)(\\v)(\\r)(\\\\)]
RUNE \'([0-9a-zA-Z~!@#$%^&*()_+\=\-\|\{\}\[\]<>,.?/\:\;\"])\'

RAWSTRING \`[^\`]*\`
INTERSTRING \"([ 0-9a-zA-Z~!`@#$%^&*()_+\=\-\|\{\}\[\]<>,.?/\:\;\']|(\\a)|(\\b)|(\\f)|(\\n)|(\\t)|(\\v)|(\\r)|(\\\")|(\\\\))*\"

LINECOMMENT \/\/.*
BLOCKCOMMENT \/\*(([^*\n]|\*+[^*/\n])*)\*+\/
BLOCKCOMMENTNEWLINE \/\*([^*]|\*+[^*/])*\*+\/
UNCLOSEDCOMMENT \/\*([^*]|\*+[^*/])*

%%

{LINECOMMENT} 

{BLOCKCOMMENT}  

{BLOCKCOMMENTNEWLINE} 	{ 
							if (CheckSemicolon()) {
								lastToken = -1;
								if (tokens) printf("tSemicolon\n");
								return ';'; 
							} 
					  	}

{UNCLOSEDCOMMENT} { fprintf(stderr, "Error: (line %d) Unclosed comment block\n", yylineno); 
					exit(1); }

[\r\n] { 
			if (CheckSemicolon()) {
				lastToken = -1;
				if (tokens) printf("tSemicolon\n");
				return ';'; 
			}
		}

<<EOF>> { 
			if (CheckSemicolon()) {
				lastToken = -1;
				if (tokens) printf("tSemicolon\n");
				return ';'; 
			}
			else {
				yyterminate();
			}
		}
					
            	
[ \a\b\f\t\v]

break    {if (tokens) printf("tBREAK\n");  RETURN(tBREAK);}
case     {if (tokens) printf("tCASE\n"); return(tCASE);}
chan     {if (tokens) printf("tCHAN\n"); RETURN(tCHAN);}
const    {if (tokens) printf("tCONST\n"); RETURN(tCONST);}
continue {if (tokens) printf("tCONTINUE\n"); RETURN(tCONTINUE);}
default  {if (tokens) printf("tDEFAULT\n"); RETURN(tDEFAULT);}
defer    {if (tokens) printf("tDEFER\n"); RETURN(tDEFER);}
else  	 {if (tokens) printf("tELSE\n"); RETURN(tELSE);}
fallthrough {if (tokens) printf("tFALLTHROUGH\n"); RETURN(tFALLTHROUGH);}
for      {if (tokens) printf("tFOR\n"); RETURN(tFOR);}
func     {if (tokens) printf("tFUNC\n"); RETURN(tFUNC);}
go       {if (tokens) printf("tGO\n"); RETURN(tGO);}
goto     {if (tokens) printf("tGOTO\n"); RETURN(tGOTO);}
if  	 {if (tokens) printf("tIF\n"); RETURN(tIF);}
import   {if (tokens) printf("tIMPORT\n"); RETURN(tIMPORT);}
map      {if (tokens) printf("tMAP\n"); RETURN(tMAP);}
package  {if (tokens) printf("tPACKAGE\n"); RETURN(tPACKAGE);}
range    {if (tokens) printf("tRANGE\n"); RETURN(tRANGE);}
return   {if (tokens) printf("tRETURN\n"); RETURN(tRETURN);}
select   {if (tokens) printf("tSELECT\n"); RETURN(tSELECT);}
struct   {if (tokens) printf("tSTRUCT\n"); RETURN(tSTRUCT);}
switch   {if (tokens) printf("tSWITCH\n"); RETURN(tSWITCH);}
type     {if (tokens) printf("tTYPE\n"); RETURN(tTYPE);}
var  	 {if (tokens) printf("tVAR\n"); RETURN(tVAR);} 
print    {if (tokens) printf("tPRINT\n"); RETURN(tPRINT);} 
println  {if (tokens) printf("tPRINTLN\n"); RETURN(tPRINTLN);} 
append   {if (tokens) printf("tAPPEND\n"); RETURN(tAPPEND);} 
len      {if (tokens) printf("tLEN\n"); RETURN(tLEN);} 
cap      {if (tokens) printf("tCAP\n"); RETURN(tCAP);} 

"=" 	{if (tokens) printf("tASSIGN\n"); RETURN(tASSIGN);} 
"+"	    {if (tokens) printf("tPLUS\n"); RETURN(tPLUS);} 
"-"	    {if (tokens) printf("tMINUS\n"); RETURN(tMINUS);} 
"*"	    {if (tokens) printf("tTIMES\n"); RETURN(tTIMES);} 
"/"	    {if (tokens) printf("tDIV\n"); RETURN(tDIV);} 
"=="	{if (tokens) printf("tEQUALS\n"); RETURN(tEQUALS);} 
"!="	{if (tokens) printf("tNE\n"); RETURN(tNE);} 
">="	{if (tokens) printf("tGE\n"); RETURN(tGE);} 
"<="	{if (tokens) printf("tLE\n"); RETURN(tLE);} 
">"	    {if (tokens) printf("tGT\n"); RETURN(tGT);} 
"<"	    {if (tokens) printf("tLT\n"); RETURN(tLT);} 
"&&"	{if (tokens) printf("tAND\n"); RETURN(tAND);} 
"||"	{if (tokens) printf("tOR\n"); RETURN(tOR);} 
"!" 	{if (tokens) printf("tNOT\n"); RETURN(tNOT);}
"("	    {if (tokens) printf("tLEFTPAREN\n"); RETURN(tLPAREN);} 
")"	    {if (tokens) printf("tRIGHTPAREN\n"); RETURN(tRPAREN);} 
"{"	    {if (tokens) printf("tLEFTBRACE\n"); RETURN(tLBRACE);} 
"}"	    {if (tokens) printf("tRIGHTBRACE\n"); RETURN(tRBRACE);}
":"	    {if (tokens) printf("tCOLON\n"); RETURN(tCOLON);}
";"	    {if (tokens) printf("tSEMICOLON\n"); RETURN(';');}

"&"     {if (tokens) printf("tBWAND\n"); RETURN(tBWAND);}
"+="    {if (tokens) printf("tPLUSASSIGN\n"); RETURN(tPLUSASSIGN);}
"&="    {if (tokens) printf("tBWANDASSIGN\n"); RETURN(tBWANDASSIGN);}
"|"     {if (tokens) printf("tBWOR\n"); RETURN(tBWOR);}
"-="    {if (tokens) printf("tMINUSASSIGN\n"); RETURN(tMINUSASSIGN);}
"|="    {if (tokens) printf("tBWORASSIGN\n"); RETURN(tBWORASSIGN);}
"^"     {if (tokens) printf("tBWXOR\n"); RETURN(tBWXOR);}
"*="    {if (tokens) printf("tTIMESASSIGN\n"); RETURN(tTIMESASSIGN);}
"^="    {if (tokens) printf("tBWXORASSIGN\n"); RETURN(tBWXORASSIGN);}
"<-"    {if (tokens) printf("tARROW\n"); RETURN(tARROW);}
"<<"    {if (tokens) printf("tLSHIFT\n"); RETURN(tLSHIFT);}
"/="    {if (tokens) printf("tDIVASSIGN\n"); RETURN(tDIVASSIGN);}
"<<="   {if (tokens) printf("tLSHIFTASSIGN\n"); RETURN(tLSHIFTASSIGN);}
"++"    {if (tokens) printf("tINC\n"); RETURN(tINC);}
":="    {if (tokens) printf("tDEFINE\n"); RETURN(tDEFINE);}
","     {if (tokens) printf("tCOMMA\n"); RETURN(tCOMMA);}
"%"     {if (tokens) printf("tREM\n"); RETURN(tREM);}
">>"    {if (tokens) printf("tRSHIFT\n"); RETURN(tRSHIFT);}
"%="    {if (tokens) printf("tREMASSIGN\n"); RETURN(tREMASSIGN);}
">>="   {if (tokens) printf("tRSHIFTASSIGN\n"); RETURN(tRSHIFTASSIGN);}
"--"    {if (tokens) printf("tDEC\n"); RETURN(tDEC);}
"..."   {if (tokens) printf("tELLIPSIS\n"); RETURN(tELLIPSIS);}
"."     {if (tokens) printf("tDOT\n"); RETURN(tDOT);}
"&^"    {if (tokens) printf("tBWANDNOT\n"); RETURN(tBWANDNOT);}
"&^="   {if (tokens) printf("tBWANDNOTASSIGN\n"); RETURN(tBWANDNOTASSIGN);}
"["     {if (tokens) printf("tLBRACKET\n"); RETURN(tLBRACKET);}
"]"     {if (tokens) printf("tRBRACKET\n"); RETURN(tRBRACKET);}


{HEX} {
	char *temp;
	yylval.intval = strtol(yytext, &temp, 16);
	if (tokens) printf("tINTVAL(%s)\n", yytext); RETURN(tINTVAL);
}

{DECIMAL} {
	yylval.intval = atoi(yytext);
	if (tokens) printf("tINTVAL(%s)\n", yytext); RETURN(tINTVAL);
}

{OCTAL} {
	char *temp;
	yylval.intval = strtol(yytext, &temp, 8);
	if (tokens) printf("tINTVAL(%s)\n", yytext); RETURN(tINTVAL);
}

{FLOAT} {
	yylval.floatval = atof(yytext);
	if (tokens) printf("tFLOATVAL(%s)\n",yytext); RETURN(tFLOATVAL);
}

{IDENTIFIER} {
	yylval.idval = strdup(yytext);
	if (tokens) printf("tIDENTIFIER(%s)\n", yytext); RETURN(tIDENTIFIER);
}

\'((\\a)|(\\b)|(\\f)|(\\n)|(\\t)|(\\v)|(\\r)|(\\\\)|(\\\'))\' {
    switch (yytext[2]) {
        case 'a':
            yylval.runeval = '\a';
            break;
        case 'b':
            yylval.runeval = '\b';
            break;
        case 'f':
            yylval.runeval = '\f';
            break;
        case 'n':
            yylval.runeval = '\n';
            break;
        case 't':
            yylval.runeval = '\t';
            break;
        case 'v':
            yylval.runeval = '\v';
            break;
        case 'r':
            yylval.runeval = '\r';
            break;
        case '\\':
            yylval.runeval = '\\';
            break;
        case '\'':
            yylval.runeval = '\'';
            break;
    }
	if (tokens) printf("tRUNEESCAPE(\\%c)\n", (char)yytext[2]); RETURN(tRUNEVAL);
}

{RUNE} {
	yylval.runeval = strdup(yytext)[1];
	if (tokens) printf("tRUNEVAL(%s)\n", yytext); RETURN(tRUNEVAL);
}

{RAWSTRING} {
	char* str = strdup(yytext);
	char* str2 = malloc(2*strlen(str)*sizeof(char));
    int i = 1;
	int k = 0;
    while (*(str+i+1) != '\0') {
        if (*(str+i) == '\\'){
			switch (*(str+i+1)) {
				case 'a':
					*(str2+k) = '\\';
					*(str2+k+1) = '\\';
					*(str2+k+2) = 'a';
					break;
				case 'b':
					*(str2+k) = '\\';
					*(str2+k+1) = '\\';
					*(str2+k+2) = 'b';
					break;
				case 'f':
					*(str2+k) = '\\';
					*(str2+k+1) = '\\';
					*(str2+k+2) = 'f';
					break;
				case 'n':
					*(str2+k) = '\\';
					*(str2+k+1) = '\\';
					*(str2+k+2) = 'n';
					break;
				case 't':
					*(str2+k) = '\\';
					*(str2+k+1) = '\\';
					*(str2+k+2) = 't';
					break;
				case 'v':
					*(str2+k) = '\\';
					*(str2+k+1) = '\\';
					*(str2+k+2) = 'v';
					break;
				case 'r':
					*(str2+k) = '\\';
					*(str2+k+1) = '\\';
					*(str2+k+2) = 'r';
					break;
				case '\\':
					*(str2+k) = '\\';
					*(str2+k+1) = '\\';
					*(str2+k+2) = '\\';
					*(str2+k+3) = '\\';
					k+=1;
					break;
				case '\'':
					*(str2+k) = '\\';
					*(str2+k+1) = '\\';
					*(str2+k+2) = '\\';
					*(str2+k+3) = '\'';
					k+=1;
					break;
				case '\"':
					*(str2+k) = '\\';
					*(str2+k+1) = '\\';
					*(str2+k+2) = '\\';
					*(str2+k+3) = '\"';
					k+=1;
					break;
			}
			k+=2;
			i++;
        }
        else {
            *(str2+k) = *(str+i);
        }
        i++;
		k++;
    }
    *(str2+k) = '\0';
	yylval.rawstringval = strdup(str2);
	if (tokens) printf("tRAWSTRINGVAL(%s)\n", str2); RETURN(tRAWSTRINGVAL);
}

{INTERSTRING} {
	char* str = strdup(yytext);
	char* str2 = malloc(2*strlen(str)*sizeof(char));
	char* displayStr = malloc(2*strlen(str)*sizeof(char));
	*(displayStr) = '\"';
    int i = 1;
    int j = 0;
	int k = 1;
    while (*(str+i+1) != '\0') {
        if (*(str+i) == '\\'){
			switch (*(str+i+1)) {
				case 'a':
					*(str2+j) = '\a';
					i++;
					*(displayStr+k) = '~';
					*(displayStr+k+1) = 'a';
					break;
				case 'b':
					*(str2+j) = '\b';
					i++;
					*(displayStr+k) = '\\';
					*(displayStr+k+1) = 'b';
					break;
				case 'f':
					*(str2+j) = '\f';
					i++;
					*(displayStr+k) = '\\';
					*(displayStr+k+1) = 'f';
					break;
				case 'n':
					*(str2+j) = '\n';
					i++;
					*(displayStr+k) = '\\';
					*(displayStr+k+1) = 'n';
					break;
				case 't':
					*(str2+j) = '\t';
					i++;
					*(displayStr+k) = '\\';
					*(displayStr+k+1) = 't';
					break;
				case 'v':
					*(str2+j) = '\v';
					i++;
					*(displayStr+k) = '~';
					*(displayStr+k+1) = 'v';
					break;
				case 'r':
					*(str2+j) = '\r';
					i++;
					*(displayStr+k) = '\\';
					*(displayStr+k+1) = 'r';
					break;
				case '\\':
					*(str2+j) = '\\';
					i++;
					*(displayStr+k) = '\\';
					*(displayStr+k+1) = '\\';
					break;
				case '\'':
					*(str2+j) = '\'';
					i++;
					*(displayStr+k) = '\\';
					*(displayStr+k+1) = '\'';
					break;
				case '\"':
					*(str2+j) = '\"';
					i++;
					*(displayStr+k) = '\\';
					*(displayStr+k+1) = '\"';
					break;
			}
			k++;
        }
        else {
            *(str2+j) = *(str+i);
			*(displayStr+k) = *(str+i);
        }
        j++;
        i++;
		k++;
    }
    *(str2+j) = '\0';
    *(displayStr+k) = '\"';
	*(displayStr+k+1) = '\0';
	yylval.interstringval = strdup(displayStr);
	if (tokens) printf("tINTERSTRINGVAL(\"%s\")\n", displayStr); RETURN(tINTERSTRINGVAL);
}



. {
	fprintf(stderr, "Error: (line %d) unexpected character '%s\n'", yylineno, yytext); 
	exit(1);
}
%%