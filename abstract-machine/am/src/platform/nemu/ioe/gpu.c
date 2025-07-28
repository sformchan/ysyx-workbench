#include <am.h>
#include <nemu.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)  //corresponding to the vgactl_port_base[1]



void __am_gpu_init() {
	int i;
	int w = 400;  // TODO: get the correct width
	int h = 300;  // TODO: get the correct height
	uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
	for (i = 0; i < w * h; i ++) fb[i] = i;
	outl(SYNC_ADDR, 1);
}


void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
	uint32_t rawdata = inl(RTC_ADDR);
	uint32_t width = rawdata >> 16;
	uint32_t height = rawdata & 0xffff;
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = width, .height = height,
    .vmemsz = width * height * sizeof(uint32_t)
  };
}

// void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
//   *cfg = (AM_GPU_CONFIG_T) {
//     .present = true, .has_accel = false,
//     .width = 400, .height = 300,
//     .vmemsz = 400 * 300 * sizeof(uint32_t)
//   };
// }

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
	//uint32_t *pixels = (uint32_t *)(uintptr_t)ctl->pixels;

  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
