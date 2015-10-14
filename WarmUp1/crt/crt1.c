#include <stdlib.h>
#include <stdio.h>
#include <sys/defs.h>

int main(int argc, char* argv[], char* envp[]);

void _start(void) {
	
	int argc=1;
	char **argv;
	char **envp;

	//reference:course hints---How Statically Linked Programs Run on Linux
	void *entry;
	__asm__ __volatile__(
		"movq %%rsp, %0"//%rsp is the stack pointer, 
						//which points to the top of the current stack frame
		:"=a" (entry)
	);
	
	argc = *((int *)entry + 2);// move addr of entry 2*int to point to argc
	argv =(char**)((int *)entry + 2)+1;//move to addr of argv
	envp = argv+argc + 1;

	int res;
	res = main(argc, argv, envp);

	exit(res);

}