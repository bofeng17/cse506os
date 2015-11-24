#include <sys/sbunix.h>
#include <sys/physical.h>
#include <sys/virmm.h>
#include <sys/process.h>
#include <sys/stdlib.h>
#include <sys/tarfs.h>
#include <sys/page_fault_handler.h>

//void page_fault_handler () {
//    uint64_t pf_addr;
//    __asm__ __volatile__("mov %%cr2, %0":"=r"(pf_addr));
//    printf("page fault happens @%p!\n",pf_addr);
//    __asm__ __volatile__("hlt");
//}

// 1st/2nd parm: registers/error_code pushed by CPU
void page_fault_handler (pt_regs *regs, uint64_t pf_err_code) {
    uint64_t pf_addr;
    task_struct *tsk = current; // Save current is critical in preemptive scheduling
    vma_struct *vma = tsk->mm->mmap;
    uint64_t vma_perm_flag = vma -> permission_flag;
    uint64_t pt_perm_flag;
    uint64_t page_frame_des; // physical addr of page frame newly allocated. Destination
    uint64_t page_frame_src; // physical addr of existed page frame read by self_ref_read(). Source
    
    __asm__ __volatile__("mov %%cr2, %0":"=r"(pf_addr));
    
    /*
     * Level 1 check:
     * check whether the given virt addr is in an addr range described VMA
     * or belong to the autho-growing stack
     */
    if (in_vma(pf_addr, vma) || belong_to_stack(pf_addr, vma)) {
        /*
         * Level 2 check:
         * if bit 0 of pf_err_code is 0 (page not present)
         * pf is caused by demand paging
         * otherwise, contiune checking
         */
        if (pf_err_code & (!PF_BIT_0)) {
            // pf caused by demand paging
            
            // TODO: translate VMA permission into pt perm
            // doesn't take into account NX bit
            pt_perm_flag = PTE_P | PTE_U ;
            if (vma_perm_flag & VM_WRITE) { // VMA has write access right
                pt_perm_flag |= PTE_W;
            }
            
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
                page_frame_des = allocate_page_user();
                self_ref_write(PT, pf_addr, page_frame_des|pt_perm_flag);
                
                // TODO: check the end of file before copying
                // Does our memory region page aligned, currently not
                if (pf_addr < vma->vm_end && pf_addr >= (vma->vm_end & CLEAR_OFFSET)) {// if in last page of a vma
                    memcpy ((void *)page_frame_des, (void *)(vma->vm_file->start + vma->file_offset) + (pf_addr - vma->vm_start), vma->vm_end - (vma->vm_end & CLEAR_OFFSET));// less than 4KB
                } else {
                    memcpy ((void *)page_frame_des, (void *)(vma->vm_file->start + vma->file_offset) + (pf_addr - vma->vm_start), 0x1000);// 4KB
                    // tricky way
                    // memcpy ((void *)page_frame_des, (void *)vma->file_offset + (pf_addr - vma->vm_start), 0x1000);// 4KB
                }
            }
        } else {
            /*
             * Level 3 check:
             * if bit 1 is 1 (write operation) && vma allows write && bit 3 is 1 (reserved bit is set to 1)
             * TODO: 3rd condition is too coarse-grained
             */
            
            //TODO: vma_perm_flag check
            if ((pf_err_code & PF_BIT_1) && (vma_perm_flag & VM_WRITE) && (pf_err_code & PF_BIT_3)) {
                // pf caused by COW
                pt_perm_flag = PTE_P | PTE_U | PTE_W;
                page_frame_des = allocate_page_user();
                page_frame_src = self_ref_read(PT, pf_addr) & CLEAR_FLAG;
                // Copy content
                memcpy((void *)page_frame_des, (void *)page_frame_src, 0x1000);
                // 5 Level in total, how to track it?
                // Modify page table
                self_ref_write(PT, pf_addr, page_frame_des | pt_perm_flag);
                
                // TODO: reference count of page frame
                // Junco TODO: reference count when allocated
                // if not shared due to COW anymore, clear reserve bit, mark as writable again
                
//                if (--(get_page_frame_descriptor(page_frame_src))->ref_count) {
//                    // clear reserve bit, set writable bit here
//                    self_ref_write(PT, pf_addr, page_frame_src | pt_perm_flag);
//                }
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
    __asm__ __volatile__ ("hlt");
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
