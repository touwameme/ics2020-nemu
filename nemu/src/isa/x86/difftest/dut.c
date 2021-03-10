#include <isa.h>
#include <isa/x86.h>
#include <monitor/difftest.h>
#include "../local-include/reg.h"
#include "difftest.h"
#include <memory/paddr.h>




bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
 

   if(pc!=ref_r->pc ){
    printf("pc wrong at %x  the ref_pc is %x\n",pc,ref_r->pc);
    printf("eflags:CF:%d,ZF:%d,SF:%d,IF:%d,DF:%d,OF:%d\n",(ref_r->eflags&EFLAG_CF)!=0,(ref_r->eflags&EFLAG_ZF)!=0,(ref_r->eflags&EFLAG_SF)!=0,(ref_r->eflags&EFLAG_IF)!=0,(ref_r->eflags&EFLAG_DF)!=0,(ref_r->eflags&EFLAG_OF)!=0);
     return false;
  }  
  if(ref_r->eax != cpu.eax){
     printf("eax wrong:dut is %x  the ref_eax is %x\n",cpu.eax,ref_r->eax);
     return false;
  }
  if(ref_r->ebx != cpu.ebx){
     printf("ebx wrong:dut is %x  the ref_ebx is %x\n",cpu.ebx,ref_r->ebx);
     return false;
  }
  if(ref_r->ecx != cpu.ecx){
     printf("ecx wrong:dut is %x  the ref_ecx is %x\n",cpu.ecx,ref_r->ecx);
     return false;
  }
  if(ref_r->edx != cpu.edx){
     printf("edx wrong:dut is %x  the ref_edx is %x\n",cpu.edx,ref_r->edx);
     return false;
  }
  if(ref_r->esp != cpu.esp){
     printf("esp wrong:dut is %x  the ref_esp is %x\n",cpu.esp,ref_r->esp);
     return false;
  }

if(ref_r->ebp!= cpu.ebp){
     printf("ebp wrong:dut is %x  the ref_ebp is %x\n",cpu.ebp,ref_r->ebp);
     return false;
  }

  if(ref_r->edi != cpu.edi){
     printf("edi wrong:dut is %x  the ref_edi is %x\n",cpu.edi,ref_r->edi);
     return false;
  }

  if(ref_r->esi != cpu.esi){
     printf("esi wrong:dut is %x  the ref_esi is %x\n",cpu.esi,ref_r->esi);
     return false;
  }   
 /*
  if( ref_r->cs != cpu.cs && ((ref_r->cs & 0xf)  == 0)){
     printf("cs wrong:dut is %x  the ref_cs is %x\n",cpu.cs,ref_r->cs);
     return true;
  } 
  */
  return true;
}


#define PMEM_SIZE (128 * 1024 * 1024)

#define PC_START  (PMEM_BASE + IMAGE_START)
#define IMAGE_S2 0x100000

void setregs(CPU_state *ref_r){
   ref_r->eax = cpu.eax;
   ref_r->ebx = cpu.ebx;
   ref_r->ecx = cpu.ecx;
   ref_r->edx = cpu.edx;
   ref_r->edi = cpu.edi;
   ref_r->esi = cpu.esi;
   ref_r->esp = cpu.esp;
   ref_r->ebp = cpu.ebp;


}

void isa_difftest_attach() {
   char* mainargs = guest_to_host(0);
   printf("1\n");
   ref_difftest_memcpy_from_dut(PMEM_BASE,mainargs,0x7c00);
   printf("2\n");
   mainargs = (char*) guest_to_host(IMAGE_S2);
   printf("3\n");
   ref_difftest_memcpy_from_dut(PMEM_BASE+IMAGE_S2,mainargs,PMEM_SIZE-IMAGE_S2);
   printf("4\n");
   ref_difftest_setregs(&cpu);
   
}

