#include <sys/sbunix.h>
#include <sys/stdio.h> //kernel should not include user header files
#include <sys/stdlib.h>//kernel should not include user header files
#include <sys/process.h>
//#include <sys/physical.h>
#include <sys/virmm.h>
#include <sys/tarfs.h>
#include <sys/elf.h>
#include <sys/string.h>



void load_elf(task_struct* task, void* file_start)
{

	
	int i;

	elf_h *elfh=(elf_h*)file_start;//find elf header
	//dprintf("LOAD_ELF: elf header \n");
	//dprintf("LOAD_ELF: \n");
	pgm_h *pgh=(pgm_h*)((uint64_t)elfh+elfh->e_phoff);// find program header

	task->rip=elfh->e_entry;//assign entry for the task

	for(i=0;i<elfh->e_phnum;i++)//go through all the program headers
	{
		if(pgh->p_type==PT_LOAD)//if the program header is loadable
		{
			//warning: didn't do the alignment, maybe a bug in future
			if(pgh->p_flag&PH_TYPE_X)//then its the .text section
			{
				task->mm->start_code=pgh->p_vaddr;
				task->mm->end_code=pgh->p_vaddr+pgh->p_memsz;
			}
			else//its the .data section
			{
				task->mm->bss=pgh->p_memsz-pgh->p_filesz;
				task->mm->start_data=pgh->p_vaddr;
				task->mm->end_data=pgh->p_vaddr+pgh->p_filesz;
			}



		}
		pgh++;
	}


	//task->mm->start_code=(uint64_t)(task->mm->start_code/PAGE_SIZE)*PAGE_SIZE;
	//task->mm->end_code=(uint64_t)(task->mm->end_code/PAGE_SIZE)*PAGE_SIZE;
	

	/*//then do mapping for virtual address of code sec
	uint64_t start_add=(new_mm->start_code/PAGE_SIZE)*PAGE_SIZE+PAGE_SIZE;
	uint64_t end_add=(new_mm->end_code/PAGE_SIZE)*PAGE_SIZE;
	uint64_t add_size=start_add-end_add;
	uint64_t add_pgnum=add_size/PAGE_SIZE;*/

	/*while(add_pgnum!=0)
	{
		page_sp* new_page=alloc_page();
		//map(start_add, new_page);
		start_add+=PAGE_SIZE;
		add_pgnum--；
	}

	return new_task;*/

}





void test_elf()
{
	//printf("lalala");
	int i;
	void* file_start=find_file("bin/hello");
	elf_h *elfh=(elf_h*)(file_start);
	printf("ELF TESTING: file_start: %x\n", file_start);
	printf("ELF TESTING: type: %x\n", elfh->e_type);
	printf("ELF TESTING: machine: %x\n", elfh->e_machine);
	printf("ELF TESTING: version: %x\n", elfh->e_type);
	printf("ELF TESTING: entry: %x\n", elfh->e_entry);
	printf("ELF TESTING: phoff: %x\n", elfh->e_phoff);
	printf("ELF TESTING: shoff: %x\n", elfh->e_shoff);
	printf("ELF TESTING: flags: %x\n", elfh->e_flags);
	printf("ELF TESTING: ehsize: %x\n", elfh->e_ehsize);
	printf("ELF TESTING: phentsize: %x\n", elfh->e_phentsize);
	printf("ELF TESTING: shnum: %x\n", elfh->e_shnum);
	printf("ELF TESTING: shstrndx: %x\n", elfh->e_shstrndx);

	pgm_h *pgh=(pgm_h*)((uint64_t)elfh+elfh->e_phoff);

	for (i = 0; i < elfh->e_phnum; i++) {
		printf("PHDR[%d]: ", i);

		printf("type: %x ", pgh[i].p_type);
		printf("flags: %x ", pgh[i].p_flag);
		printf("offset: %x ", pgh[i].p_offset);
		printf("vaddr: %x ", pgh[i].p_vaddr);
		printf("paddr: %x ", pgh[i].p_paddr);
		printf("filesz: %x ", pgh[i].p_filesz);
		printf("memsz: %x ", pgh[i].p_memsz);
		printf("align: %x ", pgh[i].p_align);
		printf("\n");
	}


    //void* file_start=find_file("bin/hello");
	task_struct* new_task=kmalloc(TASK);
	mm_struct* new_mm=kmalloc(MM);
	new_task->mm=new_mm;
	printf("ELF TESTING\n");
	load_elf(new_task, file_start);

	printf("ELF TESTING: code_start: %x\n", new_task->mm->start_code);
	printf("ELF TESTING: code_end: %x\n", new_task->mm->end_code);
	printf("ELF TESTING: data_start: %x\n", new_task->mm->start_data);
	printf("ELF TESTING: data_end: %x\n", new_task->mm->end_data);
	printf("ELF TESTING: bss: %x\n", new_task->mm->bss);
	

}