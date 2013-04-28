%{
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "command.h"
#include "make_cmd.h"
#include "y.tab.h"
#include "debug.h"
%}

ECHO	"echo"
WC	"wc"
CAT	"cat"
AWK	"awk"
LS	"ls"
TOUCH	"touch"
FGREP	"fgrep"
EGREP	"egrep"
OPTION  (-|--)[[:alpha:]]+ 
SPACE	[[:blank:]]+
WORD	([[:graph:]]{-}[<]{-}[>]{-}[|]{-}[;])+

%%
{ECHO}		{DEBUG(("\nECHO")); yylval.word=make_word(strdup(yytext)); return SCMD;}
{WC}		{DEBUG(("\nWC")); yylval.word=make_word(strdup(yytext)); return FCMD;}
{CAT}		{DEBUG(("\nCAT")); yylval.word=make_word(strdup(yytext)); return FCMD;}
{AWK}		{DEBUG(("\nAWK")); yylval.word=make_word(strdup(yytext)); return SCMD;}
{LS}		{DEBUG(("\nLS")); yylval.word=make_word(strdup(yytext)); return FCMD;}
{TOUCH}		{DEBUG(("\nTOUCH")); yylval.word=make_word(strdup(yytext)); return FCMD;}
{FGREP}		{DEBUG(("\nFGREP")); yylval.word=make_word(strdup(yytext)); return FSCMD;}
{EGREP}		{DEBUG(("\nEGREP")); yylval.word=make_word(strdup(yytext)); return FSCMD;}
{OPTION}	{DEBUG(("\nOPTION")); yylval.word=make_word(strdup(yytext)); return OPTION;} 
";"		{DEBUG((";"));return ';';}
"\n"		{DEBUG(("\\n"));return '\n';}
"<"		{DEBUG(("<"));return '<';}
">"		{DEBUG((">"));return '>';}
"|"		{DEBUG(("|"));return '|';}
{SPACE}		{;} 
{WORD}		{DEBUG(("\nSTRING")); yylval.word=make_word(strdup(yytext));return WORD;}
<<EOF>>		{DEBUG(("EOF"));return yacc_EOF;}
%%
int yywrap(void) 
{return 1;}
