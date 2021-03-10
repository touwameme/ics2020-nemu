static inline def_EHelper(mov) {
 // printf("type is %d\n",id_dest->type);
  operand_write(s, id_dest, dsrc1);
  print_asm_template2(mov);
  //printf("done\n");
}


static inline def_EHelper(movs) {
  
  
  *s0 = vaddr_read(cpu.esi,id_src1->width);
  vaddr_write(cpu.edi,*s0,id_dest->width) ;

  int incdec = id_dest->width;
  cpu.esi += incdec;
  cpu.edi += incdec;
  //printf("src_Addr is %x,movs src is %d  mov to %x\n",cpu.esi,*s0,cpu.edi);
  print_asm_template2(movs);
}

static inline def_EHelper(mmx) {
  
}


static inline def_EHelper(fetch3){
  s->opcode = instr_fetch(&s->seq_pc, 1);
  s->opcode = instr_fetch(&s->seq_pc, 2);
}
static inline def_EHelper(fetch1){
  s->opcode = instr_fetch(&s->seq_pc, 1);
}


static inline def_EHelper(push) {
  
  rtl_push(s,ddest);
  print_asm_template1(push);
}

static inline def_EHelper(pop) {
  rtl_pop(s,ddest);

  print_asm_template1(pop);
}

static inline def_EHelper(pusha) {
 
  uint32_t p = cpu.esp;
  rtl_push(s,&cpu.eax);
   rtl_push(s,&cpu.ecx);
    rtl_push(s,&cpu.edx);
     rtl_push(s,&cpu.ebx);
      rtl_push(s,&p);
       rtl_push(s,&cpu.ebp);
        rtl_push(s,&cpu.esi);
        rtl_push(s,&cpu.edi); 


  print_asm("pusha");
}

static inline def_EHelper(popa) {
    cpu.edi = vaddr_read(reg_l(R_ESP),4);
    cpu.esp+=4;
    cpu.esi = vaddr_read(reg_l(R_ESP),4);
    cpu.esp+=4;
    cpu.ebp = vaddr_read(reg_l(R_ESP),4);
    cpu.esp+=4;

    cpu.esp+=4;
    cpu.ebx = vaddr_read(reg_l(R_ESP),4);
    cpu.esp+=4;
    cpu.edx = vaddr_read(reg_l(R_ESP),4);
    cpu.esp+=4;
    cpu.ecx = vaddr_read(reg_l(R_ESP),4);
    cpu.esp+=4;
    cpu.eax = vaddr_read(reg_l(R_ESP),4);
    cpu.esp+=4;
  print_asm("popa");
}

static inline def_EHelper(leave) {
  cpu.esp = cpu.ebp;
  rtl_pop(s,&cpu.ebp);
  print_asm("leave");
}
static inline def_EHelper(xchg) {
  
  *t0 = *ddest;
  operand_write(s, id_dest, dsrc1);
  operand_write(s, id_src1, t0);
  print_asm_template2(xchg);
  
}

static inline def_EHelper(cltd) {
  if (s->isa.is_operand_size_16) {
    if(cpu.eax & 0x8000){
      cpu.edx |= 0x0000ffff;
    }
    else{
      cpu.edx &= 0xffff0000;
    }
  }
  else {
    if(cpu.eax & 0x80000000){
      cpu.edx =  0xffffffff;
    }
    else{
      cpu.edx = 0x00000000;
    }
  }

  print_asm(s->isa.is_operand_size_16 ? "cwtl" : "cltd");
}
/*
static inline def_EHelper(cwtl) {
  operand_reg(s, id_dest, false, R_EAX, 4);
  if (s->isa.is_operand_size_16) {
    rtl_sext(s,s0,ddest,1);
    *ddest = *s0&0xffff;
  }
  else {
    rtl_sext(s,s0,ddest,2);
    *ddest = *s0;
  }

  print_asm(s->isa.is_operand_size_16 ? "cbtw" : "cwtl");
}
*/

static inline def_EHelper(cwtl) {
  if (s->isa.is_operand_size_16) {
    *s0 = cpu.eax & 0xff;
    if((*s0 & 0x80)!=0 ){cpu.eax = (cpu.eax&0xffff0000) + (*s0|0xff00);}
    else{cpu.eax = (cpu.eax&0xffff0000) + *s0;}
  }
  else {
     *s0 = cpu.eax & 0xffff;
     if((*s0 & 0x8000)!=0 ){cpu.eax =  (*s0|0xffff0000);}
    else{cpu.eax =  *s0;}
  }

  print_asm(s->isa.is_operand_size_16 ? "cbtw" : "cwtl");
}


static inline def_EHelper(movsx) {
 
  id_dest->width = s->isa.is_operand_size_16 ? 2 : 4;
  rtl_sext(s, ddest, dsrc1, id_src1->width);
  operand_write(s, id_dest, ddest);
  print_asm_template2(movsx);
}

static inline def_EHelper(movzx) {
  id_dest->width = s->isa.is_operand_size_16 ? 2 : 4;
 rtl_zext(s,ddest,dsrc1,id_src1->width);
  operand_write(s, id_dest, dsrc1);
  print_asm_template2(movzx);
}

static inline def_EHelper(lea) {
  rtl_addi(s, ddest, s->isa.mbase, s->isa.moff);
  operand_write(s, id_dest, ddest);
  print_asm_template2(lea);
  
}

static inline def_EHelper(LES) {
  rtl_addi(s, dsrc1, s->isa.mbase, s->isa.moff);
  printf("addr is %x\n",*dsrc1);
  *s0 = vaddr_read(*dsrc1,id_dest->width);
  if(id_dest->width == 4){
    *ddest = *s0;
  }
  else if (id_dest->width == 2){
    *ddest = (*ddest & 0xffff0000)+ *s0;
  }
  else 
    assert(0);
  print_asm_template2(LES);
}

static inline def_EHelper(rol){
 
  *s0 = *ddest;
  *s1 = *dsrc1 & 0x1f;
  for(;*s1>0;(*s1)--){
    *s2 = *s0<<1;
    *s0 = (*s0>>(id_dest->width*8-1)&1)+*s2;
  }
  operand_write(s,id_dest,s0);

  print_asm_template2(rol);
}

static inline def_EHelper(ror){
  *s0 = *ddest;
  *s1 = *dsrc1 & 0x1f;
  for(;*s1>0;(*s1)--){
    *s2 = *s0 & 1;
    *s0 = (*s0>>1) + (*s2 << (id_dest->width*8-1));
  }

  operand_write(s,id_dest,s0);
  print_asm_template2(ror);
}
