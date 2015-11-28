#include <stdio.h>
#include <stdlib.h>
#include <sys/defs.h>

int main(int argc, char* argv[], char* envp[]) {

    char* s = malloc(100 * sizeof(char));

    printf("please input :(enter to finish):");

    scanf("%s", s);

    printf("begin printing s\n");

    printf("%s\n", s);

    printf("printing finished!\n");

    return 0;
}
