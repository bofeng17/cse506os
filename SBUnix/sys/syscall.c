#include <sys/sbunix.h>
#include <sys/syscall.h>

void ring3_test ();
void do_syscall ();

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
    hi = 0x00130008;//SYSRET CS == 0x0020, SS == 0x0018 || SYSCALL CS == 0x0008, SS == 0x0010
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

int64_t sysret_to_ring3(){
    __asm__ __volatile__("cli");
    
    //mov rip in rcx & mov rflags into r11
    __asm__ __volatile__("pushfq;"
                         "orq $0x200, (%%rsp);"//enable interrupt after switch to ring3
                         "pop %%r11;"
                         : : "c"((uint64_t)ring3_test));
    
    __asm__ __volatile__("mov $0x101000, %%rsp;"
                         ::);
    //    __asm__ __volatile__("mov $0x17, %%rax;"
    //                         ::);
    
    __asm__ __volatile__("rex.w sysret");
    return 0;
}

void syscall_to_ring0(){
    __asm__ __volatile__("rex.w syscall");//must use rex.w
    /*
     * notes for syscall handler
     * 1) mustn't change r11 & rcx
     * 2) save rsp, otherwise rsp has to be saved by libc wrapper
     */
}

void iret_to_ring3(){
    __asm__ __volatile__("cli;"
                         "mov $0x1b, %%ax;"//ds,es,fs,gs
                         "mov %%ax, %%ds;"
                         "mov %%ax, %%es;"
                         "mov %%ax, %%fs;"
                         "mov %%ax, %%gs;"
                         "pushq $0x1b;"//ss
                         "pushq $0x101000;"//rsp
                         "pushfq;"
                         "orq $0x200, (%%rsp);"
                         //"and $0x000000003fffffff, %%rcx;"
                         "pushq $0x23;"//cs
                         "pushq %%rcx;"//rip
                         "iretq;"
                         : : "c"((uint64_t)ring3_test));
}

// test whether succeed switching to ring 3
void ring3_test () {
    printf("Just a simple test");
    syscall_to_ring0();
    //__asm__ __volatile__("hlt");
}

// kernel sys_call dispatcher
void do_syscall () {
    uint16_t syscall_no;
    syscall_parameters parm;
    __asm__ __volatile__("nop"
                         :"=a"(syscall_no));
    __asm__ __volatile__("nop"
                         :"=D"(parm.rdi),"=S"(parm.rsi),"=d"(parm.rdx));
    __asm__ __volatile__("mov %%r10, %%rdi;"
                         "mov %%r8, %%rsi;"
                         "mov %%r9, %%rdx;"
                         :"=D"(parm.r10),"=S"(parm.r8),"=d"(parm.r9));
    switch (syscall_no) {
        case SYS_read:
            //do_read();
            break;
        case SYS_write:
            //do_write();
            break;
        default:
            printf("Syscall wasn't implemented");
            break;
    }
}


