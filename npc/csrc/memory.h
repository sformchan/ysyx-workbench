#ifndef __MEMORY_H__
#define __MEMORY_H__

int pmem_read(int raddr);
void pmem_write(int waddr, int wdata, int wmask);
void load_verilog_hex(const char *filename);
long load_img();
int parse_args(int argc, char *argv[]);
extern char *img_file;


#endif