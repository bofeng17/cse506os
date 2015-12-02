#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[], char* envp[]) {
   // printf("I'm going to sleep !\n");
    //printf("argc is:%d\n", argc);

    int sleep_time = 3;

    if (argc >= 2) {
        sleep_time = stoi(argv[1]);
        if(argc>2){
            printf("===[WARNING] two many arguments, use first number as sleep time!===\n");
        }
        printf("I'll sleep %d seconds! \n", sleep_time);

        if (sleep_time > 0) {
            sleep(sleep_time);
        } else {
            printf("===[ERROR] invalid sleep time:%d !===\n", sleep_time);
        }

    } else{
        printf(
                "===[WARNING] not enter sleep time, I'll sleep %d seconds (default)===\n",
                sleep_time);
        sleep(sleep_time);
    }

    return 0;
}
