//touwa

#include <cpu/exec.h>
#include "../local-include/decode.h"
#include "all-instr.h"

static inline void set_width(DecodeExecState *s, int width) {
  if (width == -1) return;
  if (width == 0) {//opcodez规定的operand大小
    width = s->isa.is_operand_size_16 ? 2 : 4;
  }
  s->src1.width = s->dest.width = s->src2.width = width;
}

/* 0x80, 0x81, 0x83 :: OR */
static inline def_EHelper(gp1) {
  switch (s->isa.ext_opcode) {
    EXW(0,add,id_dest->width) EXW(1,or,id_dest->width)  EXW(2,adc,id_dest->width) EXW(3,sbb,id_dest->width)   
    EXW(4,and,id_dest->width) EXW(5,sub,id_dest->width) EXW(6,xor,id_dest->width) EXW(7,cmp,id_dest->width)
  }
}

/* 0xc0, 0xc1, 0xd0, 0xd1, 0xd2, 0xd3 :: SHIFT INSTRUCTION*/ 
static inline def_EHelper(gp2) {
  switch (s->isa.ext_opcode) {
    EX(0,rol) EX(1,ror) EMPTY(2) EMPTY(3)
    EXW(4,shl,id_dest->width) EXW(5,shr,id_dest->width) EMPTY(6) EXW(7,sar,id_dest->width)
  }
}

/* 0xf6, 0xf7 */
static inline def_EHelper(gp3) {
  switch (s->isa.ext_opcode) {
    IDEXW(0,src_I,test,id_dest->width) EMPTY(1) EXW(2,not,id_dest->width) EXW(3,neg,id_dest->width)  
    EXW(4,mul,id_dest->width)  EXW(5,imul1,id_dest->width) EXW(6,div,id_dest->width) EXW(7,idiv,id_dest->width)
  }
}

/* 0xfe */
static inline def_EHelper(gp4) {
  switch (s->isa.ext_opcode) {
    EXW(0,inc,id_dest->width) EXW(1,dec,id_dest->width) EMPTY(2) EMPTY(3)
    EMPTY(4) EMPTY(5) EMPTY(6) EMPTY(7)
  }
}

/* 0xff */
static inline def_EHelper(gp5) {
  switch (s->isa.ext_opcode) {
    EXW(0,inc,id_dest->width) EXW(1,dec,id_dest->width) IDEXW(2,Call_Ec,call_rm,id_dest->width) EMPTY(3)
    EX(4,jmp_rm) EMPTY(5) EX(6,push) EMPTY(7)
  }
}
//IDEXW(2,Call_Ec,call,id_dest->width)
/* 0x0f 0x01*/
static inline def_EHelper(gp7) {
  switch (s->isa.ext_opcode) {
    EMPTY(0) EMPTY(1) EMPTY(2) EX(3,lidt)//```
    EMPTY(4) EMPTY(5) EMPTY(6) EMPTY(7)
  }
}

static inline def_EHelper(2byte_esc) {
  
  uint8_t opcode = instr_fetch(&s->seq_pc, 1);
  s->opcode = opcode;
  switch (opcode) {
  /* TODO: Add more instructions!!! */
    IDEX (0x01,gp7_idt ,gp7)
    IDEX (0x10,r,mmx)
    IDEX (0x80, I, jcc)
    IDEX (0x81, I, jcc)
    IDEX (0x82, I, jcc)
    IDEX (0x83, I, jcc)
    IDEX (0x84, I, jcc)
    IDEX (0x85, I, jcc)
    IDEX (0x86, I, jcc)
    IDEX (0x87, I, jcc)
    IDEX (0x88, I, jcc)
    IDEX (0x89, I, jcc)
    IDEX (0x8a, I, jcc)
    IDEX (0x8b, I, jcc)
    IDEX (0x8c, I, jcc)
    IDEX (0x8d, I, jcc)
    IDEX (0x8e, I, jcc)
    IDEX (0x8f, I, jcc)
    IDEXW (0x90, gp7_E, setcc,1)
    IDEXW (0x91, gp7_E, setcc,1)
    IDEXW (0x92, gp7_E, setcc,1)
    IDEXW (0x93, gp7_E, setcc,1)
    IDEXW (0x94, gp7_E, setcc,1)
    IDEXW (0x95, gp7_E, setcc,1)
    IDEXW (0x96, gp7_E, setcc,1)
    IDEXW (0x97, gp7_E, setcc,1)
    IDEXW (0x98, gp7_E, setcc,1)
    IDEXW (0x99, gp7_E, setcc,1)
    IDEXW (0x9a, gp7_E, setcc,1)
    IDEXW (0x9b, gp7_E, setcc,1)
    IDEXW (0x9c, gp7_E, setcc,1)
    IDEXW (0x9d, gp7_E, setcc,1)
    IDEXW (0x9e, gp7_E, setcc,1)
    IDEXW (0x9f, gp7_E, setcc,1)
    IDEX (0xa4,Ib_G2E,shl) //```
    IDEX (0xa5,cl_G2E,shl)
    IDEX (0xac,Ib_G2E,shr) //```
    IDEX (0xad,cl_G2E,shr)
    IDEX (0xaf,E2G,imul2)
    IDEXW (0xb6, mov_E2G, movzx,1)
    IDEXW (0xb7,mov_E2G,movzx,2)
    IDEX (0xbd,E2G,bsr)
    IDEXW (0xbe,mov_E2G,movsx,1)
    IDEXW (0xbf,mov_E2G,movsx,2)

    default: exec_inv(s);
  }
}

static inline void fetch_decode_exec(DecodeExecState *s) {
  uint8_t opcode;
again:
  opcode = instr_fetch(&s->seq_pc, 1);
  s->opcode = opcode;
  switch (opcode) {
    IDEXW(0x00,G2E,add,1)
    IDEX (0x01, G2E,  add)
    IDEXW(0x02, E2G,  add,1)
    IDEX (0x03, E2G,  add)
    IDEXW(0x04, I2a,  add,1)
    IDEX (0x05, I2a,  add)
    IDEXW(0x08,G2E,or,1)
    IDEX (0x09,G2E,or)
    IDEXW(0x0a,E2G,or,1)
    IDEX (0x0b,E2G,or)
    IDEXW(0x0c,eax_I,or,1)
    IDEX (0x0d,eax_I,or)
    EX   (0x0f, 2byte_esc)
    IDEXW  (0x10,G2E,adc,1)     //*```
    IDEX  (0x11,G2E,adc) 
    IDEXW  (0x12,E2G,adc,1)
    IDEX  (0x13,E2G,adc)  
    IDEXW(0x14,eax_I,adc,1)
    IDEX  (0x15,eax_I,adc) 
    IDEXW (0x18,G2E,sbb,1)      //*```
    IDEX (0x19,G2E,sbb)
    IDEXW(0x1a,E2G,sbb,1)
    IDEX (0x1b,E2G,sbb)
    IDEXW(0x1c, eax_I,sbb,1)
    IDEX (0x1d, eax_I,sbb)
    IDEXW(0x20, G2E,and,1 )
    IDEX (0x21, G2E,and )
    IDEXW (0x22,E2G,and,1 )
    IDEX (0x23, E2G,and )
    IDEXW(0x24, I2a,and,1)
    IDEX (0x25, I2a,and )
    IDEXW(0x28, G2E, sub,1 )
    IDEX (0x29, G2E, sub )
    IDEXW (0x2a, E2G, sub,1 )
    IDEX (0x2b, E2G, sub )
    IDEXW (0x2c, I2a, sub,1 )
    IDEX (0x2d, I2a, sub )
    EX    (0x2f,das)
    IDEXW(0x30, G2E,  xor,1)
    IDEX (0x31, G2E,  xor)
    IDEXW(0x32, E2G,  xor,1)
    IDEX (0x33, E2G,  xor)
    IDEXW(0x34, I2a,  xor,1)
    IDEX (0x35, I2a,  xor)
    IDEXW(0x38, G2E, cmp,1)
    IDEX (0x39, G2E,  cmp)
    IDEXW(0x3a, E2G, cmp,1)
    IDEX (0x3b, E2G,  cmp)
    IDEXW(0x3c, I2a, cmp,1)
    IDEX (0x3d, I2a,  cmp)
    EX   (0x3e,fetch1)
    IDEX (0x40, r,  inc)
    IDEX (0x41, r,  inc)
    IDEX (0x42, r,  inc)
    IDEX (0x43, r,  inc)
    IDEX (0x44, r,  inc)
    IDEX (0x45, r,  inc)
    IDEX (0x46, r,  inc)
    IDEX (0x47, r,  inc)
    IDEX (0x48, r,  dec)
    IDEX (0x49, r,  dec)
    IDEX (0x4a, r,  dec)
    IDEX (0x4b, r,  dec)
    IDEX (0x4c, r,  dec)
    IDEX (0x4d, r,  dec)
    IDEX (0x4e, r,  dec)
    IDEX (0x4f, r,  dec)
    IDEX (0x50, r,  push)
    IDEX (0x51, r,  push)
    IDEX (0x52, r,  push)
    IDEX (0x53, r,  push)
    IDEX (0x54, r,  push)
    IDEX (0x55, r,  push)
    IDEX (0x56, r,  push)
    IDEX (0x57, r,  push)
    IDEX (0x58, r,  pop)
    IDEX (0x59, r,  pop)
    IDEX (0x5a, r,  pop)
    IDEX (0x5b, r,  pop)
    IDEX (0x5c, r,  pop)
    IDEX (0x5d, r,  pop)
    IDEX (0x5e, r,  pop)
    IDEX (0x5f, r,  pop)
    EX   (0x60, pusha)
    EX   (0x61,popa)
    IDEX (0x68, I,  push)
    IDEX (0x69 , I_E2G,imul3)
    IDEXW(0x6a, push_I8,  push,1)
    IDEX(0x6b,Ib_E2G,imul3)
    IDEXW(0x70, I, jcc,1)
    IDEXW(0x71, I, jcc,1)
    IDEXW(0x72, I, jcc,1)
    IDEXW(0x73, I, jcc,1)
    IDEXW(0x74, I, jcc,1)
    IDEXW(0x75, I, jcc,1)
    IDEXW(0x76, I, jcc,1)
    IDEXW(0x77, I, jcc,1)
    IDEXW(0x78, I, jcc,1)
    IDEXW(0x79, I, jcc,1)
    IDEXW(0x7a, I, jcc,1)
    IDEXW(0x7b, I, jcc,1)
    IDEXW(0x7c, I, jcc,1)
    IDEXW(0x7d, I, jcc,1)
    IDEXW(0x7e, I, jcc,1)
    IDEXW(0x7f, I, jcc,1)
    IDEXW(0x80, I2E, gp1, 1)
    IDEX (0x81, I2E, gp1)
    IDEX (0x83, SI2E, gp1)
    IDEXW(0x84, G2E,  test, 1)
    IDEX (0x85, G2E,  test)
    IDEXW(0x86, E2G,xchg,1)
    IDEX (0x87, E2G,xchg)
    IDEXW(0x88, mov_G2E, mov, 1) 
    IDEX (0x89, mov_G2E, mov)
    IDEXW(0x8a, mov_E2G, mov, 1)
    IDEX (0x8b, mov_E2G, mov)
    IDEX (0x8d,lea_E2G,lea)
    IDEX (0x90,eax_op_r,xchg)  
    IDEX (0x91,eax_op_r,xchg)  
    IDEX (0x92,eax_op_r,xchg)  
    IDEX (0x93,eax_op_r,xchg)  
    IDEX (0x94,eax_op_r,xchg)  
    IDEX (0x95,eax_op_r,xchg)  
    IDEX (0x96,eax_op_r,xchg)  
    IDEX (0x97,eax_op_r,xchg)
    EX    (0x98,cwtl)
    EX   (0x99,cltd)  
    IDEXW(0xa0, O2a, mov, 1)
    IDEX (0xa1, O2a, mov)
    IDEXW(0xa2, a2O, mov, 1)
    IDEX (0xa3, a2O, mov)
    EXW  (0xa4,movs,1)
    EX   (0xa5,movs)
    IDEXW(0xa6,cmps_16,cmp,1)
    IDEXW(0xa8,I2a,test,1)
    IDEX (0xa9,I2a,test)
    IDEXW(0xb0, mov_I2r, mov, 1)
    IDEXW(0xb1, mov_I2r, mov, 1)
    IDEXW(0xb2, mov_I2r, mov, 1)
    IDEXW(0xb3, mov_I2r, mov, 1)
    IDEXW(0xb4, mov_I2r, mov, 1)
    IDEXW(0xb5, mov_I2r, mov, 1)
    IDEXW(0xb6, mov_I2r, mov, 1)
    IDEXW(0xb7, mov_I2r, mov, 1)
    IDEX (0xb8, mov_I2r, mov)
    IDEX (0xb9, mov_I2r, mov)
    IDEX (0xba, mov_I2r, mov)
    IDEX (0xbb, mov_I2r, mov)
    IDEX (0xbc, mov_I2r, mov)
    IDEX (0xbd, mov_I2r, mov)
    IDEX (0xbe, mov_I2r, mov)
    IDEX (0xbf, mov_I2r, mov)
    IDEXW(0xc0, Ib2E, gp2, 1)
    IDEX (0xc1, Ib2E, gp2)
    IDEX (0xc3, ret_r ,ret )
    IDEX (0xc4,lea_E2G,LES)  //```
    //IDEX (0xc5,r,LES)
    IDEXW(0xc6, mov_I2E, mov, 1)
    IDEX (0xc7, mov_I2E, mov)
    EX   (0xc9, leave) 
    IDEXW (0xcd,I,int,1)
    EX   (0xcf,iret)
    IDEXW(0xd0, gp2_1_E, gp2, 1)
    IDEX (0xd1, gp2_1_E, gp2)
    IDEXW(0xd2, gp2_cl2E, gp2, 1)
    IDEX (0xd3, gp2_cl2E, gp2)
    EX   (0xd6, nemu_trap)
    IDEX (0xe0,LOOP,loopne)
    IDEXW(0xe4,in_Ib2a,in,1)
    IDEX (0xe5,in_Ib2a,in)
    IDEXW(0xec,in_dx2a,in,1)
    IDEX (0xed,in_dx2a,in)
    IDEXW(0xe6,out_a2I,out,1)   
    IDEX (0xe7,out_a2I,out)
    IDEX (0xe8, Call_I ,call)
    IDEX (0xe9, jmp_I,jmp)
    IDEXW(0xeb, jmp_I, jmp,1)
    IDEXW(0xee,out_a2dx,out,1)
    IDEX (0xef,out_a2dx,out)
    EX   (0xf3, fetch3);
    IDEXW(0xf6, E, gp3, 1)
    IDEX (0xf7, E, gp3)
    IDEXW(0xfe, E, gp4, 1)
    IDEX (0xff, E, gp5)
  case 0x66: s->isa.is_operand_size_16 = true; goto again;
  default: exec_inv(s);
  }
}

vaddr_t isa_exec_once() {
  DecodeExecState s;
  s.is_jmp = 0;
  s.isa = (x86_ISADecodeInfo) { 0 };
  s.seq_pc = cpu.pc;

  fetch_decode_exec(&s);
  update_pc(&s);

  return s.seq_pc;
}
