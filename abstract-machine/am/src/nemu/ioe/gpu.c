#include <am.h>
#include <nemu.h>
#include<stdio.h>
#include<string.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)

#define W    400
#define H    300
#define FPS   60



void __am_gpu_init() {
/*
int i;

  int h = inl(VGACTL_ADDR)&0xffff;
  int w = inl(VGACTL_ADDR)>>16;

uint32_t *fb  = (uint32_t *)(uintptr_t)FB_ADDR;
for (i = 0;i<w*h;i++) fb[i]= i;
outl(SYNC_ADDR,1);

*/
}



void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {

  int h = inl(VGACTL_ADDR)&0xffff;
  int w = inl(VGACTL_ADDR)>>16;
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = w, .height = h,
    .vmemsz = w*h*sizeof(uint32_t)
  };

}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {

  int h = inl(VGACTL_ADDR)&0xffff;
  int w = inl(VGACTL_ADDR)>>16;
  int x = ctl->x, y = ctl->y;
  //printf("w is %d\n",h);
  uint32_t *pixels = ctl->pixels;
  uint32_t *fb  = (uint32_t *)(uintptr_t)FB_ADDR;
  for (int j = y; j<h && j<(y+ctl->h);j++){
    for (int i = x;i<w && i<(x + ctl->w);i++){
      *(fb+j*w+i) = *pixels++;
    }
  }
  if(ctl->sync){
    outl(SYNC_ADDR,1);
  }

  
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}

