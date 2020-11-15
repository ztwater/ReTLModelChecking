/*
 * test.y
 * @brief syntactic parsing for ReTL
 * @author Tanghaoran Zhang
 * @date 2020-05-31
 */

%{
	#include <stdio.h>
	#include <stdlib.h>
	#include <math.h>
	#include <string.h>
	#include "func.h"
%}

%union {
	struct ast* a;
	double d;
	int i;
	struct sList* sl;
	struct eList* el;
	struct scaVar * sv;
	struct vecVar * vv;
}

// Terminal Symbols
%token AND OR NOT IMPLY NEXT UNTIL EXISTS FORALL
%token PLUS MINUS MUL DIV POW VADD VSUB VDOT CIRC
%token COMMA PERIOD LBRACK RBRACK LPAREN RPAREN LBRACE RBRACE
%token EOL EF

%token <i> INT TRUE FALSE 
%token <i> EQ NE LE LT GT GE
%token <d> FLOAT
%token <vv> VEC_VAR 
%token <sv> SCA_VAR

%left IMPLY
%left OR
%left AND
%left NE LE LT GT GE EQ
%left PLUS MINUS
%left MUL DIV
%left POW
%right NOT

%type <a> Primarys Unarys ScalarPowExpressions ScalarMulExpressions 
%type <a> ScalarAddExpressions ScalarExpressions Expressions
%type <a> LogicAndFormula LogicOrFormula ImplyFormula ReTLFormula
%type <i> CompareOperators
%type <sl> SLists SymbolLists
%type <el> ELists
%type <a> Vectors VectorAddExpressions VectorExpressions
%type <a> Variables

%%
Files:
	ReTLFormula EF
	{
		if (pass > 0) {
			printIndent();
			fprintf(fp, "e%d = s.unify(e%d, %d)\n", $1->lab, $1->lab, hier);
			printIndent();
			fprintf(fp, "e%d = s.AND(e%d, extra_inequalities)\n", $1->lab, $1->lab);
			printIndent();
			fprintf(fp, "res = s.q_solve(e%d, var_list, all_list, xst_list, %d, res)\n",$1->lab, hier);
			fprintf(fp, "s.check(res, all_list, xst_list, %d)\n", hier);
			fprintf(fp, "print(f'Program Executing Time: {time.time()-start}s')\n");
			printf("Finish parsing.\n");
			printf("Checking Formula: e%d\n\n", $1->lab);
		}
		freeAst($1);
	}
;

ReTLFormula:
	ImplyFormula 
	{
		$$ = $1;
	}
|	NEXT LPAREN ReTLFormula RPAREN
	{
		$$ = newAst('X', $3, NULL);
		$$->lab = ++cnt;
		$$->t = 0;
		getFormula($$);
	}
|	LPAREN ReTLFormula RPAREN UNTIL LPAREN ReTLFormula RPAREN
	{
		$$ = newAst('U', $2, $6);
		$$->lab = ++cnt;
		$$->t = 0;
		getFormula($$);
	}
|	FORALL Variables PERIOD LPAREN ReTLFormula RPAREN
	{
		$$ = newAst('O', $2, $5);
		$$->lab = ++cnt;
		$$->t = 0;
		getFormula($$);
	}
|	EXISTS Variables PERIOD LPAREN ReTLFormula RPAREN
	{
		$$ = newAst('T', $2, $5);
		$$->lab = ++cnt;
		$$->t = 0;
		getFormula($$);
	}
;

Variables:
	VEC_VAR LT INT GT
	{
		bindScalars($1, $3);
		$$ = newAst('v', (struct ast*)$1, NULL);
	}
|	SCA_VAR
	{
		if(pass == 0) fprintf(fp, "var_list.append(%s)\n", $1->name);
		$$ = newAst('s', (struct ast*)$1, NULL);
	}	
;

ImplyFormula:
	LogicOrFormula
	{
		$$ = $1;
	}
|	ImplyFormula IMPLY LogicOrFormula
	{
		$$ = newAst('Y', $1, $3);
		$$->lab = ++cnt;
		getFormula($$);
	}
;

LogicOrFormula:
	LogicAndFormula
	{
		$$ = $1;
	}
|	LogicOrFormula OR LogicAndFormula
	{
		$$ = newAst('|', $1, $3);
		$$->lab = ++cnt;
		getFormula($$);
	}
;

LogicAndFormula:
	Expressions
	{
		$$ = $1;
	}
|	LogicAndFormula AND Expressions
	{
		$$ = newAst('&', $1, $3);
		$$->lab = ++cnt;
		getFormula($$);
	}
;

Expressions:
	TRUE
	{
		$$ = newAst('t', newInt($1), NULL);
		$$->lab = ++cnt;
		$$->t = 0;
		getFormula($$);
	}
|	FALSE
	{
		$$ = newAst('f', newInt($1), NULL);
		$$->lab = ++cnt;
		$$->t = 0;
		getFormula($$);
	}
|	ScalarExpressions
	{
		$$ = newAst('K', $1, NULL);
		$$->lab = ++cnt;
		$$->t = 0;
		getFormula($$);
	}
|	VectorExpressions 
	{	
		$$ = newAst('B', $1, NULL);
		$$->lab = ++cnt;
		$$->t = 0;
		getFormula($$);
	}
|	NOT Expressions
	{
		$$ = newAst('!', $2, NULL);
		$$->lab = ++cnt;
		$$->t = 0;
		getFormula($$);
	}
|	LPAREN ReTLFormula RPAREN
	{
		$$ = $2;
	}
;

VectorExpressions:
	VectorAddExpressions SymbolLists VectorAddExpressions
	{
		$$ = newVecExpr('V', $1, $3, $2)
	}
;

VectorAddExpressions:
	Vectors
	{
		$$ = $1;
	}
|	VectorAddExpressions VADD Vectors
	{
		$$ = newAst('A', $1, $3);
		$$->lab = ++cnt;
		$$->t = 0;
		getFormula($$);
	}
|	VectorAddExpressions VSUB Vectors
	{
		$$ = newAst('S', $1, $3);
		$$->lab = ++cnt;
		$$->t = 0;
		getFormula($$);
	}
;

Vectors:
	VEC_VAR
	{
		$$ = newAst('v', (struct ast*)$1, NULL);
	}					
|	CIRC Vectors
	{
		$$ = newAst('C', $2, NULL);
		$$->lab = ++cnt;
		$$->t = $2->t+1;
		getFormula($$);
	}
|	LBRACK ELists
	{
		$$ = newAst('E', (struct ast*) $2, NULL);
		$$->lab = ++cnt;
		$$->t = 0;
		getFormula($$);
	}
| 	LPAREN VectorAddExpressions RPAREN
	{
		$$ = $2;
	}
;

ELists:
	ScalarExpressions COMMA ELists
	{
		$$ = newEList($1, $3);
	}
|	ScalarExpressions RBRACK
	{
		$$ = newEList($1, NULL);
	}
;

SymbolLists:
	LBRACK SLists  		
	{
		$$ = newSList(++cnt, $2);
		if (pass > 0) {
			printIndent();
			fprintf(fp, "e%d = [", cnt); 
			getSListFormula($2);
		}
	}
;

SLists:
	CompareOperators COMMA SLists
	{
		$$ = newSList($1, $3);

	}
|	CompareOperators RBRACK
	{
		$$ = newSList($1, NULL);
	}
;

ScalarExpressions:
	ScalarAddExpressions
	{
		$$ = $1;
	}
|	ScalarExpressions CompareOperators ScalarAddExpressions
	{
		$$ = newAst($2, $1, $3);
	}	
;

ScalarAddExpressions:
	ScalarMulExpressions
	{
		$$ = $1;
	}
|	ScalarAddExpressions PLUS ScalarMulExpressions
	{
		$$ = newAst('+', $1, $3);
	}		
|	ScalarAddExpressions MINUS ScalarMulExpressions
	{
		$$ = newAst('-', $1, $3);
	}	
;

ScalarMulExpressions:
	ScalarPowExpressions								
	{
		$$ = $1;
	}
|	ScalarMulExpressions MUL ScalarPowExpressions 	
	{
		$$ = newAst('*', $1, $3);
	}	
|	ScalarMulExpressions DIV ScalarPowExpressions
	{
		$$ = newAst('/', $1, $3);
	}	
;

ScalarPowExpressions:
	Unarys
	{
		$$ = $1;
	}						
|	ScalarPowExpressions POW Unarys 
	{
		$$ = newAst('P', $1, $3);
	}	
;
	
Unarys:
	Primarys
	{
		$$ = $1;
	}
|	MINUS Primarys
	{
		$$ = newAst('M', $2, NULL);
	}		
;

CompareOperators:
	EQ 		{$$ = '='}
| 	NE 		{$$ = 'N'}
| 	GE 		{$$ = 'G'}
| 	LE 		{$$ = 'L'}
| 	GT 		{$$ = '>'}
| 	LT 		{$$ = '<'}
;

Primarys:
	INT 
	{ 
		$$ = newInt($1);
	}	
|	FLOAT 	
	{ 
		$$ = newFloat($1);
	}
|	Vectors LBRACE INT RBRACE
	{
		$$ = newIndex($1, $3);
	}
|	Vectors VDOT Vectors
	{
		$$ = newAst('D', $1, $3);
	}
|	SCA_VAR
	{
		$$ = newAst('s', (struct ast*)$1, NULL);
	}
;

%%

int main(int argc, char** argv) {
	fp = fopen("output.py","a+");
	pass = 0;
	FILE * fr = fopen("input.txt","r");
	yyin = fr;
	cnt = 0;
	hier = 0;
	layer = atoi(argv[1]);
	lList = initLayers(layer);
	fprintf(fp, "var_list = []\n");
	fprintf(fp, "all_list = []\n");
	fprintf(fp, "xst_list = []\n");
	fprintf(fp, "extra_inequalities = []\n");
	yyparse();

	printLayers(lList);
	fclose(fr);

	pass++;

	fr = fopen("input.txt","r");
	yyin = fr;
	cnt = 0;
	indent = 0;
	if (hier == 1) fprintf(fp, "all_list, xst_list = xst_list, all_list\n");
	fprintf(fp, "if len(all_list) == 0:\n\tres = []\nelse:\n\tres = [[s.Reals]*len(all_list)]\n");

	yyparse();

	freeLayers(lList);
	fclose(fr);
	fclose(fp);
	return 0;
}

/* raise an error in yyparse*/
void yyerror (char const *s)
{
  fprintf(stderr, "Error encountered: %s\n", s);
}