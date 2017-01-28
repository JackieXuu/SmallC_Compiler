%{
/**********************************************************************************
File name: smallc.l
This file is the source file to construct a lexical analyzer.
It is also used together with smallc.y to construct a syntax analyzer.
The lexical analyzer reads in lexemes from source program and return corresponding 
tokens to the syntax analyzer. The line number is also recorded.
***********************************************************************************/

#include "include/header.h"
#include "y.tab.h"

extern int yyval, yylineno;
extern char* yytext;
void yyerror(const char* msg);
%} 
  
%%
[ \t\v\f]+ 					{/*Need no actions*/}
[\n]						{yylineno = yylineno + 1;}
"(" 						{yylval.string = strdup(yytext); return LP;}
")" 						{yylval.string = strdup(yytext); return RP;}
"[" 						{yylval.string = strdup(yytext); return LB;}
"]" 						{yylval.string = strdup(yytext); return RB;}
"{" 						{yylval.string = strdup(yytext); return LC;}
"}" 						{yylval.string = strdup(yytext); return RC;}
";" 						{yylval.string = strdup(yytext); return SEMI;}
"," 						{yylval.string = strdup(yytext); return COMMA;}
"int" 						{yylval.string = strdup(yytext); return TYPE;}
"struct" 					{yylval.string = strdup(yytext); return STRUCT;}
"return" 					{yylval.string = strdup(yytext); return RETURN;}
"if" 						{yylval.string = strdup(yytext); return IF;}
"else" 						{yylval.string = strdup(yytext); return ELSE;}
"break" 					{yylval.string = strdup(yytext); return BREAK;}
"continue"					{yylval.string = strdup(yytext); return CONT;}
"for" 						{yylval.string = strdup(yytext); return FOR;}
"read"						{yylval.string = strdup(yytext); return READ;}
"write" 					{yylval.string = strdup(yytext); return WRITE;}
"!"							{yylval.string = strdup(yytext); return LOGICALNOT;}
"++"						{yylval.string = strdup(yytext); return PREINC;}
"--"						{yylval.string = strdup(yytext); return PREDEC;}
"~"							{yylval.string = strdup(yytext); return BITNOT;}
"*"							{yylval.string = strdup(yytext); return MUL;}
"/"							{yylval.string = strdup(yytext); return DIV;}
"%"							{yylval.string = strdup(yytext); return MOD;}
"+"							{yylval.string = strdup(yytext); return PLUS;}
"-"							{yylval.string = strdup(yytext); return MINUS;}
"<<"						{yylval.string = strdup(yytext); return SHL;}
">>"						{yylval.string = strdup(yytext); return SHR;}
">"							{yylval.string = strdup(yytext); return GT;}
">="						{yylval.string = strdup(yytext); return GE;}
"<"							{yylval.string = strdup(yytext); return LT;}
"<="						{yylval.string = strdup(yytext); return LE;}
"=="						{yylval.string = strdup(yytext); return EQ;}
"!="						{yylval.string = strdup(yytext); return NEQ;}
"&"							{yylval.string = strdup(yytext); return BITAND;}
"^"							{yylval.string = strdup(yytext); return BITXOR;}
"|"							{yylval.string = strdup(yytext); return BITOR;}
"&&"						{yylval.string = strdup(yytext); return LOGICALAND;}
"||"						{yylval.string = strdup(yytext); return LOGICALOR;}
"="							{yylval.string = strdup(yytext); return ASSIGN;}
"+="						{yylval.string = strdup(yytext); return PULSASSIGN;}	
"-="						{yylval.string = strdup(yytext); return MINUSASSIGN;}	
"*="						{yylval.string = strdup(yytext); return MULASSIGN;}	
"/="						{yylval.string = strdup(yytext); return DIVASSIGN;}	
"&="						{yylval.string = strdup(yytext); return ANDASSIGN;}	
"^="						{yylval.string = strdup(yytext); return XORASSIGN;}	
"|="						{yylval.string = strdup(yytext); return ORASSIGN;}	
"<<="						{yylval.string = strdup(yytext); return SHLASSIGN;}
">>="						{yylval.string = strdup(yytext); return SHRASSIGN;}	
[a-zA-Z_][a-zA-Z0-9_]* 		{yylval.string = strdup(yytext); return ID;}
([0-9]*|0[xX][0-9a-fA-F]+) 	{yylval.string = strdup(yytext); return INT;}
.							{yyerror("Error!");}
%%