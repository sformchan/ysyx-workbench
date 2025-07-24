#ifndef ROM_H
#define ROM_H

#include <stdint.h>
#define ysyx_25020047_INITADDR 0x00000000
#define ysyx_25020047_SIZE 3

extern uint32_t rom[ysyx_25020047_SIZE]; 


uint32_t read_inst(uint32_t pc);

#endif