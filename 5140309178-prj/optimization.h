/***************************************************************
File name: optimization.h
This file do some optimizations to eliminate dead code and register 
allocation.
***************************************************************/
#ifndef OPTIMIZATION_H
#define OPTIMIZATION_H

#include "include/header.h"
#include "intermediateCode.h"

void optimization() { 
    int i, j;
    Quadruple x, y;
    for (i = next_pos[0]; i < IR.size(); i = next_pos[i + 1]) {
        x = IR[i];
        if (!x.active || x.op!="move") {
            continue;
        }
        int value = x.parameters[0].value;
    if (value==-2) continue;
        int active_flag = 1;
        for (j = next_pos[i + 1]; j < IR.size(); j = next_pos[j + 1]) {
            y = IR[j];
            if (y.op=="goto" || y.op=="label") {
                active_flag = 2;
                break;
            }
            if (y.op=="sw") {
                if (y.parameters[1].type == _TMP && y.parameters[1].value == value) {
                    active_flag = 2;
                    break;
                }
            }
            if (y.parameters[1].type == _TMP && y.parameters[1].value == value) { // rundundant instructions
                y.parameters[1].value = x.parameters[1].value;
            }
            if (y.parameters[2].type == _TMP && y.parameters[2].value == value) {
                y.parameters[2].value = x.parameters[1].value;
            }
            if (y.parameters[0].type == _TMP && y.parameters[0].value == value) {
                active_flag = 0;
                break;
            }
        }
        if (active_flag == 2) {
            continue;
        }
        int flag = active_flag || (y.op!="sw");
        if (flag) {
            IR[i].active = 0;
        }
    }
    gengxin();

    for (i = next_pos[0]; i < IR.size(); i = next_pos[i + 1]) {
        x = IR[i];
        if (!x.active || x.op!="li") {
            continue;
        }
    if (x.parameters[0].value==-2) continue;
        j = next_pos[i + 1];
        if (j < IR.size()) {
            int value = x.parameters[0].value;
            y = IR[j];
            if (y.parameters[0].type == _TMP && y.parameters[0].value == value && //avoid redundant lis!
                y.parameters[2].type == _TMP && y.parameters[2].value == value) {
                IR[i].active = 0;
                IR[j].parameters[2].type = _CONSTANT;
                IR[j].parameters[2].value = x.parameters[1].value;
            }
        }
    }
    gengxin();
    
    for (i = 0; i < IR.size(); i++) {
        if (!IR[i].active) {
            continue;
        }
        x = IR[i];
        for (j = 0; j < 3; j++) {
            if (x.parameters[j].type == _TMP) {
                last_pos[x.parameters[j].value] = i;
            }
        }
    }
    for (i = 0; i < IR.size(); i++) {
        x = IR[i];
        if (!x.active || x.op!="li") {
            continue;
        }
    if (x.parameters[0].value==-2) continue;
        j = next_pos[i + 1];
        if (j < IR.size()) {
            int value = x.parameters[0].value;
            y = IR[j];
            if (y.parameters[2].type == _TMP && y.parameters[2].value == value) {//avoid redundant lis!
                if (last_pos[value] == j) {
                    IR[i].active = 0;
                    IR[j].parameters[2].type = _CONSTANT;
                    IR[j].parameters[2].value = x.parameters[1].value;
                }
            }
        }
    }
    gengxin();
}

link_reg* new_linked_list_node() {
    return (link_reg*)malloc(sizeof(link_reg));
}

link_list** new_linked_list_n(int n) {
    return (link_list**)malloc(sizeof(link_list*) * n);
}

void optimize_register_allocate(int i, int j) {
    static int which[32];
    memset(which, -1, sizeof(which));
    static int used[MAX_LEN] = {}, used_timestamp = 0;
    used_timestamp++;
    static int list[MAX_LEN], n;
    n = 0;
    static int loaded[MAX_LEN];
    static int real[MAX_LEN];
    int k, l;
    for (k = i; k <= j; k = next_pos[k + 1]) {
        Quadruple t = IR[k];
        for (l = 0; l < 3; l++) {
            if (t.parameters[l].type == _TMP) {
                int x = t.parameters[l].value;
                real[x] = -1;
                if (x != stack_pointer && x != retad_pointer && used[x] != used_timestamp) {
                    used[x] = used_timestamp;
                    list[n++] = x;
                    reg_begin[x] = k;
                    reg_end[x] = k;
                }
            }
        }
    }
    for (k = i; k <= j; k = next_pos[k + 1]) {
        link_reg* itr;
        for (itr = in[k]->begin; itr != in[k]->end; itr = itr->next) {
            int x = itr->value;
            if (k < reg_begin[x]) {
                reg_begin[x] = k;
            }
            if (k > reg_end[x]) {
                reg_end[x] = k;
            }
        }
        for (itr = out[k]->begin; itr != out[k]->end; itr = itr->next) {
            int x = itr->value;
            if (k < reg_begin[x]) {
                reg_begin[x] = k;
            }
            if (k > reg_end[x]) {
                reg_end[x] = k;
            }
        }
    }
    qsort(list, n, sizeof(int), cmp);
    for (k = 0; k < n; k++) {
        int x = list[k];
        int p = -1;
        for (l = real_register_begin; l < real_register_end; l++) {
            if (which[l] == -1) {
                p = l;
                break;
            }
            if (reg_end[which[l]] < reg_begin[x]) {
                p = l;
                break;
            }
        }
        if (p == -1) {
            p = real_register_begin;
            for (l = real_register_begin + 1; l < real_register_end; l++) {
                if (reg_end[which[l]] > reg_end[which[p]]) {
                    p = l;
                }
            }
            real[which[p]] = -1;
        }
        real[which[p] = x] = p;
        loaded[x] = 0;
    }
    for (k = 0; k < n; k++) {
        int x = list[k];
    }
    for (k = i; k <= j; k = next_pos[k + 1]) {
        Quadruple t = IR[k];
        for (l = 0; l < 3; l++) {
            if (t.parameters[l].type == _TMP) {
                int x = t.parameters[l].value;
                if (real[x] != -1) {
                    IR[k].parameters[l].real = real[x];
                    if (RegOffset[x] == -1 && !loaded[x]) {
                        loaded[x] = 1;
                        IR[k].parameters[l].needload = reg_begin[x] == i + 1;
                    }
                    if (reg_end[x] == k) {
                        IR[k].parameters[l].needclear = 1;
                    }
                }
            }
        }
    }
}

link_list* new_linked_list() {
    link_list* ptr = (link_list*)malloc(sizeof(link_list));
    ptr->begin = null;
    ptr->end = null;
    return ptr;
}

void linked_list_insert(link_list* ptr, int value) {
    if (value == stack_pointer || value == retad_pointer) {
        return;
    }
    link_reg* itr = new_linked_list_node();
    itr->value = value;
    itr->next = ptr->begin;
    ptr->begin = itr;
}



int cmp(const void* a, const void* b) {
    int x = *(int*)a;
    int y = *(int*)b;
    if (reg_begin[x] != reg_begin[y]) {
        return reg_begin[x] < reg_begin[y] ? -1 : 1;
    }
    if (reg_end[x] != reg_end[y]) {
        return reg_end[x] < reg_end[y] ? -1 : 1;
    }
    return 0;
}

int optimize_merge_set(link_list* x, link_list* y) {
    int ret = 0;
    link_reg* itr;
    for (itr = y->begin; itr != y->end; itr = itr->next) {
        if (!linked_list_find(x, itr->value)) {             //TO_SPEED_UP, using timestamp
            ret = 1;
            linked_list_insert(x, itr->value);
        }
    }
    return ret;
}

int optimize_merge_set_1(link_list* x, link_list* y, link_list* z) {
    static int visited[MAX_LEN] = {}, timestamp = 0;
    timestamp++;
    int ret = 0;
    link_reg* itr;
    for (itr = x->begin; itr != x->end; itr = itr->next) {
        visited[itr->value] = timestamp;
    }
    for (itr = z->begin; itr != x->end; itr = itr->next) {
        visited[itr->value] = timestamp;
    }
    for (itr = y->begin; itr != y->end; itr = itr->next) {
        if (visited[itr->value] != timestamp) {
            ret = 1;
            linked_list_insert(x, itr->value);
        }
    }
    return ret;
}

int optimize_calc_in_out(int i, int j) {
    int k, ret = 0;
    for (k = j; k >= i; k = prev_pos[k - 1]) {
        Quadruple t = IR[k];
        if (t.op=="ret" || t.op=="func") {
        } else if (t.op=="goto") {
            ret |= optimize_merge_set(out[k], in[label_to_ins[t.parameters[0].value]]);
        } else if (t.op=="beqz" ||t.op=="bnez" ||t.op=="bgez" ||t.op=="bgtz" ||t.op=="blez" || t.op=="bltz") {
            ret |= optimize_merge_set(out[k], in[next_pos[k + 1]]);
            ret |= optimize_merge_set(out[k], in[label_to_ins[t.parameters[1].value]]);
        } else {
            ret |= optimize_merge_set(out[k], in[next_pos[k + 1]]);
        }
        ret |= optimize_merge_set_1(in[k], out[k], def[k]);
    }
    return ret;
}



void gengxin() {
    int i;
    next_pos[IR.size()] = IR.size();
    for (i = IR.size() - 1; i >= 0; i--) {
        if (IR[i].active) {
            next_pos[i] = i;
        } else {
            next_pos[i] = next_pos[i + 1];
        }
    }
    prev_pos[-1] = -1;
    for (i = 0; i < IR.size(); i++) {
        if (IR[i].active) {
            prev_pos[i] = i;
        } else {
            prev_pos[i] = prev_pos[i - 1];
        }
    }
}

int linked_list_find(link_list* ptr, int value) {
    link_reg* itr;
    for (itr = ptr->begin; itr != ptr->end; itr = itr->next) {
        if (itr->value == value) {
            return 1;
        }
    }
    return 0;
}

void optimize() {
    next_pos = (int*)malloc(sizeof(int) * (IR.size() + 1));
    prev_pos = (int*)malloc(sizeof(int) * (IR.size() + 1)) + 1;
    gengxin();
    optimization();
}

#endif






