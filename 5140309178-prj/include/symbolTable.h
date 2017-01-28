/***************************************************************
File name: symbolTable.h
This file defines a struct 'SymbolTable', which is used to record 
information of a variable when it is first defined and record some 
information like the number of variables in a certain type.
***************************************************************/

#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
#include "header.h"
#include "treeNode.h"

using namespace std;
const int MAX_NUM = 200;
struct ptr_cmp { 
	bool operator()(const char* s1, const char* s2) const {
		return strcmp(s1,s2) < 0;
	}
};
struct SymbolTable {
	map <const char*, const char*,ptr_cmp> table;
	map <char*, vector<char *>, ptr_cmp > struct_object;
	map <char*, vector<char *>, ptr_cmp > struct_class;
	map <char*, int, ptr_cmp > struct_variable_num;
	map <char*, int, ptr_cmp> variable_num; // records the num of "int"s, the actual width should be 4 times of that
	map <char*, vector<int>, ptr_cmp> array;
	int parent_index;//which record the index of his parent in the upper level; -1 means it's the gloabl scope
} symbolTable[MAX_NUM][MAX_NUM];

/* a map to map function name to its number of parameter */
map <char*, vector<int>, ptr_cmp > function_name_para_num;
/* a map to map function name to its parameter count */
map <char*, int, ptr_cmp> func_cnt_table;
/* a map to map overload function */
map <char*,map<int,char*>, ptr_cmp > func_overload_table;
/* a vector to store array */
vector <int> array_tmp;
/* a vector to store the function */
vector <int> func_vector[MAX_NUM];
/* a vector to store the struct */
vector <char*> struct_vector[MAX_NUM];
/* set to check the repeat function or struct */
set <char*, ptr_cmp> func_set[MAX_NUM];
set <char*, ptr_cmp> struct_set[MAX_NUM];

/* variables */
int level = 0,num[MAX_NUM];
int func_cnt = 0;
int struct_cnt = 0;
int num_of_variable = 0;
int fuction_v_cnt = 0;
int struct_vector_cnt = 0;
int tmp_num = 1;
int num_struct = 0;
bool function_process = false;
bool for_loop = false;
bool have_main = false;

int num_of_var(char* s);
int strtoint(const char *s);
bool checkReservedWord(const char * identifier);
void error(int line, const char* s1, const char* s2);
void semantic_check(TreeNode* t);
void _program(TreeNode* t);
void _extdefs(TreeNode* t);
void _extdef(TreeNode* t) ;
void _sextvars(TreeNode* t);
void _sextvars(TreeNode* t, char* str);
void _extvars(TreeNode *t);
void _stspec(TreeNode* t);
void _func(TreeNode* t);
void _paras(TreeNode* t);
void _stmtblock(TreeNode* t);
void _stmts(TreeNode* t);
void _stmt(TreeNode* t);
void _defs(TreeNode* t);
void _sdefs(TreeNode* t);
void _sdecs(TreeNode* t, int in_struct);
void _decs(TreeNode* t);
void _var(TreeNode *t);
void _init(TreeNode *t);
void _exp(TreeNode *t,char* s);
void _exps(TreeNode* t);
void _arrs(TreeNode* t); 
void _args(TreeNode *t, char *s);
char* _type(TreeNode* t); 
bool _id(char* identifier,int num, int flag);
bool vector_find(vector<int> p, int num);
void overload_function_name(TreeNode* t, int num);
int vector_index(vector <int> s, int num);
int find_struct_id(char *s,char *s2);
bool find_id(TreeNode* t);
void left_value(TreeNode* t);
void semantics(TreeNode* t);


#endif
