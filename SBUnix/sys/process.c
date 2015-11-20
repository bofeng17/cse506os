#include <sys/sbunix.h>
#include <sys/stdio.h> //kernel should not include user header files
#include <sys/stdlib.h>//kernel should not include user header files
#include <sys/tarfs.h>
#include <sys/elf.h>
#include <sys/process.h>
#include <sys/virmm.h>
#include <sys/string.h>
#include <sys/gdt.h>

task_struct* front;
task_struct* end;
task_struct* current;

int track_task[PROCESS_NUMBER];

void
init_process_id ()
{
  int i;
  for (i = 0; i < PROCESS_NUMBER; i++)
    {
      track_task[i] = 0;
    }
}

int
assign_pid ()
{
  int i;
  for (i = 0; i < PROCESS_NUMBER; i++)
    {
      if (track_task[i] == 0)
	{
	  track_task[i] = 1;
	  return i;
	}
    }

  return -1;
}

//insert new task to run queue
void
add_task (task_struct * task)
{
  end->next = task;
  end = task;
  end->next = front;
}

//only set task_struct vma_struct and mm_struct
void
set_task_struct (task_struct* task)
{
  mm_struct* mstruct = kmalloc (MM);

  vma_struct* vma_code = kmalloc (VMA);
  vma_struct* vma_data = kmalloc (VMA);
  vma_struct* vma_stack = kmalloc (VMA);
  vma_struct* vma_heap = kmalloc (VMA);

  vma_code->vm_mm = mstruct;
  vma_data->vm_mm = mstruct;
  vma_stack->vm_mm = mstruct;
  vma_heap->vm_mm = mstruct;

  vma_code->vm_start = mstruct->start_code;
  vma_code->vm_end = mstruct->end_code;
  vma_code->permission_flag = VM_READ | VM_EXEC;
  vma_code->vm_next = vma_data;

  vma_data->vm_start = mstruct->start_data;
  vma_data->vm_end = mstruct->end_data;
  vma_data->permission_flag = VM_READ | VM_WRITE;
  vma_data->vm_next = vma_heap;

  vma_heap->vm_start = mstruct->brk;
  vma_heap->permission_flag = VM_READ | VM_WRITE;
  vma_heap->vm_next = vma_stack;

  vma_stack->vm_start = mstruct->start_stack;
  vma_stack->permission_flag = VM_READ;
  vma_stack->vm_next = NULL;

  mstruct->mmap = vma_code;
  task->mm = mstruct;
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
create_idle_thread ()
{
  task_struct * idle = (task_struct*) (kmalloc (TASK));

  idle->ppid = 0;
  idle->pid = assign_pid ();
  idle->kernel_stack = idle->init_kern = (uint64_t) kmalloc (KSTACK); //what is init_kern
  //idle->rip = (uint64_t) & function_idle; //idle will call schedule function
  idle->task_state = TASK_READY;
  idle->sleep_time = 0;
  idle->cr3 = get_CR3 ();
  strcpy (idle->task_name, "idle thread");
  idle->mm = NULL;
  idle->wait_pid = 0;

  front = idle;
  end = idle;
  end->next = front;
  current = idle;

  return idle;
}

void
func_init ()
{
  char* argv[2] =
    { "argv1", "argv2test" };
  char* envp[3] =
    { "envp1", "envp2test", "envp33" };

  do_execv ("bin/hello", argv, envp);

  exit (0);
}

task_struct*
create_thread_init ()
{

  task_struct * new_task = (task_struct*) (kmalloc (TASK));

  new_task->ppid = 0;
  new_task->pid = assign_pid ();
  new_task->kernel_stack = new_task->init_kern = (uint64_t) kmalloc (KSTACK); //what is init_kern
  new_task->rip = (uint64_t) & func_init;
  new_task->task_state = TASK_NEW;
  new_task->sleep_time = 0;
  new_task->cr3 = get_CR3 ();
  strcpy (new_task->task_name, "init");

  set_task_struct (new_task);

  new_task->wait_pid = 0;

  add_task (new_task);

  return new_task;

}

task_struct*
create_thread (uint64_t thread, char * thread_name)
{

  task_struct * new_task = (task_struct*) (kmalloc (TASK));

  new_task->ppid = 0;
  new_task->pid = assign_pid ();
  new_task->kernel_stack = new_task->init_kern = (uint64_t) kmalloc (KSTACK); //what is init_kern
  new_task->rip = thread;
  new_task->task_state = TASK_NEW;
  new_task->sleep_time = 0;
  new_task->cr3 = get_CR3 ();
  strcpy (new_task->task_name, thread_name);
  new_task->mm = NULL;
  new_task->wait_pid = 0;

  add_task (new_task);

  return new_task;

}

//prev stored in %rdi (because it is the first arg in function call)
//next sotred in %rsi
void
context_switch (task_struct *prev, task_struct *next)
{

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

int
count_args (char ** args)
{
  int i = 0;

  while (args[i++] != NULL)
    ;
  return i - 1;

}

int
do_execv (char* bin_name, char ** argv, char** envp)
{
  int retval = 0; // return code indicates success or not
  int argc = 1;

  // create new task
  task_struct* execv_task = (task_struct*) kmalloc (TASK);
  execv_task->pid = current->pid;
  execv_task->ppid = current->ppid;

  set_task_struct (execv_task);

  // load bin_name elf
  void* file = find_file (bin_name);

  load_elf (execv_task, file); // -1 if error

//  // map bss
//  umalloc ((void*) execv_task->mm->end_data, execv_task->mm->bss);
//
//  // map data segment
//  uint64_t data_size = execv_task->mm->end_data - execv_task->mm->start_data;
//  umalloc ((void*) execv_task->mm->start_data, data_size);
//
//  //map code/text segment
//  uint64_t code_size = execv_task->mm->end_code - execv_task->mm->start_code;
//  umalloc ((void*) execv_task->mm->start_code, code_size);

  //allocate heap
  execv_task->mm->start_brk = (uint64_t) umalloc (
      (void*) execv_task->mm->end_code, 10 * PAGE_SIZE);

  execv_task->mm->start_stack = (uint64_t) umalloc ((void*) STACK_TOP,
						    PAGE_SIZE);

  umalloc ((void*) (STACK_TOP - PAGE_SIZE), PAGE_SIZE);
  // setup new task user stack, rsp, argv, envp
  void* rsp = (void*) (STACK_TOP);

  void* tmp = rsp;
  memset (tmp, 0, 1);
  tmp = tmp - 8;

  //*((int*) tmp--) = 0;

  int i = 0;
  int j = 0;
  if (envp != NULL)
    {
      int envc = count_args (envp);
      dprintf ("%d\n", envc);
      while (envp[i] != NULL)
	{
	  dprintf ("%s\n", envp[i]);
	  j = 0;
	  while (envp[i][j] != '\0')
	    {
	      *((char*) tmp--) = envp[i][j++];
	      //printf("%c\n",*((char*)tmp+1) );
	      //printf("%c\n",envp[i][j-1] );
	    }
	  i++;
	  //printf("%s\n",(char*)tmp+1 );
	}

      memset (tmp, 0, 1);
      tmp = tmp - 8;
    }

  i = 0;

  if (argv != NULL)
    {
      argc = count_args (argv);
      dprintf ("argc is%d\n", argc);

      while (argv[i] != NULL)
	{
	  dprintf ("%s\n", argv[i]);
	  j = 0;
	  while (argv[i][j] != '\0')
	    {
	      *((char*) tmp--) = argv[i][j++];
	      //printf("%c\n",*((char*)tmp+1) );
	    }

	  i++;
	  //printf("%s\n",(char*)tmp+1 );
	}
    }

  *((int*) tmp) = argc;
  rsp = tmp;

  execv_task->rsp = (uint64_t) rsp;
  // switch current stack to user stack of created task(execv_task)

  // add execv_task to the run queue
  add_task (execv_task);

  return retval;
}

task_struct*
create_user_process (char* bin_name)
{
  task_struct * new_task = (task_struct*) (kmalloc (TASK));

  new_task->ppid = 0;
  new_task->pid = assign_pid ();
  new_task->kernel_stack = new_task->init_kern = (uint64_t) kmalloc (KSTACK); //what is init_kern

  new_task->task_state = TASK_NEW;
  new_task->sleep_time = 0;

  new_task->cr3 = get_CR3 ();

  strcpy (new_task->task_name, bin_name);
  //do_execv (bin_name, NULL, NULL);

  new_task->wait_pid = 0;

  add_task (new_task);

  return new_task;
}

void
exit (int status)
{
  // current = current->next;
  current->task_state = TASK_ZOMBIE;
  schedule ();
}
