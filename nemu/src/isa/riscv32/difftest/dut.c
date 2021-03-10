#include <isa.h>
#include <monitor/difftest.h>
#include "../local-include/reg.h"
#include "difftest.h"

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  printf("here in riscv\n");
   if(pc!=ref_r->pc){
    printf("pc wrong at %x  the ref_pc is %x\n",pc,ref_r->pc);
     return false;
  }  
  printf("near true\n");
  return false;
}

void isa_difftest_attach() {
}
