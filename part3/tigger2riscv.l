%{
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tigger2riscv.tab.hpp"

void yyerror(char*);
int lineno = 1;
%}

digit 		[0-9]
integer		(-)?{digit}+
letter 		[A-Za-z_]
Ident 		{letter}({letter}|{digit})*	

label      	l{integer}
varia 		v{integer}
func  		f_{Ident}
%%

[ \t\r\v\f]+	{ }
[\n]	{ lineno++; return EOL; }


"["	{ return ARR_L; }
"]"	{ return ARR_R; }

"malloc"	{ return MALLOC; }
"if"		{ return IF; }
"goto"		{ return GOTO; }
"call"		{ return CALL; }
"store"		{ return STORE; }
"load"		{ return LOAD; }
"loadaddr"	{ return LOADADDR; }
"return"	{ return RETURN; }
"end"		{ return END; }
":"		{ return COLON; }

"="	{ return ASSIGN; }
"+"	{ yylval.str = strdup(yytext); return OP; }
"-"	{ yylval.str = strdup(yytext); return OP; }
"*"	{ yylval.str = strdup(yytext); return OP; }
"/"	{ yylval.str = strdup(yytext); return OP; }
"%"	{ yylval.str = strdup(yytext); return OP; }
"&&"	{ yylval.str = strdup(yytext); return OP; }
"||"	{ yylval.str = strdup(yytext); return OP; }
"=="	{ yylval.str = strdup(yytext); return OP; }
"!="	{ yylval.str = strdup(yytext); return OP; }
"<"	{ yylval.str = strdup(yytext); return OP; }
">"	{ yylval.str = strdup(yytext); return OP; }
"!"	{ yylval.str = strdup(yytext); return OP; }

"a"[0-7]	{ yylval.str = strdup(yytext); return REG; }
"t"[0-6]	{ yylval.str = strdup(yytext); return REG; }
"s"[0-9]	{ yylval.str = strdup(yytext); return REG; }
"s1"[0-1]	{ yylval.str = strdup(yytext); return REG; }
"x0"		{ yylval.str = strdup(yytext); return REG; }

{label}		{ yylval.str = strdup(yytext); return LABEL; }
{func}		{ yylval.str = strdup(yytext); return FUNC; }
{varia}		{ yylval.str = strdup(yytext); return VAR; }
{integer}	{ yylval.val = atoi(yytext); return INT; }

"//"[^\n]*	{ }

. {
	char msg[100] = "unrecognized input ";
	strcat(msg, yytext);
	yyerror(msg);
}

%%

int yywrap()
{
	return 1;
}
