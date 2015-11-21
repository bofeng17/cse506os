#include <sys/sbunix.h>
#include <sys/physical.h>
#include <sys/virmm.h>
#include <sys/process.h>
#include <sys/stdlib.h>

// should be removed finally
#define PML4 4
#define PDPT 3
#define PDT 2
#define PT 1

void page_fault_handler () {
    uint64_t pf_addr;
    __asm__ __volatile__("mov %%cr2, %0":"=r"(pf_addr));
    printf("page fault happens @%p!\n",pf_addr);
    __asm__ __volatile__("hlt");
}

//void page_fault_handler () {
//    uint64_t pf_addr;
//    vma_struct *vma = current->mm->mmap;
//    uint64_t vma_perm_flag;
//    uint64_t pf_error_code; // 3 bits error_code pushed by CPU
//    __asm__ __volatile__("mov %%cr2, %0":"=r"(pf_addr));
//    
//    // for demand paging
//    // check whether the given virt addr is in an addr range described VMA
//    if (check_vma(pf_addr)) {
//        // pf caused by demand paging
//        
//        // TODO: should check VMA permission bits here
//        //       and stores in corresponding bits
//        vma_perm_flag = PTE_P | PTE_U | vma->permission_flag;
//        // TODO: need to check conditions in if statement
//        // especially for the flag bits and un-zerod page frames
//        if (!self_ref_read(PML4, pf_addr)){
//            // if PDPT isn't in memory, allocate page and map it
//            // TODO: or flag bits
//            self_ref_write(PML4, pf_addr, allocate_page_user()|vma_perm_flag);
//        }
//        if (!self_ref_read(PDPT, pf_addr)) {
//            self_ref_write(PDPT, pf_addr, allocate_page_user()|vma_perm_flag);
//        }
//        if (!self_ref_read(PDT, pf_addr)) {
//            self_ref_write(PDT, pf_addr, allocate_page_user()|vma_perm_flag);
//        }
//        if (!self_ref_read(PT, pf_addr)) {
//            // if Page Frame isn't in memory, allocate page and map it
//            // This should always be true
//            uint64_t phys = allocate_page_user();
//            self_ref_write(PT, pf_addr, phys|vma_perm_flag);
//            
//            // TODO: check the end of file before copying
//            // void *memcpy (phys, vma->file_offset+(pf_addr-vma->vm_start), 0x1000);// 4KB
//        }
//    } else {
//        // pf caused by illegal access
//        
//    }
//    
//    // for copy on write
//    if (condition) {
//        <#statements#>
//    }
//}
//
//int check_vma(uint64_t virt_addr) {
//    vma_struct *vma = current->mm->mmap;
//    while(vma){
//        if (vma->vm_start <= virt_addr && vma->vm_start > virt_addr) {
//            return 1;
//        } else {
//            vma = vma->vm_next;
//        }
//    }
//    return 0;
//}
//
