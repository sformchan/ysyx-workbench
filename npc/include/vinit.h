#ifndef __VINIT_H__
#define __VINIT_H__

#include "Vtop.h"
#include "verilated.h"
#include "svdpi.h"
#include "Vtop__Dpi.h"
//#include "verilated_fst_c.h"


extern VerilatedContext* contextp;
extern Vtop* top;

void init_verilator(int argc, char **argv);


#endif