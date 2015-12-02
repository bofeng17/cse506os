#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[], char* envp[]) {
    int i = 0;
    int pid = fork();
    if (pid >0) {
        while (1) {
            printf("a %d\n",i);
            i++;
        }
    } else {
        while (1) {
            printf("111111111111111 %d\n",i);
            i++;
        }
    }
    
    return 0;
}
