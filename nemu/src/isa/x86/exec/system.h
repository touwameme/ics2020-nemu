#include <monitor/difftest.h>

static inline def_EHelper(lidt) {
  uint32_t addr = id_dest->val;
  //printf("addr %x\n",addr);
  if(id_dest->width ==2){
    cpu.idt.limit = vaddr_read(addr,2);
    cpu.idt.base = vaddr_read(addr+2,3) & 0x00ffffff;
  }
  else if(id_dest->width == 4){
    cpu.idt.limit = vaddr_read(addr,2);
    cpu.idt.base = vaddr_read(addr+2,4) ;
  }
  else{assert(0);}
  print_asm_template1(lidt);
}





static inline def_EHelper(mov_r2cr) {
  //printf("cr0 %x  %x\n",*ddest, *dsrc1);
 *ddest = *dsrc1;
  
//printf("cr0 %x cr3 %x\n",cpu.cr0,cpu.cr3);
  //print_asm("movl %%%s,%%cr%d", reg_name(id_src1->reg, 4), id_dest->reg);
}

static inline def_EHelper(mov_cr2r) {
  
   *dsrc1 = *ddest;
  //printf("cr0 %x cr3 %x\n",cpu.cr0,cpu.cr3);
  //print_asm("movl %%cr%d,%%%s", id_src1->reg, reg_name(id_dest->reg, 4));

#ifndef __DIFF_REF_NEMU__
  difftest_skip_ref();
#endif
}
void raise_intr(DecodeExecState *s, uint32_t NO, vaddr_t ret_addr);
static inline def_EHelper(int) {
    //uint32_t idt_addr = cpu.idt.base;

    raise_intr(s,*ddest,s->seq_pc);
    

  print_asm("int %s", id_dest->str);

#ifndef __DIFF_REF_NEMU__
  difftest_skip_dut(1, 2);
#endif
}

static inline def_EHelper(iret) {
  
  rtl_pop(s,t0);
  rtl_pop(s,&cpu.cs);
  rtl_pop(s,&cpu.eflags);
  cpu.eflags |= EFLAG_IF;
  rtl_j(s,*t0);

  print_asm("iret");

#ifndef __DIFF_REF_NEMU__
  difftest_skip_ref();
#endif
}

uint32_t pio_read_l(ioaddr_t);
uint32_t pio_read_w(ioaddr_t);
uint32_t pio_read_b(ioaddr_t);
void pio_write_l(ioaddr_t, uint32_t);
void pio_write_w(ioaddr_t, uint32_t);
void pio_write_b(ioaddr_t, uint32_t);

static inline def_EHelper(in) {
  *s0 = 0;
  switch (id_dest->width)
  {
  case 1:*s0=pio_read_b(*dsrc1);break;
  case 2:*s0=pio_read_w(*dsrc1);break;
  case 4:*s0=pio_read_l(*dsrc1);break;
  default:assert(0);

  }
 //printf("s0 %x\n",*s0);
  operand_write(s,id_dest,s0);
  print_asm_template2(in);
}

static inline def_EHelper(out) {
  switch (id_dest->width)
  {
  case 1:pio_write_b(*ddest,*dsrc1);break; 
  case 2:pio_write_w(*ddest,*dsrc1);break;
  case 4:pio_write_l(*ddest,*dsrc1);break;
  default:assert(0);
    
  }

  print_asm_template2(out);
}
