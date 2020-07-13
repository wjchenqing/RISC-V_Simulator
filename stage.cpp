//
// Created by sunshine on 2020/7/8.
//

#include <bits/stdc++.h>
#include "stage.h"
#include "instruction.hpp"

_IF IF;
_ID ID;
_EX EX;
_MEM MEM;
_WB WB;

void _IF::op(_ID *_id){
    if(_id->occupied || !occupied) return;
    pc = PC;
    PC += 4;
    uint tmp = 0u;
    tmp |= ((((uint)_memory[pc+3]) << 24u) | (((uint)_memory[pc+2]) << 16u) | (((uint)_memory[pc+1]) << 8u) | ((uint)_memory[pc]));
    _id->ins = tmp;
    if(tmp == 0x0ff00513u) {
        occupied = false;
        return;
    }
    _id->pc = pc;
    _id->occupied = true;
}

void _ID::op(_EX *_ex){
    if(_ex->occupied || !occupied) return;
    instruction cur(ins);
    if(cur.rs1 != 0u && reg_occupied[cur.rs1]) return;
    if(cur.rs2 != 0u && reg_occupied[cur.rs2]) return;
    if(cur.rd != 0u) reg_occupied[cur.rd] = true;
    _ex->type = cur._type;
    _ex->pc = pc;
    _ex->occupied = true; occupied = false;
    switch(cur._type){
        case Null: break;
        case ADDI: case SLTI: case SLTIU: case XORI: case ORI: case ANDI:
            _ex->rs1_val = _register[cur.rs1]; _ex->imm = cur.imm; _ex->rd = cur.rd; break;
        case SLLI: case SRLI: case SRAI:
            _ex->rs1_val = _register[cur.rs1]; _ex->imm = cur.imm & 0x1fu; _ex->rd = cur.rd; break;
        case ADD: case SUB: case SLT: case SLTU: case XOR: case OR: case AND:
            _ex->rs1_val = _register[cur.rs1]; _ex->rs2_val = _register[cur.rs2]; _ex->rd = cur.rd; break;
        case SLL: case SRL: case SRA:
            _ex->rs1_val = _register[cur.rs1]; _ex->rs2_val = _register[cur.rs2] & 31u; _ex->rd = cur.rd; break;
        case LUI:
            _ex->imm = cur.imm; _ex->type = cur._type; _ex->rd = cur.rd; break;
        case AUIPC:
            _ex->imm = cur.imm; _ex->rd = cur.rd; break;
        case JAL:
            _ex->imm = cur.imm; _ex->rd = cur.rd; IF.occupied = false; break;
        case JALR:
            _ex->imm = cur.imm; _ex->rs1_val = _register[cur.rs1]; _ex->rd = cur.rd; IF.occupied = false; break;
        case BEQ: case BNE: case BLT: case BLTU: case BGE: case BGEU:
            _ex->rs1_val = _register[cur.rs1]; _ex->rs2_val = _register[cur.rs2]; _ex->imm = cur.imm; IF.occupied = false; break;
        case LB: case LBU: case LH: case LHU: case LW:
            _ex->rs1_val = _register[cur.rs1]; _ex->imm = cur.imm; _ex->rd = cur.rd; break;
        case SB: case SH: case SW:
            _ex->rs1_val = _register[cur.rs1]; _ex->rs2_val = _register[cur.rs2]; _ex->imm = cur.imm; break;
    }
}

void _EX::op(_MEM *_mem){
    if(_mem->occupied || !occupied) return;
    occupied = false;
    _mem->type = type;
    _mem->rd = rd;
    _mem->occupied = true;
    switch (type){
        case Null: break;
        case ADDI: _mem->rd_val = imm + rs1_val; break;
        case SLTI: _mem->rd_val = ((int)rs1_val < (int)imm); break;
        case SLTIU: _mem->rd_val = (rs1_val < imm); break;
        case XORI:  _mem->rd_val = rs1_val ^ imm; break;
        case ORI: _mem->rd_val = rs1_val | imm; break;
        case ANDI: _mem->rd_val = rs1_val & imm; break;
        case SLLI: _mem->rd_val = rs1_val << imm; break;
        case SRLI: _mem->rd_val = rs1_val >> imm; break;
        case SRAI: _mem->rd_val = (int)(rs1_val) >> imm; break;
        case ADD: _mem->rd_val = rs1_val + rs2_val; break;
        case SUB: _mem->rd_val = rs1_val - rs2_val; break;
        case SLT: _mem->rd_val = ((int)rs1_val < (int)rs2_val); break;
        case SLTU: _mem->rd_val = (rs1_val < rs2_val); break;
        case XOR: _mem->rd_val = rs1_val ^ rs2_val; break;
        case SLL: _mem->rd_val = rs1_val << rs2_val; break;
        case SRL: _mem->rd_val = rs1_val >> rs2_val; break;
        case SRA: _mem->rd_val = (int)(rs1_val) >> rs2_val; break;
        case OR: _mem->rd_val = rs1_val | rs2_val; break;
        case AND: _mem->rd_val = rs1_val & rs2_val; break;
        case LUI: _mem->rd_val = imm; break;
        case AUIPC: _mem->rd_val = imm + pc; break;
        case JAL: _mem->rd_val = pc + 4; PC = pc + (int)imm; IF.occupied = true; break;
        case JALR: _mem->rd_val = pc + 4; PC = (rs1_val + (int)imm) & (-2u); IF.occupied = true; break;
        case BEQ: PC = (rs1_val == rs2_val) ? (pc + (int)imm) & (-2u) : pc + 4; IF.occupied = true; break;
        case BNE: PC = (rs1_val != rs2_val) ? (pc + (int)imm) & (-2u) : pc + 4; IF.occupied = true; break;
        case BLT: PC = ((int)rs1_val < (int)rs2_val) ? (pc + (int)imm) & (-2u) : pc + 4; IF.occupied = true; break;
        case BLTU: PC = (rs1_val < rs2_val) ? (pc + (int)imm) & (-2u) : pc + 4; IF.occupied = true; break;
        case BGE: PC = ((int)rs1_val >= (int)rs2_val) ? (pc + (int)imm) & (-2u) : pc + 4; IF.occupied = true; break;
        case BGEU: PC = (rs1_val >= rs2_val) ? (pc + (int)imm) & (-2u) : pc + 4; IF.occupied = true; break;
        case LB: _mem->address = rs1_val + imm; break;
        case LBU: case LH: case LHU: case LW:
            _mem->address = rs1_val + imm; break;
        case SB: case SH: case SW:
            _mem->address = rs1_val + imm; _mem->val = rs2_val; break;
    }
}

void _MEM::op(_WB *_wb){
    if(_wb->occupied || !occupied) return;
    occupied = false;
    _wb->rd = rd;
    _wb->type = type;
    _wb->occupied = true;
    switch (type){
        case Null: break;
        case ADDI: case SLTI: case SLTIU: case XORI: case ORI: case ANDI: case SLLI: case SRLI: case SRAI:
        case ADD: case SUB: case SLT: case SLTU: case XOR: case SLL: case SRL: case SRA: case OR: case AND:
        case LUI: case AUIPC:
            _wb->rd_val = rd_val;
            break;
        case JAL: case JALR:
            _wb->rd_val = rd_val;
            break;
        case BEQ: case BNE: case BLT: case BLTU: case BGE: case BGEU:
            break;
        case LB:
            char tmp_b;
            memcpy(&tmp_b, _memory + address, sizeof(char));
            _wb->rd_val = (uint)tmp_b;
            break;
        case LBU:
            unsigned char tmp_bu;
            memcpy(&tmp_bu, _memory + address, sizeof(unsigned char));
            _wb->rd_val = (uint)tmp_bu;
            break;
        case LH:
            short tmp_h;
            memcpy(&tmp_h, _memory + address, sizeof(short));
            _wb->rd_val = (uint)tmp_h;
            break;
        case LHU:
            unsigned short tmp_hu;
            memcpy(&tmp_hu, _memory + address, sizeof(unsigned short));
            _wb->rd_val = (uint)tmp_hu;
            break;
        case LW:
            uint tmp_w;
            memcpy(&tmp_w, _memory + address, sizeof(uint));
            _wb->rd_val = tmp_w;
            break;
        case SB:
            char tmp_sb;
            tmp_sb = (char)val;
            memcpy(_memory + address, &tmp_sb, sizeof(char));
            break;
        case SH:
            short tmp_sh;
            tmp_sh = (short)val;
            memcpy(_memory + address, &tmp_sh, sizeof(short));
            break;
        case SW:
            memcpy(_memory + address, &val, sizeof(int));
            break;
    }
}

void _WB::op(){
    if(!occupied) return;
    occupied = false;
    reg_occupied[rd] = false;
    if(rd == 0) return;
    switch (type){
        case Null: break;
        case ADDI: case SLTI: case SLTIU: case XORI: case ORI: case ANDI: case SLLI: case SRLI: case SRAI:
        case ADD: case SUB: case SLT: case SLTU: case XOR: case SLL: case SRL: case SRA: case OR: case AND:
        case LUI: case AUIPC:
        case JAL: case JALR:
        case LB: case LBU: case LH: case LHU: case LW:
            _register[rd] = rd_val;
            break;
        default:
            break;
    }
}