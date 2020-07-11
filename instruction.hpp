//
// Created by sunshine on 2020/7/7.
//

#ifndef RISC_V_SIMULATOR_INSTRUCTION_HPP
#define RISC_V_SIMULATOR_INSTRUCTION_HPP

#include "variate.h"

enum ins_type{
    Null,
    //Integer Computational Instructions
    ADDI, SLTI, SLTIU, XORI, ORI, ANDI, SLLI, SRLI, SRAI,   ///I
    ADD, SUB, SLT, SLTU, XOR, SLL, SRL, SRA, OR, AND,       ///R
    LUI, AUIPC,                                             ///U

    //Unconditional Jump
    JAL,                                                    ///J
    JALR,                                                   ///I

    //Conditional branch
    BEQ, BNE, BLT, BLTU, BGE, BGEU,                         ///B

    //Load And Store
    LB, LBU, LH, LHU, LW,                                   ///I
    SB, SH, SW,                                             ///S

    /*  1.ADDI  2.SLTI  3.SLTIU 4.XORI  5.ORI   6.ANDI  7.SLLI
     *  8.SRLI  9.SRAI  10.ADD  11.SUB  12.SLT  13.SLTU 14.XOR
     *  15.SLL  16.SRL  17.SRA  18.OR   19.AND  20.LUI  21.AUIPC
     *  22.JAL  23.JALR 24.BEQ  25.BNE  26.BLT  27.BLTU 28.BGE
     *  29.BGEU 30.LB   31.LBU  32.LH   33.LHU  34.LW   35.SB
     *  36.SH   37.SW
     */
};

class instruction{
public:
    ins_type _type;
    uint opcode;
    uint imm;
    uint rs1, rs2, rd;
    uint funct3, funct7;
    uint cur_ins;

private:
    //do signed_extension unconditionally, change sign in particular instruction (those finished with U)
    void I_imm(){
        imm = 0u;
        if((cur_ins & 0x80000000) != 0x80000000)
            imm |= cur_ins >> 20u;
        else {
            imm = 0xfffff800u;
            imm |= cur_ins >> 20u;
        }
    }
    void S_imm(){
        imm = 0u;
        imm |= (cur_ins >> 7u) & 0x1fu;
        if((cur_ins & 0x80000000) == 0x80000000)
            imm |= 0xfffff800u;
        imm |= (cur_ins >> 20u) & 0x7e0u;
    }
    void B_imm(){
        imm = 0u;
        if((cur_ins >> 31u) == 1u)
            imm |= 0xfffff000u;
        imm |= ((cur_ins << 4u) & 0x800u);
        imm |= ((cur_ins >> 7u) & 0x1eu);
        imm |= ((cur_ins >> 20u) & 0x7e0u);
    }
    void U_imm(){
        imm = 0u;
        imm |= cur_ins & 0xfffff000u;
    }
    void J_imm(){
        imm = 0u;
        if((cur_ins >> 31u) == 1u)
            imm |= 0xfff00000u;
        imm |= cur_ins & 0xff000u;
        imm |= (cur_ins >> 20u) & 0x7feu;
        imm |= (cur_ins >> 9u) & 0x800u;
    }

public:
    instruction(): _type(Null), opcode(0u), imm(0u), rs1(0u), rs2(0u), rd(0u), funct3(0u), funct7(0u){};
    explicit instruction(uint ins){                 // ID
        _type = Null;
        imm = 0u;
        rs1 = rs2 = rd = 0u;
        funct3 = funct7 = 0u;
        cur_ins = ins;
        opcode = ins & 127u;
        switch (opcode){
            case 0x13:
                I_imm();
                rd |= (ins >> 7u) & 0x1fu;
                rs1 |= (ins >> 15u) & 0x1fu;
                funct3 |= (ins >> 12u) & 0x7u;
                switch (funct3){
                    case 0u:    _type = ADDI;   break;
                    case 2u:    _type = SLTI;   break;
                    case 3u:    _type = SLTIU;  break;
                    case 4u:    _type = XORI;   break;
                    case 6u:    _type = ORI;    break;
                    case 7u:    _type = ANDI;   break;
                    case 1u:    _type = SLLI;   break;
                    case 5u:
                        switch (ins >> 30u) {
                            case 0u:    _type = SRLI;   break;
                            case 1u:    _type = SRAI;   break;
                        }   break;
                }   break;
            case 0x33:
                rd |= (ins >> 7u) & 0x1fu;
                rs1 |= (ins >> 15u) & 0x1fu;
                rs2 |= (ins >> 20u) & 0x1fu;
                funct3 |= (ins >> 12u) & 0x7u;
                funct7 |= (ins >> 25u)  &0x7fu;
                switch (funct3){
                    case 0u:
                        switch(funct7){
                            case 0u:    _type = ADD;    break;
                            case 0x20u: _type = SUB;    break;
                        }   break;
                    case 1u:    _type = SLL;    break;
                    case 2u:    _type = SLT;    break;
                    case 3u:    _type = SLTU;   break;
                    case 4u:    _type = XOR;    break;
                    case 5u:
                        switch(funct7){
                            case 0u:    _type = SRL;    break;
                            case 0x20u: _type = SRA;    break;
                        }   break;
                    case 6u:    _type = OR;    break;
                    case 7u:    _type = AND;   break;
                }   break;
            case 0x63:
                B_imm();
                rs1 |= (ins >> 15u) & 31u;
                rs2 |= (ins >> 20u) & 31u;
                funct3 |= (ins >> 12u) & 0x7u;
                switch(funct3){
                    case 0u:    _type = BEQ;    break;
                    case 1u:    _type = BNE;    break;
                    case 4u:    _type = BLT;    break;
                    case 5u:    _type = BGE;    break;
                    case 6u:    _type = BLTU;   break;
                    case 7u:    _type = BGEU;   break;
                }   break;
            case 0x3:
                I_imm();
                rd |= (ins >> 7u) & 31u;
                rs1 |= (ins >> 15u) & 31u;
                funct3 |= (ins >> 12u) & 0x7u;
                switch(funct3){
                    case 0u:    _type = LB;     break;
                    case 1u:    _type = LH;     break;
                    case 2u:    _type = LW;     break;
                    case 4u:    _type = LBU;    break;
                    case 5u:    _type = LHU;    break;
                }   break;
            case 0x23:
                S_imm();
                rs1 |= (ins >> 15u) & 31u;
                rs2 |= (ins >> 20u) & 31u;
//                rd = rs2;
                funct3 |= (ins >> 12u) & 0x7u;
                switch(funct3){
                    case 0u:    _type = SB;     break;
                    case 1u:    _type = SH;     break;
                    case 2u:    _type = SW;     break;
                }   break;
            case 0x37:
                U_imm();
                rd |= (ins >> 7u) & 31u;
                _type = LUI;
                break;
            case 0x17:
                U_imm();
                rd |= (ins >> 7u) & 31u;
                _type = AUIPC;
                break;
            case 0x6f:
                J_imm();
                rd |= (ins >> 7u) & 31u;
                _type = JAL;
                break;
            case 0x67:
                I_imm();
                rd |= (ins >> 7u) & 31u;
                rs1 |= (ins >> 15u) & 31u;
                funct3 |= (ins >> 12u) & 0x7u;
                _type = JALR;
                break;
        }
    }

    };

#endif // RISC_V_SIMULATOR_INSTRUCTION_HPP