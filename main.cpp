//
// Created by sunshine on 2020/7/7.
//

#include <bits/stdc++.h>
#include "stage.h"

int main(){
    freopen("../riscv-testcases/testcases/array_test2.data", "r", stdin);
    char ini[10] = {0};
    int cur_mem = 0;
    while(scanf("%s", ini) != EOF){
        if(ini[0] == '@'){
            sscanf(&ini[1], "%x", &cur_mem);
        } else{
            int tmp;
            sscanf(ini, "%x", &tmp);
            _memory[cur_mem++] = (uint) tmp;
            scanf("%x", &tmp);
            _memory[cur_mem++] = (uint) tmp;
            scanf("%x", &tmp);
            _memory[cur_mem++] = (uint) tmp;
            scanf("%x", &tmp);
            _memory[cur_mem++] = (uint) tmp;
        }
    }
    IF.pc = 0u;
    IF.occupied = true;
    int cnt = 0;
    while (ID.ins != 0x0ff00513 || IF.occupied || ID.occupied || EX.occupied || MEM.occupied || WB.occupied){
        ++cnt;
        IF.op(&ID);
        ID.op(&EX);
        EX.op(&MEM);
        MEM.op(&IF, &WB);
        WB.op();
    }
    printf("%u\n", _register[10] & 255u);
    return 0;
}
