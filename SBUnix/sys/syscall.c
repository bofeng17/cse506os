#include <sys/sbunix.h>
#include <sys/syscall.h>
#include <sys/process.h>
#include <sys/virmm.h> // using STACK_TOP
#include <sys/gdt.h>

// init msr registers
void syscall_init() {
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
    hi = 0x00130008; //SYSRET CS == 0x0020+11b, SS == 0x0018+11b || SYSCALL CS == 0x0008, SS == 0x0010
    lo = 0x00000000; //reserved
    __asm__ __volatile__("wrmsr" : : "a"(lo), "d"(hi), "c"(msr));
    
    // write LSTAR MSR
    msr = 0xc0000082;
    hi = (uint32_t)((uint64_t) do_syscall >> 32); //lo target RIP
    lo = (uint32_t) (uint64_t) do_syscall; //high target RIP
    __asm__ __volatile__("wrmsr" : : "a"(lo), "d"(hi), "c"(msr));
    
    // write FMASK MSR
    msr = 0xc0000084;
    hi = 0x00000000; //reserved
    lo = 0x00000000; //set EFLAGS Mask
    __asm__ __volatile__("wrmsr" : : "a"(lo), "d"(hi), "c"(msr));
}

void sysret_to_ring3() {
    __asm__ __volatile__("cli");
    
    // manually switch ds/es/fs/gs
    __asm__ __volatile__("mov $0xc0000081, %rcx;" // read from STAR MSR
                         "rdmsr;"
                         "shr $0x10, %rdx;"
                         "add $0x8, %rdx;"// now rdx stores ss selector of ring3
                         "mov %dx, %ds;"
                         "mov %dx, %es;"
                         "mov %dx, %fs;"
                         "mov %dx, %gs;");
    
    // mov rip in rcx & mov rflags into r11
    __asm__ __volatile__("pushfq;"
                         "orq $0x200, (%%rsp);" //enable interrupt after switch to ring3
                         "pop %%r11;"
                         :: "c"(current->rip));
    
    // switch stack
    __asm__ __volatile__("pushq %%rbp;" // TODO: save rbp, causing kernel stack non-empty
                         "mov %%rsp, %%rax;"
                         :"=a"(tss.rsp0));
    __asm__ __volatile__("mov %%rax, %%rsp;"
                         ::"a"(current->rsp));
    
    // return to ring3
    __asm__ __volatile__("rex.w sysret");
}

// kernel syscall dispatcher
void do_syscall() {
    
    //TODO: 16bytes stack alignment at syscall
    //TODO: do we need to enable alignment checking?
    
    /*
     * local variables must bind to callee saved registers
     * if not, it may be bind by compiler to RCX/R11, which are caller saved
     */
    register uint64_t syscall_no __asm__("r12");    // bind to r12
    //TODO: return value type?
    register int64_t ret_val __asm__("r13") = 0;    // bind to r13
    register uint64_t saved_rdx __asm__("r14");     // bind to r14
    
    __asm__ __volatile__ ("cli");
    
    // store syscall number(rax) in r12, because rax will be used later
    // store rdx in r14
    __asm__ __volatile__ ("mov %%rdx, %1;"
                          :"=a"(syscall_no), "=r"(saved_rdx));
    
    /*
     * push all callee-saved register (order matters for popping):
     * r14, r13, r12 (already pushed)
     * r15, rbx (here), rbp (below)
     */
    __asm__ __volatile__("pushq %r15;"
                         "pushq %rbx;");
    
    // manually switch ds/es/fs/gs
    __asm__ __volatile__("mov %ss, %ax;"
                         "mov %ax, %ds;"
                         "mov %ax, %es;"
                         "mov %ax, %fs;"
                         "mov %ax, %gs;");
    
    // switch stack
    // pushed all 6 callee-saved registers onto user stack
    // TODO: need to manipulate during do_fork/do_execve
    __asm__ __volatile__("pushq %%rbp;"
                         "mov %%rsp, %%rax;"
                         :"=a"(current->rsp));// "=a"(current->rsp) modifies rdx
    __asm__ __volatile__("mov %0, %%rsp;"
                         "popq %%rbp;"    // TODO: rbp
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
    
    //__asm__ __volatile__ ("sti");
    
    
    // call correpsonding syscall service routine according to syscall no
    // conforming to System V AMD64 ABI
    /*
     * TODO: must check whether switch body modifies rdi, rsi, rdx, rcx, r8, r9
     *       switch statement modified rdx
     */
    
    /*
     * Template:
     * __asm__ __volatile__ ("mov %r14, %rdx;"); // Necessary for syscall with >= 3 parameters
     * __asm__ __volatile__ ("callq do_xxx;"
     *                       :"=a"(ret_val));
     */
    switch (syscall_no) {
        // file
        case SYS_open:
            __asm__ __volatile__ ("callq tarfs_open;"
                                  :"=a"(ret_val));
            break;
        case SYS_close:
            __asm__ __volatile__ ("callq tarfs_close;"
                                  :"=a"(ret_val));
            break;
        case SYS_opendir:
            __asm__ __volatile__ ("callq do_opendir;"
                                  :"=a"(ret_val));
            break;
        case SYS_readdir:
            __asm__ __volatile__ ("callq do_readdir;"
                                  :"=a"(ret_val));
            break;
        case SYS_closedir:
            __asm__ __volatile__ ("callq do_closedir;"
                                  :"=a"(ret_val));
            break;
        case SYS_getcwd:
            __asm__ __volatile__ ("callq get_cwd;"
                                  :"=a"(ret_val));
            break;
            
        // file & terminal
        case SYS_read:
            // TODO: This is not the final version of read.
            __asm__ __volatile__ ("mov %r14, %rdx;");
            __asm__ __volatile__ ("callq do_read;"
                                  :"=a"(ret_val));
            break;
        case SYS_write:
            __asm__ __volatile__ ("mov %r14, %rdx;");
            __asm__ __volatile__ ("callq do_write;"
                                  :"=a"(ret_val));
            break;
            
        // process
        case SYS_fork:
            __asm__ __volatile__ ("callq do_fork;"
                                  :"=a"(ret_val));
            break;
        case SYS_execve:
            __asm__ __volatile__ ("mov %r14, %rdx;");
            __asm__ __volatile__ ("callq do_execv;"
                                  :"=a"(ret_val));
            // TODO: need to verify
            
            // overwrite rcx (stored rip) to new rip
            __asm__ __volatile__ ("mov %0, (%%rsp);"
                                  ::"r"(current->rip));
            
            
            // adjust user stack for the 2nd half of do_syscall
            
            // equivalent to push r14, r13, r12, r15, rbx (order matters)
            current->rsp -= 0x28;
            // equivalent to push rbp (= STACK_TOP)
            current->rsp -= 0x8;
            *(uint64_t *) current->rsp = STACK_TOP;

            // compiler may adjust stack automatically
            // current->rsp -= 0x8;
            break;
        case SYS_exit:
            __asm__ __volatile__ ("callq do_exit;"
                                  :"=a"(ret_val));
            break;
        case SYS_yield:
            __asm__ __volatile__ ("callq do_yield;"
                                  :"=a"(ret_val));
            break;
        case SYS_sbrk:
            __asm__ __volatile__ ("callq do_sbrk;"
                                  :"=a"(ret_val));
            break;
        case SYS_ps:
            __asm__ __volatile__ ("callq do_ps;"
                                  :"=a"(ret_val));
            break;
        default:
            printf("Syscall wasn't implemented\n");
            break;
    }
    
    //__asm__ __volatile__("cli");
    
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
    __asm__ __volatile__("pushq %%rbp;" // TODO: rbp
                         "mov %%rsp, %%rax;"
                         :"=a"(tss.rsp0));
    __asm__ __volatile__("mov %%rax, %%rsp;"
                         "popq %%rbp;"
                         ::"a"(current->rsp));
    
    /*
     * pop callee-saved registers
     * rbp (already popped), rbx, r15 (saved manually)
     * r12, r13, r14 (saved by compiler)
     */
    __asm__ __volatile__("popq %%rbx;"
                         "popq %%r15;"
                         "popq %%r12;"
                         "popq %%r13;"
                         "popq %%r14;"
                         //"add $0x8,%rsp;"// TODO: be cautious @ manipulating rsp
                         ::"a"(ret_val));
    
    // return to ring3
    __asm__ __volatile__("rex.w sysret"// move ret_val into rax
                         );
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

