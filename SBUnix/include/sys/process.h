#include <sys/defs.h>
#define PROCESS_NUMBER 128
#define KSTACK_NUMBER PROCESS_NUMBER
#define MM_NUMBER PROCESS_NUMBER

#define NAME_LENGTH 32

/*task state enumeration*/
enum TASK_STATE
{
  TASK_NEW,
  TASK_READY,
  TASK_RUNNING,
  TASK_SLEEPING,
  TASK_INTERRUPTIBLE,
  TASK_UNINTERRUPTIBLE,
  TASK_ZOMBIE,
};

typedef struct vma_struct
{
  struct mm_struct *vm_mm; /* Pointer to the memory descriptor that owns the region */

  uint64_t vm_start; /* First linear address inside the region */
  uint64_t vm_end; /* First linear address after the region */
  struct vma_struct *vm_next; /* Next region in the process list */

  uint64_t permission_flag; /* Flags read, write, execute permissions */
  struct file *vm_file; /* Reference to file descriptors for file opened for writing */

} vma_struct;

typedef struct mm_struct
{

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

  uint64_t rss; /* Number of page frames allocated to the process */

  uint64_t total_vm; /* Size of the process address space (number of pages) */
  uint64_t stack_vm; /* Number of pages in the User Mode stack */

} mm_struct;

typedef struct task_struct
{

  int pid;
  int ppid;
  char task_name[NAME_LENGTH];

  struct task_struct* next; /* next process in the process list */

  mm_struct* mm; /* Pointers to memory area descriptors */
  uint64_t cr3;

  uint64_t kernel_stack; /* task stack */
  uint64_t init_kern;
  uint64_t rip; /* instruction pointer */
  uint64_t rsp; /* process stack pointer */

  uint64_t task_state; /* the current state of task */
  uint64_t sleep_time;

  int wait_pid; /* pid of child last exited */
  uint64_t alarm; /* initialize alarm */
  char cur_dir[NAME_LENGTH]; /* current directory */

} task_struct;

task_struct *
create_idle_thread ();

task_struct *
create_thread (uint64_t, char *);

void
context_switch (task_struct *, task_struct *);

void
schedule ();

void
clear_zombie (task_struct*);
