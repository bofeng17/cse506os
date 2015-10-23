#include <sys/sbunix.h>
#include <sys/gdt.h>
#include <sys/idt.h>
#include <sys/irq.h>
#include <sys/tarfs.h>
#include <sys/physical.h>

//extern uint32_t page_index;
//extern page_sp* page_struct_begin;
extern char kernmem, physbase;
uint32_t page_index=0;
uint32_t page_num=0;
uint64_t length=0;
uint32_t first=0;
extern page_sp* page_struct_start;

void start(uint32_t* modulep, void* physbase, void* physfree)
{
	page_sp* page_tmp;
	struct smap_t {
		uint64_t base, length;
		uint32_t type;
	}__attribute__((packed)) *smap;
	while(modulep[0] != 0x9001) modulep += modulep[1]+2;
	for(smap = (struct smap_t*)(modulep+2); smap < (struct smap_t*)((char*)modulep+modulep[1]+2*4); ++smap) {
		if (smap->type == 1 /* memory */ && smap->length != 0) {
		    length= smap->length;
		    page_num=length>>12;
	        page_index=(uint32_t)((smap->base)>>12);
			printf("Available Physical Memory [%x-%x]\n", smap->base, smap->base + smap->length);
		}
	}
	//printf("length: %x\n",length);
	//page_num=length>>12;
	//page_index=(uint32_t)((smap->base)>>12);
    //page_index=smap->base>>12;
    //page_struct_begin=(page_sp*)((uint64_t)(&kernmem) + (uint64_t) physfree);

    

	printf("tarfs in [%p:%p]\n", &_binary_tarfs_start, &_binary_tarfs_end);
	printf("page_num=%x\n",page_num);
	printf("page index=%x\n",page_index);
    init_phy_page(100,page_num,page_index);
    page_tmp=(page_sp*)(page_struct_start)+100;

    printf("see: %x\n",page_tmp->index);

    first=allocate_page();
    printf("first free is %x\n",first);


	// kernel starts here
	//debug
}

#define INITIAL_STACK_SIZE 4096
char stack[INITIAL_STACK_SIZE];
uint32_t* loader_stack;
//extern char kernmem, physbase;
struct tss_t tss;

void boot(void)
{
	// note: function changes rsp, local stack variables can't be practically used
	__asm__(
		"movq %%rsp, %0;"
		"movq %1, %%rsp;"
		:"=g"(loader_stack)
		:"r"(&stack[INITIAL_STACK_SIZE])
	);
	reload_gdt();
	setup_tss();
	reload_idt();
	setup_idt();
	start(
		(uint32_t*)((char*)(uint64_t)loader_stack[3] + (uint64_t)&kernmem - (uint64_t)&physbase),
		&physbase,
		(void*)(uint64_t)loader_stack[4]
	);
	printf("!!!!! start() returned !!!!!\n");
	while(1);
}
