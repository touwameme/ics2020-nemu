#include <isa.h>
#include <memory/vaddr.h>
#include <memory/paddr.h>

#define CR0_PG         0x80000000 


paddr_t isa_mmu_translate(vaddr_t vaddr, int type, int len) {
  uint32_t off_page = vaddr & 0xfff;
  uint32_t off_dir = (uint32_t)vaddr >> 22;
  uint32_t off_pte  = ((uint32_t)vaddr & 0x003ff000 ) >> 12;

  paddr_t dir = cpu.cr3;
  paddr_t pte = paddr_read(dir + off_dir * 4, 4);
  if ((pte & 1)==0) {  printf("1 error: %08x\n",pte);return MEM_RET_FAIL;}
  else{
      paddr_t pagebase = paddr_read((pte & 0xfffff000) + off_pte*4 , 4);
  
  if ((pagebase & 1)==0) return MEM_RET_FAIL;
  else
    return ((pagebase & 0xfffff000) + off_page);
  }
 
 
  return MEM_RET_FAIL;
}


int isa_vaddr_check(vaddr_t vaddr, int type, int len){
   int vm_open = ((cpu.cr0 & CR0_PG)!=0);
  if (vm_open) {
    return MEM_RET_NEED_TRANSLATE; 
  }
else{
  return MEM_RET_OK;
}
}



word_t vaddr_mmu_read(vaddr_t vaddr, int len, int type) {
  paddr_t paddr = isa_mmu_translate(vaddr,type,len);
  if(paddr == MEM_RET_FAIL){
    printf("vaddr mmu read %x :fail\n",vaddr);
   // printf("cr0 :%x  cr3: %x\n",cpu.cr0,cpu.cr3);
    assert(0);
  }
  else{
    paddr_t limit = ((vaddr /PAGE_SIZE)+1)*PAGE_SIZE;
    if(vaddr+ len > limit){
      //printf("vaddr mmu read %x :cross_page\n",vaddr);
      word_t res1 = 0;
      word_t res2 = 0;
      int len_in = limit - vaddr;
      int len_over = vaddr+ len - limit;
      uint32_t paddr_in = isa_mmu_translate(vaddr,MEM_TYPE_READ,len_in);
      res1 = paddr_read(paddr_in,len_in);
      uint32_t paddr_over = isa_mmu_translate(vaddr+len_in,MEM_TYPE_READ,len_over);
      res2 = paddr_read(paddr_over,len_over);

      //printf("cross page handled\n");
      return (res2<<(len_in*8))| res1;
      //assert(0);
    }
  }
  return paddr_read(paddr, len);    
}


void vaddr_mmu_write(vaddr_t vaddr, word_t data, int len)
{
  paddr_t paddr = isa_mmu_translate(vaddr, MEM_TYPE_WRITE, len);
  if (paddr == MEM_RET_FAIL) {
    printf("vaddr mmu write %x:fail   pc %x\n",vaddr,cpu.pc);
    assert(0);
  }
  else{
    paddr_t limit = ((vaddr /PAGE_SIZE)+1)*PAGE_SIZE;
    if(vaddr+ len > limit){
      //printf("vaddr mmu write %x:cross_page\n",vaddr);

      int len_in = limit - vaddr;
      int len_over = vaddr+ len - limit;
      uint32_t paddr_in = isa_mmu_translate(vaddr,MEM_TYPE_READ,len_in);
       uint32_t paddr_over = isa_mmu_translate(vaddr+len_in,MEM_TYPE_READ,len_over);
        paddr_write(paddr_in,data,len_in);
        paddr_write(paddr_over,data>>(len_in*8),len_over);
      }
  }
  
  paddr_write(paddr, data, len);                
} 


