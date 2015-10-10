##I: Steps of building your own OS:
- 1) Physical Memory Management 
Data Structure: Free list(link list)
Functions: Physical Page Allocation/free
- 2) Virtual Memory Management
Data Structure: Page Table, VMA
Functions: Virtual Page Allocation/free, Page Privilege Management (Writable/Executable/?)?basis for building mprotect() syscall, heap memory management, kernel malloc, ? 
- 3) Process Management
user space vs. kernel space process (also relate to virtual memory)
Data Structure: PCB, MM_struct, task_struct
Functions: fork(), basic system calls
- 4) Scheduler
Data Structure: 
Functions: 
- 5) Debugging, perhaps file system

##II: Schedule:
- 1) Week 7: 1)  *Oct 4 ? 10 is Week 7(start from Sunday)
Junco: Physical memory
Qingqing, Bo: Requirement analysis and detailed design of virtual memory.
- 2) --> Week 8-9 
- 3) --> Week 10-11 
- 4) --> Week 12-13 
- 5) --> Week 14-15   
**Note:**Nov 22 - 28 is Week 14, Thanks giving on Nov 26

##III: Meeting plan:
1. Tue, Oct 12: 
Finish requirement analysis of physical and virtual memory, resolve problems, **planning to finish memory management before the next week**
2. Fri, Oct 15:
   - Finish coding physical memory (no later than vm completion)
   - Finish detailed design of virtual memory, start to code virtual memory
3. Meet every Monday and Thursday after next week meeting.

##IV: Tips:
- The task may be far more complicate, so it's better to **code as we think and learn**
- Good steps of building each module:
	1. Requirement analysis(reading book, source code, discussion etc.)
	2. Detailed design: data structure, functions, modules,...
	3. Coding
	4. Unit testing by coder and others
	5. Integration Testing
- Work in a pipeline manner may speed up our project(while member A is coding physical memory, member B should start requirement analysis of virtual memory)
- More communication: Whenever get stuck, ask for help; whenever find good tools, share; whenever make progress, deliver
- You don't have to know everything before make your hand dirty
