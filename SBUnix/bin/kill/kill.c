#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[], char* envp[]) {
    if (argc >= 3) {
        if (!strcmp(argv[1], "-9")) {
            int pid = stoi(argv[2]);
            if (pid >=0) {
                int ret = kill(pid);
                if (ret < 0) {
                    printf("===[ERROR] kill failed !===\n");
                } else {
                    printf("===[INFO] process %d killed !===\n", pid);
                }
            } else {
                printf("===[ERROR] invalid pid!, please see README===\n");
            }
        } else {
            printf("===[ERROR] parameters invalid!, please see README===\n");
        }
    } else {
        printf("===[ERROR] parameters invalid!, please see README===\n");

    }

    return 0;
}
