#include <stdio.h>
#include <stdint.h>
#include "rom.h"



uint32_t rom[SIZE] = {
    0b00000000111100001000000000010011, // 示例指令 1
    0b00000000111100010000000100010011  // 示例指令 2
};

uint32_t read_inst(uint32_t pc) {
    // 检查地址是否对齐到 4 字节
    if (pc % 4 != 0) {
        printf("Error: PC address 0x%08x is not aligned to 4 bytes.\n", pc);
        return 0xFFFFFFFF; // 返回错误码
    }

    // 检查地址是否在合法范围内
    if (pc < INITADDR || pc >= INITADDR + SIZE * 4) {
        printf("Error: PC address 0x%08x is out of ROM range.\n", pc);
        return 0xFFFFFFFF; // 返回错误码
    }

    // 计算数组索引
    uint32_t real_addr = (pc - INITADDR) / 4;

    // 返回指令
    return rom[real_addr];
}


