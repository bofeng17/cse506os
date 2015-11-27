#include <sys/sbunix.h>
//#include <sys/stdio.h> //kernel should not include user header files
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
	vma_struct* vma_bss = kmalloc(VMA);
	vma_struct* vma_stack = kmalloc(VMA);
	vma_struct* vma_heap = kmalloc(VMA);

	vma_code->vm_mm = mstruct;
	vma_data->vm_mm = mstruct;
	vma_bss->vm_mm = mstruct;
	vma_heap->vm_mm = mstruct;
	vma_stack->vm_mm = mstruct;

	vma_code->vm_next = vma_data;
	vma_data->vm_next = vma_bss;
	vma_bss->vm_next = vma_heap;
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

	vma_struct* vma_bss = get_vma(mstruct, BSS);
	vma_bss->vm_start = mstruct->end_data;
	vma_bss->vm_end = mstruct->end_data + mstruct->bss;
	vma_bss->permission_flag = VM_READ | VM_WRITE;
<<<<<<< HEAD
=======
	vma_bss->vm_file = NULL;
	vma_bss->file_offset = 0;
>>>>>>> parent of c653a15... add copy_mm func

	vma_struct* vma_heap = get_vma(mstruct, HEAP);
	vma_heap->vm_start = mstruct->start_brk;
	vma_heap->vm_end = mstruct->brk;
	vma_heap->permission_flag = VM_READ | VM_WRITE;
<<<<<<< HEAD
=======
	vma_heap->vm_file = NULL;
	vma_heap->file_offset = 0;
>>>>>>> parent of c653a15... add copy_mm func

	vma_struct* vma_stack = get_vma(mstruct, STACK);
	vma_stack->vm_start = mstruct->start_stack;
	vma_stack->vm_end = STACK_TOP;
	vma_stack->permission_flag = VM_READ | VM_WRITE;
<<<<<<< HEAD
=======
	vma_stack->vm_file = NULL;
	vma_stack->file_offset = 0;
>>>>>>> parent of c653a15... add copy_mm func
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

<<<<<<< HEAD
	do_execv("bin/test_hello", argv, envp);

	do_fork();
=======
	do_execv("bin/test_malloc", argv, envp);

	//do_fork();
>>>>>>> parent of c653a15... add copy_mm func

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
	// create new task
	task_struct* execv_task = current;

	int argc = 0;
	int envc = 0;
	char* argv_0 = bin_name;

	set_task_struct(execv_task);

	// load bin_name elf
	struct file* file = tarfs_open(bin_name, O_RDONLY);

	if (file == NULL) {
		return -1;
	}

	if (load_elf(execv_task, file) < 0)
		return -1; // -1 if error

	setup_vma(execv_task->mm);

	//allocate heap
	uint64_t initial_heap_size = 2 * PAGE_SIZE;

	execv_task->mm->start_brk = (uint64_t) umalloc(
<<<<<<< HEAD
			(void*) ((execv_task->mm->end_data & CLEAR_OFFSET) + PAGE_SIZE),
			initial_heap_size);
=======
			(void*) (((execv_task->mm->end_data + execv_task->mm->bss)
					& CLEAR_OFFSET) + PAGE_SIZE), initial_heap_size);

>>>>>>> parent of c653a15... add copy_mm func
	execv_task->mm->brk = execv_task->mm->start_brk + initial_heap_size;

	umap((void*) STACK_TOP, PAGE_SIZE); // guarantee one page mapped above STACK_TOP
	execv_task->mm->start_stack = (uint64_t) umap(
			(void*) (STACK_TOP - PAGE_SIZE), PAGE_SIZE);

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
	tmp -= 8;	      // uint64_t is 8 bytes

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
		//tmp -= 8;	      // uint64_t is 8 bytes
		memset(tmp, 0, 8);
		tmp -= 8;	      // uint64_t is 8 bytes

	}

	// store argv pointers in the proper place of user stack
	if (argc > 0) {

		execv_task->mm->arg_end = (uint64_t) argv[argc - 1];

		int argc2 = argc;
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

	//execv_task->mm->start_stack = (uint64_t) rsp;
	setup_vma(execv_task->mm);

	return 0;

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

void set_child_pt(task_struct* child) {
	global_PML4 = (pml4_t) kmalloc(KERNPT);
	child->cr3 = (uint64_t) global_PML4 - VIR_START;

	map_kernel();

	vma_struct* vma = current->mm->mmap;

	while (vma != NULL) {
		uint64_t start_addr = vma->vm_start & CLEAR_OFFSET;
		uint64_t end_addr = vma->vm_end;

		if (end_addr % PAGE_SIZE) {
			end_addr &= CLEAR_OFFSET;
			end_addr += PAGE_SIZE;
		} else {
			end_addr &= CLEAR_OFFSET;
		}

		while (start_addr < end_addr) {

			uint64_t content = self_ref_read(PT, start_addr);
			page_sp * page = get_page_frame_descriptor(content);
<<<<<<< HEAD
			page->ref_count += 1;

=======
            /* 
             * risk!!
             * if two memory region overlaps at virtual page X,
             * the ref_count of page frame correpsonding to X will be increnased twice
             * caused by not aligned vma
             */
			page->ref_count += 1;

            // modify parent's page table
>>>>>>> parent of c653a15... add copy_mm func
			content &= PTE_R_MASK;
			content |= PTE_COW;
			self_ref_write(PT, start_addr, content);

<<<<<<< HEAD
=======
            // map child's page table
>>>>>>> parent of c653a15... add copy_mm func
			map_user_pt(start_addr, content, USERPT);

			start_addr += PAGE_SIZE;
		}
		vma = vma->vm_next;
	}

//	set_CR3(child->cr3);
}

int do_fork() {
<<<<<<< HEAD
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
	new_task->rsp = current->rsp;

	new_task->mm->start_stack = STACK_TOP;
	//umalloc((void*) (STACK_TOP - PAGE_SIZE), PAGE_SIZE);
	//child has it own stack

	new_task->task_state = TASK_READY;

	//set_cow();	//set cow bit
	set_child_pt(new_task);

	add_task(new_task);
	return new_task->pid;
	//just return child's pid

=======
    //create a new task struct
	task_struct *new_task = (task_struct *)(kmalloc(TASK));
  
    //assign new pid for child
    new_task->pid = assign_pid();
    new_task->ppid = current->pid;
    
    //copy task name
    strcpy(new_task->task_name, current->task_name);

    
    //memory portion begins here
    //child has it own kernel stack
    new_task->kernel_stack = (uint64_t) kmalloc(KSTACK);
    
    // TODO: init_kern usused
    // new_task->init_kern = new_task->kernel_stack;
    
    // only set vma_struct and mm_struct portion of task_struct
    set_task_struct(new_task);
    
    // mm-> mmap is overwritten by memcpy, so store it in temp and then restore it
    vma_struct *temp_mm = new_task->mm->mmap;
    memcpy((void*) new_task->mm, (void*) current->mm, 0x1000);
    new_task->mm->mmap = temp_mm;
    
    // TODO: VERY IMPORTANT!!!!
    // copy vma content into child from parent

    // set page table of child process, also modified new_task->cr3
	set_child_pt(new_task);

    // add child task to run queue
	new_task->task_state = TASK_READY;
    add_task(new_task);
    
    
    // TODO: tricky part begins here!!
    new_task->rip = current->rip;
    new_task->rsp = current->rsp;
    
    
    //just return child's pid
	return new_task->pid;
>>>>>>> parent of c653a15... add copy_mm func
}

void do_exit(int status) {
	// current = current->next;
	current->task_state = TASK_ZOMBIE;
    
    /*
     * TODO: unfinished
     * Check if parent process is suspended (by calling waitpid())
     * if yes, wake parent process
     */
    // find_task_struct takes as input pid, returns corresponding task_struct
//    if ((find_task_struct(current->wait_pid))->task_state == TASK_SLEEPING) {
//        (find_task_struct(current->wait_pid))->task_state = TASK_RUNNING;
//    }
    
<<<<<<< HEAD
    schedule();
=======
	schedule();
}
>>>>>>> parent of c653a15... add copy_mm func

}

void do_yield() {
	schedule();
}

