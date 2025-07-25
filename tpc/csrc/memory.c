#include <stdio.h>
#include <stdint.h>
#include <string.h>

//rom
#define ysyx_25020047_ROM_SIZE 16
#define ysyx_25020047_INITADDR 0x00000000


// uint8_t rom[ysyx_25020047_ROM_SIZE] = {
//     0xB7, 0x10, 0x00, 0x00,   
//     0x37, 0xA1, 0x00, 0x00,
//     0x03, 0x21, 0x00, 0x00,
//     0x83, 0x40, 0x10, 0x00,
//     0x23, 0x2c, 0x20, 0x00,  
//     0x73, 0x00, 0x10, 0x00,
//     0x00, 0x00, 0x00, 0x00 //test
//   };

uint8_t rom[ysyx_25020047_ROM_SIZE] = {
    0xB7, 0x90, 0x03, 0x03,
    0x03, 0x21, 0x40, 0x00,
    0x83, 0x40, 0x40, 0x00,
    0x73, 0x00, 0x10, 0x00
  };


extern "C" int pmem_read(int raddr)
{
    raddr &= ~(0x3u);
    if(raddr + 3 > ysyx_25020047_ROM_SIZE)
    {
        printf("\033[31mError: address 0x%08x is out of RAM range.\033[0m\n", raddr);
        return 0xFFFFFFFF; // 返回错误码
    }
    return rom[raddr]| (rom[raddr + 1] << 8) | (rom[raddr + 2] << 16) | (rom[raddr + 3] << 24);
}

extern "C" void pmem_write(int waddr, int wdata, int wmask)
{
    waddr &= ~(0x3u);
    if(waddr + 3 > ysyx_25020047_ROM_SIZE)
    {
        printf("\033[31mError: address 0x%08x is out of RAM range.\033[0m\n", waddr);
        return;
    }
    //wmask & 0x1 means to check if the lowest bit of wmask is set, if not, DO NOT write.
    if (wmask & 0x1) rom[waddr + 0] = (wdata >> 0) & 0xFF, printf("byte0 written\n"); 
    if (wmask & 0x2) rom[waddr + 1] = (wdata >> 8) & 0xFF, printf("byte1 written\n");
    if (wmask & 0x4) rom[waddr + 2] = (wdata >> 16) & 0xFF, printf("byte2 written\n");
    if (wmask & 0x8) rom[waddr + 3] = (wdata >> 24) & 0xFF, printf("byte3 written\n");
}


