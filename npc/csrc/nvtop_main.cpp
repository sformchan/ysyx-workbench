#include <nvboard.h>
#include <Vtop.h>
 
static top dut;
 
void nvboard_bind_all_pins(top* top);
 
 
int main()
{
  nvboard_bind_all_pins(&dut);
  nvboard_init();
 
  while(1)
  {
    nvboard_update();
    dut.eval();
  }
}
