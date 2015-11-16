#include <sys/sbunix.h>
#include <sys/defs.h>


void func_test_syscall () {
    printf("Just a simple test");
    //__asm__ __volatile__("hlt");
}

void syscall_init(){
    uint32_t hi, lo;
    uint32_t msr;
    
    //uint32_t test_hi, test_lo;
    
    
    msr = 0xc0000080;//%EFER |= $0x1m; To set EFER.SCE bit, otherwise execute sysret will cause #UD exception
    __asm__ __volatile__("rdmsr;"
                         "or $0x1, %%rax;"
                         "wrmsr"
                         :: "c"(msr));
    
    hi = 0x00130008;//SYSRET CS == 0x0020, SS == 0x0018 || SYSCALL CS == 0x0008, SS == 0x0010
    lo = 0x00000000;//reserved
    msr = 0xc0000081;//write STAR MSR
    __asm__ __volatile__("wrmsr" : : "a"(lo), "d"(hi), "c"(msr));
    
//    __asm__ __volatile__("rdmsr" : "=a"(test_lo), "=d"(test_hi) : "c"(msr));
//    printf("%x %x\n",test_hi,test_lo);
    
    //get confused here
    hi = (uint32_t)((uint64_t)func_test_syscall >> 32);//lo target RIP
    lo = (uint32_t)(uint64_t)func_test_syscall;//high target RIP
    msr = 0xc0000082;//write LSTAR MSR
    __asm__ __volatile__("wrmsr" : : "a"(lo), "d"(hi), "c"(msr));
    
//    __asm__ __volatile__("rdmsr" : "=a"(test_lo), "=d"(test_hi) : "c"(msr));
//    printf("%x %x\n",test_hi,test_lo);
    
    hi = 0x00000000;//reserved
    lo = 0x00000000;//set EFLAGS Mask
    msr = 0xc0000084;//write FMASK MSR
    __asm__ __volatile__("wrmsr" : : "a"(lo), "d"(hi), "c"(msr));
    
//    __asm__ __volatile__("rdmsr" : "=a"(test_lo), "=d"(test_hi) : "c"(msr));
//    printf("%x %x\n",test_hi,test_lo);
    
}

void iret_try(){
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
                         : : "c"((uint64_t)func_test_syscall));
}

int64_t syscall_try (){
    //int64_t ret;
    //uint32_t n=1;
    
    __asm__ __volatile__("cli");
    
   //mov rip in rcx & mov rflags into r11
    __asm__ __volatile__("pushfq;"
                         "orq $0x200, (%%rsp);"
                         "pop %%r11;"
                         : : "c"((uint64_t)func_test_syscall));
    
    __asm__ __volatile__("mov $0x101000, %%rsp;"
                         ::);
//    __asm__ __volatile__("mov $0x17, %%rax;"
//                         ::);
    
    __asm__ __volatile__("rex.w sysret"
                         :
                         :
                         : );
    return 0;
}
//
//int64_t syscall_try (){
//    int64_t ret;
//    uint32_t n=1;
//    __asm__ __volatile__("syscall"
//                         :"=a"(ret)
//                         :"a"(n)
//                         : "memory");
//    return ret;
//}