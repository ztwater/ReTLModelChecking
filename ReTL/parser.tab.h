/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

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

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     AND = 258,
     OR = 259,
     NOT = 260,
     IMPLY = 261,
     NEXT = 262,
     UNTIL = 263,
     EXISTS = 264,
     FORALL = 265,
     PLUS = 266,
     MINUS = 267,
     MUL = 268,
     DIV = 269,
     POW = 270,
     VADD = 271,
     VSUB = 272,
     VDOT = 273,
     CIRC = 274,
     COMMA = 275,
     PERIOD = 276,
     LBRACK = 277,
     RBRACK = 278,
     LPAREN = 279,
     RPAREN = 280,
     LBRACE = 281,
     RBRACE = 282,
     EOL = 283,
     EF = 284,
     INT = 285,
     TRUE = 286,
     FALSE = 287,
     EQ = 288,
     NE = 289,
     LE = 290,
     LT = 291,
     GT = 292,
     GE = 293,
     FLOAT = 294,
     VEC_VAR = 295,
     SCA_VAR = 296
   };
#endif
/* Tokens.  */
#define AND 258
#define OR 259
#define NOT 260
#define IMPLY 261
#define NEXT 262
#define UNTIL 263
#define EXISTS 264
#define FORALL 265
#define PLUS 266
#define MINUS 267
#define MUL 268
#define DIV 269
#define POW 270
#define VADD 271
#define VSUB 272
#define VDOT 273
#define CIRC 274
#define COMMA 275
#define PERIOD 276
#define LBRACK 277
#define RBRACK 278
#define LPAREN 279
#define RPAREN 280
#define LBRACE 281
#define RBRACE 282
#define EOL 283
#define EF 284
#define INT 285
#define TRUE 286
#define FALSE 287
#define EQ 288
#define NE 289
#define LE 290
#define LT 291
#define GT 292
#define GE 293
#define FLOAT 294
#define VEC_VAR 295
#define SCA_VAR 296




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 16 "parser.y"
{
	struct ast* a;
	double d;
	int i;
	struct sList* sl;
	struct eList* el;
	struct scaVar * sv;
	struct vecVar * vv;
}
/* Line 1529 of yacc.c.  */
#line 141 "parser.tab.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

