#include <sys/virmm.h>
#include <sys/process.h>
#include <sys/sbunix.h>
#include <sys/stdlib.h>

uint64_t
get_pml4e_index (uint64_t addr)
{
  return (((addr) >> PML4_SHIFT_BITS) & SHIFT_MASK);
}

uint64_t
get_pdpte_index (uint64_t addr)
{
  return (((addr) >> PDPT_SHIFT_BITS) & SHIFT_MASK);
}

uint64_t
get_pdte_index (uint64_t addr)
{
  return (((addr) >> PDT_SHIFT_BITS) & SHIFT_MASK);
}

uint64_t
get_pte_index (uint64_t addr)
{
  return (((addr) >> PT_SHIFT_BITS) & SHIFT_MASK);
}

//get table entry virtual address
uint64_t
get_entry_viraddr (uint64_t entry)
{
  return (0xFFFFFFFF80000000UL | entry);
}

// set up page directory pointer table
void*
set_pdpt (pml4_t pml4, uint64_t pml4_index)
{
  pdpt_t pdpt = (pdpt_t) allocate_page ();
  uint64_t pdpt_entry = (uint64_t) pdpt;
  pdpt_entry |= (PTE_P | PTE_W);
  //pdpt_entry &= PTE_EX; // clear executable bit
  pml4->PML4E[pml4_index] = pdpt_entry;

  return (void *) pdpt;
}

// set up page directory table
void*
set_pdt (pdpt_t pdpt, uint64_t pdpt_index)
{
  pdt_t pdt = (pdt_t) allocate_page ();
  uint64_t pdt_entry = (uint64_t) pdt;
  pdt_entry |= (PTE_P | PTE_W);
  //pdt_entry &= PTE_EX; // clear executable bit
  pdpt->PDPTE[pdpt_index] = pdt_entry;

  return (void *) pdt;
}

// set up page table
void*
set_pt (pdt_t pdt, uint64_t pdt_index)
{
  pt_t pt = (pt_t) allocate_page ();
  uint64_t pt_entry = (uint64_t) pt;
  pt_entry |= (PTE_P | PTE_W);
  //pt_entry &= PTE_EX; // clear executable bit
  pdt->PDTE[pdt_index] = pt_entry;

  return (void *) pt;
}

pml4_t global_PML4;
uint64_t ktask_base;
uint64_t kstack_base;
int task_bitmap[PROCESS_NUMBER];
int stack_bitmap[KSTACK_NUMBER];

void
init_mm ()
{

  //setup level 4 page directory
  global_PML4 = (pml4_t) allocate_page ();
  ktask_base = get_kmalloc_base () + VIR_START;
  kstack_base = ktask_base + PROCESS_NUMBER * 0x1000;

  dprintf ("ktask_base : %p\n", ktask_base);
  dprintf ("kstack_base : %p\n", kstack_base);
  memset ((void *) ktask_base, 0, PROCESS_NUMBER * 0x1000);
  memset ((void *) kstack_base, 0, KSTACK_NUMBER * 0x1000);

  memset (task_bitmap, 0, PROCESS_NUMBER);
  memset (stack_bitmap, 0, KSTACK_NUMBER);

//  int i = 0;
//  for (i = 0; i < PROCESS_NUMBER; i++)
//    dprintf ("task_bimap[%d] is: %d ", i, task_bitmap[i]);
//  for (i = 0; i < KSTACK_NUMBER; i++)
//    dprintf ("stack_bitmap[%d] is: %d ", i, task_bitmap[i]);

}

void
map_virmem_to_phymem (uint64_t vir_addr, uint64_t phy_addr)
{

  pdpt_t pdpt;
  pdt_t pdt;
  pt_t pt;

  uint64_t pml4e_index = get_pml4e_index (vir_addr);

  uint64_t pml4e = global_PML4->PML4E[pml4e_index];

  if (pml4e & PTE_P)
    {
      uint64_t pdpt64 = get_entry_viraddr (pml4e);
      pdpt64 &= CLEAR_OFFSET;
      pdpt = (pdpt_t) pdpt64;

    }
  else
    {
      pdpt = (pdpt_t) set_pdpt (global_PML4, pml4e_index);
    }

  uint64_t pdpte_index = get_pdpte_index (vir_addr);
  uint64_t pdpte = pdpt->PDPTE[pdpte_index];
  if (pdpte & PTE_P)
    {
      uint64_t pdt64 = get_entry_viraddr (pdpte);
      pdt64 &= CLEAR_OFFSET;
      pdt = (pdt_t) pdt64;

    }
  else
    {
      pdt = (pdt_t) set_pdt (pdpt, pdpte_index);
    }

  uint64_t pdte_index = get_pdte_index (vir_addr);
  uint64_t pdte = pdt->PDTE[pdte_index];
  if (pdte & PTE_P)
    {
      uint64_t pt64 = get_entry_viraddr (pdte);
      pt64 &= CLEAR_OFFSET;
      pt = (pt_t) pt64;

    }
  else
    {
      pt = (pt_t) set_pt (pdt, pdte_index);
    }

  uint64_t pte = phy_addr;
  pte |= (PTE_P | PTE_W);
  //pte &= PTE_EX; // clear executable bit
  uint64_t pte_index = get_pte_index (vir_addr);
  pt->PTE[pte_index] = pte;

}

//begin mapping physical memory from 0 to 10MB

void
initial_mapping ()
{
  uint64_t map_size = 0x2000000; //physical size 32MB

  uint64_t vir_addr = VIR_START;
  uint64_t phy_addr = PHY_START;
  uint64_t page_count = 0;
  while (phy_addr < map_size)
    {
      map_virmem_to_phymem (vir_addr, phy_addr);
      phy_addr += PAGE_SIZE;
      vir_addr += PAGE_SIZE;
      page_count++;
    }

  set_CR3 ((uint64_t) global_PML4);
}

void
set_CR3 (uint64_t CR3)
{
  __asm volatile("mov %0, %%cr3":: "b"(CR3));
}

uint64_t
get_CR3 ()
{
  uint64_t cur_cr3;
  __asm volatile("mov %%cr3, %0" : "=r" (cur_cr3));

  return cur_cr3;
}

uint64_t
get_base (int flag)
{
  if (flag)
    return ktask_base;
  else
    return kstack_base;
}

/* allocate 4K size usable kernel virtual memory
 * flag,1 indicates task_struct or
 *  	0 for kernel_stack
 */

void*
kmalloc (int flag)
{
  int i = 0;
  uint64_t base = get_base (flag);
  if (flag)
    {
      while (i < PROCESS_NUMBER)
	{
	  if (task_bitmap[i] == 0)
	    {
	      task_bitmap[i] = 1;
	      break;
	    }
	  else
	    i++;
	}
    }
  else
    {
      while (i < KSTACK_NUMBER)
	{
	  if (stack_bitmap[i] == 0)
	    {
	      stack_bitmap[i] = 1;
	      break;
	    }
	  else
	    i++;
	}
    }

  base += i * 0x1000;
  dprintf ("kmalloc return base %p\n", base);

  memset ((void *) base, 0, 0x1000);

  return (void *) (base);

}

void
kfree (void* addr, int flag)
{
  memset ((void *) addr, 0, 0x1000);
  uint64_t base = get_base (flag);
  int bitmap_pos = ((uint64_t) addr - base) / 0x1000;
  if (flag)
    task_bitmap[bitmap_pos] = 0;
  else
    stack_bitmap[bitmap_pos] = 0;
}

