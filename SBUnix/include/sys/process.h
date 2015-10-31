#ifndef _PROCESS_H
#define _PROCESS_H

#include <sys/defs.h>

//task state enumeration
enum TASK_STATE {
	TASK_NEW,
	TASK_READY,
	TASK_RUNNING,
	TASK_SLEEPING,
	TASK_INTERRUPTIBLE,
	TASK_UNINTERRUPTIBLE,
	TASK_ZOMBIE,
};

struct vma_struct {
	mm_struct *mm;
	uint64_t start;              // Our start address within vm_mm
	uint64_t end;           // The first byte after our end address within vm_mm
	vma_struct *next;     // linked list of VM areas per task, sorted by address
	uint64_t permission_flag;          // Flags read, write, execute permissions
//	uint64_t type;               // type of segment its refering to
//	struct file *file; // reference to file descriptors for file opened for writing
};

struct mm_struct {
	vma_struct *mmap;          //list of vma
//	vma_struct *mmap, *current;
	uint64_t start_code;
	uint64_t end_code;
	uint64_t start_data;
	uint64_t end_data;
	uint64_t start_brk;
	uint64_t end_brk;
	uint64_t start_stack;

//	uint64_t arg_start, arg_end, env_start, env_end;
//	uint64_t rss, total_vm, locked_vm;
};

struct task_struct {

	int pid;
	int ppid;
	uint64_t kernel_stack; //task stack
	uint64_t init_kern;
	uint64_t rip;			// instruction pointer
	uint64_t rsp;			// process stack pointer
	uint64_t task_state;    //the current state of task
	uint64_t sleep_time;
	mm_struct* mm;			// pointer to mm_struct
	uint64_t cr3;
	char task_name[20];     //task name
	task_struct* next;      //next process in the process list
//	struct fd* fd[MAX_FD];  // array of file descriptor pointers
	int wait_pid; 	        // pid of child last exited
	uint64_t alarm;         // initialize alarm
	char cur_dir[30];
//  file_t *cur_node;

};
