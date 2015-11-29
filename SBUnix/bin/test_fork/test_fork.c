#include <stdio.h>
#include <stdlib.h>
#include <sys/defs.h>

int main (int argc, char* argv[], char* envp[]) {
    pid_t pid = fork();
    if (pid > 0) {
        printf("I am parent process\n");
    } else if (pid == 0) {
        printf("I am child process\n");
    }
    return 0;
}