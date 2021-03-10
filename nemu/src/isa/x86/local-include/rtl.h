#ifndef __X86_RTL_H__
#define __X86_RTL_H__

#include <rtl/rtl.h>
#include "reg.h"
#include <isa/x86.h>
#include <memory/paddr.h>
#include <isa.h>

/* RTL pseudo instructions */


static inline def_rtl(lr, rtlreg_t* dest, int r, int width) {
  switch (width) {
    case 4: rtl_mv(s, dest, &reg_l(r)); return;
    case 1: rtl_host_lm(s, dest, &reg_b(r), 1); return;
    case 2: rtl_host_lm(s, dest, &reg_w(r), 2); return;
    default: assert(0);
  }
}

static inline def_rtl(sr, int r, const rtlreg_t* src1, int width) {
  switch (width) {
    case 4: rtl_mv(s, &reg_l(r), src1); return;
    case 1: rtl_host_sm(s, &reg_b(r), src1, 1); return;
    case 2: rtl_host_sm(s, &reg_w(r), src1, 2); return;
    default: assert(0);
  }
}

static inline def_rtl(push, const rtlreg_t* src1) {
  // esp <- esp - 4
  // M[esp] <- src1

  int width = (s->isa.is_operand_size_16)? 2:4;
  if(ddest == &cpu.esp){
    vaddr_write(cpu.esp-width,*src1,width);
    cpu.esp -= width;
  }
  else{
  cpu.esp -=width;
  vaddr_write(cpu.esp,*src1,width);
  }


  
  
}


static inline def_rtl(pop, rtlreg_t* dest) {
  // dest <- M[esp]
  // esp <- esp + 4
  int width = (s->isa.is_operand_size_16)? 2:4;
  *dest = vaddr_read(cpu.esp,width);
  cpu.esp+=width;
}
/*
static inline def_rtl(is_sub_overflow, rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1, const rtlreg_t* src2, int width) {
  // dest <- is_overflow(src1 - src2)
  uint32_t msk=0;
  switch(width){
    case 1:msk = 0x80;break;
    case 2:msk = 0x8000;break;
    case 4:msk = 0x80000000;break;
    default: assert(0);
  }
  if(((*src1&msk)!=(*src2&msk))&&((*src1&msk)!=(*res&msk))){
    *dest = 1;
  }
  else{
    *dest = 0;
  }
}

static inline def_rtl(is_sub_carry, rtlreg_t* dest,
    const rtlreg_t* src1, const rtlreg_t* src2) {
  // dest <- is_carry(src1 - src2) 
    if(*src1 < *src2){
    *dest = 1;
  }
  else{
    *dest = 0;
  }
}

static inline def_rtl(is_add_overflow, rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1, const rtlreg_t* src2, int width) {
  // dest <- is_overflow(src1 + src2)
  uint32_t msk=0;
  switch(width){
    case 1:msk = 0x80;break;
    case 2:msk = 0x8000;break;
    case 4:msk = 0x80000000;break;
  }
  if (((*src1&msk)!=(*src2&msk))&&((*src1&msk)!=(*res&msk)))
  {
    *dest = 1;
  }
  else{
    *dest = 0;
  }
}

static inline def_rtl(is_add_carry, rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1) {
  // dest <- is_carry(src1 + src2)
  if(*res < *src1){
    *dest = 1;
  }
  else{
    *dest = 0;
  }
}
*/
static inline def_rtl(is_sub_overflow, rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1, const rtlreg_t* src2, int width) {

    *t0 = ~*src2;
    *dest = ((((*src1) >> (8 * width - 1)) & 1) == (((*t0) >> (8 * width - 1)) & 1)) &&
            ((((*src1) >> (8 * width - 1)) & 1) != (((*res) >> (8 * width - 1)) & 1));


  //TODO();
}

static inline def_rtl(is_sub_carry, rtlreg_t* dest,
    const rtlreg_t* src1, const rtlreg_t* src2) {
    *dest = (*src2 > *src1)? 1 : 0;
  // dest <- is_carry(src1 - src2)
  //TODO();
}

static inline def_rtl(is_add_overflow, rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1, const rtlreg_t* src2, int width) {
    *dest =  ((*src1 >> (8 * width - 1) & 1) == (*src2 >> (8 * width - 1) & 1)) &&
             ((*src1 >> (8 * width - 1) & 1) != (*res >> (8 * width - 1) & 1));
    //dest <- is_overflow(src1 + src2)
  //TODO();
}

static inline def_rtl(is_add_carry, rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1) {
    *dest = (*res < *src1);
  // dest <- is_carry(src1 + src2)
  //TODO();
}
#define def_rtl_setget_eflags(f) \
  static inline def_rtl(concat(set_, f), const rtlreg_t* src) { \
    if(*src ){cpu.eflags |= concat(EFLAG_,f);} else {cpu.eflags &= ~(concat(EFLAG_,f));}\
  } \
  static inline def_rtl(concat(get_, f), rtlreg_t* dest) { \
    *dest = (cpu.eflags & concat(EFLAG_,f)) != 0 ; \
  }


def_rtl_setget_eflags(CF)
def_rtl_setget_eflags(OF)
def_rtl_setget_eflags(ZF)
def_rtl_setget_eflags(SF)


static inline int is_zero(const rtlreg_t* result, int width){
  uint32_t val= 0 ;
  switch(width){
    case 1 : val = *result & 0xff;break;
    case 2 : val = *result & 0xffff;break;
    case 4 : val = *result & 0xffffffff;break;
    default: assert(0);
  } 
  return val == 0;
}

static inline def_rtl(update_ZF, const rtlreg_t* result, int width) {
   if(is_zero(result,width)){
    cpu.eflags |= EFLAG_ZF;
  }
  else{
    cpu.eflags &= (~EFLAG_ZF);
  }
}

static inline int is_sign(const rtlreg_t* result, int width){
  uint32_t val ;
  switch(width){
    case 1 : val = *result & 0x80;break;
    case 2 : val = *result & 0x8000;break;
    case 4 : val = *result & 0x80000000;break;
    default:assert(0);
  } 
  return val != 0;
}


static inline def_rtl(update_SF, const rtlreg_t* result, int width) {
  // eflags.SF <- is_sign(result[width * 8 - 1 .. 0])
  if(is_sign(result,width)){
    cpu.eflags |= EFLAG_SF;
  }
  else{
    cpu.eflags &= (~EFLAG_SF);
  }
}

static inline void set_CFOF_zero(){
  cpu.eflags &= (~EFLAG_CF);
  cpu.eflags &= (~EFLAG_OF);
}
static inline def_rtl(update_ZFSF, const rtlreg_t* result, int width) {
  rtl_update_ZF(s, result, width);
  rtl_update_SF(s, result, width);
}


#endif
