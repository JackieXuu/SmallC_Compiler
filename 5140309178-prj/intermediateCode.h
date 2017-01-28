/***************************************************************
File name: intermediateCode.h
This file defines some functions used in translating productions
to intermediate code, which is three address code.
***************************************************************/


#ifndef INTERMEDIATE_CODE_H
#define INTERMEDIATE_CODE_H
#include "include/header.h"
#include "quadruple.h"
void intermediateCode();
void set_register_state_to_address(int k);
void interCode(TreeNode* t);
void interCode_extdef(TreeNode* t);
void interCode_extdefs(TreeNode* t);
void interCode_stmtblock(TreeNode* t);
void interCode_args_1(TreeNode* t, int reg);
void interCode_exps(TreeNode* t, int reg);
void interCode_stmts(TreeNode* t);
void interCode_stmt(TreeNode* t);
void interCode_assignment_1(TreeNode* t, int flag, int num);
void interCode_defs(TreeNode* t);
void interCode_assignment(int reg_l, int reg_r);
void interCode_var_local(TreeNode* t,int reg);
void interCode_decs(TreeNode* t);
void interCode_init(TreeNode* t, int reg);
void interCode_exps(TreeNode* t, int reg);
void interCode_var_global(TreeNode* t, int reg);
void interCode_extvars(TreeNode* t);
void interCode_args(TreeNode* t);
void interCode_exp(TreeNode* t);
void set_register_state_to_value(int k);
void ir_print(vector <Quadruple>);
void IR_Push(Quadruple ret);
int get_register_state(int k);
int vs_fetch_register(char* s);
void catch_value_self(int k);
int get_width(char* s);
int new_register();



void intermediateCode()
{
    main_flag = 0;
    function_begin_sp = -1;
    quadruple_flag = 0;
    label_count = interCode_level = 0;
    global_flag = 0;
    stack_pointer = new_register();
    set_register_state_to_address(stack_pointer);
    retad_pointer = new_register();
    current_sp = 0;
    interCode(root);
    fout << "GLOBAL_IR\n";
    ir_print(GLOBAL_IR);
    fout << "ASSIGN_IR\n";
    ir_print(ASSIGN_IR);
    fout << "IR\n";
    ir_print(IR);
}

void interCode(TreeNode *t) {
    int i;
    if (!strcmp("PROGRAM", t->type)){
        interCode(t->children[0]);
    }
    else if (!strcmp("EXTDEFS", t->type)){
        interCode_extdefs(t);
    }
    else if (!strcmp("EXTDEF", t->type)){
        interCode_extdef(t);
    }
    else if (!strcmp("TYPE", t->type)||!strcmp("OPERATION", t->type)||!strcmp("NULL", t->type)){

    }
    else{
        fout << t->data << endl;
        assert(0); 
    }
}

void interCode_paras(TreeNode* t) {
    if (t->childNum==0) return;
    ++tmp_num_var;
    int reg = new_register();
    IR_Push(create_quadruple_arithmetic_im("add", reg, stack_pointer, 4*tmp_num_var-4));    //pass value of parameters here!
    set_register_state_to_address(reg); 
    vs_id.push_back(t->children[1]->data);
    vs_reg.push_back(reg);
    if (t->childNum==3) interCode_paras(t->children[2]);
}

void interCode_sextvars(TreeNode* t) {
    if (t->childNum==0) return; 
    int reg = new_register();
    GLOBAL_IR.push_back(create_quadruple_gnew(get_width(t->children[0]->data)*4, reg));
    vs_id.push_back(t->children[0]->data);
    vs_reg.push_back(reg);
    if (t->childNum==2) interCode_sextvars(t->children[1]);
}

void interCode_extdef(TreeNode* t) {
    int i;
    if (!strcmp(t->children[1]->type,"FUNC")) {
        global_flag = 0;
        IR_Push(create_quadruple_function(t->children[1]->children[0]->data));
        if (!strcmp(t->children[1]->children[0]->data,"main")) {main_flag = 1;}
        int j = IR.size()-1;
        local_register_count = 0;
        function_begin_sp = current_sp;
        tmp_num_var = 0;
        interCode_paras(t->children[1]->children[1]);
        interCode_stmtblock(t->children[2]);
        IR_Push(create_quadruple_ret());
        for (i = j; i < IR.size(); i++) {
            if (IR[i].flag == 1) {
                IR[i].parameters[2].value -= local_register_count * 4;
            }
        }
        current_sp = function_begin_sp;
        function_begin_sp = -1;
        main_flag = 0;
    }
    else if (!strcmp(t->children[1]->type,"EXTVARS")) {
        global_flag = 1;
        interCode_extvars(t->children[1]);
    }
    else if (!strcmp(t->children[0]->type,"STSPEC")){ //structs
        interCode_sextvars(t->children[1]);
    }
}

int vs_fetch_register(char* s) {
    int i;
    for (i = vs_id.size() - 1; i >= 0; i--) {
        if (vs_id[i]==s) {
            return vs_reg[i];
        }
    }
    assert(0); // we shall never get here
}

int get_register_state(int k) {
    return RegState[k];
}

void interCode_extvars(TreeNode* t) {
    if (t->childNum==0) return;
        if (t->childNum == 3) {
            int reg = new_register();
            interCode_var_global(t->children[0],reg);
            if (!strcmp(t->children[2]->data,"init {}")) {
                arr_init_cnt = 0;
                interCode_args_1(t->children[2]->children[0],reg);
            }
            else {
                int tmp = new_register();
                    interCode_init(t->children[2], tmp);
                    interCode_assignment(reg, tmp);
            }
        }
        else if (t->childNum==2) {
            int reg = new_register();
            interCode_var_global(t->children[0],reg);
            interCode_extvars(t->children[1]);
       }
    else if (t->childNum==4){
        int reg = new_register();
        interCode_var_global(t->children[0],reg);
        if (!strcmp(t->children[2]->data,"init {}")) {
            arr_init_cnt = 0;
            interCode_args_1(t->children[2]->children[0],reg);
        }
        else {
            int tmp = new_register();
            interCode_init(t->children[2], tmp);
            interCode_assignment(reg, tmp);
        }
        interCode_extvars(t->children[3]);
    }
    else {
        int reg = new_register();
        interCode_var_global(t->children[0],reg);
    }
}

void interCode_init(TreeNode* t, int tmp) {
    if (!strcmp(t->data,"init")) {
        interCode_exps(t->children[0],tmp);
    }
    else {
        interCode_args(t->children[0]);
    }
}

void interCode_args_1(TreeNode* t, int reg) {
    if (t->childNum==1) {
        ++arr_init_cnt;
        if (t->children[0]->childNum==0) return;
        int tmp = new_register();
        interCode_exps(t->children[0]->children[0],tmp);
        catch_value_self(tmp);
        IR_Push(create_quadruple_sw_offset(tmp,reg,4*arr_init_cnt-4));
    }
    else {
        ++arr_init_cnt;
        if (t->children[0]->childNum==0) {interCode_args_1(t->children[1],reg);return;}
        int tmp = new_register();
        interCode_exps(t->children[0]->children[0],tmp);
        catch_value_self(tmp);
        IR_Push(create_quadruple_sw_offset(tmp,reg,4*arr_init_cnt-4));
        interCode_args_1(t->children[1],reg);
    }
}

void interCode_var_global(TreeNode* t,int reg) {
    if (t->childNum==1) {
        GLOBAL_IR.push_back(create_quadruple_gnew(get_width(t->children[0]->data)*4, reg));// global new variable
        set_register_state_to_address(reg);
        vs_id.push_back(t->children[0]->data);
        vs_reg.push_back(reg);
    }
    else {
        interCode_var_global(t->children[0],reg);
    }
}

void interCode_extdefs(TreeNode* t) {
    if (t->childNum==0) return;
    else {
        interCode_extdef(t->children[0]);
        interCode_extdefs(t->children[1]);
    }
}

void interCode_stmtblock(TreeNode* t) {
    ++interCode_level;
    ++interCode_cnt[interCode_level];
    interCode_defs(t->children[0]);
    interCode_stmts(t->children[1]);
    --interCode_level;
}

void interCode_sdecs(TreeNode* t) {
    int reg = new_register();   
    current_sp += 4*get_width(t->children[0]->data);
    int a = new_register();
    quadruple_flag = 1;
    IR_Push(create_quadruple_arithmetic_im("add", a, stack_pointer, -current_sp + function_begin_sp));
    quadruple_flag = 0;
    IR_Push(create_quadruple_move(reg, a));
    set_register_state_to_address(reg);
    vs_id.push_back(t->children[0]->data);
    vs_reg.push_back(reg);
    if (t->childNum==2) interCode_sdecs(t->children[1]);
}

void interCode_defs(TreeNode* t) {
    if (t->childNum==0) return;
    if (!strcmp(t->children[0]->type,"TYPE")) {
        global_flag = 0;
        interCode_decs(t->children[1]);
        interCode_defs(t->children[2]);
    }
    else if (!strcmp(t->children[0]->type,"STSPEC")) {
        interCode_sdecs(t->children[1]);
    }
}


void IR_Push(Quadruple ret)
{
    if (global_flag) {
        ASSIGN_IR.push_back(ret);
    }
    else {
        IR.push_back(ret);
    }
}

int new_register() {
    RegState.push_back(0);
    if (function_begin_sp == -1) {
        RegOffset.push_back(-1);
    } else {
        local_register_count++;
        RegOffset.push_back(local_register_count * 4);
    }
    return RegState.size()-1;
}

void set_register_state_to_address(int k) {
    if (k<0) return;
    RegState[k] = 1;
}

void set_register_state_to_value(int k) {
    if (k<0) return;
    RegState[k] = 0;
}

void catch_value_self(int k) {  
    if (get_register_state(k) == REGISTER_STATE_ADDRESS) {  
        IR.push_back(create_quadruple_lw(k, k));
        set_register_state_to_value(k);
    }
}

int get_label_number(Quadruple qr) {
    assert(qr.op=="label");
    return qr.parameters[0].value;
}



int get_width(char* s) {
    int tmp_level = interCode_level, tmp_depth = interCode_cnt[interCode_level];
    while (1) {
        if (symbolTable[tmp_level][tmp_depth].table.find(s)!=symbolTable[tmp_level][tmp_depth].table.end()) { //find
            return symbolTable[tmp_level][tmp_depth].variable_num[s];
        }
        else {
            int parent = symbolTable[tmp_level][tmp_depth].parent_index;
            if (parent==-1||tmp_level==0) {fprintf(stderr,"error! %s\n",s);assert(0);}
            else {
                tmp_level--;
                tmp_depth = parent;
            }
        }
    }
}

int cal_offset(char* s, char *s2) {
    int tmp_level = interCode_level, tmp_depth = interCode_cnt[interCode_level];
    while (1) {
        if (symbolTable[tmp_level][tmp_depth].table.find(s)!=symbolTable[tmp_level][tmp_depth].table.end()) { //find
            assert(!strcmp("struct",symbolTable[tmp_level][tmp_depth].table[s]));
            int i;
            for (i = 0; i < symbolTable[tmp_level][tmp_depth].struct_object[s].size(); ++i) {
                if (!strcmp(symbolTable[tmp_level][tmp_depth].struct_object[s][i],s2)) return i;
            }
            assert(0);
        }
        else {
            int parent = symbolTable[tmp_level][tmp_depth].parent_index;
            if (parent==-1||tmp_level==0) {fprintf(stderr,"error! %s\n",s);assert(0);}
            else {
                tmp_level--;
                tmp_depth = parent;
            }
        }
    }
}

void ir_print(vector <Quadruple> IR){
    int i,j;
    for (i = 0; i < IR.size(); ++i) {
        if (!IR[i].active) {
            fout<<"#" << i << endl;
            continue;
        }
        if (IR[i].op=="func") {
            fout << "#" << i;
            fout << IR[i].op << "\t";
            fout << IR[i].parameters[0].name << endl;
            continue;
        }
        if (IR[i].op=="call") {
            fout << "#"<<i;
            fout << "\t\t" << IR[i].op << "\t";
            fout << IR[i].parameters[0].name << endl;
            continue;
        }
        fout << "#" << i << "\t\t";
        fout << IR[i].op;
        for (j = 0; j < 3; ++j) {
            fout << "\t";
            if (IR[i].parameters[j].value==-1) break;
            switch(IR[i].parameters[j].type) {
                case _TMP: 
                    if (IR[i].parameters[j].value == stack_pointer) {
                        fout << "$sp";
                    } else if (IR[i].parameters[j].value == retad_pointer) {
                        fout << "$ra";
                    }
                    else if (IR[i].parameters[j].value == -2) {
                        fout << "$a0";
                    }
                    else if (IR[i].parameters[j].value==-3) {
                        fout << "$v0";
                    }
                    else if (IR[i].parameters[j].real != -1) {
                        fout << "$" << IR[i].parameters[j].real;
                    } 
                    else {
                        fout << "t" << IR[i].parameters[j].value;
                   }
                    break;
                case _CONSTANT:
                    fout << IR[i].parameters[j].value;
                    break;
                case _NAME:
                    fout << IR[i].parameters[j].name;
                    break;
                case _LABEL:
                    fout << "l"<< IR[i].parameters[j].value;
                    break;
                default: break;
            }
        }
        fout << endl;   
    }
}

void interCode_decs(TreeNode* t) {
    if (t->childNum == 3) {
        int reg = new_register();
        interCode_var_local(t->children[0],reg);
        if (!strcmp(t->children[2]->data,"init {}")) {
            arr_init_cnt = 0;
            interCode_args_1(t->children[2]->children[0],reg);
        }
        else {
            int tmp = new_register();
            interCode_init(t->children[2], tmp);
            interCode_assignment(reg, tmp);
        }
    }
    else if (t->childNum==2) {
        int reg = new_register();
        interCode_var_local(t->children[0],reg);
        interCode_decs(t->children[1]);
    }
    else if (t->childNum==4){
        int reg = new_register();
        interCode_var_local(t->children[0],reg);
        if (!strcmp(t->children[2]->data,"init {}")) {
            arr_init_cnt = 0;
            interCode_args_1(t->children[2]->children[0],reg);
        }
        else {
            int tmp = new_register(); 
                interCode_init(t->children[2], tmp);
                interCode_assignment(reg, tmp);
        }
        interCode_decs(t->children[3]);
    }
    else {
        int reg = new_register();
        interCode_var_local(t->children[0],reg);
    }
}

void interCode_var_local(TreeNode* t,int reg) {
    if (t->childNum==1) {
        current_sp += 4*get_width(t->children[0]->data);
        int a = new_register();
        quadruple_flag = 1;
        IR_Push(create_quadruple_arithmetic_im("add", a, stack_pointer, -current_sp + function_begin_sp));
        quadruple_flag = 0;
        IR_Push(create_quadruple_move(reg, a));
        set_register_state_to_address(reg);
        //fout << "reg" << reg << endl;
        vs_id.push_back(t->children[0]->data);
        vs_reg.push_back(reg);
    }
    else interCode_var_local(t->children[0],reg);
}



void interCode_stmts(TreeNode* t) {
    int i;
    if (t->childNum!=0) {
        interCode_stmt(t->children[0]);
        interCode_stmts(t->children[1]);
    }
}

void interCode_stmt(TreeNode* t) {
    if (!strcmp("return stmt",t->data)) {
        int t0 = new_register(), t1 = new_register();
        interCode_exps(t->children[1],t1);
        IR_Push(create_quadruple_arithmetic_im("add", t0, stack_pointer, 4*tmp_num_var + 4));// store the return value
        interCode_assignment(t0, t1);
        IR_Push(create_quadruple_ret());
    }
    else if (!strcmp("write stmt",t->data)) {
        int reg = new_register();// reserved for $a0 
        interCode_exps(t->children[0],reg);
        catch_value_self(reg);
        IR_Push(create_quadruple_move(-2,reg));
        if (!main_flag) {
            IR_Push(create_quadruple_sw_offset(retad_pointer, stack_pointer, -10000));
        }
        IR_Push(create_quadruple_call("write"));
        if (!main_flag) {
            IR_Push(create_quadruple_lw_offset(retad_pointer, stack_pointer, -10000));
        }
    }
    else if (!strcmp("read stmt",t->data)) {
        int reg = new_register();
        interCode_exps(t->children[0], reg);
        assert(get_register_state(reg) == REGISTER_STATE_ADDRESS);
        int v0 = -3;
        if (!main_flag) {
            IR_Push(create_quadruple_sw_offset(retad_pointer, stack_pointer, -10000));
        }
        IR_Push(create_quadruple_call("read"));
        if (!main_flag) {
            IR_Push(create_quadruple_lw_offset(retad_pointer, stack_pointer, -10000));
        }
        IR_Push(create_quadruple_sw(v0,reg));
        /* all exps that could be a left value could be here*/
        
    }
    else if (!strcmp("stmt exp",t->data)) {
        interCode_exp(t->children[0]);
    }
    else if (!strcmp("if stmt",t->data)) {
        //if (a) then b; else c     ->      (a->L1) | b | goto L2 | label: L1 | c | label: L2
        Quadruple t1 = create_quadruple_label();
        Quadruple t2 = create_quadruple_label();
        Quadruple t3 = create_quadruple_goto(get_label_number(t2));
        interCode_assignment_1(t->children[0], 0, get_label_number(t1));
        interCode_stmt(t->children[1]);
        IR_Push(t3);
        IR_Push(t1);
        if (t->childNum == 3) {
          interCode_stmt(t->children[2]);
        }
        IR_Push(t2);
    }
    // let's make the logic clear here: if the judgement is right, then we will just execute the following stmt until we meet the goto L2, avoid continuing executing the following; if it's false, we will go to L1 directly
    else if (!strcmp("stmt",t->data)) {
        interCode_stmtblock(t->children[0]);
    }
    else if (!strcmp("for stmt",t->data)) {
        Quadruple t1 = create_quadruple_label();
        Quadruple t2 = create_quadruple_label();
        /*for(a;b;c)d;      ->      a | (b->L3) | label L1 | d | label L2 | c | (!b->L1) | label L3*/
        TreeNode *a = NULL, *b = NULL, *c = NULL;
        if (t->children[0]->childNum!=0) a = t->children[0]->children[0];
        if (t->children[1]->childNum!=0) b = t->children[1]->children[0];
        if (t->children[2]->childNum!=0) c = t->children[2]->children[0];
        int old_current_sp = current_sp;
        Quadruple t3 = create_quadruple_label();
        LContinue.push_back(t2.parameters[0].value);
        LBreak.push_back(t3.parameters[0].value);
        int reg = new_register();
        if (a!=NULL) interCode_exps(a, reg);
        if (b != NULL) {
            interCode_assignment_1(b, 0, get_label_number(t3));
        } else {
            IR_Push(create_quadruple_goto(get_label_number(t3)));
        }
        IR_Push(t1);
        interCode_stmt(t->children[3]);
        IR_Push(t2);
        if (c!=NULL) interCode_exps(c, reg);
        if (b != NULL) {
            interCode_assignment_1(b, 1, get_label_number(t1));
        } else {
        IR_Push(create_quadruple_goto(get_label_number(t1)));
        }
        IR_Push(t3);
        current_sp = old_current_sp;
        LContinue.pop_back();
        LBreak.pop_back();
    }
    else if (!strcmp("continue stmt",t->data)) {
        IR_Push(create_quadruple_goto(LContinue.back()));
    }
    else if (!strcmp("break stmt",t->data)) {
        IR_Push(create_quadruple_goto(LBreak.back()));
    }
}

void interCode_assignment_1(TreeNode* t, int flag, int num) {
    int reg = new_register();
    interCode_exps(t,reg);
    catch_value_self(reg);
    if (flag==0) {
        IR_Push(create_quadruple_branch("beqz", reg, num));
    }
    else {
        IR_Push(create_quadruple_branch("bnez", reg, num));
    }
}

void interCode_exp(TreeNode* t) {
    if (t->childNum==0) return;
    int reg = new_register();
    interCode_exps(t->children[0],reg);
}

void interCode_assignment(int reg_l, int reg_r) { //problems may arise here
    catch_value_self(reg_r);
    IR_Push(create_quadruple_sw(reg_r, reg_l));
}

void interCode_args(TreeNode* t) {
    if (t->childNum>1)
        interCode_args(t->children[1]);
    if (t->children[0]->childNum==0) return;
    current_sp += 4;
    int t0 = new_register();
    //fprintf(stderr,"t0 %d\n",t0);
    interCode_exps(t->children[0]->children[0], t0);
    int t1 = new_register();
    set_register_state_to_address(t1);
    quadruple_flag = 1;
    IR_Push(create_quadruple_arithmetic_im("add", t1, stack_pointer, -current_sp + function_begin_sp));//to pass value of the parameters
    quadruple_flag = 0;
    interCode_assignment(t1, t0);
}

int find_array_size(char *s) {
    int tmp_level = interCode_level, tmp_depth = interCode_cnt[interCode_level];
    while (1) {
        if (symbolTable[tmp_level][tmp_depth].table.find(s)!=symbolTable[tmp_level][tmp_depth].table.end()) { //find
            return symbolTable[tmp_level][tmp_depth].array[s][arrs_cnt-1];
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

void interCode_arrs(TreeNode* t, int reg, char *s) {
    if (t->childNum==0) return;
    ++arrs_cnt;
    int t0 = new_register(), t1 = new_register();
    interCode_exps(t->children[0]->children[0], t0);
    catch_value_self(t0);
    catch_value_self(reg);
    int zzy = find_array_size(s);
    IR_Push(create_quadruple_arithmetic_im("mul", t1, t0, zzy));
    set_register_state_to_value(reg);
    IR_Push(create_quadruple_arithmetic("add", reg, reg, t1));
    interCode_arrs(t->children[1],reg,s);
}

void interCode_sdefs(TreeNode* t) {
}

void interCode_exps(TreeNode* t, int reg) {
    if (!strcmp(t->type,"INT")) {
        IR_Push(create_quadruple_li(reg, strtoint(t->data)));
        set_register_state_to_value(reg);
    }
    else if (!strcmp(t->data,"=")) {
        interCode_exps(t->children[0], reg);
        int tmp = new_register();
        interCode_exps(t->children[1], tmp);
        if (get_register_state(reg) == REGISTER_STATE_ADDRESS){
            interCode_assignment(reg, tmp);
        }
        else{
            error(t->line, "lvalue error",NULL);
        } 
    }
    else if (!strcmp(t->type, "OPERATION")) {
        interCode_exps(t->children[0], reg);
        catch_value_self(reg);
        int tmp = new_register();
        interCode_exps(t->children[1], tmp);
        catch_value_self(tmp);
        if (!strcmp(t->data,"*"))
                IR_Push(create_quadruple_arithmetic("mul", reg, reg, tmp));
        else if (!strcmp(t->data,"/"))
                IR_Push(create_quadruple_arithmetic("div", reg, reg, tmp));
        else if (!strcmp(t->data,"%"))
                IR_Push(create_quadruple_arithmetic("rem", reg, reg, tmp));
        else if (!strcmp(t->data,"+"))
                IR_Push(create_quadruple_arithmetic("add", reg, reg, tmp));
        else if (!strcmp(t->data,"-"))
                IR_Push(create_quadruple_arithmetic("sub", reg, reg, tmp));
        else if (!strcmp(t->data,"*"))
                IR_Push(create_quadruple_arithmetic("mul", reg, reg, tmp));
        else if (!strcmp(t->data,"|"))
                IR_Push(create_quadruple_arithmetic("or", reg, reg, tmp));
        else if (!strcmp(t->data,"&"))
                IR_Push(create_quadruple_arithmetic("and", reg, reg, tmp));
        else if (!strcmp(t->data,"^"))
                IR_Push(create_quadruple_arithmetic("xor", reg, reg, tmp));
        else if (!strcmp(t->data,"<<")) {
            IR_Push(create_quadruple_arithmetic("sll", reg, reg, tmp));
        }
        else if (!strcmp(t->data,">>")) {
            IR_Push(create_quadruple_arithmetic("srl", reg, reg, tmp));
        }
        else if (!strcmp(t->data,"&&")) {
            IR_Push(create_quadruple_arithmetic("and",tmp,reg,tmp));
            IR_Push(create_quadruple_li(reg, 1));
            Quadruple l = create_quadruple_label();
            IR_Push(create_quadruple_branch("bnez", tmp, get_label_number(l)));
            IR_Push(create_quadruple_li(reg, 0));
                IR_Push(l);
        }
        else if (!strcmp(t->data,"||")) {
            IR_Push(create_quadruple_arithmetic("or",tmp,reg,tmp));
            IR_Push(create_quadruple_li(reg, 1));
            Quadruple l = create_quadruple_label();
            IR_Push(create_quadruple_branch("bnez", tmp, get_label_number(l)));
            IR_Push(create_quadruple_li(reg, 0));
                IR_Push(l);
        }
        else {
                IR_Push(create_quadruple_arithmetic("sub", tmp, reg, tmp));
                IR_Push(create_quadruple_li(reg, 1));
                Quadruple l = create_quadruple_label();
                if (strcmp(t->data, ">=") == 0) {
                    IR_Push(create_quadruple_branch("bgez", tmp, get_label_number(l)));
                } else if (strcmp(t->data, ">") == 0) {
                    IR_Push(create_quadruple_branch("bgtz", tmp, get_label_number(l)));
                } else if (strcmp(t->data, "<=") == 0) {
                    IR_Push(create_quadruple_branch("blez", tmp, get_label_number(l)));
                } else if (strcmp(t->data,"<")==0) {
                    IR_Push(create_quadruple_branch("bltz", tmp, get_label_number(l)));
                } else if (strcmp(t->data,"==")==0) {
                IR_Push(create_quadruple_branch("beqz", tmp, get_label_number(l)));
            }
              else if (strcmp(t->data,"!=")==0) {
                IR_Push(create_quadruple_branch("bnez", tmp, get_label_number(l)));
            }
                IR_Push(create_quadruple_li(reg, 0));
                IR_Push(l);
        }
    }
    else if (!strcmp("EXPS UNARY",t->type)) {
        interCode_exps(t->children[0],reg);
        if (!strcmp(t->data, "~")){
            catch_value_self(reg);
            IR_Push(create_quadruple_arithmetic_unary("not", reg, reg));
        }
        else if (!strcmp(t->data, "!")){
             catch_value_self(reg);
             IR_Push(create_quadruple_arithmetic_unary("lnot", reg, reg));
        }
        else if (!strcmp(t->data, "++")){
            assert(get_register_state(reg) == REGISTER_STATE_ADDRESS);
            int t0 = new_register();// use t0 as a temporary reg to store the original address
            IR_Push(create_quadruple_move(t0, reg));
            set_register_state_to_value(reg);
            IR_Push(create_quadruple_lw(reg, reg));
            IR_Push(create_quadruple_arithmetic_im("add", reg, reg, 1));
            IR_Push(create_quadruple_sw(reg, t0));
        }
        else if (!strcmp(t->data, "--")){
            assert(get_register_state(reg) == REGISTER_STATE_ADDRESS);
            int t0 = new_register();
            IR_Push(create_quadruple_move(t0, reg));
            set_register_state_to_value(reg);
            IR_Push(create_quadruple_lw(reg, reg));
            IR_Push(create_quadruple_arithmetic_im("add", reg, reg, -1));
            IR_Push(create_quadruple_sw(reg, t0));
        }
        else{
            catch_value_self(reg);
            IR_Push(create_quadruple_arithmetic_unary("neg", reg, reg));
        }
    }
    else if (!strcmp("exps ()",t->data)) {
        interCode_exps(t->children[0],reg);
    }
    else if (!strcmp("exps f()",t->data)) {//function here
        current_sp += 8;
        quadruple_flag = 1;
        if (!main_flag) {
            IR_Push(create_quadruple_sw_offset(retad_pointer, stack_pointer, -current_sp + function_begin_sp));
        }
        quadruple_flag = 0;
        interCode_args(t->children[1]);
        quadruple_flag = 1;
        IR_Push(create_quadruple_arithmetic_im("add", stack_pointer, stack_pointer, -current_sp + function_begin_sp));
        quadruple_flag = 0;
        IR_Push(create_quadruple_call(t->children[0]->data));
        quadruple_flag = 1;
        IR_Push(create_quadruple_arithmetic_im("sub", stack_pointer, stack_pointer, -current_sp + function_begin_sp));
        quadruple_flag = 0;
        current_sp -= function_name_para_num[t->children[0]->data][0]*4;
        quadruple_flag = 1;
        if (!main_flag) {
            IR_Push(create_quadruple_lw_offset(retad_pointer, stack_pointer, -current_sp + function_begin_sp));
        }
        quadruple_flag = 0;
        current_sp -= 4;
        set_register_state_to_address(reg);
        int t = new_register();
        set_register_state_to_address(t);
        quadruple_flag = 1;
        IR_Push(create_quadruple_arithmetic_im("add", t, stack_pointer, -current_sp + function_begin_sp));
        quadruple_flag = 0;
        IR_Push(create_quadruple_move(reg, t));
        set_register_state_to_address(reg);
        current_sp -= 4;
    }
    else if (!strcmp("exps arr",t->data)) { //id here
        int tmp = vs_fetch_register(t->children[0]->data);
        if (get_register_state(tmp) == REGISTER_STATE_ADDRESS) {
            IR_Push(create_quadruple_move(reg, tmp));
            if (get_width(t->children[0]->data)!=1) { // an variable of type array
                arrs_cnt = 0;
                interCode_arrs(t->children[1],reg,t->children[0]->data);
                set_register_state_to_address(reg);
            } 
            else { // an varaible of type int
                set_register_state_to_address(reg);
            }
        } else {
               //array pointer
                //set_register_state_to_value(reg);
                //IR_Push(create_quadruple_lw(reg, tmp));
        }
    }
    else if (!strcmp("exps struct",t->data)) { //dot op
        int tmp = vs_fetch_register(t->children[0]->data);
        IR_Push(create_quadruple_move(reg, tmp));
        set_register_state_to_address(reg);
        int offset = 4*cal_offset(t->children[0]->data,t->children[2]->data);
        IR_Push(create_quadruple_arithmetic_im("add", reg, reg, offset));
        set_register_state_to_address(reg);
    }
    else { //all kinds of assign expressions
        interCode_exps(t->children[0],reg);
        int tmp = new_register();
        interCode_exps(t->children[1], tmp);
        assert(get_register_state(reg) == REGISTER_STATE_ADDRESS);
        int a = new_register();
        char* op;
        if (!strcmp(t->data, "*")){
            op = (char* )"mul";
        }
        else if (!strcmp(t->data, "/")){
            op = (char* )"div";
        }
        else if (!strcmp(t->data, "%")){
            op = (char* )"rem";
        }
        else if (!strcmp(t->data, "+")){
            op = (char* )"add";
        }
        else if (!strcmp(t->data, "-")){
            op = (char* )"sub";
        }
        else if (!strcmp(t->data, "<")){
            op = (char* )"sll";
        }
        else if (!strcmp(t->data, ">")){
            op = (char* )"srl";
        }
        else if (!strcmp(t->data, "&")){
            op = (char* )"and";
        }
        else if (!strcmp(t->data, "^")){
            op = (char* )"xor";
        }
        else if (!strcmp(t->data, "!")){
            op = (char* )"or";
        }
        else {
            fprintf(stderr,"t->data %s\n",t->data);
            assert(0);
        }
        assert(get_register_state(reg) == REGISTER_STATE_ADDRESS);
        IR_Push(create_quadruple_lw(a, reg));
        catch_value_self(tmp);  //problems may arise here
        IR_Push(create_quadruple_arithmetic(op, a, a, tmp));
        IR_Push(create_quadruple_sw(a, reg));
    }
}
#endif




