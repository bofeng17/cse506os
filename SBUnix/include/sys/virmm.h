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

//page map level 4 page table definition
struct PML4
{
  uint64_t PML4E[TABLE_SIZE];
};
typedef struct PML4* pml4_t;

//page directory pointer table
struct PDPT
{
  uint64_t PDPTE[TABLE_SIZE];
};
typedef struct PDPT* pdpt_t;

//page directory
struct PDT
{
  uint64_t PDTE[TABLE_SIZE];
};
typedef struct PDT* pdt_t;

//page table
struct PT
{
  uint64_t PTE[TABLE_SIZE];
};
typedef struct PT* pt_t;

void
init_mm ();

uint64_t
initial_mapping ();

void
load_CR3 ();

void*
kmalloc (int flag);

void
kfree (void* addr, int flag);
