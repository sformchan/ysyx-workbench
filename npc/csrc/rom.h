#ifndef ROM_H
#define ROM_H

#include <stdint.h>
#define INITADDR 0x80000000
#define SIZE 3

extern uint32_t rom[SIZE]; 


uint32_t read_inst(uint32_t pc);

#endif