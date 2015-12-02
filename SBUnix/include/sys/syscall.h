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

#define SYS_mmap		9

// defined by myself
#define SYS_sleep        99
#define SYS_yield       100
#define SYS_opendir     101
#define SYS_readdir     102
#define SYS_closedir    103
#define SYS_setcwd      104
#define SYS_checkfile   105
#define SYS_readrootfs  106
#define SYS_sbrk         13
#define SYS_ps           14
#define SYS_clear        15

// init msr registers
void syscall_init();
void sysret_to_ring3();
void do_syscall();
void do_exit(int status);

#endif
