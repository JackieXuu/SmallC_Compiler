/******************************************************************
File name: header.h
This file is the header file which contains some necessary files.
This file defines a struct 'Address', a struct 'Quadruple', a struct 'link_reg'
Struct 'Address': record the address and some information of register.
Struct 'Quadruple': record the type of three address code.
Struct 'link_reg': used in optimization.
*******************************************************************/


#ifndef HEADER_H
#define HEADER_H

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstdarg>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <set>
#include <cstring>
#include <algorithm>
using namespace std;
const int MAX_LEN = 100001;
enum RegType {
    _LABEL, _CONSTANT, _TMP, _NAME
};

struct Address{
    RegType type;
    string name;
    int value;
    int real;
    int needload;
    int needclear;
};

struct Quadruple{
    string op; 
    int active; 
    int flag;
    Address parameters[3];
};

int quadruple_flag;
int function_begin_sp;
int main_flag;
int global_flag;
int label_count;
int stack_pointer;
int retad_pointer;
int local_register_count = 0;
int current_sp;
int tmp_num_var = 0;
int arrs_cnt = 0;
int arr_init_cnt = 0;
int interCode_level = 0;
int interCode_cnt[200];


vector <Quadruple> IR, GLOBAL_IR, ASSIGN_IR;
vector <int> RegState, RegOffset,LContinue, LBreak, vs_reg; 
vector <string> vs_id;
Quadruple create_quadruple();
Quadruple create_quadruple_arithmetic_im(char *s, int ra, int rb, int im);
Quadruple create_quadruple_function(char *s);
Quadruple create_quadruple_sw(int ra, int rb);
Quadruple create_quadruple_lw(int ra, int rb);
Quadruple create_quadruple_arithmetic(char *s, int ra, int rb, int rc);
Quadruple create_quadruple_goto(int value);
Quadruple create_quadruple_label();
Quadruple create_quadruple_call(char *s);
Quadruple create_quadruple_branch(char *s, int a, int b);
Quadruple create_quadruple_move(int ra, int rb);


typedef struct link_reg {
    int value;
    struct link_reg* next;
} link_reg;

typedef struct {
    link_reg* begin;
    link_reg* end;
} link_list;

int* next_pos;
int* prev_pos;
int real_register_begin = 11;
int real_register_end = 26;
int last_pos[MAX_LEN];
int reg_begin[MAX_LEN];
int reg_end[MAX_LEN];
int* label_to_ins;
link_reg* null;
link_list** in;
link_list** out;
link_list** def;
link_list** use;

link_reg* new_linked_list_node();
link_list** new_linked_list_n(int n);
link_list* new_linked_list();
void linked_list_insert(link_list* ptr, int value) ;
int linked_list_find(link_list* ptr, int value);
int cmp(const void* a, const void* b);
int optimize_merge_set(link_list* x, link_list* y) ;
int optimize_merge_set_1(link_list* x, link_list* y, link_list* z);
int optimize_calc_in_out(int i, int j);
void optimize_register_allocate(int i, int j);
void gengxin();
void optimization();




#endif