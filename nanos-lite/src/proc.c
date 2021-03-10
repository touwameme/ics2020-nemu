#include <proc.h>

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;
static PCB * fg_pcb = &pcb[1];
void switch_boot_pcb() {
  current = &pcb_boot;
  //printf("switch_boot_pcb current %p\n",current);
}

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
    if(j%2 ==0)
        Log("Hello World from Nanos-lite with arg '%p' for the %dth time!", (uintptr_t)arg, j/2);
    j ++;
    yield();
  }
}


int naive_uload(PCB *pcb, const char *filename);
void context_uload(PCB*pcb,const char *filename,char * const argv[],char *const envp[]);
void context_kload(PCB *pcb,void (*entry)(void *), void *arg);



void init_proc() {
  // void* arg1 = (void*)1;
  
  
  char* argv[]= {"--skip",NULL};
  //char* envp[] = {NULL};
  char* empty[] ={NULL};
 // context_kload(&pcb[1],hello_fun,arg1);
  context_uload(&pcb[0],"/bin/hello",empty,empty);
  
  //printf("pcb0 %p\n",&pcb[0]);
  //printf("pcb0 cp %p\n",&pcb[0].cp);
  //printf("kload over pcb[0] context at %p\n",pcb[0].cp);
  
  context_uload(&pcb[1],"/bin/nterm",argv,empty);
  context_uload(&pcb[2],"/bin/menu",empty,empty);
  context_uload(&pcb[3],"/bin/pal",argv,empty);
  //printf("pcb1 %p\n",&pcb[1]);
   //printf("pcb1 cp %p\n",&pcb[1].cp);
  switch_boot_pcb();
  //printf("hello\n");
  //naive_uload(NULL,"/bin/menu") ;
  //hello_fun(arg1);
 
 // naive_uload(NULL,"/bin/nterm") ;

  Log("Initializing processes...");

  // load program here

}

extern PCB* current;


int sche_i = 0;


Context* schedule(Context *prev) {
  //printf("prev %p\n",prev);
  
  current->cp = prev;

  //current = &pcb[1];
  //return current->cp;

  if((sche_i %1000)==0 ){
    current = &pcb[0];
    sche_i = (sche_i+1)%1000;
  }
  else{
    current = fg_pcb;
    sche_i = (sche_i+1)%1000;
  }
  //current  = (current == &pcb[0])? &pcb[1]: &pcb[0];
  
  return current->cp;
}


char* files[] ={"/bin/hello","/bin/nterm","/bin/menu","/bin/pal"};

void change_fg(int i){
  if(i ==1 || i ==2 || i ==3)
  fg_pcb = &pcb[i];
  else{
    printf("input 1,2 or 3\n");
  }

//char* empty[] = {NULL};
   
  //context_uload(current,files[i],empty,empty); 
  switch_boot_pcb();
  yield();
}
