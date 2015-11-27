#include <sys/sbunix.h>
#include <sys/process.h>
#include <sys/gdt.h>
#include <sys/virmm.h>

extern task_struct* end;
extern task_struct* current;

task_struct *prev;
void
schedule ()
{

  prev = current;
  current = current->next;

//  if (current->pid == 0)
//    {
//      current = current->next;
//    }

  clear_zombie (current);

  while (current->task_state != TASK_READY)
    {
      current = current->next;
      if (current == prev)
	break;
    }

  //dprintf ("prev thread name:%s \n", prev->task_name);

  //dprintf ("current thread name:%s\n", current->task_name);
//  if (current != prev)
//    {
//  front = front->next;
  end = prev;
  // dprintf ("begin switch from %s to %s\n", prev->task_name,current->task_name);
  context_switch (prev, current);
  //  dprintf ("finish switch from %s to %s\n", prev->task_name,current->task_name);

//    }

}

void
clear_zombie (task_struct* zombie)
{
  if (zombie->task_state == TASK_ZOMBIE)
    {
      current = zombie->next;
      prev->next = zombie->next;
      zombie->next = NULL;
      kfree (zombie, TASK);
    }
}

//prev stored in %rdi (because it is the first arg in function call)
//next sotred in %rsi
void context_switch(task_struct *prev, task_struct *next) {
    
    __asm__ __volatile__ ("pushq	%rax;"
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
    
    __asm__ __volatile__ ("movq %0, %%cr3;"
                          ::"r"(next->cr3)
                          );
    
    /* save the rsp pointer of the swapped process */
    __asm__ __volatile__ ("movq %%rsp, %0"
                          :"=r"(prev->kernel_stack)
                          );
    
    /* move the stack pointer of current process to rsp register */
    __asm__ __volatile__ ("movq %0, %%rsp;"
                          ::"r"(next->kernel_stack)
                          );
    
    /* 
     * register the kernel stack 
     * cannot use next->init_kern here, bacause our kernel stack is not empty in ring3
     */
    tss.rsp0 = next->kernel_stack;
    
    //Ref. <Understanding the Linux kernel>: page 108 step 3
    /*
     * save the instruction pointer for swapped process
     * and jump to next process
     */
    if (next->mm == NULL) {
        // next is a kernel thread
        __asm__ __volatile__ ("movq $1f, %0;"
                              "sti;" //enable CPU interrupt, which is disabled by timer. Is it proper to do here?
                              "jmp %1;"
                              "1:\t"
                              :"=g"(prev->rip)
                              :"r"(next->rip)
                              );
    } else {
        // next is a user thread
        // iret to ring3
        __asm__ __volatile__ ("movq $1f, %0;"
                              "sti;" //enable CPU interrupt, which is disabled by timer. Is it proper to do here?
                              "jmp %1;"
                              "1:\t"
                              :"=g"(prev->rip)
                              :"r"(next->rip)
                              );
    }
    
    __asm__ __volatile__ ("popfq;"
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
