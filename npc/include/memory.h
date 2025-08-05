#ifndef __MEMORY_H__
#define __MEMORY_H__



#include <stdint.h>

extern "C" int pmem_read(int raddr, int flag);
extern "C" void pmem_write(int waddr, int wdata, int wmask);
void load_verilog_hex(const char *filename);
long load_img();
extern int parse_args(int argc, char *argv[]);
uint8_t *guest_to_host(uint32_t paddr);


//#define ysyx_25020047_MEM_SIZE 128 * 1024 * 1024
#define ysyx_25020047_MEM_SIZE 128 * 1024 * 1024
#define ysyx_25020047_INITADDR 0x80000000
#define ysyx_25020047_RESET_VECTOR 0x80000000
#define ysyx_25020047_IMG_SIZE 28

extern uint8_t rom[];
extern uint8_t img[];
#endif