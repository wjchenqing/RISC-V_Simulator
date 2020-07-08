//
// Created by sunshine on 2020/7/8.
//

#ifndef RISC_V_SIMULATOR_STAGE_HPP
#define RISC_V_SIMULATOR_STAGE_HPP

#include "variate.hpp"
#include "instruction.hpp"

class _IF;
class _ID;
class _EX;
class _MEM;
class _WB;

class _IF{
public:
    uint pc;
public:
    _IF():pc(0u){}
    void op(_ID *_id);
};

class _ID{
public:
    uint ins;
    uint pc;
public:
    _ID():ins(0u), pc(0u){}
    void op(_EX *_ex);
};

class _EX{
public:
    uint pc;
    ins_type type;
    uint imm;
    uint rs1_val;
    uint rs2_val;
    uint rd;
public:
    _EX(): pc(0u), type(Null), imm(0u), rs1_val(0u), rs2_val(0u), rd(0u){}
    void op(_MEM *_mem);
};

class _MEM{
public:
    uint pc;
    ins_type type;
    uint address;
    uint val;
    uint rd_val;
    uint rd;
public:
    _MEM(): pc(0u), type(Null), address(0u), val(0u), rd_val(0u), rd(0u){}
    void op(_IF *_if, _WB *_wb);
};

class _WB{
public:
    ins_type type;
    uint rd_val;
    uint rd;
public:
    _WB(): type(Null), rd_val(0u), rd(0u){}
    void op();
};

#endif //RISC_V_SIMULATOR_STAGE_HPP
