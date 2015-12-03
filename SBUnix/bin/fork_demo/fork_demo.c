/*
 * fork_demo.c
 *
 *  Created on: Dec 2, 2015
 *      Author: qqcao
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/defs.h>

int main(int argc, char* argv[], char* envp[]) {
    //int *ptr = (int *)malloc(4);
    int count = 0;
    pid_t pid = fork();
    int i = 1000;
    while (i-->0) {
        if (pid > 0) {
            count += 2;
            printf("parent count(+2) is: %d, pid is: %d, i is: %d \n", count, getpid(),i);

            //int80 call schedule
//            __asm__ __volatile__ ("int $0x80;");

        } else if (pid == 0) {

            count++;
            printf("child count(+1) is: %d, pid is: %d, i is: %d\n", count, getpid(),i);

            //int80 call schedule
//            __asm__ __volatile__ ("int $0x80;");

        } else {
            printf("error in fork!");
        }
    }
    return 0;
}

