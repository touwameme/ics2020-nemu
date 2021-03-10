#include <memory.h>
#include <proc.h>

static void *pf = NULL;

void* new_page(size_t nr_page) {
  void* tem = pf; 
  memset(tem,0,PGSIZE*nr_page); 
  pf =(void*)((int)pf + nr_page*PGSIZE);
 // printf("new_page at %p\n",pf);
  return tem;
}

static inline void* pg_alloc(int n) {   //n 为字节数
  int page_num = (n + PGSIZE - 1) / PGSIZE;
  return new_page(page_num);
}

void free_page(void *p) {
  panic("not implement yet");
}

void map(AddrSpace *as, void *va, void *pa, int prot);
/* The brk() system call handler. */
int mm_brk(uintptr_t brk) {

  if(brk > current->max_brk){
      uint32_t tem = (current->max_brk)&0xfffff000;
      while(tem < brk){
        void* paddr = new_page(1);
        
        
        map(&current->as,(void*)current->max_brk,paddr,0);
        
        tem += 4096;
        current->max_brk += 4096;
      }
  }
      return 0;

}

void init_mm() {
  pf = (void *)ROUNDUP(heap.start, PGSIZE);
  Log("free physical pages starting from %p", pf);

#ifdef HAS_VME
  vme_init(pg_alloc, free_page);
#endif
}
