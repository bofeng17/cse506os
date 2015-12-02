#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[], char* envp[]) {
//    int i = 0;
//    int pid = fork();
//    if (pid >0) {
//        while (1) {
//            printf("a %d\n",i);
//            i++;
//        }
//    } else {
//        while (1) {
//            printf("111111111111111 %d\n",i);
//            i++;
//        }
//    }
    

    printf("Hello World!\n");
    // int neg=-1;
    printf("argc is:%d\n",argc);
    int i;
    for(i=0;i<argc;i++){
        printf("argv[%d] is :%s\n",i,argv[i] );

    }

    for(i=0;i<argc;i++){
        printf("envp[%d] is :%s\n",i,envp[i] );

    }
    return 0;
}
