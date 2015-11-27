#include <sys/sbunix.h>
#include <sys/stdlib.h>//kernel should not include user header files
#include <sys/process.h>
//#include <sys/physical.h>
#include <sys/virmm.h>
#include <sys/tarfs.h>
#include <sys/elf.h>
#include <sys/string.h>

void *memmove(void *dest, const void *src, size_t n) {
	const char *s;
	char *d;

	s = src;
	d = dest;
	if (s < d && s + n > d) {
		s += n;
		d += n;
		while (n-- > 0)
			*--d = *--s;
	} else {
		while (n-- > 0)
			*d++ = *s++;
	}

	return dest;
}

int load_elf(task_struct* task, char* bin_name) {


	int i;
	int m=0;

	struct file* file = tarfs_open(bin_name, O_RDONLY);


	char* dir = strstr(bin_name, "/");
	size_t length = strlen(dir);
	size_t length1 = strlen(bin_name);
	strncpy(task->cur_dir, bin_name, length1-length+1);


	void* file_start = (void*) file->start;

	elf_h *elfh = (elf_h*) file_start; //find elf header
	//dprintf("LOAD_ELF: elf header \n");
	//dprintf("LOAD_ELF: \n");
	pgm_h *pgh = (pgm_h*) ((uint64_t) elfh + elfh->e_phoff);// find program header

	task->rip = elfh->e_entry;	//assign entry for the task

	//get the cur dir for the task


	for (i = 0; i < elfh->e_phnum; i++)	//go through all the program headers
			{
		if (pgh->p_type == PT_LOAD)	//if the program header is loadable
				{
			//warning: didn't do the alignment, maybe a bug in future
			if (pgh->p_flag & PH_TYPE_X)	//then its the .text section
					{
				task->mm->start_code = pgh->p_vaddr;
				task->mm->end_code = pgh->p_vaddr + pgh->p_memsz;
				//map code/text segment
				uint64_t code_size = task->mm->end_code - task->mm->start_code;
				umalloc((void*) task->mm->start_code, code_size);

				memmove((void*) task->mm->start_code,
						(void*) file_start + pgh->p_offset, pgh->p_filesz);

				struct vma_struct* vma_tmp = get_vma(task->mm, CODE);
				vma_tmp->vm_file = file;
				vma_tmp->file_offset = pgh->p_offset;
				m++;

			} else	//its the .data section
			{

				task->mm->start_data = pgh->p_vaddr;
				task->mm->end_data = pgh->p_vaddr + pgh->p_filesz;
				// map data segment
				uint64_t data_size = task->mm->end_data - task->mm->start_data;
                task->mm->bss = pgh->p_memsz - pgh->p_filesz;
                task->mm->end_data +=task->mm->bss;

				umalloc((void*) task->mm->start_data, data_size+task->mm->bss);

				//map bss
				//umalloc((void*) task->mm->end_data, task->mm->bss);

				memmove((void*) task->mm->start_data,
						(void*) file_start + pgh->p_offset, pgh->p_memsz);
				// WARNING!: not sure whether should memcpy bss into bss's vaddress, may be a bug in future

				struct vma_struct* vma_tmp1 = get_vma(task->mm, DATA);
				vma_tmp1->vm_file = file;
				vma_tmp1->file_offset = pgh->p_offset;
				m++;

			}

		}
		pgh++;
	}
	if(m==0)
	{
		return -1;
	}
	return 0;
}
void test_elf() {
	//printf("lalala");
	int i;
	void* file_start = find_file("bin/hello");
	elf_h *elfh = (elf_h*) (file_start);
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

	pgm_h *pgh = (pgm_h*) ((uint64_t) elfh + elfh->e_phoff);

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
	task_struct* new_task = kmalloc(TASK);
	mm_struct* new_mm = kmalloc(MM);
	new_task->mm = new_mm;
	printf("ELF TESTING\n");
	load_elf(new_task, file_start);

	printf("ELF TESTING: code_start: %x\n", new_task->mm->start_code);
	printf("ELF TESTING: code_end: %x\n", new_task->mm->end_code);
	printf("ELF TESTING: data_start: %x\n", new_task->mm->start_data);
	printf("ELF TESTING: data_end: %x\n", new_task->mm->end_data);
	printf("ELF TESTING: bss: %x\n", new_task->mm->bss);

}
