#include <am.h>
#include <nemu.h>
#include <klib.h>


static AddrSpace kas = {};
static void* (*pgalloc_usr)(int) = NULL;
static void (*pgfree_usr)(void*) = NULL;
static int vme_enable = 0;

static Area segments[] = {      // Kernel memory mappings
  NEMU_PADDR_SPACE
};

#define USER_SPACE RANGE(0x40000000, 0xc0000000)

bool vme_init(void* (*pgalloc_f)(int), void (*pgfree_f)(void*)) {
  pgalloc_usr = pgalloc_f;
  pgfree_usr = pgfree_f;

  kas.ptr = pgalloc_f(PGSIZE);

  int i;
 
  for (i = 0; i < LENGTH(segments); i ++) {
    void *va = segments[i].start;
    for (; va < segments[i].end  ; va += PGSIZE) {
      //printf("%d end %x\n",i,(uint32_t)segments[i].end);
     // printf("map va%x\n",va);
      map(&kas, va, va, 0);
    }
  }

  set_cr3(kas.ptr);
  set_cr0(get_cr0() | CR0_PG);
  //set_cr0(0x234 | 0x80000000);
  vme_enable = 1;
  //printf("vme_inti end\n");
  return true;
}

void protect(AddrSpace *as) {
  PTE *updir = (PTE*)(pgalloc_usr(PGSIZE));
  as->ptr = updir;
  as->area = USER_SPACE;
  as->pgsize = PGSIZE;
  // map kernel space
  memcpy(updir, kas.ptr, PGSIZE);
}

void unprotect(AddrSpace *as) {
}

void __am_get_cur_as(Context *c) {
  c->cr3 = (vme_enable ? (void *)get_cr3() : NULL);
}

void __am_switch(Context *c) {
  if (vme_enable && c->cr3 != NULL) {
    set_cr3(c->cr3);
  }
}


void map(AddrSpace *as, void *va, void *pa, int prot) {//设置页表
  assert(NULL != as->ptr);

//printf("%p  %d\n",va,n);


  uint32_t off_dir = (uint32_t)va >> 22;
  uint32_t off_pte  = ((uint32_t)va & 0x003ff000 ) >> 12;
  //uint32_t pageoff = (uint32_t)va & 0xfff;
  //printf("%p %p %p\n", pdiroff, pteoff, pageoff);

  uint32_t *dir = (uint32_t *)as->ptr;

  uint32_t* pte = &dir[off_dir];



  if((*pte & 1)==0){
  uint32_t* tem = (uint32_t*)pgalloc_usr(1024*4);
  *pte = ((uint32_t)tem &  0xfffff000) | 1;

  }

   
  //printf("%p\n",(*ptebase & 0xfffff000)+ pteoff*4);

  *(uint32_t*)((int)(*pte & 0xfffff000)+ off_pte*4) = ((uint32_t)pa & 0xfffff000) | 1;



  
}


Context* ucontext(AddrSpace *as, Area kstack, void *entry) {
  Context* ucontext = kstack.end -sizeof(Context);
  ucontext->eip = (uintptr_t) entry;
  ucontext->cs = 0x8; 
  ucontext->eflags |= 0x200;
  ucontext->cr3 = as->ptr;
  //ucontext->ksp = (uint32_t)ucontext;
  return ucontext;
}
