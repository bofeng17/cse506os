#include <sys/sbunix.h>
#include <sys/physical.h>
#include <sys/virmm.h>
#include <sys/process.h>
#include <sys/stdlib.h>
#include <sys/page_fault_handler.h>

//void page_fault_handler () {
//    uint64_t pf_addr;
//    __asm__ __volatile__("mov %%cr2, %0":"=r"(pf_addr));
//    printf("page fault happens @%p!\n",pf_addr);
//    __asm__ __volatile__("hlt");
//}

// 1st parm: error_code pushed by CPU
void page_fault_handler (pt_regs *regs, uint64_t pf_err_code) {
    uint64_t pf_addr;
    task_struct *tsk = current; // Save current is critical in preemptive scheduling
    vma_struct *vma = tsk->mm->mmap;
    uint64_t vma_perm_flag = vma -> permission_flag;
    uint64_t pt_perm_flag;
    uint64_t page_frame_phys; // Store the physical addr of page frame allocated
    
    __asm__ __volatile__("mov %%cr2, %0":"=r"(pf_addr));
    
    /*
     * Level 1 check:
     * check whether the given virt addr is in an addr range described VMA
     * or belong to the autho-growing stack
     */
    if (in_vma(pf_addr, vma) | belong_to_stack(pf_addr, vma)) {
        /*
         * Level 2 check:
         * if bit 0 of pf_err_code is 0 (page not present)
         * pf is caused by demand paging
         * otherwise, contiune checking
         */
        if (!(pf_err_code & PF_BIT_0)) {
            // pf caused by demand paging
            
            // TODO: translate VMA permission into pt perm
            // PTE_W: depends on vma_perm_flag
            pt_perm_flag = PTE_P | PTE_U | vma_perm_flag;
            
            // TODO: need to check conditions in if statement
            // especially for the flag bits and un-zerod page frames
            if (!self_ref_read(PML4, pf_addr)){
                // if PDPT isn't in memory, allocate page and map it
                // TODO: or flag bits
                self_ref_write(PML4, pf_addr, allocate_page_user()|pt_perm_flag);
            }
            if (!self_ref_read(PDPT, pf_addr)) {
                self_ref_write(PDPT, pf_addr, allocate_page_user()|pt_perm_flag);
            }
            if (!self_ref_read(PDT, pf_addr)) {
                self_ref_write(PDT, pf_addr, allocate_page_user()|pt_perm_flag);
            }
            if (!self_ref_read(PT, pf_addr)) {
                // if Page Frame isn't in memory, allocate page and map it
                // This should always be true
                page_frame_phys = allocate_page_user();
                self_ref_write(PT, pf_addr, page_frame_phys|pt_perm_flag);
                
                // TODO: check the end of file before copying
                // Does our memory region page aligned?
                memcpy ((void *)page_frame_phys, (void *)vma->file_offset + (pf_addr - vma->vm_start), 0x1000);// 4KB
            }
        } else {
            /*
             * Level 3 check:
             * if bit 1 is 1 (write operation) & vma allows write & bit 3 is 1 (reserved bit is set to 1)
             */
            
            //TODO: vma_perm_flag check
            if ((pf_err_code & PF_BIT_1) && vma_perm_flag && (pf_err_code & PF_BIT_3)) {
                // pf caused by COW
                
                // TODO: reference count of page frame
                
            } else {
                // pf caused by illegal access of user, kill user process
                // do_exit();
            }
        }
    } else {
        /*
         * Level 2 check:
         * if bit 2 is 1 (pf occured in user mode)
         */
        if (pf_err_code & PF_BIT_2) {
            /*
             * pf caused by illegal access of user, kill user process
             */
        } else {
            /*
             * Level 3 check:
             * if wrong syscall parm
             */
            if (search_exception_table(regs->rip)) {
                // pf caused by wrong syscall parm provided by user, kill user process
                // do_exit();
            } else {
                // pf caused by kernel bugs or (extreme memory shortage)
                printf("Kernel Pannic!\n");
                __asm__ __volatile__("hlt");
            }
        }
        
    }
}

int in_vma(uint64_t virt_addr, vma_struct *vma) {
    while(vma){
        if (vma->vm_start <= virt_addr && vma->vm_end > virt_addr) {
            return 1;
        } else {
            vma = vma->vm_next;
        }
    }
    return 0;
}

int belong_to_stack(uint64_t virt_addr, vma_struct *vma) {
    return 0;
}

// TODO: done with do_read, do_write ...
int search_exception_table(uint64_t rip) {
    return 0;
}
