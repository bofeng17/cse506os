#include <sys/sbunix.h>
#include <sys/process.h>
#include <sys/gdt.h>
#include <sys/virmm.h>

extern task_struct* end;
extern task_struct* current;

task_struct *prev;

void schedule() {

    prev = current;

    //set previous running task state to task_ready
//    if (!(current->task_state == TASK_ZOMBIE||current->task_state == TASK_DEAD)) {
    if (current->task_state == TASK_RUNNING) {
        //not delete zombie from run queue, just skip it
        //zombie become dead when its parent read the return value
        // a kernel thread named clean_dead will run occasionally and clear dead tasks
        current->task_state = TASK_READY;
    }

    current = current->next;

    //current won't be null, if only one task exists, that must be idle
    while ((current->task_state != TASK_READY) || (current->pid == 0)) {
        current = current->next;
        if (current == prev) {
            //already traverse the run queue, so break
            // TODO: don't switch back to idle if there is other process
            current = idle;
            break;
        }
    }

    end = current;

    current->task_state = TASK_RUNNING;
    // there is no need switching from one task to itself
    if (current != prev) {
        context_switch(prev, current);
    }

}

// delete dead tasks from run queue
void clean_dead() {

//    task_struct* pre = NULL;
//    task_struct* cur = current->next;
//    while (cur->task_state != TASK_DEAD) {
//        pre = cur;
//        cur = cur->next;
//    }
//
//    pre->next = cur->next;
//    kfree(cur);

    task_struct* cur = current;
    task_struct* dead;
    while (cur->next != current && cur->next != NULL) {
        if (cur->next->task_state == TASK_DEAD) {
            dead = cur->next;
            cur->next = cur->next->next;        //delete dead task

//            pid_list[dead->pid] = 0;
            kfree((void*) dead->init_kern, KSTACK);
            free_vma(dead->mm->mmap);        // free VMA
            kfree((void*) dead->mm, MM);
            kfree(dead, TASK);

        } else
            cur = cur->next;        //not delete, just move to next task
    }
}

//prev stored in %rdi (because it is the first arg in function call)
//next sotred in %rsi
void context_switch(task_struct *prev, task_struct *next) {

    //    __asm__ __volatile__ ("pushq  %rax;"
    //                          "pushq  %rbx;"
    //                          "pushq  %rcx;"
    //                          "pushq  %rdx;"
    //                          "pushq  %rsi;"
    //                          "pushq  %rdi;"
    //                          "pushq  %rbp;"
    //                          "pushq  %rsp;"
    //                          "pushq  %r8;"
    //                          "pushq  %r9;"
    //                          "pushq  %r10;"
    //                          "pushq  %r11;"
    //                          "pushq  %r12;"
    //                          "pushq  %r13;"
    //                          "pushq  %r14;"
    //                          "pushq  %r15;"
    //                          "pushfq;");

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

    // register next's kernel stack
    tss.rsp0 = next->init_kern;

    //Ref. <Understanding the Linux kernel>: page 108 step 3
    /*
     * save the instruction pointer for swapped process
     * and jump to next process
     */
    // next is a kernel thread or a user process
    __asm__ __volatile__ ("movq $1f, %0;"
            "sti;" //enable CPU interrupt, which is disabled by timer. Is it proper to do here?
            "jmp %1;"
            "1:\t;"
            :"=g"(prev->rip)
            :"r"(next->rip)
    );
    /*
     * how to ret to ring3 now:
     * for process executed for the 1st time
     *      this is a new process created by fork
     *      rely on code in do_fork to ret to ring3 (sysret insteasd of iret)
     * for process executed not for the 1st time
     *      function returnss: context switch -> schedule ->
     *      interrupt handler -> user process
     */

    //    __asm__ __volatile__ ("popfq;"
    //                          "popq   %r15;"
    //                          "popq   %r14;"
    //                          "popq   %r13;"
    //                          "popq   %r12;"
    //                          "popq   %r11;"
    //                          "popq   %r10;"
    //                          "popq   %r9;"
    //                          "popq   %r8;"
    //                          "popq   %rsp;"
    //                          "popq   %rbp;"
    //                          "popq   %rdi;"
    //                          "popq   %rsi;"
    //                          "popq   %rdx;"
    //                          "popq   %rcx;"
    //                          "popq   %rbx;"
    //                          "popq   %rax;");
    //TODO: movl %eax, last
    //Ref. <Understanding the Linux kernel>: page 108 step 9
}
