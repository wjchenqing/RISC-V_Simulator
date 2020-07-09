//
// Created by sunshine on 2020/7/7.
//

#include <bits/stdc++.h>
#include "stage.h"

int main(){
    char ini[10] = {0};
    int cur_mem = 0;
    while(scanf("%s", ini) != EOF){
        if(ini[0] == '@'){
            sscanf(&ini[1], "%x", &cur_mem);
        } else{
            int tmp;
            sscanf(ini, "%x", &tmp);
            _memory[cur_mem+3] = (uint) tmp;
            scanf("%x", &tmp);
            _memory[cur_mem+2] = (uint) tmp;
            scanf("%x", &tmp);
            _memory[cur_mem+1] = (uint) tmp;
            scanf("%x", &tmp);
            _memory[cur_mem] = (uint) tmp;
            cur_mem += 4;
        }
    }
    IF.pc = 0u;
    IF.occupied = true;
    while (ID.ins != 0x0ff00513 || IF.occupied || ID.occupied || EX.occupied || MEM.occupied || WB.occupied){
        IF.op(&ID);
        ID.op(&EX);
        EX.op(&MEM);
        MEM.op(&IF, &WB);
        WB.op();
    }
    printf("%u\n", _register[10] & 255u);
    return 0;
}
