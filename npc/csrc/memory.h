#ifndef __MEMORY_H__
#define __MEMORY_H__



#include <stdint.h>

extern "C" int pmem_read(int raddr);
extern "C" void pmem_write(int waddr, int wdata, int wmask);
void load_verilog_hex(const char *filename);
long load_img();
int parse_args(int argc, char *argv[]);
extern char *img_file;


//#define ysyx_25020047_MEM_SIZE 128 * 1024 * 1024
#define ysyx_25020047_MEM_SIZE 128 * 1024 * 1024
#define ysyx_25020047_INITADDR 0x80000000

extern uint8_t rom[];

#endif