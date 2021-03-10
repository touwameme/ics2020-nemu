#include "cc.h"

static inline def_EHelper(jmp) {
  // the target address is calculated at the decode stage
  rtl_j(s, s->jmp_pc);

  print_asm("jmp %x", s->jmp_pc);
}

static inline def_EHelper(jcc) {
  // the target address is calculated at the decode stage
  uint32_t cc = s->opcode & 0xf;

  switch(id_dest->width){
    case 1:s->jmp_pc = s->seq_pc + (int8_t)(*ddest);break;
    case 2:s->jmp_pc = s->seq_pc + (int16_t)(*ddest);break;
    case 4:s->jmp_pc = s->seq_pc + (int32_t)(*ddest);break;   //(s->seq_pc & 0x00000000)+(s->seq_pc + *ddest)&0xffffffff;break;
    default: assert(0);
  }
  *s0 = 0;
  rtl_setcc(s, s0, cc);
  rtl_jrelop(s, RELOP_NE, s0, rz, s->jmp_pc);

  print_asm("j%s %x", get_cc_name(cc), s->jmp_pc);
}

static inline def_EHelper(jmp_rm) {
  
  rtl_jr(s, ddest);
  print_asm("jmp *%s", id_dest->str);
}

static inline def_EHelper(call) {
  // the target address is calculated at the decode stage
  rtl_push(s,&(s->seq_pc));
  rtl_j(s,id_dest->imm);
  print_asm("call %x", s->jmp_pc);
}


static inline def_EHelper(loopne){
  *ddest -=1;
  if((cpu.eflags & EFLAG_ZF)==0 && (*ddest != 0)){
     rtl_j(s, s->jmp_pc);
  }
  print_asm("loopne %x", s->jmp_pc);
}


static inline def_EHelper(ret) {
  rtl_pop(s,t0);

  rtl_j(s,*t0);
  print_asm("ret");
}

static inline def_EHelper(ret_imm) {
  TODO();

  print_asm("ret %s", id_dest->str);
}

static inline def_EHelper(call_rm) {
  rtl_push(s,&(s->seq_pc));
  rtl_j(s,*ddest);

  print_asm("call *%s", id_dest->str);
}
