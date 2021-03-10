#ifndef __ISA_X86_H__
#define __ISA_X86_H__

#include <common.h>


// memory
#define x86_IMAGE_START 0x100000
#define x86_PMEM_BASE 0x0
#define FLAGS_NUM 6
#define C_F 0
#define Z_F 1
#define S_F 2
#define I_F 3
#define D_F 4
#define O_F 5
#define EFLAG_CF 0x1
#define EFLAG_ZF 0x40
#define EFLAG_SF 0x80
#define EFLAG_IF 0x200
#define EFLAG_DF 0x400
#define EFLAG_OF 0x800
#define EFLAG_TF 0x100

// reg

/* TODO: Re-organize the `CPU_state' structure to match the register
 * encoding scheme in i386 instruction format. For example, if we
 * access cpu.gpr[3]._16, we will get the `bx' register; if we access
 * cpu.gpr[1]._8[1], we will get the 'ch' register. Hint: Use `union'.
 * For more details about the register encoding scheme, see i386 manual.
 */

typedef struct {

  union{
    	struct{	
    union{
    uint32_t _32;
    uint16_t _16;
    uint8_t _8[2];
    };
  } gpr[8];
struct{
  rtlreg_t eax, ecx, edx, ebx, esp, ebp, esi, edi;
  uint32_t eflags;
   rtlreg_t cr3,cr0;
   bool INTR;
  vaddr_t pc;
  uint32_t ip,cs, ss, ds, es, fs, gs;
  rtlreg_t ksp;
  struct {
      uint16_t limit;
      uint32_t base; 
  } idt;  //use 48bits 
};
  };
  /* Do NOT change the order of the GPRs' definitions. */

  /* In NEMU, rtlreg_t is exactly uint32_t. This makes RTL instructions
   * in PA2 able to directly access these registers.
   */
  
} x86_CPU_state;

// decode
typedef struct {
  bool is_operand_size_16;
  uint8_t ext_opcode;
  const rtlreg_t *mbase;
  rtlreg_t mbr;
  word_t moff;
} x86_ISADecodeInfo;


#define suffix_char(width) ((width) == 4 ? 'l' : ((width) == 1 ? 'b' : ((width) == 2 ? 'w' : '?')))
//#define isa_vaddr_check(vaddr, type, len) (MEM_RET_OK)
#define x86_has_mem_exception() (false)

#endif
