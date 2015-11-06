#include <sys/sbunix.h>
#include <sys/gdt.h>
#include <sys/idt.h>
#include <sys/tarfs.h>
#include <sys/virmm.h>
#include <sys/process.h>
#include <stdlib.h>

#define INITIAL_STACK_SIZE 4096
char stack[INITIAL_STACK_SIZE]; //stakc used by boot
uint32_t* loader_stack; //points to top of the OS loader stack. Seems loader is 32-bit instead of 64-bit
extern char kernmem, physbase; //only symbols-only declared here. Defined in linker script
struct tss_t tss;

size_t console_row;
size_t console_column;
uint32_t page_index = 0;
uint32_t page_num = 0;
uint64_t length = 0;
uint32_t first = 0;
page_sp* page_struct_start;

void
start (uint32_t* modulep, void* physbase, void* physfree)
{

  struct smap_t
  {
    uint64_t base, length;
    uint32_t type;
  }__attribute__((packed)) *smap;
  while (modulep[0] != 0x9001)
    modulep += modulep[1] + 2;
  for (smap = (struct smap_t*) (modulep + 2);
      smap < (struct smap_t*) ((char*) modulep + modulep[1] + 2 * 4); ++smap)
    {
      if (smap->type == 1 /* mem
       ory */&& smap->length != 0)
	{
	  length = smap->length;
	  page_num = length >> 12;
	  page_index = (uint32_t) ((smap->base) >> 12);

	  printf ("smaplength is %x, smapbase is %x\n", smap->length,
		  smap->base);

	  printf ("Available Physical Memory [%x-%x]\n", smap->base,
		  smap->base + smap->length);
	}
    }

  page_struct_start = (page_sp*) (0xffffffff80000000UL + physfree);
//    num_for_init=(((uint64_t) physfree >>12)+256+1);
//    kmalloc_base=(num_for_init<<12);
  //printf("vmalloc_base=%x\n",kmalloc_base);
  printf ("tarfs in [%p:%p]\n", &_binary_tarfs_start, &_binary_tarfs_end);
  dprintf ("page_num=%x\n", page_num);
  dprintf ("page index=%x\n", page_index);
  init_phy_page (get_num_init ((uint64_t) physfree), page_num, page_index);
//  dprintf ("kmalloc base: %x\n", get_kmalloc_base ());

#if DEBUG
  page_sp* page_tmp = (page_sp*) (page_struct_start) + 100;

  dprintf("see: %x\n", page_tmp->index);
#endif

  first = allocate_page ();
  dprintf ("first free is %x\n", first);

  dprintf ("kernmem starts in %p\n", &kernmem);

  init_mm ();  //uint64_t pagecount = initial_mapping();

  initial_mapping ();	// map 32MB physical memory to virtual memory
  load_CR3 ();

  init_phy_page (8192, page_num, page_index); //init first 32mb as used, kmalloc take over

//  int* ttest1 = kmalloc (TASK);
//  dprintf ("sizeof ttest1 is:%d", sizeof(ttest1));
//  dprintf (" ttest1[999] is:%d\n", ttest1[999]);
//
//
//  int* stest1 = kmalloc (KSTACK);
//  dprintf ("sizeof test2 is:%d", sizeof(stest1));
//  dprintf (" stest1[999] is:%d\n", stest1[999]);
//
//  int* ttest2 = kmalloc (TASK);
//  dprintf ("sizeof ttest2 is:%d", sizeof(ttest2));
//  dprintf (" ttest2[999] is:%d\n", ttest2[999]);
//  int* stest2 = kmalloc (KSTACK);
//  dprintf ("sizeof stest2 is:%d", sizeof(stest2));
//  dprintf (" stest2[999] is:%d\n", stest2[999]);

  while (1)
    ;

}

void
boot (void)
{
// note: function changes rsp, local stack variables can't be practically used
//	register char *s, *v;
  __asm__(
      "movq %%rsp, %0;" //loader_stack points to old rsp, namely, to the top of OS loader stack
      "movq %1, %%rsp;"//now rsp points to the top of stack[INITIAL_STACK_SIZE];
      :"=g"(loader_stack)
      :"r"(&stack[INITIAL_STACK_SIZE])
  );

  reload_gdt ();
  setup_tss ();

  console_initialize ();

  reload_idt ();
  timer_init ();

  start (
      (uint32_t*) ((char*) (uint64_t) loader_stack[3] + (uint64_t)
	  & kernmem - (uint64_t) & physbase),
      &physbase, (void*) (uint64_t) loader_stack[4]);

//	s = "!!!!! start() returned !!!!!";
//	for(v = (char*)0xb8000; *s; ++s, v += 2) *v = *s;

  while (1)
    ;
}
