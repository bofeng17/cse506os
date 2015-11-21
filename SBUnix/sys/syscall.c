#include <sys/sbunix.h>
#include <sys/syscall.h>
#include <sys/process.h>
#include <sys/gdt.h>

// init msr registers 
void syscall_init(){
    uint32_t hi, lo;
    uint32_t msr;
    
    // set EFER.SCE bit, i.e. %EFER |= $0x1m, otherwise execute sysret will cause #UD exception
    msr = 0xc0000080;
    __asm__ __volatile__("rdmsr;"
                         "or $0x1, %%rax;"
                         "wrmsr"
                         :: "c"(msr));
    
    //write STAR MSR
    msr = 0xc0000081;
    hi = 0x00130008;//SYSRET CS == 0x0020+11b, SS == 0x0018+11b || SYSCALL CS == 0x0008, SS == 0x0010
    lo = 0x00000000;//reserved
    __asm__ __volatile__("wrmsr" : : "a"(lo), "d"(hi), "c"(msr));
    
    
    // write LSTAR MSR
    msr = 0xc0000082;
    hi = (uint32_t)((uint64_t)do_syscall >> 32);//lo target RIP
    lo = (uint32_t)(uint64_t)do_syscall;//high target RIP
    __asm__ __volatile__("wrmsr" : : "a"(lo), "d"(hi), "c"(msr));
    
    
    // write FMASK MSR
    msr = 0xc0000084;
    hi = 0x00000000;//reserved
    lo = 0x00000000;//set EFLAGS Mask
    __asm__ __volatile__("wrmsr" : : "a"(lo), "d"(hi), "c"(msr));
}

void sysret_to_ring3(){
    __asm__ __volatile__("cli");
    
    // manually switch ds/es/fs/gs
    __asm__ __volatile__("mov $0xc0000081, %rcx;" // read from STAR MSR
                         "rdmsr;"
                         "shr $0x10, %rdx;"
                         "add $0x8, %rdx;" // now rdx stores ss selector of ring3
                         "mov %dx, %ds;"
                         "mov %dx, %es;"
                         "mov %dx, %fs;"
                         "mov %dx, %gs;");
    
    // mov rip in rcx & mov rflags into r11
    __asm__ __volatile__("pushfq;"
                         "orq $0x200, (%%rsp);"//enable interrupt after switch to ring3
                         "pop %%r11;"
                         : : "c"(current->rip));
    
    // switch stack
    __asm__ __volatile__("pushq %%rbp;"// TODO: save rbp, causing kernel stack non-empty
                         "mov %%rsp, %%rax;"
                         :"=a"(tss.rsp0));
    __asm__ __volatile__("mov %%rax, %%rsp;"
                         ::"a"(current->rsp));
    
    // return to ring3
    __asm__ __volatile__("rex.w sysret");
}

// kernel syscall dispatcher
void do_syscall () {
    
    //TODO: 16bytes stack alignment at syscall
    //TODO: do we need to enable alignment checking?
    
    /* 
     * local variables must bind to callee saved registers
     * if not, it may be bind by compiler to RCX/R11, which are caller saved
     */
    register uint64_t syscall_no __asm__("r12");// bind to r12
    //TODO: return value type?
    register int64_t ret_val __asm__("r13") = 0;// bind to r13
    
    __asm__ __volatile__ ("cli");
    
    // store syscall number(rax) in r12, because rax will be used later
    // store rdx in r14
    __asm__ __volatile__("pushq %%r14;"// push callee-saved register before using it
                         "mov %%rdx, %%r14;"// store rdx in r14
                         :"=a"(syscall_no));
  
    // manually switch ds/es/fs/gs
    __asm__ __volatile__("mov %ss, %ax;"
                         "mov %ax, %ds;"
                         "mov %ax, %es;"
                         "mov %ax, %fs;"
                         "mov %ax, %gs;");
    
    // switch stack
    __asm__ __volatile__("pushq %%rbp;"
                         "mov %%rsp, %%rax;"
                         :"=a"(current->rsp));// "=a"(current->rsp uses rdx
    __asm__ __volatile__("mov %0, %%rsp;"
                         "popq %%rbp;"// TODO: rbp
                         ::"a"(tss.rsp0));
    
    /* push r11 (stored rflags), rcx (stored rip),
     * in case of syscall service routine mofidy them.
     * Must do this after switch stack
     */
    __asm__ __volatile__ ("pushq %r11;"
                          "pushq %rcx;");
    
    /* 
     * line 1: To obey x64 calling convention (passing parameters) 
     *         in calling individual service routine
     * line 2: To recover rdx
     */
    __asm__ __volatile__ ("mov %r10, %rcx;"
                          "mov %r14, %rdx");
    
    __asm__ __volatile__ ("sti");
    
    // call correpsonding syscall service routine according to syscall no
    /* 
     * TODO: must check whether switch body modifies rdi, rsi, rdx, rcx, r8, r9
     *       now it changes rdx, so...line swtich+2
     */
    /* 
     * Template:
     * __asm__ __volatile__ ("mov %r14, %rdx;"); // maybe to removed
     * __asm__ __volatile__ ("callq do_xxx;"
     *                       :"=a"(ret_val));
     */
    switch (syscall_no) {
        case SYS_read:
            __asm__ __volatile__ ("mov %r14, %rdx;");
            //ret_val = do_read();
            break;
        case SYS_write:
            //ret_val = do_write();
            __asm__ __volatile__ ("mov %r14, %rdx;");
            __asm__ __volatile__ ("mov %%rsi, %%rdi;"
                                  "mov $0, %%rax;"
                                  "callq printf;"
                                  :"=a"(ret_val));
            break;
        case SYS_fork:
            printf("fork No. 57\n");
            break;
        case SYS_execve:
            printf("execve No. 59\n");
            break;
        case SYS_exit:
            __asm__ __volatile__ ("callq do_exit;"
                                  :"=a"(ret_val));
            break;
        default:
            printf("Syscall wasn't implemented\n");
            break;
    }
    
    __asm__ __volatile__("cli");
    
    // manually switch ds/es/fs/gs, must do this before sysret
    __asm__ __volatile__("mov $0xc0000081, %rcx;" // read from STAR MSR
                         "rdmsr;"
                         "shr $0x10, %rdx;"
                         "add $0x8, %rdx;"// now rdx stores ss selector of ring3
                         "mov %dx, %ds;"
                         "mov %dx, %es;"
                         "mov %dx, %fs;"
                         "mov %dx, %gs;");
    
    /* pop rcx (stored rip), r11 (stored rflags).
     * Must do this before switch stack back to user's
     */
    __asm__ __volatile__ ("popq %rcx;"
                          "popq %r11;");
    
    // switch back to user stack
    __asm__ __volatile__("pushq %%rbp;"// TODO: rbp
                         "mov %%rsp, %%rax;"
                         :"=a"(tss.rsp0));
    __asm__ __volatile__("mov %%rax, %%rsp;"
                         "popq %%rbp;"
                         ::"a"(current->rsp));
    
    // return to ring3
    __asm__ __volatile__("pop %%r14;"// pop callee saved registers, which are saved by compiler
                         "pop %%r13;"
                         "pop %%r12;"
                         "add $0x8,%%rsp;" // TODO: be cautious @ manipulating rsp
                         "rex.w sysret"// move ret_val into rax
                         ::"a"(ret_val));
}


//void iret_to_ring3(){
//    __asm__ __volatile__("cli;"
//                         "mov $0x1b, %%ax;"//ds,es,fs,gs
//                         "mov %%ax, %%ds;"
//                         "mov %%ax, %%es;"
//                         "mov %%ax, %%fs;"
//                         "mov %%ax, %%gs;"
//                         "pushq $0x1b;"//ss
//                         "pushq $0x101000;"//rsp
//                         "pushfq;"
//                         "orq $0x200, (%%rsp);"
//                         //"and $0x000000003fffffff, %%rcx;"
//                         "pushq $0x23;"//cs
//                         "pushq %%rcx;"//rip
//                         "iretq;"
//                         : : "c"((uint64_t)ring3_test));
//}

