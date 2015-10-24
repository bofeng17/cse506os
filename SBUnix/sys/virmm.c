#include <sys/virmm.h>
#include <sys/physical.h>

#include <sys/defs.h> //includes typedef for uint64_t etc.
#define TABLE_SIZE 512// 2^9=512 or in hex 0x1000

/*References:
 * I.AMD64 Architecture Programmer’s Manual Volume 2: System Programming
 * 		Figure 5-1       on p119
 * 		Figure 5-(17-21) on p133
 * II.Intel® 64 and IA-32 Architectures Software Developer’s ManualVolume 3A: System Programming Guide, Part 1
 * 		Figure 4-8 on page 4-20 Vol. 3A
 *
 */

#define PML4_SHIFT_BITS 39
#define PDPT_SHIFT_BITS 30
#define PDT_SHIFT_BITS 21
#define PT_SHIFT_BITS 12

#define SHIFT_MASK 0x1FF

/* Page table entry flags */
#define PTE_P		0x001	// Present
#define PTE_W		0x002	// Writable
#define PTE_U		0x004	// User
#define PTE_PWT		0x008	// Write-Through
#define PTE_PCD		0x010	// Cache-Disable
#define PTE_A		0x020	// Accessed
#define PTE_D		0x040	// Dirty
#define PTE_PS		0x080	// Page Size
#define PTE_MBZ		0x180	// Bits must be zero
#define PTE_COW		0x100	// Copy-on-write

//page map level 4 page table definition
struct PML4 {
	uint64_t PML4E[TABLE_SIZE];
};

//page directory pointer table
struct PDPT {
	uint64_t PDPTE[TABLE_SIZE];
};

//page directory
struct PD {
	uint64_t PDE[TABLE_SIZE];
};

//page table
struct PT {
	uint64_t PTE[TABLE_SIZE];
};

uint64_t get_pml4e_index(uint64_t addr) {
	return (((addr) >> PML4_SHIFT_BITS) & SHIFT_MASK);
}

uint64_t get_pdpte_index(uint64_t addr) {
	return (((addr) >> PDPT_SHIFT_BITS) & SHIFT_MASK);
}

uint64_t get_pdte_index(uint64_t addr) {
	return (((addr) >> PDT_SHIFT_BITS) & SHIFT_MASK);
}

uint64_t get_pte_index(uint64_t addr) {
	return (((addr) >> PT_SHIFT_BITS) & SHIFT_MASK);
}

// get PDPT entry addr from PML4
void* get_pdpt_addr_from_PML4(struct PML4 *pml4, uint64_t pml4e_index) {
	uint64_t pdpt = (uint64_t) allocate_page();
	pdpt |= (PTE_P | PTE_W | PTE_U);
	pml4->PML4E[pml4e_index] = pdpt;
	return (void *) pdpt;
}

// get PDT entry addr from PML4
void* get_pdt_addr_from_PML4(struct PML4 *pml4, uint64_t pml4e_index) {
	uint64_t pdt = (uint64_t) allocate_page();
	pdt |= (PTE_P | PTE_W | PTE_U);
	pml4->PML4E[pml4e_index] = pdt;
	return (void *) pdt;
}

// get PT entry addr from PML4
void* get_pt_addr_from_PML4(struct PML4 *pml4, uint64_t pml4e_index) {
	uint64_t pt = (uint64_t) allocate_page();
	pt |= (PTE_P | PTE_W | PTE_U);
	pml4->PML4E[pml4e_index] = pt;
	return (void *) pt;
}


struct PML4 *pml4;

void init_pagetables(uint64_t physbase, uint64_t vir_addr_kernel) {
//	uint64_t pml4e_index = get_pml4e_index(vir_addr_kernel);
//	uint64_t pdpte_index = get_pdpte_index(vir_addr_kernel);
//	uint64_t pdte_index = get_pdte_index(vir_addr_kernel);

	//setup each level addr pointer

	//begin mapping kernel from physbase to physfree


}
