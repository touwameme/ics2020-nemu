#include <am.h>
#include <x86.h>
#include <klib.h>

#define NR_IRQ         256     // IDT size
#define SEG_KCODE      1
#define SEG_KDATA      2

static Context* (*user_handler)(Event, Context*) = NULL;

void __am_irq0();
void __am_vecsys();
void __am_vectrap();
void __am_vecnull();

 void __am_get_cur_as(Context *c);
 bool __am_switch(Context *c);

Context* __am_irq_handle(Context *c) {

  __am_get_cur_as(c);
  //printf("cur as is %p\n",c);

  if (user_handler) {
    Event ev = {0};
    switch (c->irq) {
      case 0x20: ev.event = EVENT_IRQ_TIMER;break;
      case 0x80 : ev.event = EVENT_SYSCALL; break;
      case 0x81 : ev.event = EVENT_YIELD; break;
      default: printf("unkown irq %x\n",c->irq);ev.event = EVENT_ERROR; break;
    }

    c = user_handler(ev, c);
    //printf("after handle_irq  context %p\n",c);
    assert(c != NULL);
  }
  __am_switch(c);
  return c;
}

bool cte_init(Context*(*handler)(Event, Context*)) {
  static GateDesc32 idt[NR_IRQ];

  // initialize IDT
  for (unsigned int i = 0; i < NR_IRQ; i ++) {
    idt[i]  = GATE32(STS_TG, KSEL(SEG_KCODE), __am_vecnull, DPL_KERN);
  }

  // ----------------------- interrupts ----------------------------
  idt[32]   = GATE32(STS_IG, KSEL(SEG_KCODE), __am_irq0,    DPL_KERN);
  // ---------------------- system call ----------------------------
  idt[0x80] = GATE32(STS_TG, KSEL(SEG_KCODE), __am_vecsys,  DPL_USER);
  idt[0x81] = GATE32(STS_TG, KSEL(SEG_KCODE), __am_vectrap, DPL_KERN);

  set_idt(idt, sizeof(idt));

  // register event handler
  user_handler = handler;
  return true;
}


Context* kcontext(Area kstack, void (*entry)(void *), void *arg) {
  Context* kcontext = kstack.end -sizeof(Context)- 2*sizeof(uintptr_t);
  //printf("kcontext arg at %p\n",kcontext-sizeof(uintptr_t));
  //printf("kstack start: %p, end: %p\n",kstack.start,kstack.end);
  *(uintptr_t*)(kstack.end-sizeof(uintptr_t)) = (uintptr_t)arg;
    kcontext->cr3 = NULL;
    kcontext->eip = (uintptr_t) entry;
    kcontext->cs = 0x8; 
    kcontext->eflags |= 0x200;   //set IF 1
    
  
  return kcontext;
}
void yield() {
 // printf("am_yield\n");
  asm volatile("int $0x81");
}

void *sbrk(intptr_t increment) {
  return (void *)0;
}

bool ienabled() {
  return false;
}

void iset(bool enable) {
}
