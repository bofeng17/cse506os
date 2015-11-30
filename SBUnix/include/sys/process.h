#ifndef _PROCESS_H
#define _PROCESS_H

#include <sys/defs.h>

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

#define VM_READ         0x00000001      /* currently active flags */
#define VM_WRITE        0x00000002
#define VM_EXEC         0x00000004
#define VM_SHARED       0x00000008

typedef struct vma_struct {
	struct mm_struct *vm_mm; /* Pointer to the memory descriptor that owns the region */

	uint64_t vm_start; /* First linear address inside the region */
	uint64_t vm_end; /* First linear address after the region */
	struct vma_struct *vm_next; /* Next region in the process list */

	uint64_t permission_flag; /* Flags read, write, execute permissions */
	struct file *vm_file; /* Reference to file descriptors for file opened for writing */
	uint64_t file_offset; /* the vm_start byte in memory corresoonding to file_offset byte in file, used by demand paging*/
} vma_struct;

typedef struct mm_struct {

	struct vma_struct *mmap; /* Pointer to the head of the list of memory region objects */

	uint64_t start_code; /* Initial address of executable code */
	uint64_t end_code; /* Final address of executable code */

	uint64_t start_data; /* Initial address of initialized data */
	uint64_t end_data; /* Final address of initialized data */

	uint64_t start_brk; /* Initial address of the heap */
	uint64_t brk; /* Current final address of the heap */

	uint64_t start_stack; /* Initial address of User Mode stack */

	uint64_t arg_start; /* Initial address of command-line arguments */
	uint64_t arg_end; /* Final address of command-line arguments */

	uint64_t env_start; /* Initial address of environment variables */
	uint64_t env_end; /* Final address of environment variables */

	uint64_t bss; // size of bss segment
	uint64_t rss; /* Number of page frames allocated to the process */

	uint64_t total_vm; /* Size of the process address space (number of pages) */
	uint64_t stack_vm; /* Number of pages in the User Mode stack */

} mm_struct;

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
	uint64_t sleep_time;

	int wait_pid; /* pid of child last exited */
	uint64_t alarm; /* initialize alarm */
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

task_struct *
create_idle_thread();

task_struct*
create_thread_init();

task_struct *
create_thread(uint64_t, char *);

void
context_switch(task_struct *, task_struct *);

void
schedule();

void
clear_zombie(task_struct*);

task_struct *create_user_process(char* bin_name);

void copy_mm(task_struct *);

extern task_struct *current;

#define CODE 0
#define DATA 1
#define HEAP 2
#define STACK 3
// get specific vma of mm
vma_struct* get_vma(mm_struct* mm, int flag);

#define DO_EXECV_TMP_ADDR_TRANSLATE(x) (STACK_TOP - (tmp_vir_addr + PAGE_SIZE - x))

// exit error code
#define ILLEGAL_MEM_ACC 1 // illegal memmory access, killed by page fault handler
task_struct *find_task_struct(int pid);

void do_exit(int status);

void do_yield();


#endif
