%{
/*****************************************************************
File name: smallc.y
This file is the source file to construct a syntax analyzer and 
is used together with smallc.l.
The syntax analyzer receives tokens from the lexical analyzer and
report possible errors.
*****************************************************************/
#include "include/treeNode.h"
#include "include/header.h"
#include "lex.yy.c"
#include "semanticAnalysis.h"
#include "intermediateCode.h"
#include "optimization.h"
#include "codeGeneration.h"
using namespace std;
void yyerror(const char* msg);
extern int yylineno;
extern char* yytext;

%}

%union{
	struct TreeNode* node;
	char* string;
}
/* declare tokens */
%token <string> INT ID SEMI COMMA
%token <string> TYPE LP RP LB RB LC RC STRUCT RETURN
%token <string> IF ELSE BREAK CONT FOR DOT
%token <string> SHLASSIGN SHRASSIGN ORASSIGN XORASSIGN ANDASSIGN DIVASSIGN MULASSIGN MINUSASSIGN PULSASSIGN ASSIGN
%token <string> LOGICALOR LOGICALAND BITOR BITXOR BITAND NEQ EQ GT GE LT LE
%token <string> SHL SHR PLUS MINUS MUL DIV MOD LOGICALNOT PREINC PREDEC BITNOT UMINUS
%token <string> READ WRITE
/* declare type */
%type <node> PROGRAM EXTDEFS EXTDEF SEXTVARS EXTVARS STSPEC FUNC PARAS STMTBLOCK 
%type <node> STMTS STMT DEFS SDEFS SDECS DECS VAR INIT EXP EXPS ARRS ARGS

%nonassoc IFX 
%nonassoc ELSE

%right SHLASSIGN SHRASSIGN ORASSIGN XORASSIGN ANDASSIGN DIVASSIGN MULASSIGN MINUSASSIGN PULSASSIGN ASSIGN
%left LOGICALOR
%left LOGICALAND
%left BITOR
%left BITXOR
%left BITAND
%left NEQ EQ
%left GT GE LT LE
%left SHL SHR
%left PLUS MINUS
%left MUL DIV MOD
%right LOGICALNOT PREINC PREDEC BITNOT UMINUS
%left LP RP LB RB DOT
%start PROGRAM

%%
PROGRAM: 	EXTDEFS {root = $$ = new_node(yylineno, "PROGRAM", "program", 1, $1);}
		;

EXTDEFS:	EXTDEF EXTDEFS {$$ = new_node(yylineno, "EXTDEFS", "extdefs", 2, $1, $2);}
		| {$$ = new_node(yylineno, "NULL", "null", 0);}
		; 

EXTDEF:		TYPE EXTVARS SEMI {$$ = new_node(yylineno, "EXTDEF", "extdef", 2, new_node(yylineno, "TYPE", $1, 0), $2);}
		| STSPEC SEXTVARS SEMI {$$ = new_node(yylineno, "EXTDEF", "extdef", 2, $1, $2);}
		| TYPE FUNC STMTBLOCK {$$ = new_node(yylineno, "EXTDEF", "extdef", 3, new_node(yylineno, "TYPE", $1, 0), $2, $3);}
		;

SEXTVARS:	ID {$$ = new_node(yylineno, "SEXTVARS", "sextvars", 1, new_node(yylineno, "ID", $1, 0));}
		| ID COMMA SEXTVARS {$$ = new_node(yylineno, "SEXTVARS", "sextvars", 2, new_node(yylineno, "ID", $1, 0), $3);}
		| {$$ = new_node(yylineno, "NULL", "null", 0);}
		;

EXTVARS:	VAR {$$ = new_node(yylineno, "EXTVARS", "extvars", 1, $1);}
		| VAR ASSIGN INIT {$$ = new_node(yylineno, "EXTVARS", "extvars", 3, $1, new_node(yylineno, "OPERATION", $2, 0), $3);}
		| VAR COMMA EXTVARS {$$ = new_node(yylineno, "EXTVARS", "extvars", 2, $1, $3);}
		| VAR ASSIGN INIT COMMA EXTVARS {$$ = new_node(yylineno, "EXTVARS", "extvars",4, $1, new_node(yylineno, "OPERATION", $2, 0), $3, $5);}
		| {$$ = new_node(yylineno, "NULL", "null", 0);}
		;

STSPEC:		STRUCT ID LC SDEFS RC {$$ = new_node(yylineno, "STSPEC", "stspec", 3, new_node(yylineno, "Keywords", $1, 0), new_node(yylineno, "ID", $2, 0), $4);}
		| STRUCT LC SDEFS RC {$$ = new_node(yylineno, "STSPEC", "stspec", 2, new_node(yylineno, "Keywords", $1, 0), $3);}
		| STRUCT ID {$$ = new_node(yylineno, "STSPEC", "stspec", 2, new_node(yylineno, "Keywords", $1, 0), new_node(yylineno, "ID", $2, 0));}
		;

FUNC: 		ID LP PARAS RP {$$ = new_node(yylineno, "FUNC", "func", 2, new_node(yylineno, "ID", $1, 0), $3);}
		;

PARAS:		TYPE ID COMMA PARAS {$$ = new_node(yylineno, "PARAS", "paras", 3, new_node(yylineno, "TYPE", $1, 0), new_node(yylineno, "ID", $2, 0), $4);}
		| TYPE ID {$$ = new_node(yylineno, "PARAS", "paras", 2, new_node(yylineno, "TYPE", $1, 0), new_node(yylineno, "ID", $2, 0));}
		| {$$ = new_node(yylineno, "NULL", "null", 0);}
		;

STMTBLOCK:	LC DEFS STMTS RC {$$ = new_node(yylineno, "STMTBLOCK", "stmtblock", 2, $2, $3);}
		;

STMTS:		STMT STMTS {$$ = new_node(yylineno, "STMTS", "stmts", 2, $1, $2);}
		| {$$ = new_node(yylineno, "NULL", "null", 0);}
		;

STMT: 		EXP SEMI {$$ = new_node(yylineno, "STMT", "stmt exp", 1, $1);}
		| STMTBLOCK {$$ = new_node(yylineno, "STMT", "stmt", 1, $1);}
		| RETURN EXPS SEMI {$$ = new_node(yylineno, "STMT", "return stmt", 2, new_node(yylineno, "Keywords", $1, 0), $2);}
		| IF LP EXPS RP STMT %prec IFX {$$ = new_node(yylineno, "STMT", "if stmt", 2, $3, $5);}
		| IF LP EXPS RP STMT ELSE STMT %prec ELSE {$$ = new_node(yylineno, "STMT", "if stmt", 3, $3, $5,$7);}
		| FOR LP EXP SEMI EXP SEMI EXP RP STMT {$$ = new_node(yylineno, "STMT", "for stmt", 4, $3, $5, $7, $9);}
		| CONT SEMI {$$ = new_node(yylineno, "STMT", "continue stmt", 1, new_node(yylineno, "Keywords", $1, 0));}
		| BREAK SEMI {$$ = new_node(yylineno, "STMT", "break stmt", 1, new_node(yylineno, "Keywords", $1, 0));}
		| READ LP EXPS RP SEMI{$$ = new_node(yylineno,"STMT","read stmt",1, $3);}
		| WRITE LP EXPS RP SEMI{$$ = new_node(yylineno,"STMT","write stmt",1, $3);}
		;

DEFS: 		TYPE DECS SEMI DEFS {$$ = new_node(yylineno, "DEFS", "defs", 3, new_node(yylineno, "TYPE", $1, 0), $2, $4);}
		| STSPEC SDECS SEMI DEFS {$$ = new_node(yylineno, "DEFS", "defs", 3, $1, $2, $4);}
		| {$$ = new_node(yylineno, "NULL", "null", 0);}
		;
 
SDEFS: 		TYPE SDECS SEMI SDEFS {$$ = new_node(yylineno, "SDEFS", "sdefs", 3, new_node(yylineno, "TYPE", $1, 0), $2, $4);}
		| {$$ = new_node(yylineno, "NULL", "null", 0);}
		;

SDECS: 		ID COMMA SDECS {$$ = new_node(yylineno, "SDECS", "sdecs", 2, new_node(yylineno, "ID", $1, 0), $3);}
		| ID {$$ = new_node(yylineno, "SDECS", "sdecs", 1, new_node(yylineno, "ID", $1, 0));}
		;

DECS: 		VAR {$$ = new_node(yylineno, "DECS", "decs", 1, $1);}
		| VAR COMMA DECS {$$ = new_node(yylineno, "DECS", "decs", 2, $1, $3);}
		| VAR ASSIGN INIT COMMA DECS {$$ = new_node(yylineno, "DECS", "decs", 4, $1, new_node(yylineno, "OPERATION", $2, 0), $3, $5);}
		| VAR ASSIGN INIT {$$ = new_node(yylineno, "DECS", "decs", 3, $1, new_node(yylineno, "OPERATION", $2, 0), $3);}
		;

VAR: 		ID {$$ = new_node(yylineno, "VAR", "var", 1, new_node(yylineno, "ID", $1, 0));}
		| VAR LB INT RB {$$ = new_node(yylineno, "VAR", "var", 2, $1, new_node(yylineno, "INT", $3, 0));}
		;

INIT: 		EXPS {$$ = new_node(yylineno, "INIT", "init", 1, $1);}
		| LC ARGS RC {$$ = new_node(yylineno, "INIT", "init {}", 1, $2);}
		;

EXP: 		EXPS {$$ = new_node(yylineno, "EXP", "exp", 1, $1);}
		| {$$ = new_node(yylineno, "NULL", "null", 0);}
		;


EXPS: 		EXPS SHLASSIGN EXPS { $$ = new_node(yylineno,"EXPS", $2, 2, $1,$3); }
		| EXPS SHRASSIGN EXPS { $$ = new_node(yylineno,"EXPS", $2, 2, $1,$3); }
		| EXPS ORASSIGN EXPS { $$ = new_node(yylineno,"EXPS", $2, 2, $1,$3); }
		| EXPS XORASSIGN EXPS { $$ = new_node(yylineno,"EXPS", $2, 2, $1,$3); }
		| EXPS ANDASSIGN EXPS { $$ = new_node(yylineno,"EXPS", $2, 2, $1,$3); }
		| EXPS DIVASSIGN EXPS { $$ = new_node(yylineno,"EXPS", $2, 2, $1,$3); }
		| EXPS MULASSIGN EXPS { $$ = new_node(yylineno,"EXPS", $2, 2, $1,$3); }
		| EXPS MINUSASSIGN EXPS { $$ = new_node(yylineno,"EXPS", $2, 2, $1,$3); }
		| EXPS PULSASSIGN EXPS { $$ = new_node(yylineno,"EXPS", $2, 2, $1,$3); }
		| EXPS ASSIGN EXPS { $$ = new_node(yylineno,"EXPS", $2, 2, $1,$3); }
		| EXPS MUL EXPS { $$ = new_node(yylineno,"OPERATION", $2, 2, $1,$3); }
		| EXPS DIV EXPS { $$ = new_node(yylineno,"OPERATION", $2, 2, $1,$3); }
		| EXPS MOD EXPS { $$ = new_node(yylineno,"OPERATION", $2, 2, $1,$3); }
		| EXPS PLUS EXPS { $$ = new_node(yylineno,"OPERATION", $2, 2, $1,$3); }
		| EXPS MINUS EXPS { $$ = new_node(yylineno,"OPERATION", $2, 2, $1,$3); }
		| EXPS SHL EXPS { $$ = new_node(yylineno,"OPERATION", $2, 2, $1,$3); }
		| EXPS SHR EXPS { $$ = new_node(yylineno,"OPERATION", $2, 2, $1,$3); }
		| EXPS GT EXPS { $$ = new_node(yylineno,"OPERATION", $2, 2, $1,$3); }
		| EXPS GE EXPS { $$ = new_node(yylineno,"OPERATION", $2, 2, $1,$3); }
		| EXPS LT EXPS { $$ = new_node(yylineno,"OPERATION", $2, 2, $1,$3); }
		| EXPS LE EXPS { $$ = new_node(yylineno,"OPERATION", $2, 2, $1,$3); }
		| EXPS NEQ EXPS { $$ = new_node(yylineno,"OPERATION", $2, 2, $1,$3); }
		| EXPS EQ EXPS { $$ = new_node(yylineno,"OPERATION", $2, 2, $1,$3); }
		| EXPS BITAND EXPS { $$ = new_node(yylineno,"OPERATION", $2, 2, $1,$3); }
		| EXPS BITXOR EXPS { $$ = new_node(yylineno,"OPERATION", $2, 2, $1,$3); }
		| EXPS BITOR EXPS { $$ = new_node(yylineno,"OPERATION", $2, 2, $1,$3); }
		| EXPS LOGICALAND EXPS { $$ = new_node(yylineno,"OPERATION", $2, 2, $1,$3); }
		| EXPS LOGICALOR EXPS { $$ = new_node(yylineno,"OPERATION", $2, 2, $1,$3); }
		| BITNOT EXPS {$$ = new_node(yylineno,"EXPS UNARY", $1, 1, $2);}
		| LOGICALNOT EXPS {$$ = new_node(yylineno,"EXPS UNARY", $1, 1, $2);}
		| PREINC EXPS {$$ = new_node(yylineno,"EXPS UNARY", $1, 1, $2);}
		| PREDEC EXPS {$$ = new_node(yylineno,"EXPS UNARY", $1, 1, $2);}		
		| MINUS EXPS %prec UMINUS { $$ = new_node(yylineno,"EXPS", "exps", 2, $1,$2); }
		| LP EXPS RP { $$ = new_node(yylineno,"EXPS", "exps ()", 1, $2); }
		| ID LP ARGS RP { $$ = new_node(yylineno,"EXPS", "exps f()", 2, new_node(yylineno,"ID", $1, 0),$3); }
		| ID ARRS { $$ = new_node(yylineno,"EXPS", "exps arr", 2, new_node(yylineno,"ID", $1, 0),$2); }
		| ID DOT ID { $$ = new_node(yylineno,"EXPS", "exps struct", 3, new_node(yylineno,"ID", $1, 0),new_node(yylineno,"OPERATION", $2, 0),new_node(yylineno,"ID", $3, 0)); }
		| INT { $$ = new_node(yylineno,"INT", $1, 0); }
		;	

ARRS: 		LB EXP RB ARRS { $$ = new_node(yylineno,"ARRS", "arrs", 2, $2,$4); }
		| {$$ = new_node(yylineno, "NULL", "null", 0);}
		;

ARGS: 		EXP COMMA ARGS { $$ = new_node(yylineno,"ARGS", "args", 2, $1,$3); }
		| EXP { $$ = new_node(yylineno, "ARGS", "args", 1, $1); }
		;


%%

void yyerror(const char *msg)
{
	fflush(stdout);
	fprintf(stderr, "Error: %s at line %d\n", msg,yylineno);
	fprintf(stderr, "Parser does not expect '%s'\n",yytext);
}

int main(int argc, char *argv[]){
	if (argc == 1){
		fprintf(stderr, "\n%s\n", "Please write your code in the shell. Input <CTRL-C> to exit.");
		fprintf(stderr, "%s\n", "Or you can specify the source code path. \nExample --> $./scc InputFile OutputFile\n");
	}
	else if (argc == 2){
		FILE* fin = freopen(argv[1], "r", stdin);
		if (!fin) { 
			return fprintf (stderr, "Error: Input file %s does not exist!\n", argv[1]);
		}
		fprintf(stderr,"InputFile --> %s\n",argv[1]);
		freopen("MIPSCode.s", "w", stdout);
	}
	else if (argc == 3){
		FILE* fin = freopen(argv[1], "r", stdin);
		FILE* fout = freopen(argv[2], "w", stdout);
		if (!fin) { 
			return fprintf (stderr, "Error: Input file %s does not exist!\n", argv[1]);
		}
		fprintf(stderr,"-------------------\n");
		fprintf(stderr,"InputFile -> %s\n",argv[1]);
	}
	else {
		return fprintf (stderr, "Error: Invalid parameter number!\n");
	}

	clock_t start,end;
	double total_time_spent;

	start = clock();

	if (!yyparse()){
/*		printf("Abstract Syntax Tree: \n");
		print_tree(root);*/
		fprintf(stderr,"Parsing complete.\n");
		semantics(root);
		fprintf(stderr, "Complete semantic analysis! \n");
		intermediateCode();
		fprintf(stderr, "Complete intermediate code! \n");
		optimize();
		fprintf(stderr, "Complete optimization! \n");
		codeGeneration();
		fprintf(stderr, "Complete! \n");
	}
	else{
		printf("Error!");
	}
	
	end = clock();
	total_time_spent = (double)(end - start)* 1000 / CLOCKS_PER_SEC;
	fprintf(stderr, "Total time spent: %.2fms\n",total_time_spent );
	return 0; 
}

int yywrap()
{
	return 1;
}