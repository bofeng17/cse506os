#ifndef _SYS_SYSCALL_H
#define _SYS_SYSCALL_H

#include <sys/defs.h>

#define SYS_exit       60
#define SYS_brk        12
#define SYS_fork       57
#define SYS_getpid     39
#define SYS_getppid   110
#define SYS_execve     59
#define SYS_wait4      61
#define SYS_nanosleep  35
#define SYS_alarm      37
#define SYS_getcwd     79
#define SYS_chdir      80
#define SYS_open        2
#define SYS_read        0
#define SYS_write       1
#define SYS_lseek       8
#define SYS_close       3
#define SYS_pipe       22
#define SYS_dup        32
#define SYS_dup2       33
#define SYS_getdents   78

//struct syscall_parm {
//    uint64_t rdi;
//    uint64_t rsi;
//    uint64_t rdx;
//    uint64_t r10;
//    uint64_t r8;
//    uint64_t r9;
//} __attribute__((packed));
//typedef struct syscall_parm syscall_parameters;

// init msr registers
void syscall_init();
void do_syscall ();
void sysret_to_ring3();

#endif
