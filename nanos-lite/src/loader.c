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



static uintptr_t loader(PCB *pcb, const char *filename) {
Elf_Ehdr elfheader ;
Elf_Phdr proheader;
int fd = fs_open(filename,0,0);
if(fd == -1){
    return 0;
  }
  else{

  //printf("fd is %d\n",fd);

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
     fs_lseek(fd,proheader.p_offset,SEEK_SET);
    
    
     #ifdef HAS_VME
    
    int pagenum = (((proheader.p_vaddr+proheader.p_memsz)&0xfffff000)-(proheader.p_vaddr &0xfffff000))/PGSIZE +1;
    uint32_t vaddr = proheader.p_vaddr;
    uint32_t code_limit = proheader.p_vaddr + proheader.p_filesz;
    //printf("code limit %x\n",proheader.p_vaddr + proheader.p_memsz);
    for(int j=0;j<pagenum;j++){
      uint32_t paddr = (uint32_t)new_page(1);
      paddr = (paddr &0xfffff000) + (vaddr&0xfff);


      if(vaddr < code_limit){
        if((vaddr+PGSIZE)< code_limit){
          
          size_t len = (vaddr&0xfffff000) + PGSIZE - vaddr;
          fs_read(fd,(void*)paddr,len);
          }
        else{
        //printf("L2\n");
        size_t len = code_limit - vaddr;
        //fs_lseek(fd,PGSIZE*j+proheader.p_offset,SEEK_SET);
        fs_read(fd,(void*)paddr,len);
        memset((void*)(paddr+len),0,PGSIZE-len);
           }
      }
      else{
        //printf("L3\n");
        memset((void*)paddr,0,PGSIZE);
      }
      
      map(&pcb->as,(void*)(vaddr&0xfffff000),(void*)paddr,0);
      vaddr = (vaddr&0xfffff000) + PGSIZE;
      pcb->max_brk = vaddr;
      }

    #else 
   // memset((void*)(proheader.p_vaddr),0,proheader.p_memsz);
   
    fs_read(fd,(void*)proheader.p_vaddr,proheader.p_filesz);
    memset((void*)(proheader.p_vaddr + proheader.p_filesz),0,proheader.p_memsz-proheader.p_filesz);
    #endif
   }
  }
 // TODO();
  
  }
  return elfheader.e_entry;
  
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

Context* kcontext(Area kstack, void (*entry)(void *), void *arg);

void context_kload(PCB *pcb,void (*entry)(void *), void *arg){
  Area kstack;
  kstack.start= pcb->stack;
  kstack.end = kstack.start + sizeof(pcb->stack);
  pcb->cp = kcontext(kstack,entry,arg);
  
  
}

void parse_arg(PCB* pcb,const char * filename,char * const argv[],char *const envp[]){
 //printf("parse start\n");
    char* parg =(char*) (pcb->cp->eax) - 0x10;  //在ustack.end下0x10处存放参数
  
  int str_size = 0;
  int argv_num = 0;
  //printf("parse safe\n");
  if(argv[0] != NULL){
    //printf("parse ii safe   argv0 %s\n",argv[0]);
  while(argv[argv_num]!=NULL){str_size += (strlen(argv[argv_num])+1);argv_num++; }
  }
// printf("parse safe\n");

  argv_num++;

  int envp_num = 0;
  
  if(envp[0] != NULL){
  while(envp[envp_num]!=NULL){str_size += (strlen(envp[envp_num])+1);envp_num++;}
  }

  int n = 1+ strlen(filename);
  str_size +=n;
//printf("parse safe\n");
  char* str_area = (char*)((int)parg - str_size);
   *(uintptr_t*)((int)str_area -sizeof(char*)) = 0;
  uintptr_t* envp_area = (uintptr_t*)((int)str_area - sizeof(char*)*(envp_num+1));
   *(uintptr_t*)((int)envp_area -sizeof(char*)) = 0;
  uintptr_t* argv_area = (uintptr_t*)((int)envp_area - sizeof(char*)*(argv_num+1));

  
  parg =(char*)((int)parg - n);
  argv_area[0] = (uintptr_t)parg;
  strcpy(parg,filename);
   //printf("arg%d %s\n",0,filename);
  if(argv_num > 1){
  for(int i = 1;i<argv_num;i++){
    int n = strlen(argv[i-1])+1;
    parg =(char*)((int)parg - n);
    argv_area[i] = (uintptr_t)parg;
    strcpy(parg,argv[i-1]);
   
  }

  }

  if(envp_num != 0){
 for(int i = 0;i<envp_num;i++){
    int n = strlen(envp[i])+1;
    parg =(char*)((int)parg - n);
    envp_area[i] = (uintptr_t)parg;
    strcpy(parg,envp[i]);
  }
  }
  
  int* argc =(int *)((int)(argv_area)-sizeof(int));
  *argc = argv_num;
  
  pcb->cp->eax = (uintptr_t)argc;
  //printf("parse argc at %p\n",pcb->cp->eax );
  
}


extern Area heap;

static char* argv_tem[10] = {NULL}; 

void context_uload(PCB*pcb,const char *filename,char * const argv[],char *const envp[]){
  //printf("c uload  argv at %p  argv0 %s\n",argv,argv[0]);
  
  protect(&pcb->as);
  memset(argv_tem,0,10*sizeof(char*));
  Area kstack;
  kstack.start= pcb->stack;
  kstack.end = kstack.start + sizeof(pcb->stack);
  //printf("ustack start: %p end %p\n",kstack.start,kstack.end);
  //kstack.start= heap.end - sizeof(pcb->stack);
  AddrSpace* as = &pcb->as;
  int argnum = 0;
  for(;argnum<10 && argv[argnum]!=NULL;argnum++ ){
    argv_tem[argnum] = argv[argnum];
  }
  uintptr_t entry = loader(pcb, filename);    //loader 修改了argv的值
  
  pcb->cp = ucontext(as,kstack,(void*)(entry));
  pcb->cp->eax = (intptr_t)(new_page(8))+8*PGSIZE;
 
 int pagenum = 8;  ///stack  32kB


for (int i=0;i<pagenum;i++){
void* paddr = (void*)(pcb->cp->eax -(pagenum - i) * PGSIZE);
map(&pcb->as, (void *)((uint32_t)pcb->as.area.end - (pagenum - i) * PGSIZE), paddr, 0);
//printf("%x\n",(uint32_t)(pcb->as.area.end - (pagenum - i) * PGSIZE));
}
  


 parse_arg(pcb,filename,argv_tem,envp);
 //printf("uload end\n");
}


