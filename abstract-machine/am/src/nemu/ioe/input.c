#include <am.h>
#include <nemu.h>

#define KEYDOWN_MASK 0x8000

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
 /*
  int k =inl(KBD_ADDR);
  kbd->keydown = (k & KEYDOWN_MASK ? true : false);
  kbd->keycode = k & ~KEYDOWN_MASK;
 */
 int k = inw(KBD_ADDR);
 kbd->keydown = k>>15;
 kbd->keycode = k & 0xff;

}