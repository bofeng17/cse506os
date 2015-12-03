#include <stdio.h>
#include <stdlib.h>
#include <sys/defs.h>

#define LINE_LENGTH 100

int main(int argc, char* argv[], char* envp[]) {

    int i;
    for(i=1;i<argc;i++){
        printf("%s ",argv[i] );
    }

    printf("\n");
    return 0;
}



