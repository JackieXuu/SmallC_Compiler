/***************************************************************
File name: codeGeneration.h
This file defines some functions which can translate the intermediate
code into MIPS code.
***************************************************************/
#ifndef CODEGENERATION_H
#define CODEGENERATION_H

#include "include/header.h"
#include "include/treeNode.h"
#include "intermediateCode.h"
#include "optimization.h"
const int MAX_LENGTH = 10000;

char* tempname = "temp";
char* varname = "var";
char* labelname = "l";
char ibuffer[MAX_LENGTH];
int in_main;
int which[32];

void register_allocation() {
    label_to_ins = (int*)malloc(sizeof(int) * IR.size());
    null = new_linked_list_node();
    null->next = null;
    in = new_linked_list_n(IR.size() + 1);
    out = new_linked_list_n(IR.size() + 1);
    def = new_linked_list_n(IR.size() + 1);
    use = new_linked_list_n(IR.size() + 1);
    int i, j, k;
    for (i = prev_pos[IR.size() - 1]; i >= 0; i = prev_pos[j - 1]) {
        for (j = i; j >= 0; j = prev_pos[j - 1]) {
            if (IR[j].op=="func") {
                break;
            }
        }
        assert(j >= 0);
        for (k = i; k >= j; k = prev_pos[k - 1]) {
            in[k] = new_linked_list();
            out[k] = new_linked_list();
            def[k] = new_linked_list();
            use[k] = new_linked_list();
            Quadruple t = IR[k];
            if (t.op=="label") {
                label_to_ins[t.parameters[0].value] = k;
            } else if (t.op=="func") {
            } else if (t.op=="call") {
            } else if (t.op=="ret") {
            } else if (t.op=="goto") {
            } else if (t.op=="beqz" ||t.op=="bnez" ||t.op=="bgez" ||t.op=="bgtz" ||t.op=="blez" ||t.op=="bltz") {
                linked_list_insert(use[k], t.parameters[0].value);
                linked_list_insert(in[k], t.parameters[0].value);
            } else if (t.op=="add" ||t.op=="sub" ||t.op=="mul" ||t.op=="div" ||t.op=="rem" ||t.op=="or" ||t.op=="xor" ||t.op=="and" ||t.op=="sll" ||t.op=="srl") {
                if (t.parameters[2].type == _TMP) {
                    linked_list_insert(use[k], t.parameters[2].value);
                    linked_list_insert(in[k], t.parameters[2].value);
                }
                linked_list_insert(use[k], t.parameters[1].value);
                linked_list_insert(in[k], t.parameters[1].value);
                int flag = t.parameters[1].value != t.parameters[0].value;
                if (t.parameters[2].type == _TMP) {
                    flag &= t.parameters[2].value != t.parameters[0].value;
                }
                if (flag) {
                    linked_list_insert(def[k], t.parameters[0].value);
                }
            } else if (t.op=="neg" ||t.op=="not" ||t.op=="lnot" ||t.op=="lw") {
                linked_list_insert(use[k], t.parameters[1].value);
                linked_list_insert(in[k], t.parameters[1].value);
                if (t.parameters[1].value != t.parameters[0].value) {
                    linked_list_insert(def[k], t.parameters[0].value);
                }
            } else if (t.op=="li") {
                linked_list_insert(def[k], t.parameters[0].value);
            } else if (t.op=="sw") {
                linked_list_insert(use[k], t.parameters[0].value);
                linked_list_insert(in[k], t.parameters[0].value);
                linked_list_insert(use[k], t.parameters[1].value);
                linked_list_insert(in[k], t.parameters[1].value);
            } else if (t.op=="move") {
                linked_list_insert(use[k], t.parameters[1].value);
                linked_list_insert(in[k], t.parameters[1].value);
                if (t.parameters[1].value != t.parameters[0].value) {
                    linked_list_insert(def[k], t.parameters[0].value);
                }
            } else {
        cout << t.op << endl;
                assert(0);
            }
        }
        while (optimize_calc_in_out(j, i));
        optimize_register_allocate(j, i);
    }
}


int get_register_id(Address ptr, int k) {
    if (ptr.value == -2) {
    return 4;
    }
    if (ptr.value == -3) {
    return 2;
    }
    if (ptr.real != -1) {
        return ptr.real;
    }
    if (ptr.value == stack_pointer) {
        return 29;
    }
    if (ptr.value == retad_pointer) {
        return 31;
    }
    if (ptr.value == -3) {
    return 2;
    }
    return k;
}

int interprete_fetch_register(Address ptr, int k) {
    if (ptr.value == -2) {
    return 4;
    }
    if (ptr.value == -3) {
    return 2;
    }
    if (ptr.real != -1) {
        return ptr.real;
    }
    int reg = ptr.value;
    if (ptr.value == -3) {
    return 2;
    }
    if (reg == stack_pointer) {
        return 29;
    } else if (reg == retad_pointer) {
        return 31;
    } else {
        if (RegOffset[reg] == -1) {
            printf("\t lw $%d, %d($a3)\n", k, reg * 4);
        } else {
            printf("\t lw $%d, %d($sp)\n", k, -RegOffset[reg]);
        }
    }
    return k;
}

void interprete_store_register(char* s, int reg) {
    if (reg == -2) {
    printf("\t move $a0, %s\n", s);
    }
    if (reg == -3) {
    printf("\t move $v0, %s\n", s);
    }
    if (reg == stack_pointer) {
        printf("\t move $sp, %s\n", s);
    } else if (reg == retad_pointer) {
        printf("\t move $ra, %s\n", s);
    } 
    else {
        if (RegOffset[reg] == -1) {
            printf("\t sw %s, %d($a3)\n", s, reg * 4);
        } else {
            printf("\t sw %s, %d($sp)\n", s, -RegOffset[reg]);
        }
    }
}

void interprete_global() {
        printf(".data\n");
        printf("%s: .space %d\n", tempname, RegState.size() * 4);
        int i;
        int count = 0;
        for (i = 0; i < GLOBAL_IR.size(); i++) {
            if (GLOBAL_IR[i].op=="gnew") {
                    printf("%s%d: .space %d\n", varname, count++, GLOBAL_IR[i].parameters[0].value);
            } else {
                    printf("%s%d: .asciiz \"%s\"\n", varname, count++, GLOBAL_IR[i].parameters[0].name.c_str());
                    printf(".align 2\n");
            }
        }
}

void interprete_generate_mips(Quadruple tuple,int delta) {
    string s = tuple.op;
    int i;
    for (i = 0; i < 3; i++) {
            if (tuple.parameters[i].real != -1&&tuple.parameters[i].value!=-1) {
                    which[tuple.parameters[i].real] = tuple.parameters[i].value;
            }
            if (tuple.parameters[i].needload) {
                    printf("\t lw $%d, %d($a3)\n", tuple.parameters[i].real, 4 * tuple.parameters[i].value);
            }
    }
    if (s=="label") {
            printf("%s%d:\n", labelname, tuple.parameters[0].value);
        } else if (s=="func") { 
            memset(which, -1, sizeof(which));
            printf("%s:\n", tuple.parameters[0].name.c_str());
    } else if (s=="ret") {
            if (in_main) {
                    printf("\t j End\n");
            } else {
                    printf("\t jr $ra\n");
            }
        } else if (s=="goto") {
            printf("\t j %s%d\n", labelname, tuple.parameters[0].value);
    } else if (s=="beqz"||s=="bnez" || s=="bgez" || s=="bgtz" || s=="blez" || s=="bltz") {
            int src = interprete_fetch_register(tuple.parameters[0], 8); //$t0
            printf("\t %s $%d, %s%d\n", s.c_str(), src, labelname, tuple.parameters[1].value);
        } else if (s=="add"||s=="sub"||s=="mul"||s=="div"||s=="rem"||s=="or"||s=="xor"||s=="and"||s=="sll"||s=="srl" ) {
                int dest = get_register_id(tuple.parameters[0], 8);
                int src1 = interprete_fetch_register(tuple.parameters[1], 9);
                if (tuple.parameters[2].type == _CONSTANT) {
                        printf("\t %s $%d, $%d, %d\n", s.c_str(), dest, src1, tuple.parameters[2].value);
                } else {
                        int src2 = interprete_fetch_register(tuple.parameters[2], 10);
                        printf("\t %s $%d, $%d, $%d\n", s.c_str(), dest, src1, src2);
                }
                if (dest == 8) {
                        interprete_store_register("$8", tuple.parameters[0].value);
                }
        } else if (s=="neg"|| s=="not") {
            int src = interprete_fetch_register(tuple.parameters[1], 9);
            int dest = get_register_id(tuple.parameters[0], 8);
            printf("\t %s $%d, $%d\n", s.c_str(), dest, src);
            if (dest == 8) {
                    interprete_store_register("$8", tuple.parameters[0].value);
            }
        } else if (s=="lnot") {
            static int lnot_count = 0;
            int src = interprete_fetch_register(tuple.parameters[1], 9);
            int dest = get_register_id(tuple.parameters[0], 8);
            if (dest != src) {
                    printf("\t li $%d, 1\n", dest);
                    printf("\t beqz $%d, lnot%d\n", src, lnot_count);
                    printf("\t li $%d, 0\n", dest);
                    printf("\t lnot%d:\n", lnot_count);
                    if (dest == 8) {
                        interprete_store_register("$8", tuple.parameters[0].value);
                    }
            } else {
                    printf("\t move $a1, $%d\n", dest);
                    printf("\t li $%d, 1\n", dest);
                    printf("\t beqz $a1, lnot%d\n", lnot_count);
                    printf("\t li $%d, 0\n", dest);
                    printf("\t lnot%d:\n", lnot_count);
                    if (dest == 8) {
                        interprete_store_register("$8", tuple.parameters[0].value);
                    }
            }
            lnot_count++;
        } else if (s=="li") {
            int dest = get_register_id(tuple.parameters[0], 8);
            printf("\t li $%d, %d\n", dest, tuple.parameters[1].value);
            if (dest == 8) {
                    interprete_store_register("$8", tuple.parameters[0].value);
         }
        } else if (s=="lw") {
            int src = interprete_fetch_register(tuple.parameters[1], 9);
            int dest = get_register_id(tuple.parameters[0], 8);
            printf("\t %s $%d, %d($%d)\n", s.c_str(), dest, tuple.parameters[2].value, src);
            if (dest == 8) {
                    interprete_store_register("$8", tuple.parameters[0].value);
            }
        } else if (s=="sw") {
            int src = interprete_fetch_register(tuple.parameters[0], 8);
            int dest = interprete_fetch_register(tuple.parameters[1], 9);
            printf("\t %s $%d, %d($%d)\n", s.c_str(), src, tuple.parameters[2].value, dest);
        } else if (s=="move") {
            int src = interprete_fetch_register(tuple.parameters[1], 9);
            int dest = get_register_id(tuple.parameters[0], 8);
            printf("\t move $%d, $%d\n", dest, src);
            if (dest == 8) {
                interprete_store_register("$8", tuple.parameters[0].value);
            }
        } else if (s=="call") {
        int flag = tuple.parameters[0].name!="__read";
            flag &= tuple.parameters[0].name!="__write";
            if (flag) {
                    for (i = real_register_begin; i < real_register_end; i++) {
                        if (which[i] != -1) {   
                                int x = which[i];
                                assert(x != stack_pointer && x != retad_pointer);
                                if (RegOffset[x] == -1) {
                                    printf("\t sw $%d, %d($a3)\n", i, x * 4);
                                } else {
                                    printf("\t sw $%d, %d($sp)\n", i, delta - RegOffset[x]);
                                }
                        }
                    }
            }
            printf("\t jal %s\n", tuple.parameters[0].name.c_str());
            if (flag) {
                    for (i = real_register_begin; i < real_register_end; i++) {
                        if (which[i] != -1) {
                                int x = which[i];
                                if (RegOffset[x] == -1) {
                                    printf("\t lw $%d, %d($a3)\n", i, x * 4);
                                } else {
                                    printf("\t lw $%d, %d($sp)\n", i, delta - RegOffset[x]);
                                }
                        }
                    }
            }
    }
    else {
            assert(0);
        }
    for (i = 0; i < 3; i++) {
        if (tuple.parameters[i].needclear) {
            which[tuple.parameters[i].real] = -1;
        }
    }
}

void interprete_local() {
    printf("\n.text\n");
    FILE* builtin = fopen("input.s", "r");
        while (fgets(ibuffer, MAX_LENGTH, builtin)) {
            printf("%s", ibuffer);
        }
    int i = 0;
    in_main = 0;
    for (i = 0; i < IR.size(); ++i) {
        if (!IR[i].active) continue;
        string op = IR[i].op;
        int delta = 0;
        if (op=="call"&&IR[i].parameters[0].name!="__write"&&IR[i].parameters[0].name!="__read") {
                    assert(IR[prev_pos[i - 1]].parameters[2].type == _CONSTANT);
                    delta = -IR[prev_pos[i - 1]].parameters[2].value;
            }
        interprete_generate_mips(IR[i],delta);
        if (op=="func") {
                    if (IR[i].parameters[0].name=="main") {
                        printf("\t la $a3, %s\n", tempname);
                        in_main = 1;
                        int j = i;
                        int count = 0;
                        for (i = 0; i < GLOBAL_IR.size(); i++) {
                                printf("\t la $t0, %s%d\n", varname, count);
                                printf("\t sw $t0, %d($a3)\n", GLOBAL_IR[i].parameters[1].value * 4);
                                count++;
                        }
                     for (i = 0; i < ASSIGN_IR.size(); i++) {
                            interprete_generate_mips(ASSIGN_IR[i], -1);
                     }
                     i = j;
                     }
            } 
    }
    puts("End:");
        puts("\t li $v0, 10");
        puts("syscall");
}

void codeGeneration() {
    register_allocation();
    interprete_global();
    printf("%s\n", "finish global");
    interprete_local();
}


#endif
