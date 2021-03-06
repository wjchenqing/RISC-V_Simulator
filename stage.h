//
// Created by sunshine on 2020/7/8.
//

#ifndef RISC_V_SIMULATOR_STAGE_H
#define RISC_V_SIMULATOR_STAGE_H

#include "variate.h"
#include "instruction.hpp"

class _IF;
class _ID;
class _EX;
class _MEM;
class _WB;

extern _IF IF;
extern _ID ID;
extern _EX EX;
extern _MEM MEM;
extern _WB WB;

class _IF{
public:
    bool occupied;
    uint pc;
public:
    _IF():pc(0u), occupied(false) {}
    void op(_ID *_id);
};

class _ID{
public:
    bool occupied;
    uint ins;
    uint pc;
    int predictor;
public:
    _ID():ins(0u), pc(0u), occupied(false), predictor(0){}
    void op(_EX *_ex);
};

class _EX{
public:
    bool occupied;
    uint pc;
    ins_type type;
    uint imm;
    uint rs1_val;
    uint rs2_val;
    uint rd;
    int predictor;
public:
    _EX(): pc(0u), type(Null), imm(0u), rs1_val(0u), rs2_val(0u), rd(0u), occupied(false), predictor(0){}
    void op(_MEM *_mem);
};

class _MEM{
public:
    bool occupied;
    ins_type type;
    uint pc;
    uint address;
    uint val;
    uint rd_val;
    uint rd;
public:
    _MEM(): pc(0u), type(Null), address(0u), val(0u), rd_val(0u), rd(0u), occupied(false) {}
    void op(_WB *_wb);
};

class _WB{
public:
    bool occupied;
    ins_type type;
    uint rd_val;
    uint rd;
public:
    _WB(): type(Null), rd_val(0u), rd(0u), occupied(false){}
    void op();
};

#endif //RISC_V_SIMULATOR_STAGE_H
