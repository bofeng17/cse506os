#include <stdio.h>
#include <stdlib.h>
#include <sys/defs.h>

int main (int argc, char* argv[], char* envp[]) {
    pid_t pid = fork();
    int count = 5;
    while (((count--) > 0) && pid > 0){
        printf("I am parent process %d\n", getpid());
        pid = fork();
    }
    if (pid == 0) {
        printf("I am user process %d\n", getpid());
//        while (1) {
//            switch (getpid()) {
//                case 3:
//                    printf("1\n");
//                    break;
//                case 4:
//                    printf("aaaaaaaaaaaaa\n");
//                    break;
//                case 5:
//                    printf("2222\n");
//                    break;
//                case 6:
//                    printf("bbbbbbbbbbbbb\n");
//                    break;
//                case 7:
//                    printf("3333333\n");
//                    break;
//                case 8:
//                    printf("ccccccccccccc\n");
//                    break;
//            }
//        }
    }
}

//int main(int argc, char* argv[], char* envp[]) {
//    //int *ptr = (int *)malloc(4);
//    int count = 0;
//    int status = 0;
//    pid_t pid = fork();
//    int i = 6;
//    while (i-->0) {
//        if (pid > 0) {
//            count += 2;
//            printf("parent count(+2) is: %d, pid is: %d, i is: %d \n", count, getpid(),i);
//
//            waitpid(pid, &status, 0);
//
//            //int80 call schedule
//            __asm__ __volatile__ ("int $0x80;");
////            while (1) {
////                printf("a %d\n", count);
////                count++;
////            }
//        } else if (pid == 0) {
//
//            count++;
//            printf("child count(+1) is: %d, pid is: %d, i is: %d\n", count, getpid(),i);
//
//            char* argv[3] = { "a1", "a2", NULL };
//            char* envp[4] = { "e1", "e2", "e3", NULL };
//            execve("bin/test_hello", argv, envp);
//
//            //int80 call schedule
//            __asm__ __volatile__ ("int $0x80;");
////            while (1) {
////                printf("11111111111111111 %d\n", count);
////                count++;
////            }
//        } else {
//            printf("error in fork!");
//        }
//    }
//    return 0;
//}
