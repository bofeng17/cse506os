#include <stdio.h>
#include <stdlib.h>
#include <sys/defs.h>

//int main (int argc, char* argv[], char* envp[]) {
//    pid_t pid = fork();
//    if (pid > 0) {
//        printf("I am parent process\n");
//    } else if (pid == 0) {
//        printf("I am child process\n");
//        
//        char* argv[3] = { "a1", "a2", NULL };
//        char* envp[4] = { "e1", "e2", "e3", NULL };
//        execve("bin/test_hello", argv, envp);

int main(int argc, char* argv[], char* envp[]) {
    //int *ptr = (int *)malloc(4);
    int count = 0;
    pid_t pid = fork();
    int i = 10;
    while (i-->0) {
        if (pid > 0) {
            count += 2;
            printf("parent count(+2) is: %d, pid is: %d, i is: %d \n", count, getpid(),i);

            //int80 call schedule
            __asm__ __volatile__ ("int $0x80;");

        } else if (pid == 0) {

            count++;
            printf("child count(+1) is: %d, pid is: %d, i is: %d\n", count, getpid(),i);

            //int80 call schedule
            __asm__ __volatile__ ("int $0x80;");

        } else {
            printf("error in fork!");
        }
    }
    return 0;
}
