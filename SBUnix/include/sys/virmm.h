#ifndef _VIRMM_H
#define _VIRMM_H

#include <sys/defs.h> //includes typedef for uint64_t etc.
#include <sys/physical.h>

#define TABLE_SIZE 512// 2^9=512 or in hex 0x1000
#define PAGE_SIZE	0x1000 //4KB
#define VIR_START 0xFFFFFFFF80000000UL
#define PHY_START 0x0
/*References:
 * I.AMD64 Architecture Programmer’s Manual Volume 2: System Programming
 * 		Figure 5-1       on p119
 * 		Figure 5-(17-21) on p133
 * II.Intel® 64 and IA-32 Architectures Software Developer’s ManualVolume 3A: System Programming Guide, Part 1
 * 		Figure 4-8 on page 4-20 Vol. 3A
 *
 */

#define USER_VIR_START 0x0000000000400000// user virtual memory starting addr

#define PML4_SHIFT_BITS 39
#define PDPT_SHIFT_BITS 30
#define PDT_SHIFT_BITS 21
#define PT_SHIFT_BITS 12

#define SHIFT_MASK 0x1FF

/* Page table entry flags */
#define PTE_P		0x001	// Present
#define PTE_W		0x002	// Writable
#define PTE_U		0x004	// User
#define PTE_EX      0x7FFFFFFFFFFFFFFF //set execute bit 63
#define CLEAR_OFFSET       0xFFFFFFFFFFFFF000

#define PTE_PWT		0x008	// Write-Through
#define PTE_PCD		0x010	// Cache-Disable
#define PTE_A		0x020	// Accessed
#define PTE_D		0x040	// Dirty
#define PTE_PS		0x080	// Page Size
#define PTE_MBZ		0x180	// Bits must be zero
#define PTE_COW		0x100	// Copy-on-write

//flags for kmalloc
#define KERNPT 0 // kernel page table
#define TASK 1
#define KSTACK 2
#define MM 3
#define USERPT 4 //user page table
#define VMA 5
#define FILE 6

#define VM_READ         0x00000001      /* currently active flags */
#define VM_WRITE        0x00000002
#define VM_EXEC         0x00000004
#define VM_SHARED       0x00000008

#define STACK_TOP 0x00000000ffffffff

#define PML4 4
#define PDPT 3
#define PDT 2
#define PT 1

//page map level 4 page table definition
struct pml4_t
{
  uint64_t PML4E[TABLE_SIZE];
};
typedef struct pml4_t* pml4_t;

//page directory pointer table
struct pdpt_t
{
  uint64_t PDPTE[TABLE_SIZE];
};
typedef struct pdpt_t* pdpt_t;

//page directory
struct pdt_t
{
  uint64_t PDTE[TABLE_SIZE];
};
typedef struct pdt_t* pdt_t;

//page table
struct pt_t
{
  uint64_t PTE[TABLE_SIZE];
};
typedef struct pt_t* pt_t;

void
init_mm ();

void
initial_mapping ();

uint64_t
get_CR3 ();

void
set_CR3 (uint64_t CR3);

void*
kmalloc (int flag);

void
kfree (void* addr, int flag);

void*
umalloc (void* addr, size_t size);

/* Self-reference:
 * make 510th entry of PML4, instead of 511th entry which is used by kernel mapping, 
 * points to the 1st entry of PML4
 */

/*
 * level: page table level
 * entry_correpond_to_vir: the virtual address specifying which entry to write
 *                e.g. when a page fault happens, the virtual addr. causing it
 is read from CR2 register and passed to this parameter
 * entry_val_phy: the physical addr. of page frame/next level page table
 *                to be written to the entry specified by entry_correpond_to_vir
 */
void
self_ref_write (int level, uint64_t entry_correpond_to_vir,
		uint64_t entry_val_phy);

/*
 * entry_correpond_to_vir: same to self_ref_write
 * return: the physical addr. of page frame/next level page table
 *         read from the entry specified by entry_correpond_to_vir
 */
uint64_t
self_ref_read (int level, uint64_t entry_correpond_to_vir);

#endif
