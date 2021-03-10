#include <proc.h>
#include <elf.h>
#include<fs.h>

#define DEFALUT_ADDR  0x4000000
extern uint32_t ramdisk_start;

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);
size_t fs_open(const char* pathname,int flags,int mode);
size_t fs_write(int fd,const void* buf,size_t count);
size_t fs_read(int fd, void* buf,size_t len);
size_t fs_close(int fd);
size_t fs_lseek(int fd,size_t offset, int whence);

/*
typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);
size_t invalid_read(void *buf, size_t offset, size_t len);
size_t invalid_write(const void *buf, size_t offset, size_t len);

size_t serial_write(const void *buf, size_t offset, size_t len);
enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB};
typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
  size_t open_offset;
} Finfo;
static Finfo file_table[] __attribute__((used)) = {
  [FD_STDIN]  = {"stdin", 0, 0, invalid_read, invalid_write,0},
  [FD_STDOUT] = {"stdout", 0, 0, invalid_read, serial_write ,0},
  [FD_STDERR] = {"stderr", 0, 0, invalid_read, serial_write,0},

#include "files.h"
};
*/

static uintptr_t loader(PCB *pcb, const char *filename) {
  Elf_Ehdr elfheader ;
  Elf_Phdr proheader;
  int fd = fs_open(filename,0,0);
  //printf("fd is %d\n",fd);
  if(fd == -1){
    return 0;
  }
  else{
  fs_lseek(fd,0,SEEK_SET);
  fs_read(fd,&elfheader,sizeof(Elf_Ehdr));
  int phnum = elfheader.e_phnum;
  fs_lseek(fd,elfheader.e_phoff,SEEK_SET);
  for (int i = 0;i<phnum;i++)
 {
   fs_lseek(fd,elfheader.e_phoff + i*sizeof(Elf_Phdr),SEEK_SET);
   fs_read(fd,&proheader,sizeof(Elf_Phdr));
  //printf("off is %x\npaddr is %x\nvaddr is %x\nfilesize is %x\nmemsize is %x\n",proheader.p_offset,proheader.p_paddr,proheader.p_vaddr,proheader.p_filesz,proheader.p_memsz);
   if(proheader.p_type == PT_LOAD){
   // memset((void*)(proheader.p_vaddr),0,proheader.p_memsz);
   fs_lseek(fd,proheader.p_offset,SEEK_SET);
    fs_read(fd,(void*)proheader.p_vaddr,proheader.p_filesz);
    memset((void*)(proheader.p_vaddr + proheader.p_filesz),0,proheader.p_memsz-proheader.p_filesz);
   }
  }

  
 // TODO();
 
  return elfheader.e_entry;
  }
  fs_close(fd);
}

int naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  //printf("asd\n"); 
  Log("Jump to entry = %p", entry);
  if(entry != 0){
  ((void(*)())entry) ();
  }
  return entry;
}

/*   备份
static uintptr_t loader(PCB *pcb, const char *filename) {
  Elf_Ehdr elfheader ;
  Elf_Phdr proheader;
  //printf("here\n");
  //fs_read(1,&elfheader,sizeof(Elf_Ehdr));

//ramdisk_read(&elfheader,0,sizeof(Elf_Ehdr));

int fd = -1;
fd = fs_open(filename,0,0);
//printf("loading file%d  name :%s\n",fd,file_table[fd].name);
fs_read(fd,&elfheader,sizeof(Elf_Ehdr));

//printf("pnum is %x\n poff is %x\n",elfheader.e_phnum,elfheader.e_phoff);
//printf("nun is %x\n",elfheader.e_phnum);
//printf("poff is %x\n",elfheader.e_phoff);

//printf("off is %x\npaddr is %x\nvaddr is %x\nfilesize is %x\nmemsize is %x\n",proheader.p_offset,proheader.p_paddr,proheader.p_vaddr,proheader.p_filesz,proheader.p_memsz);

  //printf("entry is %p\n",elfheader.e_entry);
  // printf("off is %p\n",proheader.p_offset);
fs_lseek(fd,elfheader.e_phoff,SEEK_SET);
for (int i = 0;i<elfheader.e_phnum;i++){
 // ramdisk_read(&proheader,elfheader.e_phoff+i*elfheader.e_phentsize,sizeof(Elf_Phdr));
  //printf("\n\n\n");

  fs_read(fd,&proheader,sizeof(Elf_Phdr));

  //printf("off is %x\npaddr is %x\nvaddr is %x\nfilesize is %x\nmemsize is %x\n",proheader.p_offset,proheader.p_paddr,proheader.p_vaddr,proheader.p_filesz,proheader.p_memsz);
  

  
  if(proheader.p_type == PT_LOAD){
    
   
    
    void* vaddr = (void*)proheader.p_vaddr;
    uint32_t filesize = proheader.p_filesz;
    uint32_t memsize = proheader.p_memsz;
    uint32_t offset = proheader.p_offset;

    fs_lseek(fd,offset,SEEK_SET);
    fs_read(fd,(void*)vaddr,filesize);

    ramdisk_read(vaddr,offset,filesize) ;
    memset((void*)(vaddr + filesize),0,memsize-filesize);
  }
}



*/
