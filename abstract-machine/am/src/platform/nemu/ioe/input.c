#include <am.h>
#include <nemu.h>

#define KEYDOWN_MASK 0x8000

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
//   kbd->keydown = 0;
//   kbd->keycode = AM_KEY_NONE;
	uint32_t rawdata = inl(KBD_ADDR);
	kbd->keydown = (rawdata & KEYDOWN_MASK) ? 1 : 0;
	kbd->keycode = (rawdata & ~KEYDOWN_MASK) ? rawdata & ~KEYDOWN_MASK : AM_KEY_NONE;
}
