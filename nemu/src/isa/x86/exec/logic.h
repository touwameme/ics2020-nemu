#include "cc.h"
#include "string.h"
static inline def_EHelper(test) {  


  rtl_and(s,s0,ddest,dsrc1); 
  
  //printf("res is  %x  wid is %d \n",v,id_dest->width);
  
  set_CFOF_zero();
  rtl_update_ZFSF(s,s0,id_dest->width);
  print_asm_template2(test);
}

static inline def_EHelper(and) {
  
  rtl_and(s,s0,ddest,dsrc1);
  //printf( "%x  &  %x  =  %x\n",*ddest,*dsrc1,*s0 );
  operand_write(s, id_dest, s0);
  set_CFOF_zero();
  rtl_update_ZFSF(s,s0,id_dest->width);
  print_asm_template2(and);
}
  

static inline def_EHelper(xor) {
  rtl_xor(s,s0,ddest,dsrc1);
  operand_write(s, id_dest, s0);
  set_CFOF_zero();
  rtl_update_ZFSF(s,s0,id_dest->width);
  print_asm_template2(xor);
}

static inline def_EHelper(or) {
  rtl_or(s,s0,ddest,dsrc1);
  
  operand_write(s, id_dest, s0);
  set_CFOF_zero();
  rtl_update_ZFSF(s,s0,id_dest->width);

  print_asm_template2(or);
}

static inline def_EHelper(sar) {
  rtl_sar(s,s0,ddest,dsrc1);
  
  operand_write(s, id_dest, s0);
  rtl_update_ZFSF(s,s0,id_dest->width);
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(sar);
}

static inline def_EHelper(shl) {
  rtl_shl(s,s0,ddest,dsrc1);
  cpu.eflags &= (~EFLAG_CF);
  cpu.eflags &= (~EFLAG_OF);
  operand_write(s, id_dest, s0);
  rtl_update_ZFSF(s,s0,id_dest->width);
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(shl);
}

static inline def_EHelper(shld) {
  *s2 = *dsrc1 % (id_src1->width * 8);
  rtl_shl(s,s0,ddest,s2);
  uint32_t v = *s0;   
  switch (id_dest->width)
  {
  case 1:v &= 0xff;v += *ddest & 0xffffff00;break;
  case 2:v &= 0xffff;v += *ddest & 0xffff0000;break;
  case 4:break;
  default:assert(0);
  }
  if(*s2 ==0){*s1 = 0;}
  else{
  rtl_shri(s,s1,dsrc1,id_src1->width*8-*s2);
  }
  v += *s1;
  operand_write(s, id_dest, &v);
  rtl_update_ZFSF(s,s0,id_dest->width);
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(shld);
}

static inline def_EHelper(shrd){
 *s2 = *dsrc1 % (id_src1->width * 8);
  rtl_shr(s,s0,ddest,s2); 
  uint32_t v = *s0;   
  switch (id_dest->width)
  {
  case 1:v &= 0xff;v += *ddest & 0xffffff00;break;
  case 2:v &= 0xffff;v += *ddest & 0xffff0000;break;
  case 4:break;
  default:assert(0);
  }
  if(*s2 ==0){*s1 = 0;}
  else{
  rtl_shli(s,s1,dsrc1,id_src1->width*8-*s2);
  }
  switch (id_dest->width)
  {
  case 1:*s1 &= 0x00ffffff;break;
  case 2:*s1 &= 0x0000ffff;break;
  case 4:break;
  default:assert(0);
  }
  v += *s1;
  operand_write(s, id_dest, &v);
  rtl_update_ZFSF(s,s0,id_dest->width);
  print_asm_template2(shrd);

}


static inline def_EHelper(shr) {
  *s1 = *ddest;
  *s2 = *dsrc1;
  rtl_shr(s,s0,s1,s2);
  cpu.eflags &= (~EFLAG_CF);
  cpu.eflags &= (~EFLAG_OF);
  operand_write(s, id_dest, s0);
  rtl_update_ZFSF(s,s0,id_dest->width);
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(shr);
}




static inline def_EHelper(setcc) {
  uint32_t cc = s->opcode & 0xf;
  rtl_setcc(s, ddest, cc);
  operand_write(s, id_dest, ddest);

  print_asm("set%s %s", get_cc_name(cc), id_dest->str);
}

static inline def_EHelper(not) {
  rtl_not(s,s0,ddest);
  operand_write(s, id_dest, s0);
  rtl_update_ZFSF(s,s0,id_dest->width);
  print_asm_template1(not);
}


static inline def_EHelper(bsr){
  if(*dsrc1 == 0){cpu.eflags |= EFLAG_ZF;}    
  else{
  *s1= id_dest->width*8-1;
  cpu.eflags &= (~EFLAG_ZF); 
  
  while((*dsrc1 >> *s1) ==0){
    (*s1)--;
  }
  operand_write(s, id_dest, s1);
  }
  print_asm_template2(bsr);
}
