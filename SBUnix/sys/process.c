#include <sys/sbunix.h>
#include <sys/stdio.h> //kernel should not include user header files
#include <sys/stdlib.h>//kernel should not include user header files
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

void
func_init ()
{

  exit (0);
}

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

  mm_struct* mstruct = kmalloc (MM);

  // set in do_fork
  mstruct->start_code = (uint64_t) umalloc (PAGE_SIZE);
  mstruct->end_code = mstruct->start_code + PAGE_SIZE; // need to be set actual code size of the binary file

  mstruct->start_data = (uint64_t) umalloc (PAGE_SIZE);
  mstruct->end_data = mstruct->start_data + PAGE_SIZE; // need to be set actual data size of the binary file

  mstruct->start_stack = (uint64_t) umalloc (PAGE_SIZE);

  vma_struct* vma_code = kmalloc (VMA);
  vma_struct* vma_data = kmalloc (VMA);
  vma_struct* vma_stack = kmalloc (VMA);

  vma_code->vm_mm = mstruct;
  vma_data->vm_mm = mstruct;
  vma_stack->vm_mm = mstruct;

  vma_code->vm_start = mstruct->start_code;
  vma_code->vm_end = mstruct->end_code;
  vma_code->permission_flag = VM_READ | VM_EXEC;

  vma_data->vm_start = mstruct->start_data;
  vma_data->vm_end = mstruct->end_data;
  vma_data->permission_flag = VM_READ | VM_WRITE;

  vma_stack->vm_start = mstruct->start_stack;
  vma_stack->permission_flag = VM_READ;

  vma_code->vm_next = vma_data;
  vma_data->vm_next = vma_stack;
  vma_stack->vm_next = NULL;

  mstruct->mmap = vma_code;
  new_task->mm = mstruct;

  new_task->mm = NULL;
  new_task->wait_pid = 0;

  end->next = new_task;
  end = new_task;
  end->next = front;

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

  end->next = new_task;
  end = new_task;
  end->next = front;

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

  new_task->wait_pid = 0;

  end->next = new_task;
  end = new_task;
  end->next = front;

  return new_task;
}

void
exit (int status)
{
  // current = current->next;
  current->task_state = TASK_ZOMBIE;
  schedule ();
}
