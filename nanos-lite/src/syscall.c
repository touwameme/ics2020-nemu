#include <common.h>
#include "syscall.h"
#include <sys/time.h>
#include "../include/proc.h"
size_t fs_open(const char* pathname,int flags,int mode);
size_t fs_write(int fd,const void* buf,size_t count);
size_t fs_read(int fd, void* buf,size_t len);
size_t fs_close(int fd);
size_t fs_lseek(int fd,size_t offset, int whence);

static inline uint32_t inl(int port) {
  uint32_t data;
  asm volatile ("inl %1, %0" : "=a"(data) : "d"((uint16_t)port));
  return data;
}
# define SERIAL_PORT  0x3f8
# define KBD_ADDR     0x60
# define RTC_ADDR     0x48
# define VGACTL_ADDR  0x100
# define AUDIO_ADDR   0x200


void sys_exit(uintptr_t x);

int sys_yield(){
  //printf("sys_yield\n");
  yield();  
  return 0;
}
int naive_uload(PCB *pcb, const char *filename);
void switch_boot_pcb();
void context_uload(PCB*pcb,const char *filename,char *const argv[],char *const envp[]);

static int sys_execve(const char *filename, char *const argv[],char *const envp[]){


      
char* empty[] = {NULL};
context_uload(current,filename, argv,empty);
//printf("here\n");
if(current->cp->eip != -1){
switch_boot_pcb();
//printf("here\n");
yield();
}

return -1;
}

void sys_exit(uintptr_t x){
//halt(0);
char* empty[]={NULL};
 // printf("sys_exit\n");
sys_execve("/bin/nterm",empty,empty);
}




int mm_brk(uintptr_t brk);


int sys_brk(intptr_t x){
 // printf("brk\n");
  #ifdef HAS_VME
  return mm_brk(x);
  #endif 

  return 0;
}

int sys_open(const char* pathname,int flags,int mode){
  return fs_open(pathname,flags,mode);
}

int sys_write(int fd,const void* buf,size_t count){
  //printf("sys write fd is %d  count is  %d\n",fd,count);
  return fs_write( fd, buf,count);
}
int sys_read(int fd, void* buf,size_t len){
  return fs_read(fd,buf,len);
}

int sys_close(int fd){
  //printf("fd is %d\n",fd);
  return fs_close(fd);
}
size_t sys_lseek(int fd,size_t offset, int whence){
  return fs_lseek(fd,offset,whence);
}

size_t sys_gettimeofday(struct timeval* tv,struct  timezone * tz)
{
  tv->tv_usec = inl(RTC_ADDR);
  tv->tv_sec = inl(RTC_ADDR + 4);
  return 0;
  // back  not implement tz
};





Context* do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;
  //printf("\ndo syscall %d\n",a[0]);
  
  switch (a[0]) {
    case SYS_exit: sys_exit(a[1]);break;
    case SYS_yield:c->GPRx =  sys_yield();break;
    case SYS_open: c->GPRx = sys_open((const char*)a[3],a[2],a[1]);break;
    case SYS_read: c->GPRx = sys_read(a[3],(void*)a[2],a[1]);break;
    case SYS_write:c->GPRx = sys_write(a[3],(void*)a[2],a[1]) ;break;
    case SYS_kill: panic("implement syscall_kill");break;
    case SYS_getpid: panic("implement syscall_getpid");break;
    case SYS_close:c->GPRx = sys_close(a[3]);break;
    case SYS_lseek:c->GPRx = sys_lseek(a[3],a[2],a[1]);/*printf("lseek return is %d\n",c->GPRx)*/;break;
    case SYS_brk:  c->GPRx = sys_brk(a[3]);break;
    case SYS_fstat:        panic("implement syscall_fstat");break;
    case SYS_time:         panic("implement syscall_time");break;
    case SYS_signal:       panic("implement syscall_signal");break;
    case SYS_execve:c->GPRx= sys_execve((char*)a[3],(char **)a[2],(char **)a[1]);break;
    case SYS_fork:         panic("implement syscall_fork");break;
    case SYS_link:         panic("implement syscall_link");break;
    case SYS_unlink:       panic("implement syscall_unlink");break;
    case SYS_wait:         panic("implement syscall_wait");break;
    case SYS_times:        panic("implement syscall_times");break;
    case SYS_gettimeofday: c->GPRx =sys_gettimeofday((struct timeval*)a[3],(struct  timezone *)a[2]);break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

//printf("end syscall ret %p\n",c);
return c;
}
/*
enum {
  SYS_exit,
  SYS_yield,
  SYS_open,
  SYS_read,
  SYS_write,
  SYS_kill,
  SYS_getpid,
  SYS_close,
  SYS_lseek,
  SYS_brk,
  SYS_fstat,
  SYS_time,
  SYS_signal,
  SYS_execve,
  SYS_fork,
  SYS_link,
  SYS_unlink,
  SYS_wait,
  SYS_times,
  SYS_gettimeofday
};
*/
