#include <sys/sbunix.h>
//#include <sys/stdio.h> //kernel should not include user header files
#include <sys/stdlib.h>//kernel should not include user header files
#include <sys/tarfs.h>
#include <sys/elf.h>
#include <sys/process.h>
#include <sys/virmm.h>
#include <sys/string.h>
#include <sys/gdt.h>
#include <sys/syscall.h>

task_struct* front;
task_struct* end;
task_struct* current;

int track_task[PROCESS_NUMBER];

int count_args(char ** args) {
    if (args == NULL)
        return 0;
    int i = 0;
    
    while (args[i++] != NULL)
        ;
    return i - 1;
    
}

void init_process_id() {
    int i;
    for (i = 0; i < PROCESS_NUMBER; i++) {
        track_task[i] = 0;
    }
}

int assign_pid() {
    int i;
    for (i = 0; i < PROCESS_NUMBER; i++) {
        if (track_task[i] == 0) {
            track_task[i] = 1;
            return i;
        }
    }
    
    return -1;
}

//only set task_struct vma_struct and mm_struct
void set_task_struct(task_struct* task) {
    mm_struct* mstruct = kmalloc(MM);
    
    vma_struct* vma_code = kmalloc(VMA);
    vma_struct* vma_data = kmalloc(VMA);
    //vma_struct* vma_bss = kmalloc(VMA);
    vma_struct* vma_heap = kmalloc(VMA);
    vma_struct* vma_stack = kmalloc(VMA);
    
    vma_code->vm_mm = mstruct;
    vma_data->vm_mm = mstruct;
    // vma_bss->vm_mm = mstruct;
    vma_heap->vm_mm = mstruct;
    vma_stack->vm_mm = mstruct;
    
    vma_code->vm_next = vma_data;
    vma_data->vm_next = vma_heap;
    //    vma_data->vm_next = vma_bss;
    //    vma_bss->vm_next = vma_heap;
    vma_heap->vm_next = vma_stack;
    vma_stack->vm_next = NULL;
    
    mstruct->mmap = vma_code;
    task->mm = mstruct;
}

// copy mm_struct & vma from current to new_task
void copy_mm(task_struct *new_task) {
    // copy mm_struct
    mm_struct *cur_mm = current->mm;
    mm_struct *new_mm = kmalloc(MM);
    new_task->mm = new_mm;
    
    memcpy((void*) new_mm, (void*) cur_mm, 0x1000); // copy whole mm_struct
    
    // copy vma_chain
    vma_struct* cur_vma = cur_mm->mmap;
    vma_struct* new_vma = NULL;
    vma_struct* prev_new_vma = NULL;
    /*
     * modify vma_chain header ptr in mm_struct, which is the
     * sole difference between two mm_struct
     */
    while (cur_vma) {
        // if it is the 1st vma
        if (new_vma == NULL) {
            new_vma = kmalloc(VMA);
            // have vma_chain header ptr in mm_struct point to the 1st vma
            new_mm->mmap = new_vma;
            // copy whole vma
            memcpy((void*) new_vma, (void*) cur_vma, 0x1000);
            // modify the mm_struct the new_vma points to
            new_vma->vm_mm = new_mm;
            // prev_new_vma points to the previous vma of new_vma, helps build vma_chain
            prev_new_vma = new_vma;
            
            cur_vma = cur_vma->vm_next;
        } else {
            new_vma = kmalloc(VMA);
            // copy whole vma
            memcpy((void*) new_vma, (void*) cur_vma, 0x1000);
            // modify the mm_struct the new_vma points to
            new_vma->vm_mm = new_mm;
            // prev_new_vma points to the previous vma of new_vma, helps build vma_chain
            prev_new_vma->vm_next = new_vma;
            prev_new_vma = new_vma;
            
            cur_vma = cur_vma->vm_next;
        }
        new_vma->vm_next = NULL;
    }
}

// use flag as one of CODE, DATA, HEAP, STACK
vma_struct* get_vma(mm_struct* mm, int flag) {
    vma_struct * ret_vma = mm->mmap;
    
    while (flag-- > 0) {
        ret_vma = ret_vma->vm_next;
    }
    
    return ret_vma;
}

void setup_vma(mm_struct* mstruct) {
    
    vma_struct* vma_code = get_vma(mstruct, CODE);
    vma_code->vm_start = mstruct->start_code;
    vma_code->vm_end = mstruct->end_code;
    vma_code->permission_flag = VM_READ | VM_EXEC;
    
    vma_struct* vma_data = get_vma(mstruct, DATA);
    vma_data->vm_start = mstruct->start_data;
    vma_data->vm_end = mstruct->end_data + mstruct->bss;
    vma_data->permission_flag = VM_READ | VM_WRITE;
    
    //    vma_struct* vma_bss = get_vma(mstruct, BSS);
    //    vma_bss->vm_start = mstruct->end_data;
    //    vma_bss->vm_end = mstruct->end_data + mstruct->bss;
    //    vma_bss->permission_flag = VM_READ | VM_WRITE;
    //    vma_bss->vm_file = NULL;
    //    vma_bss->file_offset = 0;
    
    vma_struct* vma_heap = get_vma(mstruct, HEAP);
    vma_heap->vm_start = mstruct->start_brk;
    vma_heap->vm_end = mstruct->brk;
    vma_heap->permission_flag = VM_READ | VM_WRITE;
    vma_heap->vm_file = NULL;
    vma_heap->file_offset = 0;
    
    vma_struct* vma_stack = get_vma(mstruct, STACK);
    vma_stack->vm_start = mstruct->start_stack;
    vma_stack->vm_end = STACK_TOP;
    vma_stack->permission_flag = VM_READ | VM_WRITE;
    vma_stack->vm_file = NULL;
    vma_stack->file_offset = 0;
}

void func_init() {
    char* argv[3] = { "a1", "a2", NULL };
    
    char* envp[4] = { "e1", "e2", "e3", NULL };
    
    // transform kernel thread init to user process
    
    // allocate mm_struct and vma for init
    set_task_struct(current);
    
    // allocate heap
    current->mm->start_brk = (uint64_t) umalloc((void*)HEAP_BASE, PAGE_SIZE);
    current->mm->brk = current->mm->start_brk + PAGE_SIZE;
    
    // allocate stack
    umalloc((void*) STACK_TOP, PAGE_SIZE); // guarantee one page mapped above STACK_TOP
    umalloc((void*) (STACK_TOP - PAGE_SIZE), PAGE_SIZE); // map one page initially
    current->mm->start_stack = STACK_TOP - STACK_PAGES * PAGE_SIZE;
    
    // vma_chain setup by setup_vma() in do_execev
    do_execv("bin/test_fork", argv, envp);
    
    //    do_execv("bin/sbush", argv, envp);
}


//insert new task to run queue
void add_task(task_struct * task) {
    
    end->next = task;
    end = task;
    end->next = front;
    
}


task_struct*
create_thread(void* thread, char * thread_name) {
    
    task_struct * new_task = (task_struct*) (kmalloc(TASK));
    
    new_task->ppid = 0;
    new_task->pid = assign_pid();
    new_task->kernel_stack = new_task->init_kern = (uint64_t) kmalloc(KSTACK); //what is init_kern
    new_task->rip =(uint64_t) thread;
    new_task->task_state = TASK_NEW;
    new_task->sleep_time = 0;
    new_task->cr3 = get_CR3();
    strcpy(new_task->task_name, thread_name);
    new_task->mm = NULL;
    new_task->wait_pid = 0;
    
    if(current==NULL){
        current=new_task;
        front=new_task;
        end=new_task;
    }
    
    add_task(new_task);
    
    return new_task;
    
}

#define ARGV_PARAMS 1
#define ENVP_PARAMS 0
// set params (argv, envp) to the top area of user stack, return params number
int set_params_to_stack(uint64_t* rsp_p, char *** params_p, int flag) {
    int i = 0;
    int j = 0;
    int params_no = 0;
    uint64_t rsp64 = *rsp_p;
    void* rsp = (void*) rsp64;
    char** params = *params_p;
    if (params != NULL) {
        params_no = count_args(params);
        //dprintf("param_no is %d\n", params_no);
        i = params_no - 1;
        while (params[i] != NULL) {
            //dprintf("%s\n", params[i]);
            
            rsp = (char*) rsp - strlen(params[i]);
            
            j = 0;
            while (params[i][j] != '\0') {
                *((char*) rsp++) = params[i][j++];
                //printf("%c\n",*((char*)tmp+1) );
                //printf("%c\n",envp[i][j-1] );
            }
            rsp = (char*) rsp - strlen(params[i]);
            
            if (flag == ENVP_PARAMS) {
                params[i] = (char*) rsp;
            }
            
            if (flag == ARGV_PARAMS) {
                params[i + 1] = (char*) rsp;
            }
            rsp = (char*) rsp - 1;
            *((char*) rsp) = '\0';
            
            //dprintf ("tmp is %p", tmp);
            i--;
            //printf("%s\n",(char*)tmp+1 );
        }
        
    }
    
    *rsp_p = (uint64_t) rsp;
    return params_no;
}

int do_execv(char* bin_name, char ** argv, char** envp) {
    task_struct* execv_task = current;
    
    int argc = 0;
    int envc = 0;
    char* argv_0 = bin_name;
    
    /*
     * setup heap
     * each process has at least one page in heap
     * besides, argv is passed via heap
     * so here needn't allocate page frames
     * should be cautious about the assumption
     */
    current->mm->start_brk = HEAP_BASE;
    execv_task->mm->brk = HEAP_BASE + PAGE_SIZE;
    
    /*
     * setup stack: different from heap
     * although each process has at least one page in stack
     * envp is passed via stack which may be overwritten without allocating new stack
     * 1) allocate a page frame 2) map a temporary virt addr to it
     * 3) copy envp onto that page 4) remap it near STACK_TOP
     * 5) free the original page nearing STACK_TOP
     * TODO: decrease ref_count of physical page, if reaches -1, free that page
     * still share stack top protection page after reallocation
     */
    uint64_t tmp_vir_addr = 0xffffffff80000000UL;
    
    execv_task->mm->start_stack = STACK_TOP - STACK_PAGES * PAGE_SIZE;
    
    // map a tmp_vir_addr (0xffffffff80000000UL) to the allocated page frame
    self_ref_write(PT, tmp_vir_addr, allocate_page_user() | PTE_P | PTE_U | PTE_W);
    
    // flushing TLB immediately after modifying page table
    // only after flushing TLB can we start to make memory access
    __asm__ __volatile__ ("mov %0, %%cr3;"
                          ::"r"(current->cr3));
    
    
    // copy argc, argv, envp onto the temporary virt addr
    void* rsp = (void*) (tmp_vir_addr + PAGE_SIZE);
    void* tmp = rsp - 1;
    uint64_t tmp2 = (uint64_t) tmp;
    
    // save envp string to the top area of user stack
    envc = set_params_to_stack(&tmp2, &envp, ENVP_PARAMS);
    
    // save argv string to the top area of user stack
    argc = set_params_to_stack(&tmp2, &argv, ARGV_PARAMS);
    
    tmp = (void*) tmp2;
    //copy argv_0 (binary name) and set argv0 pointer
    tmp = (char*) tmp - strlen(argv_0);
    int j = 0;
    while (argv_0[j] != '\0') {
        *((char*) tmp++) = argv_0[j++];
    }
    tmp = (char*) tmp - strlen(argv_0);
    argv[0] = (char*) tmp;
    
    argc += 1;
    
    // align last byte
    tmp = (void *) ((uint64_t) tmp & 0xfffffffffffffff8UL);
    
    // set null pointer between string area and envp
    tmp -= 8;	      // uint64_t is 8 bytes
    memset(tmp, 0, 8);
    tmp -= 8;	      // uint64_t is 8 bytes
    
    // store envp pointers in the proper place of user stack
    if (envc > 0) {
        execv_task->mm->env_end = DO_EXECV_TMP_ADDR_TRANSLATE((uint64_t) envp[envc - 1]);
        while (envc-- > 0) {
            // envp[envc] = (char*) tmp--;
            *((uint64_t*) tmp) = DO_EXECV_TMP_ADDR_TRANSLATE((uint64_t) envp[envc]);
            tmp = (uint64_t*) tmp - 1;
        }
        execv_task->mm->env_start = DO_EXECV_TMP_ADDR_TRANSLATE((uint64_t) envp[0]);
        // set 0 between envp and argv
        //tmp -= 8;	      // uint64_t is 8 bytes
        memset(tmp, 0, 8);
        tmp -= 8;	      // uint64_t is 8 bytes
        
    }
    
    // store argv pointers in the proper place of user stack
    if (argc > 0) {
        
        execv_task->mm->arg_end = DO_EXECV_TMP_ADDR_TRANSLATE((uint64_t) argv[argc - 1]);
        
        int argc2 = argc;
        while (argc2-- > 0) {
            //argv[argc2] = (char*) tmp--;
            *((uint64_t*) tmp) = DO_EXECV_TMP_ADDR_TRANSLATE((uint64_t) argv[argc2]);
            tmp = (uint64_t*) tmp - 1;
        }
        execv_task->mm->arg_start = DO_EXECV_TMP_ADDR_TRANSLATE((uint64_t) argv[0]);
    }
    
    *((uint64_t*) tmp) = argc;
    rsp = tmp;
    
    // remap tmp_vir_addr to near STACK_TOP
    execv_task->rsp = STACK_TOP - (tmp_vir_addr + PAGE_SIZE - (uint64_t) rsp);
    self_ref_write(PT, STACK_TOP - PAGE_SIZE, self_ref_read(PT, tmp_vir_addr));
    self_ref_write(PT, tmp_vir_addr, 0);
    
    /*
     * argc, argv, envp may be either stored on stack or rodata segment,
     * must finish copying them onto user stack before load_elf,
     * bacause load_elf will overwrite rodata segment
     */
    if (load_elf(execv_task, bin_name) < 0)
        return -1; // -1 if error
    
    // flushing TLB immediately after modifying page table
    // only after flushing TLB can we start to make memory access
    __asm__ __volatile__ ("mov %0, %%cr3;"
                          ::"r"(current->cr3));
    
    // setup vma_chain according to mm_struct
    // needn't flush TLB because setup_vma doesn't modify page tale
    setup_vma(execv_task->mm);
    
    // return to ring3, moved from func_init to here
    sysret_to_ring3();
    
    return 0;
    
}

task_struct*
create_user_process(char* bin_name) {
    task_struct * new_task = (task_struct*) (kmalloc(TASK));
    
    new_task->ppid = 0;
    new_task->pid = assign_pid();
    new_task->kernel_stack = new_task->init_kern = (uint64_t) kmalloc(KSTACK); //what is init_kern
    
    new_task->task_state = TASK_NEW;
    new_task->sleep_time = 0;
    
    new_task->cr3 = get_CR3();
    
    strcpy(new_task->task_name, bin_name);
    //do_execv (bin_name, NULL, NULL);
    
    new_task->wait_pid = 0;
    
    add_task(new_task);
    
    return new_task;
}

void set_child_pt(task_struct* child) {
    global_PML4 = (pml4_t) kmalloc(KERNPT);
    child->cr3 = (uint64_t) global_PML4 - VIR_START;
    
    map_kernel();
    
    vma_struct* vma = current->mm->mmap;
    
    while (vma != NULL) {
        uint64_t start_addr = vma->vm_start & CLEAR_OFFSET;
        uint64_t end_addr = vma->vm_end;
        
        if (end_addr % PAGE_SIZE) {
            end_addr &= CLEAR_OFFSET;
            end_addr += PAGE_SIZE;
        } else {
            end_addr &= CLEAR_OFFSET;
        }
        
        if (vma -> vm_next == NULL) {
            // This is the vma for stack
            // Need to map one more page at the end to protect stack
            end_addr += PAGE_SIZE;
        }
        
        while (start_addr < end_addr) {
            /*
             * for stack and heap/demand paging, must check if content is 0
             */
            // fix:
            //            if (self_ref_read(PML4, start_addr)) {
            //                // always true
            //                if (self_ref_read(PDPT, start_addr)) {
            //                    if (self_ref_read(PDT, start_addr)) {
            //                        if (self_ref_read(PT, start_addr)) {
            //                            content
            //                        }
            //                    }
            //                }
            //            }
            // TODO: this assumes PT is already in memory, buggy!
            uint64_t content = self_ref_read(PT, start_addr);
            if (content) {
                page_sp * page = get_page_frame_descriptor(content);
                /*
                 * risk!!
                 * if two memory region overlaps at virtual page X,
                 * the ref_count of page frame correpsonding to X will be increnased twice
                 * caused by not aligned vma
                 */
                page->ref_count += 1;
                
                // modify parent's page table
                content &= PTE_R_MASK;
                content |= PTE_COW;
                self_ref_write(PT, start_addr, content);
                
                // map child's page table
                map_user_pt(start_addr, content, USERPT);
            }
            start_addr += PAGE_SIZE;
        }
        vma = vma->vm_next;
    }
    
    
    // flushing TLB
    __asm__ __volatile__ ("mov %0, %%cr3;"
                          ::"r"(current->cr3));
}

// rewrite at Nov 26
int do_fork() {
    //create a new task struct
    task_struct *new_task = (task_struct *) (kmalloc(TASK));
    
    //assign new pid for child
    new_task->pid = assign_pid();
    new_task->ppid = current->pid;
    
    //copy task name
    strcpy(new_task->task_name, "child");
    
    //memory portion begins here
    //child has it own kernel stack
    new_task->init_kern = new_task->kernel_stack = (uint64_t) kmalloc(KSTACK);
    
    /*
     * copy into child from parent:
     * mm_struct, vma_chain
     */
    copy_mm(new_task);
    
    // set page table of child process, also modified new_task->cr3
    set_child_pt(new_task);
    
    
    // add child task to run queue
    new_task->task_state = TASK_READY;
    add_task(new_task);
    
    
    // TODO: tricky part starts here
    
    /*
     * copy kernel stack: copy saved rbp from parent's kernel stack to child's
     * in do_syscall:
     *      pushq %r11 (saved rflags)
     *      pushq %rcx (saved rip)
     *      call  do_fork
     * in do_fork:
     *      ... (ignored)
     * (lower address)
     */
    // copy ret_addr of save rflags (i=1), saved rip, ret addr of do_fork
    for (int i = 1; i <= 3; i++) {
        *((uint64_t *)new_task->init_kern - i) = *((uint64_t *)current->init_kern - i);
    }
    
    // set (instead of copying) top of kernel stack:
    new_task->kernel_stack = new_task->init_kern - 0x18;
    // TODO: verify whether new_task->kernel_stack points to child process's own kernel stack now
    
    
    // don't need to copy user stack content: due to COW
    // copy top of user stack:
    new_task->rsp = current->rsp;
    
    
    // set rip (not simply copied from parent): child process begins executing @ label child_ret below
    __asm__ __volatile__ ("movq $child_ret, %0;"
                          :"=r"(new_task->rip));
    
    // tricky part ends here
    
    int chld_ret = new_task->pid;
    // always true if statement, just for child return not being optimized out
    if (chld_ret) {
        // for parent: return child's pid
        return new_task->pid;
    }
    
    // for child: return 0
    __asm__ __volatile__ ("child_ret:\t;"
                          "mov $0, %0;"
                          "retq;"
                          :"=r"(chld_ret));
    // useless return, for the sake of passing compiling
    return chld_ret;
}

int do_ps(ps_t ps) {
    task_struct* cur = current;
    int c = 0;
    do  {
        ps->id[c] = cur->pid;
        strcpy(ps->name[c], cur->task_name);
        //       strcpy(ps->state[c], cur->task_name);
        
        //ps->state[c] = cur->task_state;
        switch (cur->task_state) {
            case TASK_NEW:
                strcpy(ps->state[c], "new       ");
                break;
            case TASK_READY:
                strcpy(ps->state[c], "ready     ");
                break;
            case TASK_RUNNING:
                strcpy(ps->state[c], "running   ");
                break;
            case TASK_SLEEPING:
                strcpy(ps->state[c], "sleeping  ");
                break;
            case TASK_BLOCKED:
                strcpy(ps->state[c], "blocked   ");
                break;
            case TASK_ZOMBIE:
                strcpy(ps->state[c], "zombie    ");
                break;
            case TASK_DEAD:
                strcpy(ps->state[c], "dead      ");
                break;
            default: strcpy(ps->state[c], "unknown   ");
        }
        
        cur = cur->next;
        c++;
    }while((cur != current));
    return c;
}

int do_getpid(){
    return current->pid;
}

int do_getppid(){
    return current->ppid;
}

// 3rd parm ignored
pid_t do_waitpid(pid_t pid, int *status, int options){
    if (current->wait_pid != pid) {
        // if child process not exited
        current->task_state = TASK_BLOCKED;
        //int 0x80 call schedule, so that registers can be pushed
        __asm__ __volatile__ ("int $0x80;");
    }
    // TODO: release the resource of child process
    
    // pass ret_val of child to *status
    *status = find_task_struct(pid)->ret_val;
    
    // if child process already exited, return at once
    return pid;
}

void do_exit(int status) {
    task_struct *parent;
    current->task_state = TASK_ZOMBIE;
    current->ret_val = status;
    
    /*
     * Check if parent process is suspended (by calling waitpid())
     * if yes, wake parent process
     */
    parent = find_task_struct(current->ppid);
    parent->wait_pid = current->pid;
    if (parent->task_state == TASK_BLOCKED) {
        parent->task_state = TASK_READY;
    }
    // current exited, so doesn't need push registers
    schedule();
}

void do_sleep(uint32_t seconds) {
    current->task_state = TASK_SLEEPING;
    current->sleep_time = seconds * 1000;
    __asm__ __volatile__ ("int $0x80;");
}

void do_yield() {
    // Although it is safe to call schedule directly instaed of int 0x80
    __asm__ __volatile__ ("int $0x80;");
}

// find task_struct in run queue according to its pid
task_struct *find_task_struct(int pid) {
    task_struct *run = current->next;
    while(run -> pid != pid){
        run = run -> next;
        if(run == current)
            return NULL;
    }
    return run;
}

// decrease sleep_time field of task_struct who are sleeping
void sleep_time_decrease() {
    task_struct *task_sleep = current->next;
    while (task_sleep != current) {
        if (task_sleep->task_state == TASK_SLEEPING) {
            task_sleep->sleep_time -= IRQ0_period;
            if (task_sleep->sleep_time <= 0) {
                task_sleep->sleep_time = 0;
                task_sleep->task_state = TASK_READY;
            }
        }
        task_sleep = task_sleep->next;
    }
}
