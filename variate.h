//
// Created by sunshine on 2020/7/7.
//

#ifndef RISC_V_SIMULATOR_VARIATE_H
#define RISC_V_SIMULATOR_VARIATE_H

typedef unsigned int uint;
typedef unsigned char uchar;

extern uchar _memory[100000000];
extern uint _register[32];
extern uint reg_occupied[32];

extern uint PC;

#endif //RISC_V_SIMULATOR_VARIATE_H
