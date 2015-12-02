#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[], char* envp[]) {
    pid_t pid;
    int status;
    // login here
    printf("init process started: \n");
    while (1) {
        status = 0;

        pid = fork();
        if (pid > 0) {
            waitpid(pid, &status, 0);
            if (status == -1) { // process.h: #define ILLEGAL_MEM_ACC 1
                printf("shell killed by illegal memory access!\n");
            }
        } else if (pid == 0) {
            execve("bin/sbush", argv, envp);
        } else {
            printf("error in fork!\n");
        }
    }
    return 0;
}
