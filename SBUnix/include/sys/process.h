#ifndef _PROCESS_H
#define _PROCESS_H

#include <sys/defs.h>
#include <sys/virmm.h>

#define KERNPT_NUMBER 128
#define PROCESS_NUMBER 128
#define KSTACK_NUMBER PROCESS_NUMBER
#define MM_NUMBER PROCESS_NUMBER
#define USERPT_NUMBER (PROCESS_NUMBER*10)
#define VMA_NUMBER (PROCESS_NUMBER*10)
#define FILE_NUMBER PROCESS_NUMBER

#define NAME_LENGTH 32

#define ARGV 10
#define ENVP 100

/*task state enumeration*/
enum TASK_STATE {
	TASK_NEW,
	TASK_READY,
	TASK_RUNNING,
	TASK_SLEEPING,
	TASK_BLOCKED,
	TASK_ZOMBIE,
    TASK_DEAD
};


typedef struct task_struct {

	int pid;
	int ppid;
	char task_name[NAME_LENGTH];

	struct task_struct* next; /* next process in the process list */

	mm_struct* mm; /* Pointers to memory area descriptors */
	uint64_t cr3;

	uint64_t kernel_stack; /* process kernel stack pointer */
	uint64_t init_kern; /* top of kernel stack */
	uint64_t rip; /* instruction pointer */
	uint64_t rsp; /* process user stack pointer */

	uint64_t task_state; /* the current state of task */
	uint64_t sleep_time; /* sleep time (mili second) remained */

	int wait_pid; /* pid of child last exited */
	int64_t ret_val; /* ret_val of the process */
	char cur_dir[NAME_LENGTH]; /* current directory */

} task_struct;

#define PS_NO 128
typedef struct ps_state {
    int id[PS_NO];
    char name[PS_NO][NAME_LENGTH];
    char state[PS_NO][NAME_LENGTH];
} ps_state;

typedef ps_state* ps_t;


int
do_execv(char*, char**, char**);

int do_fork();

void
func_init();

//task_struct *
//create_idle_thread();
//
//task_struct*
//create_thread_init();

task_struct *
create_thread(void*, char *);

void
context_switch(task_struct *, task_struct *);

void
schedule();

void
clean_dead();

task_struct *create_user_process(char* bin_name);

void copy_mm(task_struct *);

extern task_struct *current;
extern task_struct* idle;

extern int pid_list[PROCESS_NUMBER*4];

#define DO_EXECV_TMP_ADDR_TRANSLATE(x) (STACK_TOP - (tmp_vir_addr + PAGE_SIZE - x))

// exit error code
#define ILLEGAL_MEM_ACC 1 // illegal memmory access, killed by page fault handler

int do_getpid();

int do_getppid();

pid_t do_waitpid(pid_t pid, int *status, int options);

void do_exit(int status);

void do_sleep(uint32_t seconds);

void do_yield();

task_struct *find_task_struct(int pid);

extern uint32_t IRQ0_period; // period timer interrupt
void sleep_time_decrease();

#endif
