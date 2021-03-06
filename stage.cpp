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

int counter[64];
int success = 0, total = 0;

void _IF::op(_ID *_id){
    if(_id->occupied || !occupied) return;
    _id->pc = pc;
    uint tmp = 0u;
    tmp |= ((((uint)_memory[pc+3]) << 24u) | (((uint)_memory[pc+2]) << 16u) | (((uint)_memory[pc+1]) << 8u) | ((uint)_memory[pc]));
    _id->ins = tmp;
    pc += 4;
    _id->occupied = true;
}

void _ID::op(_EX *_ex){
    if(_ex->occupied || !occupied) return;
    if(ins == 0x0ff00513u) {
        occupied = false;
        IF.occupied = false;
        return;
    }
    instruction cur(ins);
    if(cur.rs1 != 0u && reg_occupied[cur.rs1]) return;
    if(cur.rs2 != 0u && reg_occupied[cur.rs2]) return;
    _ex->type = cur._type;
    _ex->pc = pc;
    switch(cur._type){
        case Null: break;
        case ADDI: case SLTI: case SLTIU: case XORI: case ORI: case ANDI:
            if(cur.rs1 == 0u || (reg_occupied[cur.rs1] == 0)){
                _ex->rs1_val = _register[cur.rs1];
            } else if(MEM.rd == cur.rs1){
                _ex->rs1_val = MEM.rd_val;
            } else
                return;
            _ex->imm = cur.imm; _ex->rd = cur.rd; break;
        case SLLI: case SRLI: case SRAI:
            if(cur.rs1 == 0u || (reg_occupied[cur.rs1]) == 0){
                _ex->rs1_val = _register[cur.rs1];
            } else if(MEM.rd == cur.rs1){
                _ex->rs1_val = MEM.rd_val;
            } else return;
            _ex->imm = cur.imm & 0x1fu; _ex->rd = cur.rd; break;
        case ADD: case SUB: case SLT: case SLTU: case XOR: case OR: case AND:
            if(cur.rs1 == 0u || (reg_occupied[cur.rs1]) == 0){
                _ex->rs1_val = _register[cur.rs1];
            } else if(MEM.rd == cur.rs1){
                _ex->rs1_val = MEM.rd_val;
            } else return;
            if(cur.rs2 == 0u || (reg_occupied[cur.rs2]) == 0){
                _ex->rs2_val = _register[cur.rs2];
            } else if(MEM.rd == cur.rs2){
                _ex->rs2_val = MEM.rd_val;
            } else return;
            _ex->rd = cur.rd; break;
        case SLL: case SRL: case SRA:
            if(cur.rs1 == 0u || (reg_occupied[cur.rs1] == 0)){
                _ex->rs1_val = _register[cur.rs1];
            } else if(MEM.rd == cur.rs1){
                _ex->rs1_val = MEM.rd_val;
            } else return;
            if(cur.rs2 == 0u || (reg_occupied[cur.rs2] == 0)){
                _ex->rs2_val = _register[cur.rs2] & 31u;
            } else if(MEM.rd == cur.rs2){
                _ex->rs2_val = MEM.rd_val & 31u;
            } else return;
            _ex->rd = cur.rd; break;
        case LUI:
            _ex->imm = cur.imm; _ex->type = cur._type; _ex->rd = cur.rd; break;
        case AUIPC:
            _ex->imm = cur.imm; _ex->rd = cur.rd; break;
        case JAL:
            _ex->imm = cur.imm; _ex->rd = cur.rd; IF.pc = pc + (int)cur.imm; break;
        case JALR:
            if(cur.rs1 == 0u || (reg_occupied[cur.rs1] == 0)){
                _ex->rs1_val = _register[cur.rs1];
            } else if(MEM.rd == cur.rs1){
                _ex->rs1_val = MEM.rd_val;
            } else return;
            _ex->imm = cur.imm;
            _ex->rd = cur.rd; IF.pc = (_ex->rs1_val + (int)cur.imm) & (-2u); break;
        case BEQ: case BNE: case BLT: case BLTU: case BGE: case BGEU:
            if(cur.rs1 == 0u || (reg_occupied[cur.rs1] == 0)){
                _ex->rs1_val = _register[cur.rs1];
            } else if(MEM.rd == cur.rs1){
                _ex->rs1_val = MEM.rd_val;
            } else return;
            if(cur.rs2 == 0u || (reg_occupied[cur.rs2] == 0)){
                _ex->rs2_val = _register[cur.rs2];
            } else if(MEM.rd == cur.rs2){
                _ex->rs2_val = MEM.rd_val;
            } else return;
            _ex->imm = cur.imm;
            predictor = counter[(pc >> 2u) & 63u] & 2u;
            IF.pc = (predictor ? pc + (int)cur.imm : pc + 4);
            _ex->predictor = predictor;
            ++total;
            break;
        case LB: case LBU: case LH: case LHU: case LW:
            if(cur.rs1 == 0u || (reg_occupied[cur.rs1] == 0)){
                _ex->rs1_val = _register[cur.rs1];
            } else if(MEM.rd == cur.rs1){
                _ex->rs1_val = MEM.rd_val;
            } else return;
            _ex->imm = cur.imm; _ex->rd = cur.rd; break;
        case SB: case SH: case SW:
            if(cur.rs1 == 0u || (reg_occupied[cur.rs1] == 0)){
                _ex->rs1_val = _register[cur.rs1];
            } else if(MEM.rd == cur.rs1){
                _ex->rs1_val = MEM.rd_val;
            } else return;
            if(cur.rs2 == 0u || (reg_occupied[cur.rs2] == 0)){
                _ex->rs2_val = _register[cur.rs2];
            } else if(MEM.rd == cur.rs2){
                _ex->rs2_val = MEM.rd_val;
            } else return;
            _ex->imm = cur.imm; break;
    }
    if(cur.rd != 0u) reg_occupied[cur.rd]++;
    _ex->occupied = true; occupied = false;
}

void _EX::op(_MEM *_mem){
    if(_mem->occupied || !occupied) return;
    occupied = false;
    _mem->type = type;
    _mem->rd = rd;
    _mem->pc = pc;
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
        case JAL: _mem->rd_val = pc + 4; break;
        case JALR: _mem->rd_val = pc + 4; break;
        case BEQ:
            if (rs1_val == rs2_val) {
                counter[(pc >> 2u) & 63u] = std::min(counter[(pc >> 2u) & 63u] + 1, 3);
                if(predictor) {
                    ++success;
                } else{
                    IF.pc =(pc + (int)imm) & (-2u);
                    ID.occupied = false;
                }
            } else{
                counter[(pc >> 2u) & 63u] = std::max(counter[(pc >> 2u) & 63u] - 1, 0);
                if(!predictor) {
                    ++success;
                } else{
                    IF.pc = pc + 4;
                    ID.occupied = false;
                }
            }
            break;
        case BNE: if (rs1_val != rs2_val) {
                counter[(pc >> 2u) & 63u] = std::min(counter[(pc >> 2u) & 63u] + 1, 3);
                if(predictor) {
                    ++success;
                } else{
                    IF.pc =(pc + (int)imm) & (-2u);
                    ID.occupied = false;
                }
            } else{
                counter[(pc >> 2u) & 63u] = std::max(counter[(pc >> 2u) & 63u] - 1, 0);
                if(!predictor) {
                    ++success;
                } else{
                    IF.pc = pc + 4;
                    ID.occupied = false;
                }
            }
            break;
        case BLT: if ((int)rs1_val < (int)rs2_val) {
                counter[(pc >> 2u) & 63u] = std::min(counter[(pc >> 2u) & 63u] + 1, 3);
                if(predictor) {
                    ++success;
                } else{
                    IF.pc =(pc + (int)imm) & (-2u);
                    ID.occupied = false;
                }
            } else{
                counter[(pc >> 2u) & 63u] = std::max(counter[(pc >> 2u) & 63u] - 1, 0);
                if(!predictor) {
                    ++success;
                } else{
                    IF.pc = pc + 4;
                    ID.occupied = false;
                }
            }
            break;
        case BLTU: if (rs1_val < rs2_val) {
                counter[(pc >> 2u) & 63u] = std::min(counter[(pc >> 2u) & 63u] + 1, 3);
                if(predictor) {
                    ++success;
                } else{
                    IF.pc =(pc + (int)imm) & (-2u);
                    ID.occupied = false;
                }
            } else{
                counter[(pc >> 2u) & 63u] = std::max(counter[(pc >> 2u) & 63u] - 1, 0);
                if(!predictor) {
                    ++success;
                } else{
                    IF.pc = pc + 4;
                    ID.occupied = false;
                }
            }
            break;
        case BGE: if ((int)rs1_val >= (int)rs2_val) {
                counter[(pc >> 2u) & 63u] = std::min(counter[(pc >> 2u) & 63u] + 1, 3);
                if(predictor) {
                    ++success;
                } else{
                    IF.pc =(pc + (int)imm) & (-2u);
                    ID.occupied = false;
                }
            } else{
                counter[(pc >> 2u) & 63u] = std::max(counter[(pc >> 2u) & 63u] - 1, 0);
                if(!predictor) {
                    ++success;
                } else{
                    IF.pc = pc + 4;
                    ID.occupied = false;
                }
            }
            break;
        case BGEU: if (rs1_val >= rs2_val) {
                counter[(pc >> 2u) & 63u] = std::min(counter[(pc >> 2u) & 63u] + 1, 3);
                if(predictor) {
                    ++success;
                } else{
                    IF.pc =(pc + (int)imm) & (-2u);
                    ID.occupied = false;
                }
            } else{
                counter[(pc >> 2u) & 63u] = std::max(counter[(pc >> 2u) & 63u] - 1, 0);
                if(!predictor) {
                    ++success;
                } else{
                    IF.pc = pc + 4;
                    ID.occupied = false;
                }
            }
            break;
        case LB: _mem->address = rs1_val + imm; break;
        case LBU: case LH: case LHU: case LW:
            _mem->address = rs1_val + imm; break;
        case SB: case SH: case SW:
            _mem->address = rs1_val + imm; _mem->val = rs2_val; break;
    }
    pc = imm = rs1_val = rs2_val = rd = 0u; type = Null;
}
int mem_cnt = 0;
void _MEM::op(_WB *_wb){
    if(_wb->occupied || !occupied) return;
    if(mem_cnt < 3) {
        mem_cnt++;
        return;
    }
    mem_cnt = 0;
    occupied = false;
    _wb->rd = rd;
    _wb->type = type;
    _wb->occupied = true;
    switch (type){
        case Null: break;
        case ADDI: case SLTI: case SLTIU: case XORI: case ORI: case ANDI: case SLLI: case SRLI: case SRAI:
        case ADD: case SUB: case SLT: case SLTU: case XOR: case SLL: case SRL: case SRA: case OR: case AND:
        case LUI: case AUIPC: case JAL: case JALR:
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
    pc = address = val = rd = rd_val = 0u; type = Null;
}

void _WB::op(){
    if(!occupied) return;
    occupied = false;
    if(rd == 0) return;
    switch (type){
        case Null: break;
        case ADDI: case SLTI: case SLTIU: case XORI: case ORI: case ANDI: case SLLI: case SRLI: case SRAI:
        case ADD: case SUB: case SLT: case SLTU: case XOR: case SLL: case SRL: case SRA: case OR: case AND:
        case LUI: case AUIPC:
        case JAL: case JALR:
        case LB: case LBU: case LH: case LHU: case LW:
            reg_occupied[rd]--;
            _register[rd] = rd_val;
            break;
        default:
            break;
    }
    rd = rd_val = 0u; type = Null;
}