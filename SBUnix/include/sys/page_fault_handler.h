#ifndef _pf_handler_H
#define _pf_handler_H

#include <sys/defs.h>
#include <sys/process.h>

struct pt_regs_t {
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
}__attribute__((packed));

typedef struct pt_regs_t pt_regs;

/*
 * pf_error_code
 * bit 0: 0: caused by not present, i.e. Present flag in the Page Table entry is clear
 *        1: caused by invalid access right
 * bit 1: 0: by r/x
 *        1: by w
 * bit 2: 0: occurred while the processor was in Kernel Mode
 *        1: occurred in user mode
 * bit 3: 0: not caused by reserved bit violation
          1: caused by a reserved bit set to 1 in some
 paging-structure entry.
 */

#define PF_BIT_0 0x1
#define PF_BIT_1 0x2
#define PF_BIT_2 0x4
#define PF_BIT_3 0x8


// also declared in idt.h
void page_fault_handler (pt_regs *regs, uint64_t pf_err_code);

/*
 * check whether the given virt addr is in an addr range described by VMA
 * ret 1 if found, otherwise 0
 */
vma_struct *in_vma(uint64_t virt_addr, vma_struct *vma);

/*
 * check whether the given virt addr belongs to stack
 * ret 1 if found, otherwise 0
 */
int belong_to_stack(uint64_t virt_addr, vma_struct *vma);

/*
 * check whether the instruction casued pf is in exception table,
 * which contains all legal kernel access to user space 
 * (usually in syscall service routine)
 * equivalent to checking whether the pf in kernel is due to
 * wrong parm provided by user process or kernel's own bug
 * ret 1 if in, otherwise 0
 */
int search_exception_table(uint64_t rip);

#endif