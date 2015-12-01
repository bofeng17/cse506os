#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[], char* envp[]) {
    printf("I'm going to sleep !\n");

    if (argc == 2) {
        int sleep_time = stoi(argv[1]);
        printf("sleep time is:%d\n", sleep_time);
        if (sleep_time > 0) {
            sleep(sleep_time);
        } else {
            printf("===[ERROR] invalid sleep time:%d !===\n", sleep_time);
        }
    }

    return 0;
}
