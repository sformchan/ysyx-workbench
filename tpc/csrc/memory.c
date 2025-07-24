#include <stdio.h>
#include <stdint.h>
#include "rom.h"



uint32_t rom[ysyx_25020047_SIZE] = {
    0b00000011000000111001000100110111, 
    0b00000000000100000000000001110011  // ebreak
};

uint32_t read_inst(uint32_t pc) {
    // 检查地址是否对齐到 4 字节
    if (pc % 4 != 0) {
        printf("Error: PC address 0x%08x is not aligned to 4 bytes.\n", pc);
        return 0xFFFFFFFF; // 返回错误码
    }

    // 检查地址是否在合法范围内
    if (pc < ysyx_25020047_INITADDR || pc >= ysyx_25020047_INITADDR + ysyx_25020047_SIZE * 4) {
        printf("Error: PC address 0x%08x is out of ROM range.\n", pc);
        return 0xFFFFFFFF; // 返回错误码
    }

    // 计算数组索引
    uint32_t real_addr = (pc - ysyx_25020047_INITADDR) / 4;

    // 返回指令
    return rom[real_addr];
}


