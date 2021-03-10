#include <common.h>
Context* do_syscall(Context *c) ;
Context* schedule(Context *prev);

static Context* do_event(Event e, Context* c) {
  //printf("do_event %d\n",e);
  
  switch (e.event) {
    case EVENT_SYSCALL:return do_syscall(c);break;
    case EVENT_YIELD:return schedule(c);break;
    case EVENT_IRQ_TIMER: Log("EventIrq_Timer");yield();break;
    default: panic("Unhandled event ID = %d", e.event);
  }

  return c;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  cte_init(do_event);
}
