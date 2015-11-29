#include <stdio.h>
#include <stdlib.h>
#include <sys/defs.h>

int main (int argc, char* argv[], char* envp[]) {
    int *ptr = (int *)malloc(4);
    pid_t pid = fork();
    if (pid > 0) {
        *ptr = 0x10;
        printf("I am parent process %d\n",*ptr);
    } else if (pid == 0) {
        *ptr = 0x20;
        printf("I am child process %d\n", *ptr);
    }
    return 0;
}