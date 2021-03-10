#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/time.h>
#include <assert.h>
int FB_W = -1;
int FB_H = -1;
void NDL_updatefb();
int cen_off_w = 0;
int cen_off_h = 0;
size_t events_read(void *buf, size_t offset, size_t len);

static int evtdev = -1;
static int fbdev = -1;
static int screen_w = 0, screen_h = 0;

static struct timeval boottime;

uint32_t NDL_GetTicks() {
  
  struct timeval now ;
  gettimeofday(&now,NULL);
  uint32_t ticks= (now.tv_sec - boottime.tv_sec)*1000 + (now.tv_usec - boottime.tv_usec)/1000 ;
  //printf("ticks is %d\n",ticks);
  return ticks;
}
int open(const char *path, int flags, ...);

int NDL_PollEvent(char *buf, int len) {
  //printf("NDL_Pollevent\n");
  int fd = open("/dev/events",0,0);
  int res = read(fd,buf,len);
  //printf("res is %d\n",res);
  close(fd);  
  
  return res;
}



void NDL_OpenCanvas(int *w, int *h) {
  
  //printf("opencanvas\n");
    int fd = open("/proc/dispinfo",0,0);
    char buf[128]={0};
    read(fd,&buf,sizeof(buf));
    //printf("buf: %s\n",buf);
    sscanf(buf,"%*[^:]:%*[ ]%d\n%*[^:]:%*[ ]%d\n",&FB_W,&FB_H);
    //printf("w is %d h is %d\n",FB_W,FB_H);
    assert(FB_W >=0);

  if (getenv("NWM_APP")) {
    int fbctl = 4;
    fbdev = 5;
    if(*w == 0 || *h ==0){
      *w = FB_W;
      *h = FB_H;
    }
    screen_w = *w; screen_h = *h;
    
    
    char buf[64];
    int len = sprintf(buf, "%d %d", screen_w, screen_h);
    // let NWM resize the window and create the frame buffer
    write(fbctl, buf, len);
    while (1) {
      // 3 = evtdev
      int nread = read(3, buf, sizeof(buf) - 1);
      if (nread <= 0) continue;
      buf[nread] = '\0';
      if (strcmp(buf, "mmap ok") == 0) break;
    }
    close(fbctl);
  }
  else{
    if(*w ==0 || *h ==0){
    *w = FB_W;
    *h = FB_H;
   // printf("hw == 0\n");
    }
  }
  close(fd);
 // printf("opencarnan W is %d H is %d\n",*w,*h);
}

void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) { 

 int canvas_w = FB_W;
 int canvas_h= FB_H;

 // centralize

 
 x += cen_off_w;
 y += cen_off_h;
 
 

 //printf("x[%d]  W[%d]  w[%d]  y[%d]  H[%d]  h[%d]\n'",x,FB_W,w,y,FB_H,h);
 int fd = open("/dev/fb",0,0);
 
 //printf("fd is %d\n",fd);
 int offset = ((y>=0?y:0)*canvas_w + (x>=0?x:0));
 int len;
 if(x>=0){
 len = (x+w<canvas_w)?w:canvas_w-x;
 }
 else{
   len = (x+w<canvas_w)?x+w:canvas_w;
 }


 
 int cnt_r = 0;
  for (int i = 0;i<h;i++){
    if(  y+i >=0 && y+i<canvas_h){
    lseek(fd,offset+cnt_r*(canvas_w),SEEK_SET);
    write(fd,pixels+i*w,len);
    cnt_r++;
    }
  }
 close(fd);
}

void NDL_OpenAudio(int freq, int channels, int samples) {
}

void NDL_CloseAudio() {
}

int NDL_PlayAudio(void *buf, int len) {
  return 0;
}

int NDL_QueryAudio() {
  return 0;
}

void NDL_updatefb(){
  int fd = open("/proc/dispinfo",0,0);
  char buf[128]={0};
 // printf("read dispinfo \n");
  read(fd,buf,sizeof(buf));
 // printf("buf: %s\n",buf);
  sscanf(buf,"%*[^:]:%*[ ]%d\n%*[^:]:%*[ ]%d\n",&FB_W,&FB_H);
  //printf("w is %d h is %d\n",FB_W,FB_H);
  assert(FB_W >=0);
  close(fd);
}

int NDL_Init(uint32_t flags) {
  if (getenv("NWM_APP")) {
    evtdev = 3;
  }
  //printf("NDL_init\n");
  gettimeofday(& boottime,NULL);
  
  NDL_updatefb();
  return 0;
}

void NDL_Quit() {
}
