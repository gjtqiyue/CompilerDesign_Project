/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    tIDENTIFIER = 258,
    tINTVAL = 259,
    tFLOATVAL = 260,
    tSTRINGVAL = 261,
    tVAR = 262,
    tINT = 263,
    tFLOAT = 264,
    tBOOL = 265,
    tSTRING = 266,
    tTRUE = 267,
    tFALSE = 268,
    tIF = 269,
    tELSE = 270,
    tWHILE = 271,
    tREAD = 272,
    tPRINT = 273,
    tCOLON = 274,
    tASSIGN = 275,
    tSEMICOLON = 276,
    tPLUS = 277,
    tMINUS = 278,
    tTIMES = 279,
    tDIV = 280,
    tEQUAL = 281,
    tNOTEQUAL = 282,
    tGEQ = 283,
    tLEQ = 284,
    tGREATER = 285,
    tSMALLER = 286,
    tAND = 287,
    tOR = 288,
    tNOT = 289,
    tLEFTPAREN = 290,
    tRIGHTPAREN = 291,
    tLEFTCURLY = 292,
    tRIGHTCURLY = 293,
    tUMINUS = 294,
    tCOMMENT = 295
  };
#endif
/* Tokens.  */
#define tIDENTIFIER 258
#define tINTVAL 259
#define tFLOATVAL 260
#define tSTRINGVAL 261
#define tVAR 262
#define tINT 263
#define tFLOAT 264
#define tBOOL 265
#define tSTRING 266
#define tTRUE 267
#define tFALSE 268
#define tIF 269
#define tELSE 270
#define tWHILE 271
#define tREAD 272
#define tPRINT 273
#define tCOLON 274
#define tASSIGN 275
#define tSEMICOLON 276
#define tPLUS 277
#define tMINUS 278
#define tTIMES 279
#define tDIV 280
#define tEQUAL 281
#define tNOTEQUAL 282
#define tGEQ 283
#define tLEQ 284
#define tGREATER 285
#define tSMALLER 286
#define tAND 287
#define tOR 288
#define tNOT 289
#define tLEFTPAREN 290
#define tRIGHTPAREN 291
#define tLEFTCURLY 292
#define tRIGHTCURLY 293
#define tUMINUS 294
#define tCOMMENT 295

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 12 "mini.y" /* yacc.c:1909  */

    int intval;
    float floatval;
    char *stringval;
    char *identifier;

#line 141 "y.tab.h" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


extern YYSTYPE yylval;
extern YYLTYPE yylloc;
int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
