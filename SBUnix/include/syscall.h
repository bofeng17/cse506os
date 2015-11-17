#ifndef _SYSCALL_H
#define _SYSCALL_H

#include <sys/defs.h>
#include <sys/syscall.h>

static __inline uint64_t syscall_0(uint64_t n) {
	long ret;
    __asm__ __volatile__("syscall"
                    :"=a"(ret)
                    :"a"(n)
                    : "memory");
    return ret;
}

static __inline uint64_t syscall_1(uint64_t n, uint64_t a1) {
	long ret;
    __asm__ __volatile__("syscall"
                    :"=a"(ret)
                    :"a"(n), "D"(a1)
                    : "memory");
    return ret;
}

static __inline uint64_t syscall_2(uint64_t n, uint64_t a1, uint64_t a2) {
	long ret;
    __asm__ __volatile__("syscall"
                    :"=a"(ret)
                    :"a"(n), "D"(a1), "S"(a2)
                    : "memory");
    return ret;
}

static __inline uint64_t syscall_3(uint64_t n, uint64_t a1, uint64_t a2, uint64_t a3) {
	long ret;
    __asm__ __volatile__("syscall"
                    :"=a"(ret)
                    :"a"(n), "D"(a1), "S"(a2), "d"(a3)
                    : "memory");
    return ret;
}

static __inline uint64_t syscall_6(uint64_t n, uint64_t a1, uint64_t a2, uint64_t a3, uint64_t a4, uint64_t a5,uint64_t a6) {
    
    long ret;
    register long r10 __asm__("r10") = a4;
    register long r8 __asm__("r8") = a5;
    register long r9 __asm__("r9") = a6;

    __asm__ __volatile__("syscall"
                    :"=a"(ret)
                    :"a"(n),"D"(a1),"S"(a2),"d" (a3), "r" (r10), "r" (r8), "r" (r9)
                    :"memory");
    return ret;
}

#endif
