//
// Created by sunshine on 2020/7/7.
//
#include <bits/stdc++.h>
#include "variate.hpp"
#include "instruction.hpp"
using namespace std;

int main(){
    char ini[10] = {0};
    uint cur_mem = 0;
    while(scanf("%s", ini) != EOF){
        if(ini[0] == '@'){
            sscanf(&ini[1], "%x", &cur_mem);
        } else{
            sscanf(ini, "%x", &_memory[cur_mem + 3]);
            scanf("%x", &_memory[cur_mem + 2]);
            scanf("%x", &_memory[cur_mem + 1]);
            scanf("%x", &_memory[cur_mem]);
            cur_mem += 4;
        }
    }
}
