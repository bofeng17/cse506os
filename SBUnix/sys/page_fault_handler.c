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
void page_fault_handler(pt_regs *regs, uint64_t pf_err_code) {
    uint64_t pf_addr;
    task_struct *tsk = current; // Save current is critical in preemptive scheduling
    vma_struct *vma = tsk->mm->mmap;
    uint64_t pt_perm_flag;
    uint64_t page_frame_des; // physical addr of page frame newly allocated. Destination
    uint64_t page_frame_src; // physical addr of existed page frame read by self_ref_read(). Source
    uint64_t tmp_vir_addr = 0xffffffff80000000UL; // used in COW to map newly allocated page frame
    uint64_t tmp_phys_addr; // used in COW to store the original mapping of stolen tmp_vir_addr
    
    __asm__ __volatile__("mov %%cr2, %0":"=r"(pf_addr));
    
//    dprintf("pf happens @%p, caused by %p\n", pf_addr, regs->rip);
//    if (pf_err_code & PF_BIT_0) {
//        dprintf("invalid access right | ");
//    } else {
//        dprintf("not present | ");
//    }
//    if (pf_err_code & PF_BIT_1) {
//        dprintf("write | ");
//    } else {
//        dprintf("read or execute | ");
//    }
//    if (pf_err_code & PF_BIT_2) {
//        dprintf("in user mode\n");
//    } else {
//        dprintf("in kernel mode\n");
//    }

    
    /*
     * Level 1 check:
     * check whether the given virt addr is in an addr range described VMA
     * or belong to the autho-growing stack
     */
    if ((vma = in_vma(pf_addr, vma)) || belong_to_stack(pf_addr, vma)) {
        /*
         * Level 2 check:
         * if bit 0 of pf_err_code is 0 (page not present)
         * pf is caused by demand paging
         * otherwise, contiune checking
         */
        if (!(pf_err_code & PF_BIT_0)) {
            // pf caused by demand paging
            
            // Part 1 of translating VMA permission into Page Table permission
            // doesn't take into account NX bit
            pt_perm_flag = PTE_P | PTE_U | PTE_W;
            
            // if PDPT isn't in memory (present bit is 0), allocate page and map it
            if (!(self_ref_read(PML4, pf_addr) & PTE_P)) {
                // TODO: or flag bits
                self_ref_write(PML4, pf_addr,
                               allocate_page_user() | pt_perm_flag);
            }
            if (!(self_ref_read(PDPT, pf_addr) & PTE_P)) {
                self_ref_write(PDPT, pf_addr,
                               allocate_page_user() | pt_perm_flag);
            }
            if (!(self_ref_read(PDT, pf_addr) & PTE_P)) {
                self_ref_write(PDT, pf_addr,
                               allocate_page_user() | pt_perm_flag);
            }
            if (!(self_ref_read(PT, pf_addr) & PTE_P)) {
                // if Page Frame isn't in memory, allocate page and map it
                // This should always be true
                page_frame_des = allocate_page_user();
                self_ref_write(PT, pf_addr, page_frame_des | pt_perm_flag);
                
                // check if memory region is file-backed or anonymous (do nothing here)
                if (vma->vm_file != NULL) {
                    // file-backed memory region, copy content from file
                    // check if it is the last page of this vma before copying
                    if (pf_addr < vma->vm_end
                        && pf_addr >= ((vma->vm_end - 1) & CLEAR_OFFSET)) {
                        memcpy((void *) (pf_addr & CLEAR_OFFSET),
                               (void *) ((vma->vm_file->start + vma->file_offset)
                                         + ((pf_addr & CLEAR_OFFSET) - vma->vm_start)),
                               vma->vm_end - ((vma->vm_end - 1) & CLEAR_OFFSET)); // less than 4KB
                    } else {
                        memcpy((void *) (pf_addr & CLEAR_OFFSET),
                               (void *) ((vma->vm_file->start + vma->file_offset)
                                         + ((pf_addr & CLEAR_OFFSET) - vma->vm_start)),
                               PAGE_SIZE);                // 4KB
                    }
                }
                
                // Part 2 of translating VMA permission into Page Table permission
                if (!(vma->permission_flag & VM_WRITE)) {
                    // if VMA doesn't have write access right, remove write right from pt
                    pt_perm_flag &= ~PTE_W;
                    self_ref_write(PT, pf_addr, page_frame_des | pt_perm_flag);
                }
            }
        } else {
            /*
             * Level 3 check:
             * if bit 1 is 1 (write operation) && vma allows write && bit 3 is 1 (reserved bit is set to 1)
             */
            if ((pf_err_code & PF_BIT_1) && (vma->permission_flag & VM_WRITE)
                && (self_ref_read(PT, pf_addr) & PTE_COW)) {
                // TODO: branch never reached by testing
                // pf caused by COW
                pt_perm_flag = PTE_P | PTE_U | PTE_W;
                page_frame_src = self_ref_read(PT, pf_addr) & CLEAR_FLAG;
                
                /*
                 * if page frame is not shared (ref_count == 0) due to COW anymore,
                 * clear reserved bit, mark as writable again
                 */
                // TODO: verify correntness here (when COW page fault)
                // TODO: verify reference count when allocated/fork
                if (!(get_page_frame_descriptor(page_frame_src)->ref_count)) {
                    self_ref_write(PT, pf_addr,
                                   (page_frame_src | pt_perm_flag) & (~PTE_COW));
                } else {
                    page_frame_des = allocate_page_user();
//                    dprintf("physical page %p allocated\n", page_frame_des);
                    /*
                     * steal tmp_vir_addr (0xffffffff80000000UL) and point it to the allocated page frame
                     * so that we can copy content into that page frame
                     * before stealing, save the original mapping
                     */
                    tmp_phys_addr = self_ref_read(PT, tmp_vir_addr);
                    self_ref_write(PT, tmp_vir_addr, page_frame_des | pt_perm_flag);
                    
                    // Copy content
                    memcpy((void *) tmp_vir_addr, (void *) (pf_addr & CLEAR_OFFSET),
                           PAGE_SIZE);
                    
                    // Modify PT
                    self_ref_write(PT, pf_addr, page_frame_des | pt_perm_flag);
                    
                    // decrease ref_count of page frame
                    get_page_frame_descriptor(page_frame_src)->ref_count--;
                    
                    // restore the original mapping of tmp_vir_addr
                    self_ref_write(PT, tmp_vir_addr, tmp_phys_addr);
                    
                }
            } else {
                // TODO: branch never reached by testing
                // pf caused by illegal access of user, kill user process
                do_exit(-ILLEGAL_MEM_ACC);
            }
        }
    } else {
        /*
         * Level 2 check:
         * if bit 2 is 1 (pf occured in user mode)
         */
        if (pf_err_code & PF_BIT_2) {
            // TODO: branch never reached by testing
            // pf caused by illegal access of user, kill user process
            do_exit(-ILLEGAL_MEM_ACC);
        } else {
            /*
             * Level 3 check:
             * if wrong syscall parm
             */
            if (search_exception_table(regs->rip)) {
                // TODO: branch never reached by testing
                // pf caused by wrong syscall parm provided by user, kill user process
                do_exit(-ILLEGAL_MEM_ACC);
            } else {
                // pf caused by kernel bugs or (extreme memory shortage)
                printf("Kernel Pannic @ %p!\n", pf_addr);
                __asm__ __volatile__("hlt");
            }
        }
    }
    //    __asm__ __volatile__ ("hlt");
}

vma_struct *in_vma(uint64_t virt_addr, vma_struct *vma) {
    while (vma) {
        if (vma->vm_start <= virt_addr && vma->vm_end > virt_addr) {
            return vma;
        } else {
            vma = vma->vm_next;
        }
    }
    return NULL;
}

int belong_to_stack(uint64_t virt_addr, vma_struct *vma) {
    return 0;
}

// TODO: done with do_read, do_write ...
int search_exception_table(uint64_t rip) {
    return 0;
}
