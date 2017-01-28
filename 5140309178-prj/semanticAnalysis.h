/***************************************************************
File name: semanticAnalysis.h
This file defines some functions to de the semantic analysis, 
if we meet some errors then we call error() function and terminate
the program.
***************************************************************/


#ifndef SEMANTICANALYSIS_H
#define SEMANTICANALYSIS_H
#include "include/symbolTable.h"

void semantic_check(TreeNode* t){
	if (!strcmp("PROGRAM", t->type)){
		_program(t);
	}
	else if (!strcmp("EXTDEFS", t->type)){
		_extdefs(t);
	}
	else if (!strcmp("EXTDEF", t->type)){
		_extdef(t);
	}
	else if (!strcmp("SEXTVARS", t->type)){
		_sextvars(t);
	}
	else if (!strcmp("EXTVARS", t->type)){
		_extvars(t);
	}
	else if (!strcmp("STSPEC", t->type)){
		_stspec(t);
	}
	else if (!strcmp("FUNC", t->type)){
		_paras(t);
	}
	else if (!strcmp("STMTS", t->type)){
		_stmts(t);
	}
	else if (!strcmp("STMT", t->type)){
		_stmt(t);
	}
	else if (!strcmp("STMTBLOCK", t->type)){
		_stmtblock(t);
	}
	else if (!strcmp("DEFS", t->type)){
		_defs(t);
	}
	else if (!strcmp("SDEFS", t->type)){
		_sdefs(t);
	}
	else if (!strcmp("DECS", t->type)){
		_decs(t);
	}
	else if (!strcmp("VAR", t->type)){
		_var(t);
	}
	else if (!strcmp("INIT", t->type)){
		_init(t);
	}
	else if (!strcmp("TYPE", t->type)){
		_type(t);
	}
	else if (!strcmp("EXP", t->type)){
		_exp(t, NULL);
	}
	else if (!strcmp("EXPS", t->type)){
		_exps(t);
	}
	else if (!strcmp("ARRS", t->type)){
		_arrs(t);
	}
	else if (!strcmp("OPERATION", t->type)||!strcmp("Keywords", t->type)||!strcmp("INT", t->type)||!strcmp("NULL", t->type)||!strcmp("EXPS UNARY", t->type)){

	}
	else{
		fprintf(stderr, "Can not find the type of %s\n", t->data); 
		exit(1);
	}
}


// PROGRAM : EXTDEFS
void _program(TreeNode* t){
	_extdefs(t->children[0]);
}

// EXTDEFS : EXTDEF EXTDEFS 
//         | e
void _extdefs(TreeNode* t){
	if (t->childNum != 0){
		_extdef(t->children[0]);
		_extdefs(t->children[1]);		
	}
}

/* First check the global variable
 * Then, if like struct a {}  struct_class[a]={all variable} struct_variable_num[a]={num}
 * else struct a b then we can know that we define a struct and b object, we should check whether we have a struct
 */

// EXTDEF : TYPE EXTVARS SEMI
//        | STSPEC SEXTVARS SEMI
//        | TYPE FUNC STMTBLOCK
void _extdef(TreeNode* t) {
	if (!strcmp("extvars", t->children[1]->data)){//global variable declearations
		_extvars(t->children[1]);
	}
	else if (!strcmp("stspec", t->children[0]->data)){//struct declarations
		_stspec(t->children[0]);
		if (t->children[0]->childNum==3) {
			symbolTable[level][num[level]].struct_class[t->children[0]->children[1]->data] = struct_vector[struct_vector_cnt];
			symbolTable[level][num[level]].struct_variable_num[t->children[0]->children[1]->data] = num_struct;
		}
		if (!strcmp("ID", t->children[0]->children[1]->type)&&t->children[0]->childNum == 2){
				_sextvars(t->children[1], t->children[0]->children[1]->data);
		}
		else{
			_sextvars(t->children[1]);
		}
	}
	else { //function declarations
		_func(t->children[1]);
		function_process = true;
		_stmtblock(t->children[2]);
		function_process = false;
	}
}


// SEXTVARS : ID
//			| ID COMMA SEXTVARS
// 			| e
void _sextvars(TreeNode* t){
	if (t->childNum == 0) return;
	if (_id(t->children[0]->data, t->children[0]->line, 1)){
		symbolTable[level][num[level]].table[t->children[0]->data] = "struct";
		symbolTable[level][num[level]].struct_object[t->children[0]->data] = struct_vector[struct_vector_cnt];
		symbolTable[level][num[level]].variable_num[t->children[0]->data] = num_struct;
		if (level > 0){
			symbolTable[level][num[level]].parent_index = num[level-1];
		}
		else{
			symbolTable[level][num[level]].parent_index = -1;
		}
	}
	else{
		error(t->children[0]->line, "Mutiple definition of ", t->children[0]->data);
	}
	if (t->childNum == 2) _sextvars(t->children[1]);
}

void _sextvars(TreeNode* t, char* str){
	if (t->children == 0) return;
	int tmp_i = level, tmp_j = num[level];
	while(1){
		if (symbolTable[tmp_i][tmp_j].struct_object.find(str) != symbolTable[tmp_i][tmp_j].struct_object.end()){
			break;
		}
		else{
			int parent_index = symbolTable[tmp_i][tmp_j].parent_index;
			if (parent_index == -1 || tmp_j == 0){
				error(t->children[0]->line, str, "structure not defined.");
			}
			else{
				tmp_i--;
				tmp_j = parent_index;
			}
		}
	}
	if (_id(t->children[0]->data, t->children[0]->line, 1)){
		symbolTable[level][num[level]].struct_object[t->children[0]->data] = symbolTable[tmp_i][tmp_j].struct_class[str];
		symbolTable[level][num[level]].table[t->children[0]->data] = "struct";
		symbolTable[level][num[level]].variable_num[t->children[0]->data] = symbolTable[level][num[level]].struct_variable_num[str];
	}	
	else{
		error(t->children[0]->line, "Mutiple definition of ", t->children[0]->data);
	}
	if (t->childNum == 2) _sextvars(t->children[1], str);
}

// EXTVARS : VAR
//		   | VAR ASSIGN INIT
//		   | VAR COMMA EXTVARS
//		   | VAR ASSIGN INIT COMMA EXTVARS
//		   | e

void _extvars(TreeNode *t) {
	if (t->childNum == 0) return;	
	else {
		int i;
		tmp_num = 1;
		array_tmp.clear();
		_var(t->children[0]);
		for (i = 1; i < t->childNum;++i) {
			semantic_check(t->children[i]);
		}
	}
	
}


// STSPEC : STRUCT ID LC SDEFS RC
//        | STRUCT LC SDEFS RC
//        | STRUCT ID
/* Lose struct ID */
void _stspec(TreeNode* t){
	++struct_cnt;
	if (!strcmp("ID", t->children[1]->data) && !(strcmp("sdefs", t->children[2]->data))){
		num_struct = 0;
		++struct_vector_cnt;
		_sdefs(t->children[2]);
	}
	else if (!strcmp("sdefs", t->children[1]->data)){
		num_struct = 0;
		++struct_vector_cnt;
		_sdefs(t->children[1]);
	}
}



// FUNC : ID LP PARAS RP
void _func(TreeNode* t){
	++func_cnt;
	if (_id(t->children[0]->data, t->children[0]->line, 0)){
		num_of_variable = 0;
		_paras(t->children[1]); /**/
		if (function_name_para_num.find(t->children[0]->data)==function_name_para_num.end()){
			/* new function name*/ 
			if (!strcmp(t->children[0]->data, "main")){
				have_main = true;
			} 
			function_name_para_num[t->children[0]->data] = func_vector[fuction_v_cnt];
			function_name_para_num[t->children[0]->data].push_back(num_of_variable);
			++fuction_v_cnt;			
		}
		else{ // overloaded function
			if (!vector_find(function_name_para_num[t->children[0]->data],num_of_variable)) { //overloaded function
		   		function_name_para_num[t->children[0]->data].push_back(num_of_variable);
				if (function_name_para_num[t->children[0]->data].size()>=2) {
					func_overload_table[t->children[0]->data][function_name_para_num[t->children[0]->data][0]] = t->children[0]->data;
					char *ch = t->children[0]->data;
					overload_function_name(t->children[0],num_of_variable);
					func_overload_table[ch][num_of_variable] = t->children[0]->data;
					function_name_para_num[t->children[0]->data] = func_vector[fuction_v_cnt];
					function_name_para_num[t->children[0]->data].push_back(num_of_variable);
					++fuction_v_cnt;
				}
			}
			else{
				error(t->children[0]->line, "Mutiple definition of ", t->children[0]->data);
			}
		}

	}
	else{
		error(t->children[0]->line, "Conflicting name of ", t->children[0]->data);
	}
}


// PARAS : TYPE ID COMMA PARAS
//       | TYPE ID
//       | e
void _paras(TreeNode* t){
	if (t->childNum == 0) return;
	++num_of_variable;
	if (symbolTable[level+1][num[level+1]+1].table.find(t->children[1]->data) == symbolTable[level+1][num[level+1]+1].table.end()){
		symbolTable[level+1][num[level+1]+1].table[t->children[1]->data] = "int";
		symbolTable[level+1][num[level+1]+1].variable_num[t->children[1]->data] = 1;
	}
	else{
		error(t->children[1]->line, "Mutiple definition of ", t->children[1]->data);
	}
	if (t->childNum == 3) _paras(t->children[2]);
}



// STMTBLOCK : LC DEFS STMTS RC
void _stmtblock(TreeNode* t) {
	++level;
	++num[level];
	if (level>0){
		symbolTable[level][num[level]].parent_index = num[level-1];
	}
	else{
		symbolTable[level][num[level]].parent_index = -1;
	} 	
	_defs(t->children[0]);
	_stmts(t->children[1]);
	--level;
}


// STMTS : STMT STMTS
//       | e

void _stmts(TreeNode* t) {
	int i;
	for (i = 0; i < t->childNum; ++i) {
		semantic_check(t->children[i]);
	}
}

// STMT : EXP SEMI
//      | STMTBLOCK
//      | RETURN EXP SEMI
//      | IF LP EXP RP STMT %prec LTIF
//      | IF LP EXP RP STMT ELSE STMT
//      | FOR LP EXP SEMI EXP SEMI EXP RP STMT
//      | CONT SEMI
//      | BREAK SEMI
void _stmt(TreeNode* t) {
	if (!strcmp("stmt exp",t->data)||!strcmp("stmt",t->data)) {
		int i;
		for (i = 0; i < t->childNum; ++i) {
			semantic_check(t->children[i]);
		}
	}
	else if (!strcmp("return stmt",t->data)) {
		if (!function_process) error(t->line, "return not in a function",NULL);
		_exps(t->children[1]);
	}
	else if (!strcmp("if stmt",t->data)) {
		int i;
		for (i = 0; i < t->childNum; ++i) {
			semantic_check(t->children[i]);
		}
	}
	else if (!strcmp("for stmt",t->data)) {
		int i;
		for (i = 0; i < t->childNum-1; ++i) {
			_exp(t->children[i],NULL);
		}
		for_loop = true;
		_stmt(t->children[t->childNum-1]);
		for_loop =  false;
	}
	else if (!strcmp("read stmt",t->data)) {
		left_value(t->children[0]);
	}
	else if( !strcmp("write stmt",t->data)) {
		_exps(t->children[0]);
	}
	else if ( !strcmp("break stmt", t->data) || !strcmp("continue stmt", t->data)){
		if (!for_loop) {
			error(t->children[0]->line, t->children[0]->data,"not in a loop");
		}
	}
}

// DEFS : TYPE DECS SEMI DEFS
//      | STSPEC SDECS SEMI DEFS
//      | e

void _defs(TreeNode* t) {
	int i;
	if (t->childNum==0) return;
	if (!strcmp(t->children[0]->type,"TYPE")) {
		for (i = 0; i < t->childNum; ++i) {
			semantic_check(t->children[i]);
		}
		return;
	}
	if (t->children[0]->childNum==2&&!strcmp(t->children[0]->data,"stspec")) {
			_stspec(t->children[0]);
			_sextvars(t->children[1], t->children[0]->children[1]->data);
			return;
		}
	for (i = 0; i < t->childNum; ++i) {
		semantic_check(t->children[i]);
	}
}

// SDEFS : TYPE SDECS SEMI SDEFS
//       | e
void _sdefs(TreeNode* t){
	int i;
	if (t->childNum == 0) return;
	else{
		_sdecs(t->children[1], 1);			/*loss */
		_sdefs(t->children[2]);
	}
}


// SDECS : ID COMMA SDECS
//       | ID

void _sdecs(TreeNode* t, int in_struct) {
	/* if in struct, then every struct should not have the same definination of ID define */
	/* I wonder what is the function of struct_vector */
	if (in_struct) {
		++num_struct;
		if (struct_set[struct_cnt].find(t->children[0]->data)==struct_set[struct_cnt].end()) {//no multiple declarations in a struct
			struct_set[struct_cnt].insert(t->children[0]->data);
			struct_vector[struct_vector_cnt].push_back(t->children[0]->data);
		}
		else {
			error(t->children[0]->line, "Mutiple definition of ", t->children[0]->data);
		}
		if (t->childNum==2) {
			_sdecs(t->children[1], in_struct);
		}
	}
	/* If not in struct, that is struct object then the ID define should not mutiple and it shouldn't the same as function name */
	/* add this id in table as struct, id implies the variable */
	/* width table as id to how many variable it has */
	else {
	if (_id(t->children[0]->data,t->children[0]->line, 1)) {
			symbolTable[level][num[level]].table[t->children[0]->data] = "struct";
			symbolTable[level][num[level]].struct_object[t->children[0]->data] = struct_vector[struct_vector_cnt];
			symbolTable[level][num[level]].variable_num[t->children[0]->data] = num_struct;
		if (level>0) symbolTable[level][num[level]].parent_index = num[level-1]; //which stmtblock in the upper level is my parent
		else symbolTable[level][num[level]].parent_index = -1;
		}
		else {
			error(t->children[0]->line, "Mutiple definition of ", t->children[0]->data);
		}
	}
}

// DECS : VAR
//      | VAR COMMA DECS
//      | VAR ASSIGN INIT COMMA DECS
//      | VAR ASSIGN INIT
void _decs(TreeNode* t){	
	int i;
	tmp_num = 1;
	array_tmp.clear();
	_var(t->children[0]);
	for (i = 1; i < t->childNum;++i) {
		semantic_check(t->children[i]);
	}
}

// VAR : ID
//     | VAR LB INT RB

/* First detect the num of dimension of array
 * Next if a[1][2] array_tmp{2 1}
 * table[a] = "int"
 * calculate every dimension need how many 4 bytes
 */

void _var(TreeNode *t) {
	if (t->childNum==1) {
		if (_id(t->children[0]->data,t->children[0]->line, 0)) {  //new variable name
			if (level==0&&function_name_para_num.find(t->children[0]->data)!=function_name_para_num.end()) {
				error(t->children[0]->line, "Mutiple definition of ", t->children[0]->data);
			}
			symbolTable[level][num[level]].table[t->children[0]->data] = "int";
			symbolTable[level][num[level]].variable_num[t->children[0]->data] = tmp_num;
			int i,j;
			vector <int> final_vector;
			for (i = 0; i < array_tmp.size(); ++i) {
				int t = 4;
				for (j = i+1; j < array_tmp.size(); ++j) {
					t *= array_tmp[i];
				}
				final_vector.push_back(t);
			}
			symbolTable[level][num[level]].array[t->children[0]->data] = final_vector;
		}
		else {error(t->children[0]->line, "Mutiple definition of ", t->children[0]->data);}
	}
	else {
		int intnum = strtoint(t->children[1]->data);
		if (intnum < 0) error(t->line, "index can't be less than zero",NULL);
		array_tmp.push_back(intnum);
		tmp_num *= intnum;
		_var(t->children[0]);
	}
}

// INIT : EXP
//      | LC ARGS RC

void _init(TreeNode *t) {
	if (!strcmp(t->data,"init")) {
		_exps(t->children[0]);
	}
	else {
		_args(t,NULL); //array initialization
	}
}

// EXP : EXPS
//     | e

void _exp(TreeNode *t,char* s) {
	int i;
	if (s!=NULL) {
		if (t->childNum!=0){
			++func_cnt_table[s];
		}
	}
	if (t->childNum>0) {
		_exps(t->children[0]);
	}
}

// EXPS : EXPS BINARYOP EXPS
//      | UNARYOP EXPS
//      | LP EXPS RP
//      | ID LP ARGS RP
//      | ID ARRS
//      | ID DOT ID
//      | INT
void _exps(TreeNode* t) {
	if (!strcmp(t->data,"exps f()")) {
		if (function_name_para_num.find(t->children[0]->data)==function_name_para_num.end()) { //don't find the function
			error(t->children[0]->line, t->children[0]->data,"not declared");
		}
		else { 
			int func_tmp = func_cnt_table[t->children[0]->data];
			func_cnt_table[t->children[0]->data] = 0;
			_args(t->children[1],t->children[0]->data);
		if (vector_find(function_name_para_num[t->children[0]->data],func_cnt_table[t->children[0]->data])) {
				int v_num = vector_index(function_name_para_num[t->children[0]->data],func_cnt_table[t->children[0]->data]);
				if (v_num>0) {
					int t_num = func_cnt_table[t->children[0]->data];
					overload_function_name(t->children[0],t_num); 
				}
			}
			else {
				error(t->children[0]->line, "number of arguments doesn't match: function", t->children[0]->data);
			}
			func_cnt_table[t->children[0]->data] = func_tmp;
		}
	}
	else if (!strcmp(t->data,"exps ()")) {
		_exps(t->children[0]);
	}
	else if (!strcmp(t->data,"exps arr")) { //intergers or arrays!!!!
		if (!find_id(t->children[0])) {
			error(t->children[0]->line, "No variable named",t->children[0]->data);
		}
		if (find_struct_id(t->children[0]->data,NULL)) {
				error(t->children[0]->line, t->children[0]->data,"is a struct");
		}
		if (t->children[1]->childNum==0) {
			if (num_of_var(t->children[0]->data)!=1) {
				error(t->children[0]->line, t->children[0]->data,"is not an integer");
			}
		}
		if (t->children[1]->childNum!=0) {  // it should be an array
			if (num_of_var(t->children[0]->data)==1) {
				error(t->children[0]->line, t->children[0]->data,"is not array");
			}
		}
	}
	else if (!strcmp(t->type,"EXPS UNARY")) { //unary operations
		if (!strcmp(t->data,"++")||!strcmp(t->data,"--")) {
			left_value(t->children[1]);
		}
		_exps(t->children[1]);
	}
	else if (!strcmp(t->data,"exps struct")) { //structs!!!
		int sjtu = find_struct_id(t->children[0]->data,t->children[2]->data);
		if (sjtu==0) {
			error(t->children[0]->line, "No such struct named", t->children[0]->data);
		}
		else if (sjtu==-1){
			error(t->children[2]->line, "struct has no varibale named", t->children[2]->data);
		}
	}
	else if (!strcmp(t->data,"=")) {
		left_value(t->children[0]);
		_exps(t->children[0]);
		_exps(t->children[1]);
	}
	else {
		int i;
		for (i = 0; i < t->childNum; ++i) {
			_exps(t->children[i]);
		}
	}
}

// ARRS : LB EXP RB ARRS
//      | e

void _arrs(TreeNode* t) { //arrays
	int i;
	for (i = 0; i < t->childNum; ++i) {
		semantic_check(t->children[i]);
	}
}

// ARGS : EXP COMMA ARGS
//      | EXP

void _args(TreeNode *t, char *s) { //arguments of function
	if (s==NULL) return;
	if (t->childNum==2) {
		_exp(t->children[0],s);//1 means it's the check process of arguemnts
		_args(t->children[1],s);
	}
	else {
		_exp(t->children[0],s);
	}
}


char* _type(TreeNode* t) {
	return t->data;
}

bool vector_find(vector<int> p, int num) {
	int i, sz = p.size();
	for (i = 0; i < sz; ++i) {
		if (p[i]==num) return true;
	}
	return false;
}

void overload_function_name(TreeNode* t, int num) {
	char *s = new char[strlen(t->data)+2];
	s[0] = '_';
	s[1] = num + '0';
	int i;
	for (i = 2; i < 2+strlen(t->data); ++i) {
		s[i] = t->data[i-2];
	}
	t->data = s;
}



bool _id(char* identifier,int line, int flag) { 
	if (checkReservedWord(identifier)) {
		error(line, identifier, " is reserved word" );
		return false;
	}
	if (symbolTable[level][num[level]].table.find(identifier)!=symbolTable[level][num[level]].table.end()){	//already exist
		error(line, "Mutiple definition of ",identifier);
		return false; 
	}
	if (flag){
		if (function_name_para_num.find(identifier)!=function_name_para_num.end()){
			error(line, identifier, " is a function defined before.");
			return false;
		}
	}

	return true; //new variable name
}



int vector_index(vector <int> s, int num) {
	int sz =  s.size();
	int i;
	for (i = 0; i < sz; ++i) {
		if (s[i]==num) return i;
	}
}



int find_struct_id(char *s,char *s2) {
	int tmp_level = level, tmp_depth = num[level];
        int bool_find = 0;
	while (1) {
if (symbolTable[tmp_level][tmp_depth].table.find(s)!=symbolTable[tmp_level][tmp_depth].table.end()&&!strcmp(symbolTable[tmp_level][tmp_depth].table[s],"struct")) { 
			//find
			if (s2==NULL) return 1;
			int i;
			for (i = 0; i < symbolTable[tmp_level][tmp_depth].struct_object[s].size(); ++i) {
				
				if (!strcmp(symbolTable[tmp_level][tmp_depth].struct_object[s][i],s2)) return 1;
			}
			return -1;
		}
		else {
			int parent = symbolTable[tmp_level][tmp_depth].parent_index;
			if (parent==-1||tmp_level==0) break;
			else {
				tmp_level--;
				tmp_depth = parent;
			}
		}
	}
	return bool_find;
}

bool find_id(TreeNode* t) {
	int tmp_level = level, tmp_depth = num[level];
	bool bool_find = false;
	while (1) {
		if (symbolTable[tmp_level][tmp_depth].table.find(t->data)!=symbolTable[tmp_level][tmp_depth].table.end()) { //find
			return true;
		}
		else {
			int parent = symbolTable[tmp_level][tmp_depth].parent_index;
			if (parent==-1||tmp_level==0) break;
			else {
				tmp_level--;
				tmp_depth = parent;
			}
		}
	}
	return bool_find;
}

void left_value(TreeNode* t) {
	if (!strcmp(t->data,"=")) {
		left_value(t->children[0]);
		_exps(t->children[1]);
	}
	else if (!strcmp(t->data,"exps arr")||!strcmp(t->data,"exps struct")||!strcmp(t->data,"exps ()")) {
		return;
	}
	else {
		error(t->line, "left value required as left operand assignment",NULL);
	}
}

void semantics(TreeNode* t) {
	semantic_check(t);
	if (!have_main) {
		fprintf(stderr,"fatal error! undefined reference to main()\n");
		exit(1);
	}
}

int num_of_var(char* s) {
	int tmp_level = level, tmp_depth = num[level];
	while (1) {
		if (symbolTable[tmp_level][tmp_depth].table.find(s)!=symbolTable[tmp_level][tmp_depth].table.end()) { //find
			return symbolTable[tmp_level][tmp_depth].variable_num[s];
		}
		else {
			int parent = symbolTable[tmp_level][tmp_depth].parent_index;
			if (parent==-1||tmp_level==0) assert(0);
			else {
				tmp_level--;
				tmp_depth = parent;
			}
		}
	}
}
 
int strtoint(const char *s) {
	assert(s!=NULL);
	if (strlen(s)>=2&&s[0]=='0'&&(s[1]=='x'||s[1]=='X')) return strtol(s,NULL,16);
	if (strlen(s)>=2&&s[0]=='0'&&s[1]!='x'&&s[1]!='X') return strtol(s,NULL,8);
	return strtol(s,NULL,10);	
}

bool checkReservedWord(const char * identifier){
	if (strcmp(identifier,"if")==0 || strcmp(identifier,"for") == 0 || strcmp(identifier,"break") == 0) return true;
	if (strcmp(identifier,"continue") == 0 || strcmp(identifier,"return") == 0 || strcmp(identifier,"void") == 0) return true;
	if (strcmp(identifier,"read") == 0 || strcmp(identifier,"write") == 0) return true;
	return false;
}

void error(int line, const char* s1, const char* s2){
	fprintf(stderr, "At line %d: ", line);
	if (s1 == NULL) fprintf(stderr, "%s\n", s2);
	else if (s2 == NULL) fprintf(stderr, "%s\n", s1);
	else fprintf(stderr, "%s %s\n", s1, s2);
	exit(1);
}
#endif
