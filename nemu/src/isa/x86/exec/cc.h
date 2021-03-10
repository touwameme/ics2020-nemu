#ifndef __CC_H__
#define __CC_H__
#include <isa.h>
#include "../local-include/rtl.h"

enum {
  CC_O, CC_NO, CC_B,  CC_NB,
  CC_E, CC_NE, CC_BE, CC_NBE,
  CC_S, CC_NS, CC_P,  CC_NP,
  CC_L, CC_NL, CC_LE, CC_NLE
};


/* Condition Code */

static inline const char* get_cc_name(int subcode) {
  static const char *cc_name[] = {
    "o", "no", "b", "nb",
    "e", "ne", "be", "nbe",
    "s", "ns", "p", "np",
    "l", "nl", "le", "nle"
  };
  return cc_name[subcode];
}

static inline void rtl_setcc(DecodeExecState *s, rtlreg_t* dest, uint32_t subcode) {
  uint32_t invert = subcode & 0x1;

  // TODO: Query EFLAGS to determine whether the condition code is satisfied.
  // dest <- ( cc is satisfied ? 1 : 0)    
  switch (subcode & 0xe) {      
    //case CC_O:  *dest = (*dest&0xffffff00) + ((cpu.eflags & EFLAG_OF) !=0)? 1:0);break;    only change one byte
   /*
    case CC_O:  *dest = (*dest&0xffffff00) +((((cpu.eflags & EFLAG_OF) !=0))? 1:0);break;
    case CC_B:  *dest = (*dest&0xffffff00) +((((cpu.eflags & EFLAG_CF) !=0))? 1:0);break;
    case CC_E:  *dest = (*dest&0xffffff00) +((((cpu.eflags & EFLAG_ZF) !=0))? 1:0);break;
    case CC_BE: *dest = (*dest&0xffffff00) +((((cpu.eflags & EFLAG_CF) !=0) || ((cpu.eflags & EFLAG_ZF) !=0))? 1:0);break;
    case CC_S:  *dest = (*dest&0xffffff00) +((((cpu.eflags & EFLAG_SF) !=0))? 1:0);break;
    case CC_L:  *dest = (*dest&0xffffff00) +((((cpu.eflags & EFLAG_SF) !=0) != ((cpu.eflags & EFLAG_OF) !=0))? 1:0);break;
    case CC_LE: *dest = (*dest&0xffffff00) +((((cpu.eflags & EFLAG_ZF) !=0) || ((cpu.eflags & EFLAG_SF) !=0) != ((cpu.eflags & EFLAG_OF) !=0))? 1:0);break;
    */
   case CC_O:  *dest  = ((((cpu.eflags & EFLAG_OF) !=0))? 1:0);break;
    case CC_B:  *dest = ((((cpu.eflags & EFLAG_CF) !=0))? 1:0);break;
    case CC_E:  *dest = ((((cpu.eflags & EFLAG_ZF) !=0))? 1:0);break;
    case CC_BE: *dest = ((((cpu.eflags & EFLAG_CF) !=0) || ((cpu.eflags & EFLAG_ZF) !=0))? 1:0);break;
    case CC_S:  *dest = ((((cpu.eflags & EFLAG_SF) !=0))? 1:0);break;
    case CC_L:  *dest = ((((cpu.eflags & EFLAG_SF) !=0) != ((cpu.eflags & EFLAG_OF) !=0))? 1:0);break;
    case CC_LE: *dest = ((((cpu.eflags & EFLAG_ZF) !=0) || ((cpu.eflags & EFLAG_SF) !=0) != ((cpu.eflags & EFLAG_OF) !=0))? 1:0);break;
    default: panic("should not reach here");
    case CC_P: panic("PF is not supported");
  }

  if (invert) {
    rtl_xori(s, dest, dest, 0x1);
  }
  assert((*dest&0xff) == 0 || (*dest&0xff) == 1);
}

#endif
