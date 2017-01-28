/***************************************************************
File name: quadruple.h
This file defines some functions used in quadruple creation.
***************************************************************/


#ifndef QUADRUPLE_H
#define QUADRUPLE_H

#include "include/treeNode.h"
#include "include/header.h"
#include "semanticAnalysis.h"
#include <cstring>
#define REGISTER_STATE_ADDRESS 1
ofstream fout("InterCode");


Quadruple create_quadruple_move(int ra, int rb) {
    Quadruple ret = create_quadruple();
    ret.op = "move";
    ret.parameters[0].type = _TMP;
    ret.parameters[0].value = ra;
    ret.parameters[1].type = _TMP;
    ret.parameters[1].value = rb;
    return ret;
}

Quadruple create_quadruple() {
    Quadruple ret;
    ret.flag = quadruple_flag;
    ret.active = 1;
    ret.parameters[0].value = -1;
    ret.parameters[0].real = -1;
    ret.parameters[0].needload = 0;
    ret.parameters[0].needclear = 0;

    ret.parameters[1].value = -1;
    ret.parameters[1].real = -1;
    ret.parameters[1].needload = 0;
    ret.parameters[1].needclear = 0;

    ret.parameters[2].value = -1;
    ret.parameters[2].real = -1;
    ret.parameters[2].needload = 0;
    ret.parameters[2].needclear = 0;
    return ret;
}

Quadruple create_quadruple_sw_offset(int ra, int rb, int offset) {
    Quadruple ret = create_quadruple();
    ret.op = "sw";
    ret.parameters[0].type = _TMP;
    ret.parameters[0].value = ra;
    ret.parameters[1].type = _TMP;
    ret.parameters[1].value = rb;
    ret.parameters[2].type = _CONSTANT;
    ret.parameters[2].value = offset;
    return ret;
}

Quadruple create_quadruple_gnew(int size, int reg) {  
    Quadruple ret = create_quadruple();
    ret.op = "gnew";
    ret.parameters[0].type = _CONSTANT;
    ret.parameters[0].value = size;
    ret.parameters[1].type = _TMP;
    ret.parameters[1].value = reg;
    return ret;
}

Quadruple create_quadruple_function(char *s) {
    Quadruple ret = create_quadruple();
    ret.op = "func";
    ret.parameters[0].type = _NAME;
    if (strcmp(s, "main") != 0) {
        int n = strlen(s);
        char* t = new char[n+3];
        t[0] = t[1] = '_';
        memcpy(t + 2, s, sizeof(char) * n);
        t[n + 2] = '\0';
        ret.parameters[0].name = t;
    ret.parameters[0].value = -2;
    } else {
        ret.parameters[0].name = s;
    ret.parameters[0].value = -2;
    }
    return ret;
}

Quadruple create_quadruple_lw(int ra, int rb) {
    Quadruple ret = create_quadruple();
    ret.op = "lw";
    ret.parameters[0].type = _TMP;
    ret.parameters[0].value = ra;
    ret.parameters[1].type = _TMP;
    ret.parameters[1].value = rb;
    ret.parameters[2].type = _CONSTANT;
    ret.parameters[2].value = 0;
    return ret;
}

Quadruple create_quadruple_li(int ra, int im) {
    Quadruple ret = create_quadruple();
    ret.op = "li";
    ret.parameters[0].type = _TMP;
    ret.parameters[0].value = ra;
    ret.parameters[1].type = _CONSTANT;
    ret.parameters[1].value = im;
    return ret;
}

Quadruple create_quadruple_ret() {
    Quadruple ret = create_quadruple();
    ret.op = "ret";
    return ret;
}

Quadruple create_quadruple_arithmetic(char *s, int ra, int rb, int rc) {
    Quadruple ret = create_quadruple();
    ret.op = s;
    ret.parameters[0].type = _TMP;
    ret.parameters[0].value = ra;
    ret.parameters[1].type = _TMP;
    ret.parameters[1].value = rb;
    ret.parameters[2].type = _TMP;
    ret.parameters[2].value = rc;
    return ret;
}

Quadruple create_quadruple_sw(int ra, int rb) {
    Quadruple ret = create_quadruple();
    ret.op = "sw";
    ret.parameters[0].type = _TMP;
    ret.parameters[0].value = ra;
    ret.parameters[1].type = _TMP;
    ret.parameters[1].value = rb;
    ret.parameters[2].type = _CONSTANT;
    ret.parameters[2].value = 0;
    return ret;
}

Quadruple create_quadruple_goto(int value) {
    Quadruple ret = create_quadruple();
    ret.op = "goto";
    ret.parameters[0].type = _LABEL;
    ret.parameters[0].value = value;
    return ret;
}

Quadruple create_quadruple_label() {
    Quadruple ret = create_quadruple();
    ret.op = "label";
    ret.parameters[0].type = _LABEL;
    ret.parameters[0].value = label_count++;
    return ret;
}

Quadruple create_quadruple_call(char *s) {
    Quadruple ret = create_quadruple();
    ret.op = "call";
    ret.parameters[0].type = _NAME;
    if (strcmp(s, "main") != 0) {
        int n = strlen(s);
        char* t = (char*)malloc(sizeof(char) * (n + 3));
        t[0] = t[1] = '_';
        memcpy(t + 2, s, sizeof(char) * n);
        t[n + 2] = '\0';
        ret.parameters[0].name = t;
    } else {
        ret.parameters[0].name = s;
    }
    return ret;
}

Quadruple create_quadruple_branch(const char *s, int a, int b) {
    Quadruple ret = create_quadruple();
    ret.op = s;
    ret.parameters[0].type = _TMP;
    ret.parameters[0].value = a;
    ret.parameters[1].type = _LABEL;
    ret.parameters[1].value = b;
    return ret;
}

Quadruple create_quadruple_arithmetic_im(const char *s, int ra, int rb, int im) {
    Quadruple ret = create_quadruple();
    ret.op = s;
    ret.parameters[0].type = _TMP;
    ret.parameters[0].value = ra;
    ret.parameters[1].type = _TMP;
    ret.parameters[1].value = rb;
    ret.parameters[2].type = _CONSTANT;
    ret.parameters[2].value = im;
    return ret;
}

Quadruple create_quadruple_arithmetic_unary(const char *s, int ra, int rb) {
    Quadruple ret = create_quadruple();
    ret.op = s;
    ret.parameters[0].type = _TMP;
    ret.parameters[0].value = ra;
    ret.parameters[1].type = _TMP;
    ret.parameters[1].value = rb;
    return ret;
}

Quadruple create_quadruple_lw_offset(int ra, int rb, int offset) {
    Quadruple ret = create_quadruple();
    ret.op = "lw";
    ret.parameters[0].type = _TMP;
    ret.parameters[0].value = ra;
    ret.parameters[1].type = _TMP;
    ret.parameters[1].value = rb;
    ret.parameters[2].type = _CONSTANT;
    ret.parameters[2].value = offset;
    return ret;
}
#endif