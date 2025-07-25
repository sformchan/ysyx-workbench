#include <stdio.h>
#include <stdint.h>


//rom
#define ysyx_25020047_ROM_SIZE 3
#define ysyx_25020047_INITADDR 0x00000000

int rom[ysyx_25020047_ROM_SIZE] = {
    0b00000000000000000001000010110111, 
    0b00000000000000001010000100000011,
    0b00000000000100000000000001110011  // ebreak
};

extern "C" int read_inst(int pc) {
    // 检查地址是否对齐到 4 字节
    if (pc % 4 != 0) {
        printf("\033[31mError: PC address 0x%08x is not aligned to 4 bytes.\033[0m\n", pc);
        return 0xFFFFFFFF; // 返回错误码
    }

    // 检查地址是否在合法范围内
    if (pc < ysyx_25020047_INITADDR || pc >= ysyx_25020047_INITADDR + ysyx_25020047_ROM_SIZE * 4) {
        printf("\033[31mError: PC address 0x%08x is out of ROM range.\033[0m\n", pc);
        return 0xFFFFFFFF; // 返回错误码
    }

    // 计算数组索引
    uint32_t real_addr = (pc - ysyx_25020047_INITADDR) / 4;

    // 返回指令
    return rom[real_addr];
}



//ram
#define ysyx_25020047_RAM_SIZE 64 * 1024  //64kb
int8_t ram[ysyx_25020047_RAM_SIZE] = {1};

extern "C" int pmem_read(int raddr)
{
    raddr &= ~(0x3u);
    if(raddr + 3 > ysyx_25020047_RAM_SIZE)
    {
        printf("\033[31mError: address 0x%08x is out of RAM range.\033[0m\n", raddr);
        return 0xFFFFFFFF; // 返回错误码
    }
    return ram[raddr]| (ram[raddr + 1] << 8) | (ram[raddr + 2] << 16) | (ram[raddr + 3] << 24);
}

extern "C" void pmem_write(int waddr, int wdata, int wmask)
{
    waddr &= ~(0x3u);
    if(waddr + 3 > ysyx_25020047_RAM_SIZE)
    {
        printf("\033[31mError: address 0x%08x is out of RAM range.\033[0m\n", waddr);
        return;
    }
    //wmask & 0x1 means to check if the lowest bit of wmask is set, if not, DO NOT write.
    if (wmask & 0x1) ram[waddr + 0] = (wdata >> 0) & 0xFF;  
    if (wmask & 0x2) ram[waddr + 1] = (wdata >> 8) & 0xFF;
    if (wmask & 0x4) ram[waddr + 2] = (wdata >> 16) & 0xFF;
    if (wmask & 0x8) ram[waddr + 3] = (wdata >> 24) & 0xFF;
}


