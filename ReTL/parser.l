%{
#include <stdio.h>
#include <string.h>
#include "parser.tab.h"
#include "func.h"
%}

%%
true 				{yylval.i = 1;return TRUE;}
false				{yylval.i = 0;return FALSE;}
and 				{return AND;}
or 					{return OR;}
not 				{return NOT;}
circ 				{return CIRC;}
next 				{return NEXT;}
until 				{return UNTIL;}
exists 				{return EXISTS;}
forall 				{return FORALL;}
[0-9]+				{yylval.i = atoi(yytext); return INT;}
[0-9]+\.[0-9]+		{yylval.d = atof(yytext); return FLOAT;}
\+					{return PLUS;}
\-					{return MINUS;}
\*					{return MUL;}
\/					{return DIV;}
\-\>				{return IMPLY;}
\!\=				{return NE;}
\<\=				{return LE;}
\<					{return LT;}
\>					{return GT;}
\>\=				{return GE;}
\= 					{return EQ;}
\*\*				{return POW;}
\[					{return LBRACK;}
\]					{return RBRACK;}
\( 					{return LPAREN;}
\)					{return RPAREN;}
\{					{return LBRACE;}
\}					{return RBRACE;}
\,					{return COMMA;}
\.					{return PERIOD;}
x[0-9]*				{yylval.vv = vLookUp(yytext, 0); return VEC_VAR;}
c[0-9]*				{yylval.sv = sLookUp(yytext); return SCA_VAR;}
v\+					{return VADD;}
v\-					{return VSUB;}
v\.					{return VDOT;}
\n 					{return EOL;}
end					{return EF;}
. 					{}

%%

int yywrap(){
	return 1;
}
