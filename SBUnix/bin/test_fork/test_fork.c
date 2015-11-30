#include <stdio.h>
#include <stdlib.h>
#include <sys/defs.h>

int main (int argc, char* argv[], char* envp[]) {
    pid_t pid = fork();
    if (pid > 0) {
        printf("I am parent process\n");
    } else if (pid == 0) {
        printf("I am child process\n");
        
        char* argv[3] = { "a1", "a2", NULL };
        char* envp[4] = { "e1", "e2", "e3", NULL };
        execve("bin/test_hello", argv, envp);
    }
    return 0;
}