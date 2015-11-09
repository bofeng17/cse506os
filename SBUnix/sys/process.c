#include <sys/sbunix.h>
#include <stdio.h> //kernel should not include user header files
#include <stdlib.h>//kernel should not include user header files
#include <sys/process.h>
#include <sys/virmm.h>
#include <sys/string.h>
#include <sys/gdt.h>

#define MAX_task_num 128

extern task_struct* testa;
extern task_struct* testb;


task_struct* current;

int track_task[MAX_task_num];


void init_process_id()
{
    int i;
    for(i=0;i<MAX_task_num;i++)
    {
        track_task[i]=0;
    }
}

int assign_pid()
{
    int i;
    for(i=0;i<MAX_task_num;i++)
    {
        if(track_task[i]==0)
        {
            track_task[i]=1;
            return i;
        }
    }
    
    return -1;
}

task_struct* create_thread_idle()
{
    
    task_struct * new_task=(task_struct*)(kmalloc(1));
    
    new_task->ppid=0;
    new_task->pid=assign_pid();
    new_task->kernel_stack=new_task->rsp=new_task->init_kern=(uint64_t)kmalloc(0);//what is init_kern
    // TODO
    //new_task->rip;//undecided
    new_task->task_state=TASK_RUNNING;
    new_task->sleep_time=0;
    new_task->cr3=get_CR3();
    strcpy(new_task->task_name, "idle");
    new_task->mm=NULL;
    new_task->wait_pid=0;
    
    //new_task->next=new_task;
    //current=new_task;
    
    return new_task;
}


task_struct* create_thread_a()
{
    
    task_struct * new_task=(task_struct*)(kmalloc(1));
    
    new_task->ppid=0;
    new_task->pid=assign_pid();
    new_task->kernel_stack=new_task->init_kern=(uint64_t)kmalloc(0);//what is init_kern
    new_task->rip=(uint64_t)(&function_a);
    new_task->task_state=TASK_RUNNING;
    new_task->sleep_time=0;
    new_task->cr3=get_CR3();
    strcpy(new_task->task_name, "a");
    new_task->mm=NULL;
    new_task->wait_pid=0;
    /*
     current->next=new_task;
     new_task->next=current;
     current=new_task;
     */
    
    return new_task;
    
    
    
}

task_struct* create_thread_b()
{
    
    task_struct * new_task=(task_struct*)(kmalloc(1));
    
    new_task->ppid=0;
    new_task->pid=assign_pid();
    new_task->kernel_stack=new_task->init_kern=(uint64_t)kmalloc(0);//what is init_kern
    new_task->rip=(uint64_t)(&function_b);
    new_task->task_state=TASK_RUNNING;
    new_task->sleep_time=0;
    strcpy(new_task->task_name, "b");
    new_task->mm=NULL;
    new_task->wait_pid=0;
    new_task->cr3=get_CR3();
    
    /*
     current->next=new_task;
     new_task->next=current;
     current=new_task;
     */
    
    
    
    return new_task;
    
    
    
}

task_struct* create_thread_c()
{
    
    task_struct * new_task=(task_struct*)(kmalloc(1));
    
    new_task->ppid=0;
    new_task->pid=assign_pid();
    new_task->kernel_stack=new_task->init_kern=(uint64_t)kmalloc(0);//what is init_kern
    new_task->rip=(uint64_t)(&function_c);
    new_task->task_state=TASK_RUNNING;
    new_task->sleep_time=0;
    strcpy(new_task->task_name, "c");
    new_task->mm=NULL;
    new_task->wait_pid=0;
    new_task->cr3=get_CR3();
    
    /*
     current->next=new_task;
     new_task->next=current;
     current=new_task;
     */
    
    
    
    return new_task;
    
    
    
}

void function_a()
{
    int i;
    
    for(i=0;i<8;i++)
    {
        printf("im thread a %d \n",i);
        __asm__ __volatile__ ("hlt");
        //context_switch(testa, testb);
    }
    
    printf("a is finished");
    
}

extern task_struct* testc;


void function_b()
{
    int i;
    for(i=0;i<8;i++)
    {
        printf("im thread b %d\n", i);
        __asm__ __volatile__ ("hlt");
        //context_switch(testb, testc);
        
    }
    printf("b is finished");
    
}

void function_c()
{
    int i;
    for(i=0;i<8;i++)
    {
        printf("im thread c %d\n", i);
        __asm__ __volatile__ ("hlt");
        //context_switch(testc, testa);
        
    }
    printf("c is finished");
    
}

//prev stored in %rdi (because it is the first arg in function call)
//next sotred in %rsi
void context_switch(task_struct *prev, task_struct *next) {
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
    
    //TODO: movl %eax, last
    //Here we should
    //Ref. <Understanding the Linux kernel>: page 108 step 9
    
}




