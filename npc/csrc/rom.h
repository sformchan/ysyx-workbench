#ifndef ROM_H
#define ROM_H

#include <stdint.h>

#define SIZE 2

extern uint32_t rom[SIZE]; 


uint32_t read_inst(uint32_t pc);

#endif