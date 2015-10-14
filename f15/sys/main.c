#include <sys/sbunix.h>
#include <sys/gdt.h>
#include <sys/idt.h>
#include <sys/tarfs.h>

size_t console_row;
size_t console_column;

void start(uint32_t* modulep, void* physbase, void* physfree)
{
	struct smap_t {
		uint64_t base, length;
		uint32_t type;
	}__attribute__((packed)) *smap;
	while(modulep[0] != 0x9001) modulep += modulep[1]+2;
	for(smap = (struct smap_t*)(modulep+2); smap < (struct smap_t*)((char*)modulep+modulep[1]+2*4); ++smap) {
		if (smap->type == 1 /* memory */ && smap->length != 0) {
			printf("Available Physical Memory [%x-%x]\n", smap->base, smap->base + smap->length);
		}
	}
	printf("tarfs in [%p:%p]\n", &_binary_tarfs_start, &_binary_tarfs_end);

	// kernel starts here
	while(1);//start function should never return
}

#define INITIAL_STACK_SIZE 4096
char stack[INITIAL_STACK_SIZE];//stakc used by boot
uint32_t* loader_stack;//points to top of the OS loader stack. Seems loader is 32-bit instead of 64-bit
extern char kernmem, physbase;//only symbols-only declared here. Defined in linker script
struct tss_t tss;


void boot(void)
{
	// note: function changes rsp, local stack variables can't be practically used
	register char *s, *v;
	__asm__(
		"movq %%rsp, %0;"//loader_stack points to old rsp, namely, to the top of OS loader stack 
		"movq %1, %%rsp;"//now rsp points to the top of stack[INITIAL_STACK_SIZE];
		:"=g"(loader_stack)
		:"r"(&stack[INITIAL_STACK_SIZE])
	);
    
    reload_gdt();
	setup_tss();

	console_initialize();
    
	reload_idt();
    timer_init();
    
	start(
		(uint32_t*)((char*)(uint64_t)loader_stack[3] + (uint64_t)&kernmem - (uint64_t)&physbase),
		&physbase,
		(void*)(uint64_t)loader_stack[4]
	);

	s = "!!!!! start() returned !!!!!";	
	for(v = (char*)0xb8000; *s; ++s, v += 2) *v = *s;

	while(1);
}
