#include <sys/virmm.h>
#include <sys/sbunix.h>
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

//get table entry virtual address
uint64_t get_entry_viraddr(uint64_t entry){
	 return (0xFFFFFFFF80000000UL | entry );
}

/* allocate page directory pointer table entry */
void* pdpt_alloc(struct PML4 *pml4, uint64_t pml4_indx)
{
	struct PDPT *pdpt = (struct PDPT *)allocate_page();
	uint64_t pdpt_entry = (uint64_t)pdpt;
	pdpt_entry |= (PTE_P|PTE_W|PTE_U);
	pdpt_entry &= PTE_EX;// clear executable bit
	pml4->PML4E[pml4_indx] = pdpt_entry;

	return (void *)pdpt;
}

/* allocate page directory entry */
void* pdt_alloc(struct PDPT *pdpt, uint64_t pdpt_indx)
{
	struct PDT *pdt = (struct PDT*)allocate_page();
        uint64_t pdt_entry = (uint64_t)pdt;
        pdt_entry |= (PTE_P|PTE_W|PTE_U);
        pdt_entry&= PTE_EX;// clear executable bit
        pdpt->PDPTE[pdpt_indx] = pdt_entry;

	return (void *)pdt;
}

/* allocate pt entry */
void* pt_alloc(struct PDT *pdt, uint64_t pdt_indx)
{
	struct PT *pt = (struct PT *)allocate_page();
        uint64_t pt_entry = (uint64_t)pt;
        pt_entry |= (PTE_P|PTE_W|PTE_U);
        pt_entry &= PTE_EX;// clear executable bit
        pdt->PDTE[pdt_indx] = pt_entry;

	return (void *)pt;
}


// get PT entry addr from PML4 given virtual address
void* get_pt_addr_from_PML4(struct PML4 *pml4,uint64_t vir_addr) {
	struct PDPT	*pdpt;
	struct PDT	*pdt;
	struct PT	*pt;

	uint64_t pml4e_index = get_pml4e_index(vir_addr);
	uint64_t pdpte_index = get_pdpte_index(vir_addr);
	uint64_t pdte_index = get_pdte_index(vir_addr);
//	uint64_t pte_index = get_pte_index(vir_addr);


	uint64_t pml4e = pml4->PML4E[pml4e_index];

	if(pml4e & PTE_P){
		uint64_t pdpt64=get_entry_viraddr(pml4e);
		pdpt64 &= CLEAR_OFFSET;
		pdpt = (struct PDPT *)pdpt64;

	}
	else
		pdpt = (struct PDPT*)pdpt_alloc(pml4, pml4e_index);

	uint64_t pdpte = pdpt->PDPTE[pdpte_index];
	if(pdpte & PTE_P){
		uint64_t pdt64=get_entry_viraddr(pdpte);
		pdt64 &=CLEAR_OFFSET;
		pdt = (struct PDT*)pdt64;

	}
	else
		pdt = (struct PDT*)pdt_alloc(pdpt, pdpte_index);

	uint64_t pdte = pdt->PDTE[pdte_index];
	if(pdte & PTE_P){
		uint64_t pt64=get_entry_viraddr(pdte);
		pt64 &=CLEAR_OFFSET;
        pt = (struct PT*)pt64;

	}
    else
		pt = (struct PT*)pt_alloc(pdt, pdte_index);


	return (void *) pt;
}





struct PML4 *pml4;

void init_pagetables() {

	//setup each level addr pointer
	pml4 = (struct PML4 *)allocate_page();
}

void map_virmem_to_phymem(uint64_t vir_addr,uint64_t phy_addr){

	struct PT* pt;


	pt=(struct PT*) get_pt_addr_from_PML4(pml4, vir_addr);

	uint64_t pte = phy_addr;
	pte |= (PTE_P|PTE_W|PTE_U);
	pte &= PTE_EX;// clear executable bit
	uint64_t pte_index=get_pte_index(vir_addr);
	pt->PTE[pte_index] = pte;


}

//begin mapping physical memory from 0 to 10MB

uint64_t initial_mapping(){
	uint64_t map_size=0xA00000;//physical size 10MB

	uint64_t vir_addr = 0xFFFFFFFF80000000UL;
	uint64_t phy_addr = 0x0;
	uint64_t page_count=0;
	while(phy_addr < map_size){
		map_virmem_to_phymem(vir_addr, phy_addr);
		phy_addr += PAGE_SIZE;
		vir_addr += PAGE_SIZE;
		page_count++;
	}

	return page_count;
}

void set_CR3(struct PML4 *pml4)
{
	uint64_t base_pgdir_addr = (uint64_t)pml4;
	__asm volatile("mov %0, %%cr3":: "b"(base_pgdir_addr));
}

void load_CR3(){
	printf("pml4 %x",pml4);
	set_CR3(pml4);
}


