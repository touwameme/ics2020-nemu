#include <cpu/exec.h>
#include "rtl.h"

void read_ModR_M(DecodeExecState *s, Operand *rm, bool load_rm_val, Operand *reg, bool load_reg_val);

static inline void operand_reg(DecodeExecState *s, Operand *op, bool load_val, int r, int width) {
  op->type = OP_TYPE_REG;
  op->reg = r;

  if (width == 4) {
    op->preg = &reg_l(r);
  } else {
    assert(width == 1 || width == 2);
    op->preg = &op->val;
    if (load_val) rtl_lr(s, &op->val, r, width);
  }

  print_Dop(op->str, OP_STR_SIZE, "%%%s", reg_name(r, width));
}

static inline void operand_imm(DecodeExecState *s, Operand *op, bool load_val, word_t imm, int width) {
  op->type = OP_TYPE_IMM;
  op->imm = imm;
  if (load_val) {
    rtl_li(s, &op->val, imm);
    op->preg = &op->val;
  }
  print_Dop(op->str, OP_STR_SIZE, "$0x%x", imm);
}

// decode operand helper
#define def_DopHelper(name) void concat(decode_op_, name) (DecodeExecState *s, Operand *op, bool load_val)

/* Refer to Appendix A in i386 manual for the explanations of these abbreviations */

/* Ib, Iv */
static inline def_DopHelper(I) {
  /* pc here is pointing to the immediate */
  word_t imm = instr_fetch(&s->seq_pc, op->width);
  operand_imm(s, op, load_val, imm, op->width);
}




/* I386 manual does not contain this abbreviation, but it is different from
 * the one above from the view of implementation. So we use another helper
 * function to decode it.
 */
/* sign immediate */
static inline def_DopHelper(SI) {
  assert(op->width == 1 || op->width == 4);

  /* TODO: Use instr_fetch() to read `op->width' bytes of memory
   * pointed by 's->seq_pc'. Interpret the result as a signed immediate,
   * and call `operand_imm()` as following.
   *
   operand_imm(s, op, load_val, ???, op->width);
   */
   word_t imm  = instr_fetch(&s->seq_pc,op->width);
   if(op->width == 1 &&((imm>>7)&1)) 
      imm  |= 0xffffff00; 
   operand_imm(s, op, load_val, imm, op->width);
}
static inline def_DopHelper(I8) {
  assert(op->width == 2 || op->width == 4);
   word_t imm  = instr_fetch(&s->seq_pc,op->width);
   operand_imm(s, op, load_val, imm, op->width);
}

/* I386 manual does not contain this abbreviation.
 * It is convenient to merge them into a single helper function.
 */
/* AL/eAX */
static inline def_DopHelper(a) {
  operand_reg(s, op, load_val, R_EAX, op->width);
}


/* This helper function is use to decode register encoded in the opcode. */
/* XX: AL, AH, BL, BH, CL, CH, DL, DH
 * eXX: eAX, eCX, eDX, eBX, eSP, eBP, eSI, eDI
 */
static inline def_DopHelper(r) {
  operand_reg(s, op, load_val, s->opcode & 0x7, op->width);
}

/* I386 manual does not contain this abbreviation.
 * We decode everything of modR/M byte in one time.
 */
/* Eb, Ew, Ev
 * Gb, Gv
 * Cd,
 * M
 * Rd
 * Sw
 */

static inline void operand_rm(DecodeExecState *s, Operand *rm, bool load_rm_val, Operand *reg, bool load_reg_val) {
  read_ModR_M(s, rm, load_rm_val, reg, load_reg_val);
}





/* Ob, Ov */
static inline def_DopHelper(O) {
  op->type = OP_TYPE_MEM;
  s->isa.moff = instr_fetch(&s->seq_pc, 4);
  s->isa.mbase = rz;
  if (load_val) {
    rtl_lm(s, &op->val, s->isa.mbase, s->isa.moff, op->width);
    op->preg = &op->val;
  }

  print_Dop(op->str, OP_STR_SIZE, "0x%x", s->isa.moff);
}

/* Eb <- Gb
 * Ev <- Gv
 */
static inline def_DHelper(G2E) {
  operand_rm(s, id_dest, true, id_src1, true);
}

static inline def_DHelper(mov_G2E) {
  operand_rm(s, id_dest, false, id_src1, true);
}

/* Gb <- Eb
 * Gv <- Ev
 */
static inline def_DHelper(E2G) {
  operand_rm(s, id_src1, true, id_dest, true);
}
static inline def_DHelper(Eb2G) {
  operand_rm(s, id_src1, true, id_dest, true);
  id_src1->width = 1;
}



static inline def_DHelper(Ew2G) {
  operand_rm(s, id_src1, true, id_dest, true);
  id_src1->width = 2;
}


static inline def_DHelper(mov_E2G) {
  operand_rm(s, id_src1, true, id_dest, false);
}



static inline def_DHelper(mov_cr){
  uint32_t v = instr_fetch(&s->seq_pc, 1);
  //printf("rmr %x\n",v);
  dsrc1 = &reg_l(v&0x7);

  //printf("index %x\n",v&0x7);
  switch ((v>>3)&0x7)
  {
  case 0: ddest = &cpu.cr0;break;
      
  case 3: ddest = &cpu.cr3;break;
  default:assert(0);
    break;
  } 

}

static inline def_DHelper(lea_M2G) {
  operand_rm(s, id_src1, false, id_dest, false);
}
static inline def_DHelper(lea_E2G) {
  operand_rm(s, id_src1, false, id_dest, true);
}

/* AL <- Ib
 * eAX <- Iv
 */
static inline def_DHelper(I2a) {
  decode_op_a(s, id_dest, true);
  decode_op_I(s, id_src1, true);
}

static inline def_DHelper(in_Ib2a) {
  decode_op_a(s, id_dest, true);
  id_src1->width = 1;
  decode_op_I(s, id_src1, true);
}



/* Gv <- EvIb
 * Gv <- EvIv
 * use for imul */
static inline def_DHelper(I_E2G) {
  operand_rm(s, id_src2, true, id_dest, false);
  decode_op_I(s, id_src1, true);
}


static inline def_DHelper(Jmp){
    decode_op_SI(s,id_dest,false);
    s->jmp_pc =s->seq_pc +  id_dest->simm ;
}

/* Eb <- Ib
 * Ev <- Iv
 */
static inline def_DHelper(I2E) {
  operand_rm(s, id_dest, true, NULL, false);
  decode_op_I(s, id_src1, true);
}

static inline def_DHelper(cmps_16){

  *ddest = vaddr_read(cpu.esi&0x0000ffff,id_dest->width);
  *dsrc1 = vaddr_read(cpu.edi&0x0000ffff,id_src1->width);
}

static inline def_DHelper(mov_I2E) {
  operand_rm(s, id_dest, false, NULL, false);
  decode_op_I(s, id_src1, true);
}

static inline def_DHelper(eax_op_r) {
  decode_op_r(s, id_src1, true);
 decode_op_a(s, id_dest, true);
}
static inline def_DHelper(eax_I) {
  decode_op_I(s, id_src1, true);
  decode_op_a(s, id_dest, true);
}


static inline def_DHelper(eax_E) {
  operand_rm(s, id_dest, true, NULL, false);
 decode_op_a(s, id_dest, true);
}
/* XX <- Ib
 * eXX <- Iv
 */
static inline def_DHelper(I2r) {
  decode_op_r(s, id_dest, true);
  decode_op_I(s, id_src1, true);
}
static inline def_DHelper(r2I8) {
  decode_op_r(s, id_src1, true);
  id_dest->width=1;
  decode_op_I(s, id_dest, true);
}




static inline def_DHelper(mov_I2r) {
  decode_op_r(s, id_dest, false);
  id_dest->type = OP_TYPE_REG;
  decode_op_I(s, id_src1, true);
  id_src1->type=OP_TYPE_IMM;
}

/* used by unary operations */
static inline def_DHelper(I) {
  decode_op_I(s, id_dest, true);
}

static inline def_DHelper(push_I8) {
  decode_op_I(s, id_dest, true);
  rtl_sext(s,ddest,ddest,id_dest->width);
}

static inline def_DHelper(src_I) {
  decode_op_I(s, id_src1, true);
}
static inline def_DHelper(jmp_I) {
  decode_op_I(s, id_dest, true);
  switch(id_dest->width){
    case 1:s->jmp_pc = s->seq_pc +(int8_t)(*ddest);break;
    case 2:s->jmp_pc = s->seq_pc +(int16_t)(*ddest);break;
    case 4:s->jmp_pc = s->seq_pc +(int32_t)(*ddest);break;   //(s->seq_pc & 0x00000000)+(s->seq_pc + *ddest)&0xffffffff;break;
    default: assert(0);
  }

  *ddest = s->seq_pc + (signed char)(id_src1->val);
}



static inline def_DHelper(LOOP){
  id_src1->width = 1;
  decode_op_I(s, id_src1, true);
  s->jmp_pc = s->seq_pc +(int8_t)(*dsrc1);
  operand_reg(s, id_dest, true, R_ECX, id_dest->width);

}

static inline def_DHelper(r) {
  decode_op_r(s, id_dest, true);
}
static inline def_DHelper(src_r) {
  decode_op_r(s, id_src1, true);
}

static inline def_DHelper(E) {
  operand_rm(s, id_dest, true, NULL, false);
}


static inline def_DHelper(Call_I){
  decode_op_I(s,id_src1,true);
  rtl_addi(s,&(id_dest->imm),&(id_src1->imm),s->seq_pc);
};
static inline def_DHelper(Call_Ec){
  switch (id_dest->width)
  {
  case 2: *ddest = (*ddest)&0xffff;break;
  case 4: break;
  default:assert(0);
  }
};

static inline def_DHelper(Call_E){
  operand_rm(s, id_src1, true, NULL, false);
  switch (id_src1->width)
  {
  case 2: s->seq_pc = (*dsrc1)&0xffff;break;
  case 4: s->seq_pc = (*dsrc1);break;
  default:assert(0);
  }
};
static inline def_DHelper(setcc_E) {
  operand_rm(s, id_dest, false, NULL, false);
}




static inline def_DHelper(gp7_idt){
  operand_rm(s, id_dest, false, NULL, false);
  *ddest = *s->isa.mbase + s->isa.moff;
}

static inline def_DHelper(gp7_E) {
  operand_rm(s, id_dest, false, NULL, false);
}
static inline def_DHelper(gp7_E2G) {
  operand_rm(s, id_src1, false, id_dest, false);
}
static inline def_DHelper(gp7_Eb2G) {
  id_src1->width = 1; 
  operand_rm(s, id_src1, true, id_dest, false);
  if(id_src1->type == OP_TYPE_MEM) *dsrc1 = id_src1->val;
 // printf("d is %x  s is %x \n",*ddest,*dsrc1);
}

static inline def_DHelper(idt){
  id_dest->width = 2;
  decode_op_I(s, id_dest, true);
}

static inline def_DHelper(gp7_G2E) {
  operand_rm(s, id_dest, false, id_src1, false);
}


/* used by test in group3 */
static inline def_DHelper(test_I) {
  decode_op_I(s, id_src1, true);
}

static inline def_DHelper(SI2E) {
  assert(id_dest->width == 2 || id_dest->width == 4);
  operand_rm(s, id_dest, true, NULL, false);
  id_src1->width = 1;
  decode_op_SI(s, id_src1, true);
  if (id_dest->width == 2) {
    *dsrc1 &= 0xffff;
  }
}

static inline def_DHelper(SI_E2G) {
  assert(id_dest->width == 2 || id_dest->width == 4);
  operand_rm(s, id_src2, true, id_dest, false);
  id_src1->width = 1;
  decode_op_SI(s, id_src1, true);
  if (id_dest->width == 2) {
    *dsrc1 &= 0xffff;
  }
}

static inline def_DHelper(gp2_1_E) {
  operand_rm(s, id_dest, true, NULL, false);
  operand_imm(s, id_src1, true, 1, 1);
}


static inline def_DHelper(gp2_cl2E) {
  operand_rm(s, id_dest, true, NULL, false);
  // shift instructions will eventually use the lower
  // 5 bits of %cl, therefore it is OK to load %ecx
  operand_reg(s, id_src1, true, R_ECX, 4);
}



static inline def_DHelper(Ib2E) {
  operand_rm(s, id_dest, true, NULL, false);
  id_src1->width = 1;
  decode_op_I(s, id_src1, true);
}
static inline def_DHelper(SextIb2E) {
  operand_rm(s, id_dest, true, NULL, false);
  id_src1->width = 1;
  decode_op_I(s, id_src1, true);
  rtl_sext(s,dsrc1,dsrc1,id_src1->width);
  switch (id_dest->width)
  {
  case 2:*dsrc1 &= 0xffff;id_src1->width = 2; break;
  case 4:id_src1->width = 4; break;
  default:assert(0);
  }
}




/* Ev <- GvIb
 * use for shld/shrd */
static inline def_DHelper(Ib_G2E) {
  operand_rm(s, id_dest, true, id_src2, true);
  id_src1->width = 1;
  decode_op_I(s, id_src1, true);
}

static inline def_DHelper(Ib_E2G) {
  operand_rm(s, id_src2, true, id_dest, true);
  id_src1->width = 1;
  decode_op_I(s, id_src1, true);
}



/* Ev <- GvCL
 * use for shld/shrd */
static inline def_DHelper(cl_G2E) {
  operand_rm(s, id_dest, true, id_src2, true);
  // shift instructions will eventually use the lower
  // 5 bits of %cl, therefore it is OK to load %ecx
  operand_reg(s, id_src1, true, R_ECX, 4);
}

static inline def_DHelper(O2a) {
  decode_op_O(s, id_src1, true);
  decode_op_a(s, id_dest, false);
}

static inline def_DHelper(a2O) {
  decode_op_a(s, id_src1, true);
  decode_op_O(s, id_dest, false);
}

static inline def_DHelper(d2a) {
  dsrc1 = ddest;
  decode_op_a(s, id_dest, true);
}

static inline def_DHelper(a){
  decode_op_a(s, id_dest, true);
}


static inline def_DHelper(J) {
  decode_op_SI(s, id_dest, false);
  // the target address can be computed in the decode stage
  s->jmp_pc = id_dest->simm + s->seq_pc;
}

static inline def_DHelper(ret_r){
  operand_reg(s,id_dest,true,R_ESP,4);
}

static inline def_DHelper(push_SI) {
  decode_op_SI(s, id_dest, true);
}

static inline def_DHelper(in_I2a) {
  id_src1->width = 1;
  decode_op_I(s, id_src1, true);
  decode_op_a(s, id_dest, false);
}

static inline def_DHelper(in_dx2a) {
  operand_reg(s, id_src1, true, R_DX, 2);
  decode_op_a(s, id_dest, false);
}

static inline def_DHelper(out_a2I) {
  decode_op_a(s, id_src1, true);
  id_dest->width = 1;
  decode_op_I(s, id_dest, true);
}

static inline def_DHelper(out_a2dx) {
  decode_op_a(s, id_src1, true);
  operand_reg(s, id_dest, true, R_DX, 2);
}

static inline void operand_write(DecodeExecState *s, Operand *op, rtlreg_t* src) {
  if (op->type == OP_TYPE_REG) { rtl_sr(s, op->reg, src, op->width); }
  else if (op->type == OP_TYPE_MEM) { rtl_sm(s, s->isa.mbase, s->isa.moff, src, op->width); }
  else { printf("dest is %d",*ddest);assert(0); }
}
 
