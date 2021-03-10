#include <cpu/exec.h>
#include "local-include/rtl.h"
#include <stdio.h>

void raise_intr(DecodeExecState *s, uint32_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
  uint32_t idt_addr = cpu.idt.base;
  uint32_t gate_addr = idt_addr + 8*NO;
  //printf("NO is %x  idte is %x",NO,idt_addr);
  int32_t jmp_addr = vaddr_read(gate_addr,2)+ (vaddr_read(gate_addr+6,2)<<16);
 
  rtl_push(s,&cpu.eflags);

 rtl_push(s,&cpu.cs);
 rtl_push(s,&ret_addr);
 
 cpu.eflags &= (~EFLAG_IF);
 
  rtl_j(s,jmp_addr);
}

void query_intr(DecodeExecState *s) {
  TODO();
}
