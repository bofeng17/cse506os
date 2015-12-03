#include <stdio.h>
#include <stdlib.h>
#include <sys/defs.h>

int main(int argc, char* argv[], char* envp[]) {
    //int *ptr = (int *)malloc(4);
    int count = 0;
    int status = 0;
    pid_t pid = fork();
    int i = 6;
    while (i-->0) {
        if (pid > 0) {
            count += 2;
            printf("parent count(+2) is: %d, pid is: %d, i is: %d \n", count, getpid(),i);

            waitpid(pid, &status, 0);

            //int80 call schedule
//            __asm__ __volatile__ ("int $0x80;");
//            while (1) {
//                printf("a %d\n", count);
//                count++;
//            }
        } else if (pid == 0) {

            count++;
            printf("child count(+1) is: %d, pid is: %d, i is: %d\n", count, getpid(),i);

            char* argv[3] = { "a1", "a2", NULL };
            char* envp[4] = { "e1", "e2", "e3", NULL };
            printf("I'll begin executing hello\n");

            execve("bin/hello", argv, envp);

            //int80 call schedule
//            __asm__ __volatile__ ("int $0x80;");
//            while (1) {
//                printf("11111111111111111 %d\n", count);
//                count++;
//            }
        } else {
            printf("error in fork!");
        }
    }
    return 0;
}
