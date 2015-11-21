#ifndef _pf_handler_H
#define _pf_handler_H

#include <sys/defs.h>

// also declared in idt.h
void page_fault_handler ();
int check_vma(uint64_t virt_addr);

/* 
 * pf_error_code
 * bit 0: 0: caused by not present, i.e. Present flag in the Page Table entry is clear
 *        1: caused by invalid access right
 * bit 1: 0: by r/x
 *        1: by w
 * bit 2: 0: occurred while the processor was in Kernel Mode
 *        1: occurred in user mode
 */

#endif