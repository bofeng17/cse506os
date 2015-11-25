#include <sys/sbunix.h>
#include <sys/stdio.h> //kernel should not include user header files
#include <sys/stdlib.h>//kernel should not include user header files
#include <sys/tarfs.h>
#include <sys/elf.h>
#include <sys/process.h>
#include <sys/virmm.h>
#include <sys/string.h>
#include <sys/gdt.h>
#include <sys/syscall.h>

task_struct* front;
task_struct* end;
task_struct* current;

int track_task[PROCESS_NUMBER];

int count_args(char ** args) {
	if (args == NULL)
		return 0;
	int i = 0;

	while (args[i++] != NULL)
		;
	return i - 1;

}

void init_process_id() {
	int i;
	for (i = 0; i < PROCESS_NUMBER; i++) {
		track_task[i] = 0;
	}
}

int assign_pid() {
	int i;
	for (i = 0; i < PROCESS_NUMBER; i++) {
		if (track_task[i] == 0) {
			track_task[i] = 1;
			return i;
		}
	}

	return -1;
}

//insert new task to run queue
void add_task(task_struct * task) {
	end->next = task;
	end = task;
	end->next = front;
}

//only set task_struct vma_struct and mm_struct
void set_task_struct(task_struct* task) {
	mm_struct* mstruct = kmalloc(MM);

	vma_struct* vma_code = kmalloc(VMA);
	vma_struct* vma_data = kmalloc(VMA);
	vma_struct* vma_stack = kmalloc(VMA);
	vma_struct* vma_heap = kmalloc(VMA);

	vma_code->vm_mm = mstruct;
	vma_data->vm_mm = mstruct;
	vma_stack->vm_mm = mstruct;
	vma_heap->vm_mm = mstruct;

	vma_code->vm_next = vma_data;

	vma_data->vm_next = vma_heap;

	vma_heap->vm_next = vma_stack;

	vma_stack->vm_next = NULL;

	mstruct->mmap = vma_code;
	task->mm = mstruct;
}

// use flag as one of CODE, DATA, HEAP, STACK
vma_struct* get_vma(mm_struct* mm, int flag) {
	vma_struct * ret_vma = mm->mmap;

	while (flag-- > 0) {
		ret_vma = ret_vma->vm_next;
	}

	return ret_vma;
}

void setup_vma(mm_struct* mstruct) {

	vma_struct* vma_code = get_vma(mstruct, CODE);
	vma_code->vm_start = mstruct->start_code;
	vma_code->vm_end = mstruct->end_code;
	vma_code->permission_flag = VM_READ | VM_EXEC;

	vma_struct* vma_data = get_vma(mstruct, DATA);
	vma_data->vm_start = mstruct->start_data;
	vma_data->vm_end = mstruct->end_data;
	vma_data->permission_flag = VM_READ | VM_WRITE;

	vma_struct* vma_heap = get_vma(mstruct, HEAP);
	vma_heap->vm_start = mstruct->brk;
	vma_heap->permission_flag = VM_READ | VM_WRITE;

	vma_struct* vma_stack = get_vma(mstruct, STACK);
	vma_stack->vm_start = mstruct->start_stack;
	vma_stack->vm_end = STACK_TOP;
	vma_stack->permission_flag = VM_READ | VM_WRITE;
}
//void
//function_idle ()
//{
//  int i = 0;
//  while (1)
//    {
//      dprintf ("I'm idle %d \n", i++);
//      schedule ();
////      __asm__ __volatile__ ("hlt");
//    }
//}

task_struct *
create_idle_thread() {
	task_struct * idle = (task_struct*) (kmalloc(TASK));

	idle->ppid = 0;
	idle->pid = assign_pid();
	idle->kernel_stack = idle->init_kern = (uint64_t) kmalloc(KSTACK); //what is init_kern
	//idle->rip = (uint64_t) & function_idle; //idle will call schedule function
	idle->task_state = TASK_READY;
	idle->sleep_time = 0;
	idle->cr3 = get_CR3();
	strcpy(idle->task_name, "idle thread");
	idle->mm = NULL;
	idle->wait_pid = 0;

	front = idle;
	end = idle;
	end->next = front;
	current = idle;

	return idle;
}

void func_init() {
	char* argv[3] = { "a1", "a2", NULL };

	char* envp[4] = { "e1", "e2", "e3", NULL };

	do_execv("bin/test_hello", argv, envp);

	sysret_to_ring3();

	//do_exit (0);
}

task_struct*
create_thread_init() {

	task_struct * new_task = (task_struct*) (kmalloc(TASK));

	new_task->ppid = 0;
	new_task->pid = assign_pid();
	new_task->kernel_stack = new_task->init_kern = (uint64_t) kmalloc(KSTACK); //what is init_kern
	new_task->rip = (uint64_t) & func_init;
	new_task->task_state = TASK_NEW;
	new_task->sleep_time = 0;
	new_task->cr3 = get_CR3();
	strcpy(new_task->task_name, "init");

	set_task_struct(new_task);

	new_task->wait_pid = 0;

	add_task(new_task);

	return new_task;

}

task_struct*
create_thread(uint64_t thread, char * thread_name) {

	task_struct * new_task = (task_struct*) (kmalloc(TASK));

	new_task->ppid = 0;
	new_task->pid = assign_pid();
	new_task->kernel_stack = new_task->init_kern = (uint64_t) kmalloc(KSTACK); //what is init_kern
	new_task->rip = thread;
	new_task->task_state = TASK_NEW;
	new_task->sleep_time = 0;
	new_task->cr3 = get_CR3();
	strcpy(new_task->task_name, thread_name);
	new_task->mm = NULL;
	new_task->wait_pid = 0;

	add_task(new_task);

	return new_task;

}

//prev stored in %rdi (because it is the first arg in function call)
//next sotred in %rsi
void context_switch(task_struct *prev, task_struct *next) {

	__asm__ __volatile__ (
			"pushq	%rax;"
			"pushq	%rbx;"
			"pushq	%rcx;"
			"pushq	%rdx;"
			"pushq	%rsi;"
			"pushq	%rdi;"
			"pushq	%rbp;"
			"pushq	%rsp;"
			"pushq	%r8;"
			"pushq	%r9;"
			"pushq	%r10;"
			"pushq	%r11;"
			"pushq	%r12;"
			"pushq	%r13;"
			"pushq	%r14;"
			"pushq	%r15;"
			"pushfq;");
	//Is it OK to switch cr3 first?
	/* move the current process page table base address to cr3 register */

	__asm__ __volatile__ (
			"movq %0, %%cr3;"
			::"r"(next->cr3)
	);

	/* save the rsp pointer of the swapped process */
	__asm__ __volatile__ (
			"movq %%rsp, %0"
			:"=r"(prev->kernel_stack)
	);

	/* move the stack pointer of current process to rsp register */
	__asm__ __volatile__ (
			"movq %0, %%rsp;"
			::"r"(next->kernel_stack)
	);

	//TODO: tss.rsp0 = next->init_kern;
	/* set the kernel stack */
	//Ref. <Understanding the Linux kernel>: page 108 step 3
	/*
	 * save the instruction pointer for swapped process
	 * and jump to next process
	 */
	__asm__ __volatile__ (
			"movq $1f, %0;"
			"sti;" //enable CPU interrupt, which is disabled by timer. Is it proper to do here?
			"jmp %1;"
			"1:\t"
			:"=g"(prev->rip)
			:"r"(next->rip)
	);

	__asm__ __volatile__ (
			"popfq;"
			"popq	%r15;"
			"popq	%r14;"
			"popq	%r13;"
			"popq	%r12;"
			"popq	%r11;"
			"popq	%r10;"
			"popq	%r9;"
			"popq	%r8;"
			"popq	%rsp;"
			"popq	%rbp;"
			"popq	%rdi;"
			"popq	%rsi;"
			"popq	%rdx;"
			"popq	%rcx;"
			"popq	%rbx;"
			"popq	%rax;");
	//TODO: movl %eax, last
	//Here we should
	//Ref. <Understanding the Linux kernel>: page 108 step 9

}

#define ARGV_PARAMS 1
#define ENVP_PARAMS 0
// set params (argv, envp) to the top area of user stack, return params number
int set_params_to_stack(uint64_t* rsp_p, char *** params_p, int flag) {
	int i = 0;
	int j = 0;
	int params_no = 0;
	uint64_t rsp64 = *rsp_p;
	void* rsp = (void*) rsp64;
	char** params = *params_p;
	if (params != NULL) {
		params_no = count_args(params);
		dprintf("param_no is %d\n", params_no);
		i = params_no - 1;
		while (params[i] != NULL) {
			dprintf("%s\n", params[i]);

			rsp = (char*) rsp - strlen(params[i]);

			j = 0;
			while (params[i][j] != '\0') {
				*((char*) rsp++) = params[i][j++];
				//printf("%c\n",*((char*)tmp+1) );
				//printf("%c\n",envp[i][j-1] );
			}
			rsp = (char*) rsp - strlen(params[i]);

			if (flag == ENVP_PARAMS) {
				params[i] = (char*) rsp;
			}

			if (flag == ARGV_PARAMS) {
				params[i + 1] = (char*) rsp;
			}
			rsp = (char*) rsp - 1;
			*((char*) rsp) = '\0';

			//dprintf ("tmp is %p", tmp);
			i--;
			//printf("%s\n",(char*)tmp+1 );
		}

	}
	*rsp_p = (uint64_t) rsp;
	return params_no;
}

int do_execv(char* bin_name, char ** argv, char** envp) {
	int retval = 0;
	// create new task
	task_struct* execv_task = current;

	int argc = 0;
	int envc = 0;
	char* argv_0 = bin_name;

	set_task_struct(execv_task);

	// load bin_name elf
	struct file* file = tarfs_open(bin_name);

	load_elf(execv_task, file); // -1 if error

	setup_vma(execv_task->mm);

	//allocate heap
	uint64_t initial_heap_size = 10 * PAGE_SIZE;
	execv_task->mm->start_brk = (uint64_t) umalloc(
			(void*) execv_task->mm->end_data, initial_heap_size);
	execv_task->mm->brk = execv_task->mm->start_brk + initial_heap_size;

	execv_task->mm->start_stack = (uint64_t) umap((void*) STACK_TOP, PAGE_SIZE);
	umap((void*) (STACK_TOP - PAGE_SIZE), PAGE_SIZE);

	// setup new task user stack, rsp, argv, envp
	void* rsp = (void*) (STACK_TOP);

	void* tmp = rsp - 1;

	uint64_t tmp2 = (uint64_t) tmp;

	// save envp string to the top area of user stack
	envc = set_params_to_stack(&tmp2, &envp, ENVP_PARAMS);

	// save argv string to the top area of user stack
	argc = set_params_to_stack(&tmp2, &argv, ARGV_PARAMS);

	tmp = (void*) tmp2;
	//copy argv_0 (binary name) and set argv0 pointer
	tmp = (char*) tmp - strlen(argv_0);
	int j = 0;
	while (argv_0[j] != '\0') {
		*((char*) tmp++) = argv_0[j++];
	}
	tmp = (char*) tmp - strlen(argv_0);
	argv[0] = (char*) tmp;

	argc += 1;

	// align last byte
	tmp = (void *) ((uint64_t) tmp & 0xfffffff8);

	// set null pointer between string area and envp
	tmp -= 8;	      // uint64_t is 8 bytes
	memset(tmp, 0, 8);

	// store envp pointers in the proper place of user stack
	if (envc > 0) {
		execv_task->mm->env_end = (uint64_t) envp[envc - 1];
		while (envc-- > 0) {
			// envp[envc] = (char*) tmp--;
			*((uint64_t*) tmp) = (uint64_t) envp[envc];
			tmp = (uint64_t*) tmp - 1;
		}
		execv_task->mm->env_start = (uint64_t) envp[0];
		// set 0 between envp and argv
		memset(tmp, 0, 8);
		tmp -= 8; // uint64_t is 8 bytes

	}

	// store argv pointers in the proper place of user stack
	if (argc > 0) {

		execv_task->mm->arg_end = (uint64_t) argv[argc - 1];

		int argc2 = argc - 1;
		while (argc2-- > 0) {
			//argv[argc2] = (char*) tmp--;
			*((uint64_t*) tmp) = (uint64_t) argv[argc2];
			tmp = (uint64_t*) tmp - 1;
		}
		execv_task->mm->arg_start = (uint64_t) argv[0];
	}

	*((uint64_t*) tmp) = argc;
	rsp = tmp;

	execv_task->rsp = (uint64_t) rsp;
	// switch current stack to user stack of created task(execv_task)

	// add execv_task to the run queue
	add_task(execv_task);

	return retval;
}

task_struct*
create_user_process(char* bin_name) {
	task_struct * new_task = (task_struct*) (kmalloc(TASK));

	new_task->ppid = 0;
	new_task->pid = assign_pid();
	new_task->kernel_stack = new_task->init_kern = (uint64_t) kmalloc(KSTACK); //what is init_kern

	new_task->task_state = TASK_NEW;
	new_task->sleep_time = 0;

	new_task->cr3 = get_CR3();

	strcpy(new_task->task_name, bin_name);
	//do_execv (bin_name, NULL, NULL);

	new_task->wait_pid = 0;

	add_task(new_task);

	return new_task;
}

void set_cow_routine(int flags, uint64_t vaddr) {
	//read the entry back
	uint64_t tmp = self_ref_read(flags, vaddr);
	//set the entry with cow bit
	tmp = (uint64_t)(tmp | PTE_COW | PTE_R);
	//write the entry back
	self_ref_write(flags, vaddr, tmp);
}

void set_cow() {
	//calculate the code , data, stack, bss size
	uint64_t i;
	uint64_t code_size = current->mm->end_code - current->mm->start_code;
	uint64_t code_start = current->mm->start_code;
	uint64_t data_size = current->mm->end_data - current->mm->start_data;
	uint64_t data_start = current->mm->start_data;
	uint64_t stack_size = STACK_TOP - current->mm->start_stack;
	uint64_t stack_start = current->mm->start_stack;
	uint64_t bss_size = current->mm->bss;
	uint64_t bss_start = current->mm->end_data;

	//set cow bit for PML4, PDPT of code, 
	//PML4 and PDPT should able able to cover the length of code
	set_cow_routine(PML4, code_start);
	set_cow_routine(PDPT, code_start);

	//set cow bit for PDT, PT of code, 
	//these two may not able to cover length of code, so we set cow bit for every address
	for (i = 0; i < code_size; i++) {

		set_cow_routine(PDT, code_start);
		set_cow_routine(PT, code_start);

		code_start += 1;
	}

	//set cow bit for PML4, PDPT of data, 
	//PML4 and PDPT should able able to cover the length of data
	set_cow_routine(PML4, data_start);
	set_cow_routine(PDPT, data_start);

	//set cow bit for PDT, PT of data, 
	//these two may not able to cover length of data, so we set cow bit for every address
	for (i = 0; i < data_size; i++) {

		set_cow_routine(PDT, data_start);
		set_cow_routine(PT, data_start);

		data_start += 1;
	}

	//set cow bit for PML4, PDPT of stack, 
	//PML4 and PDPT should able able to cover the length of stack
	set_cow_routine(PML4, stack_start);
	set_cow_routine(PDPT, stack_start);

	//set cow bit for PDT, PT of stack, 
	//these two may not able to cover length of stack, so we set cow bit for every address
	for (i = 0; i < stack_size; i++) {

		set_cow_routine(PDT, stack_start);
		set_cow_routine(PT, stack_start);

		stack_start += 1;
	}

	//set cow bit for PML4, PDPT of heap, 
	//PML4 and PDPT should able able to cover the length of heap
	set_cow_routine(PML4, bss_start);
	set_cow_routine(PDPT, bss_start);

	//set cow bit for PDT, PT of heap, 
	//these two may not able to cover length of heap, so we set cow bit for every address
	for (i = 0; i < bss_size; i++) {
		set_cow_routine(PDT, bss_start);
		set_cow_routine(PT, bss_start);

		bss_start += 1;
	}

}

int do_fork() {
	task_struct * new_task = (task_struct*) (kmalloc(TASK));
	//create a new task struct

	set_task_struct(new_task);
	new_task->pid = assign_pid();
	new_task->ppid = current->pid;
	//assign new pid for child

	//copy contents from parent to child
	strcpy(new_task->task_name, current->task_name);
	memcpy((void*) new_task->mm, (void*) current->mm, 0x1000);
	new_task->cr3 = current->cr3;
	new_task->kernel_stack = (uint64_t) kmalloc(KSTACK);
	//child has it own kernel stack

	new_task->rip = current->rip;
	new_task->mm->start_stack = (uint64_t) umalloc((void*) STACK_TOP,
			PAGE_SIZE);
	//child has it own stack

	new_task->task_state = TASK_READY;

	set_cow();	//set cow bit

	return new_task->pid;
	//just return child's pid

}

void do_exit(int status) {
	// current = current->next;
	current->task_state = TASK_ZOMBIE;
	schedule();
}
